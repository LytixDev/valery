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
#include "valery/env.h"
#include "valery/prompt.h"
#include "valery/interpreter/lex.h"
#include "valery/interpreter/parser.h"
#include "valery/interpreter/interpreter.h"


static volatile int received_sigint = 0;
struct termios originalt, newt;


static void catch_sigint(int signal)
{
    if (signal == SIGINT)
        received_sigint = 1;
}

static int valery_interpret(char *input)
{
    int rc;
    struct tokenlist_t *tl = tokenize(input);
    ASTNodeHead *expr = parse(tl);
    rc = interpret(expr);
    //tokenlist_dump(tl);
    //ast_print(expr);

    tokenlist_free(tl);
    ast_free(expr);
    return rc;
}

static int valery(char *source)
{
    struct env_t *env = init_env();

    if (source == NULL) {
        struct hist_t *hist = init_hist(env_get(env->env_vars, "HOME"));
        char input_buffer[COMMAND_LEN] = {0};
        signal(SIGINT, catch_sigint);

        /* main loop */
        while (1) {
            env_update(env);
            prompt(hist, env->ps1, input_buffer);

            /* skip exec if ctrl+c is caught */
            if (received_sigint) {
                received_sigint = 0;
                signal(SIGINT, catch_sigint);
                goto end_loop;
            }

            hist_t_save(hist, input_buffer);

            if (strcmp(input_buffer, "") == 0)
                goto end_loop;
            else if (strcmp(input_buffer, "exit") == 0)
                break;

            /* loop enters here means "ordinary" commands were typed in */
            valery_interpret(input_buffer);

        /* clears all buffers */
        end_loop:
            //TODO: should we not zero out bytes on demand?
            memset(input_buffer, 0, COMMAND_LEN);
        }

        /* free and write to file before exiting */
        hist_t_write(hist);
        hist_t_free(hist);
    } else {
        valery_interpret(source);
    }

    env_free(env);
    return 0;
}

int main(int argc, char *argv[])
{
    /*
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
    */

    return valery(NULL);
}
