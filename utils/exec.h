/*
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


#ifndef EXEC
#define EXEC

#include "lexer.h"
#include "histfile.h"
#include "../valery.h"

#define CHILD_PID 0
#define READ_END 0
#define WRITE_END 1

/* types */
typedef enum stream_flags {
    STREAM1_VACANT      = 1 << 0,
    STREAM1_CLOSE       = 1 << 1,
    STREAM2_VACANT      = 1 << 2,
    STREAM2_CLOSE       = 1 << 3,
    NEXT_IS_PIPE        = 1 << 4,
    CAME_FROM_PIPE      = 1 << 5,
    NEXT_IS_REDIRECT    = 1 << 6,
    CAME_FROM_REDIRECT  = 1 << 7
} stream_flags;


/*
 * keeps track of stream flags that determine where input and output is directed.
 */
typedef struct exec_ctx {
    int stream1[2];
    int stream2[2];
    int flags;
} exec_ctx;


/* functions */
/*
 * forks the process and attempts to executes the given program.
 * calls the builtin 'which' (builtin/which.c) to get the full program path.
 * returns 0 if succesfull, else 1.
 */
int valery_exec_program(char *program_name, char *argv[], int argc, struct env_t *env);

/* evalutes how the tokens should be executed */
int valery_eval_token(char *program_name, char *argv[], int argc, struct env_t *env, struct hist_t *hist);

/* parses the tokens and calls eval on them accordingly */
int valery_parse_tokens(struct tokenized_str_t *ts, struct env_t *env, struct hist_t *hist);

/*
 * how the three aformentioned functions relate to each other:
 *
 * [tokenized buffer] -> valery_parse_tokens() -> valery_eval_tokens() -> valery_exec_program();
 */

#endif
