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

#include "valery/valery.h"
#include "valery/env.h"
#include "valery/prompt.h"
#include "valery/interpreter/lex.h"
#include "valery/interpreter/parser.h"
#include "valery/interpreter/interpreter.h"
#include "builtins/builtins.h"


static volatile int received_sigint = 0;


static inline void catch_sigint(int signal)
{
    if (signal == SIGINT)
        received_sigint = 1;
}

static int valery_interpret(char *input)
{
    int rc;
    struct tokenlist_t *tl = tokenize(input);
#ifdef DEBUG_INTERPRETER
    tokenlist_dump(tl);
#endif
    struct ast_node_t *expr = parse(tl);
#ifdef DEBUG_INTERPRETER
    ast_print(expr);
#endif
    rc = interpret(expr);

    tokenlist_free(tl);
    ast_free(expr);
    return rc;
}

static int valery(char *source)
{
    struct env_t *env = env_init();

    if (source != NULL) {
        valery_interpret(source);
    } else {
        /* interactive mode */
        struct hist_t *hist = hist_init(env_get(env->env_vars, "HOME"));
        struct prompt_t *p = prompt_malloc();
        signal(SIGINT, catch_sigint);

        /* main loop */
        while (1) {
            env_update(env);
            prompt(p, hist, env->ps1);
            /* skip exec if ctrl+c is caught */
            if (received_sigint) {
                received_sigint = 0;
                signal(SIGINT, catch_sigint);
                continue;
            }
            hist_save(hist, p->buf);
            if (strcmp(p->buf, "") == 0)
                continue;
            else if (strcmp(p->buf, "exit") == 0)
                break;

            /* loop enters here means "ordinary" commands were typed in */
            valery_interpret(p->buf);
        }

        /* free and write to file before exiting */
        hist_write(hist);
        hist_free(hist);
        prompt_free(p);
    }

    env_free(env);
    return 0;
}

int main(int argc, char *argv[])
{
    //TODO: proper arg parsing
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
            return valery(argv[2]);
        }
    }

    return valery(NULL);
}
