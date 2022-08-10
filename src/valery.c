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
#include <string.h>
#include <signal.h>
#include <termios.h>

#include "valery.h"
#include "valery/load_config.h"
#include "valery/prompt.h"
#include "valery/exec.h"
#include "builtins/builtins.h"


static volatile int received_sigint = 0;
struct termios originalt, newt;
static bool INTERACTIVE;


static void catch_exit_signal(int signal)
{
    if (signal == SIGINT)
        received_sigint = 1;
}

int valery(char *arg)
{
    struct tokenized_str_t *ts;
    struct hist_t *hist = NULL;
    struct env_t *env = env_t_malloc();
    int rc;

    rc = parse_config(env);
    if (rc == 1) {
        fprintf(stderr, "error parsing .valeryrc");
        env_t_free(env);
        return 1;
    }

    /* create tokenized_str_t object. Will reused same object every loop. */
    ts = tokenized_str_t_malloc();

    if (INTERACTIVE) {
        char input_buffer[COMMAND_LEN] = {0};
        char hist_file_path[MAX_ENV_LEN] = {0};

        snprintf(hist_file_path, MAX_ENV_LEN, "%s/%s", env_get(env, "HOME"), HISTFILE_NAME);
        /* establish a connection to the hist file */
        hist = hist_t_malloc(hist_file_path);

        signal(SIGINT, catch_exit_signal);

        /* main loop */
        while (1) {
            env_update(env);
            prompt(hist, env->ps1, input_buffer);

            /* skip exec if ctrl+c is caught */
            if (received_sigint) {
                received_sigint = 0;
                signal(SIGINT, catch_exit_signal);
                goto end_loop;
            }

            hist_t_save(hist, input_buffer);

            if (strcmp(input_buffer, "") == 0)
                goto end_loop;
            else if (strcmp(input_buffer, "exit") == 0)
                break;

            /* loop enters here means ordinary command was typed in */
            rc = tokenize(ts, env, input_buffer);
#ifdef DEBUG
        tokenized_str_t_print(ts);
#endif /* DEBUG */
            if (rc == 0)
                valery_parse_tokens(ts, env, hist);

        /* clears all buffers */
        end_loop:
            tokenized_str_t_clear(ts);
            memset(input_buffer, 0, COMMAND_LEN);
        }

        /* free and write to file before exiting */
        hist_t_write(hist);
        hist_t_free(hist);
    } else {
        rc = tokenize(ts, env, arg);
        if (rc == 0)
            valery_parse_tokens(ts, env, hist);
    }

    env_t_free(env);
    tokenized_str_t_free(ts);
    return rc;
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            help();
            return 0;
        }

        if (strcmp(argv[1], "--license") == 0) {
            license();
            return 0;
        }

        if (strcmp(argv[1], "-c") == 0) {
            if (argc == 2) {
                printf("valery: '-c' option requires an argument\n");
                return 1;
            }
            INTERACTIVE = false;
            return valery(argv[2]);
        }
    }

    INTERACTIVE = true;
    return valery(NULL);
}
