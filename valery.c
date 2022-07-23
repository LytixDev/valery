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

static inline void catch_exit_signal(int signal)
{
    received_sigint = 1;
}

struct env_t *malloc_env()
{
    struct env_t *env = (env_t *) malloc(sizeof(env_t));
    env->paths = (char **) malloc(STARTING_PATHS * sizeof(char *));
    for (int i = 0; i < STARTING_PATHS; i++)
        env->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    env->total_paths = STARTING_PATHS;
    env->current_path = 0;
    env->PATH = (char *) malloc(MAX_ENV_LEN * sizeof(char));
    env->exit_code = 0;
    env->PS1 = (char *) malloc(MAX_ENV_LEN * sizeof(char));
    env->HOME = (char *) malloc(MAX_ENV_LEN * sizeof(char));
    return env;
}

void resize_path_len(struct env_t *env, int new_len) {
    env->paths = (char **) realloc(env->paths, new_len * sizeof(char *));
    for (int i = env->total_paths; i < new_len; i++)
        env->paths[i] = (char *) malloc(MAX_ENV_LEN * sizeof(char));

    env->total_paths = new_len;
    return;
}

void free_env(struct env_t *env)
{
    if (env == NULL)
        return;
    
    for (int i = 0; i < env->total_paths; i++)
        free(env->paths[i]);

    free(env->paths);
    free(env->PATH);
    free(env->PS1);
    free(env->HOME);
    free(env);
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        printf("%s\n", argv[1]);
        if (strcmp(argv[1], "--help") == 0) {
            help();
            return 0;
        }
    }

    struct env_t *env = malloc_env();
    struct hist_t *hist;
    struct tokenized_str_t *ts;
    char hist_file_path[MAX_ENV_LEN];
    char input_buffer[COMMAND_LEN] = {0};
    char cmd[COMMAND_LEN];
    char args[COMMAND_LEN];
    int rc;
    int rc_env;

    signal(SIGINT, catch_exit_signal);
    disable_term_flags();

    rc = parse_config(env);
    if (rc == 1) {
        fprintf(stderr, "error parsing .valeryrc");
        free_env(env);
        enable_term_flags();
        return 1;
    }

    /* establish a connection to the hist file */
    snprintf(hist_file_path, MAX_ENV_LEN, "%s/%s", env->HOME, HISTFILE_NAME);
    hist = malloc_history(hist_file_path);

    /* create tokenized_str_t object. Will reused same object every loop. */
    ts = tokenized_str_t_malloc();

    /* main loop */
    while (1) {
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
        rc = tokenize(ts, input_buffer);
        if (rc == 0) {
            rc_env = valery_parse_tokens(ts, env, hist);
            env->exit_code = rc_env;
        }

    /* clears all buffers */
    end_loop:
        tokenized_str_t_clear(ts);
        memset(input_buffer, 0, COMMAND_LEN);
        cmd[0] = 0;
        args[0] = 0;
    }

    /* free and write to file before exiting */
    write_commands_to_hist_file(hist);
    free_env(env);
    free_history(hist);
    tokenized_str_t_free(ts);

    printf("Exiting ...\n");
    enable_term_flags();
    return 0;
}
