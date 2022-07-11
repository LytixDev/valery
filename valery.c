/*
 *  Experimental interactive UNIX-like shell
 *   
 *  Copyright (C) 2022 Nicolai Brand 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <memory.h>

#include "valery.h"
#include "utils/load_config.h"
#include "utils/prompt.h"
#include "utils/histfile.h"
#include "utils/exec.h"
#include "utils/lexer.h"
#include "builtin/builtins.h"


static volatile int received_sigint = 0;
static struct termios originalt, newt;


void disable_term_flags()
{
    tcgetattr(STDIN_FILENO, &originalt);
    newt = originalt;
    /* change so buffer don't require new line or similar to return */
    newt.c_lflag &= ~ICANON;          
    newt.c_cc[VTIME] = 0;
    newt.c_cc[VMIN] = 1;
    /* turn of echo */
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void enable_term_flags()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &originalt);
}

void catch_exit_signal(int signal)
{
    received_sigint = 1;
}

struct ENV *malloc_env()
{
    struct ENV *env = (ENV *) malloc(sizeof(ENV));
    env->exit_code = 0;
    env->PS1 = (char *) malloc(sizeof(char) * MAX_ENV_LEN);
    env->PATH = (char *) malloc(sizeof(char) * MAX_ENV_LEN);
    env->HOME = (char *) malloc(sizeof(char) * MAX_ENV_LEN);
    return env;
}

void free_env(struct ENV *env)
{
    if (env == NULL)
        return

    free(env->PS1);
    free(env->PATH);
    free(env->HOME);
    free(env);
}

int main()
{
    struct ENV *env = malloc_env();
    char hist_file_path[MAX_ENV_LEN];
    char input_buffer[COMMAND_LEN] = {0};
    char cmd[COMMAND_LEN];
    char args[COMMAND_LEN];

    signal(SIGINT, catch_exit_signal);
    disable_term_flags();

    int rc = parse_config(env);
    if (rc == 1) {
        fprintf(stderr, "error parsing .valeryrc");
        free_env(env);
        return 1;
    }

    /* establish a connection to the hist file */
    snprintf(hist_file_path, MAX_ENV_LEN, "%s/%s", env->HOME, HISTFILE_NAME);
    struct hist_t *hist = malloc_history(hist_file_path);

    /* main loop */
    while (1) {
        /* TODO: can we reuse instead of mallocing a new one each loop? */
        struct tokens_t *tokens = malloc_tokens_t();
        prompt(hist, env->PS1, input_buffer);

        /* skip exec if ctrl+c is caught */
        if (received_sigint) {
            received_sigint = 0;
            signal(SIGINT, catch_exit_signal);
            goto end_loop;
        }

        save_command(hist, input_buffer);

        if (strcmp(input_buffer, "") == 0)
            goto end_loop;
        else if (strcmp(input_buffer, "exit") == 0)
            break;

        /* loop enters here means ordinary command was typed in */
        tokenize(tokens, input_buffer);
        rc = valery_exec_buffer(tokens, env, hist);
        env->exit_code = rc;

        /* clears all buffers */
    end_loop:
        memset(input_buffer, 0, COMMAND_LEN);
        free_tokens_t(tokens);
        cmd[0] = 0;
        args[0] = 0;
    }

    /* free and write to file before exiting */
    write_commands_to_hist_file(hist);
    free_env(env);
    free_history(hist);

    printf("\nExiting ...\n");
    enable_term_flags();

    return 0;
}
