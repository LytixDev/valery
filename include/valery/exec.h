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

#include "valery/lexer.h"
#include "valery/histfile.h"
#include "valery/env.h"

#define CHILD_PID 0
#define READ_END 0
#define WRITE_END 1

/* types */
/*
 * adam and seth streams:
 * the adam stream will always be the first stream utilized for pipes.
 * if the pipes are chained, the adam stream will be piped into the seth stream.
 * if pipe is chained once more, the adam stream will be reused, and so on.
 * example:
 * 'A | B | C | D'
 * 1. adam stream is piped.
 * 2. program A writes to adam.
 * 3. seth stream is piped.
 * 4. program B reads from adam and writes to seth. 
 * 5. adam is closed.
 * 6. adam is piped (adam has now been recycled).
 * 7. program C reads from seth and writes to adam.
 * 8. seth is closed.
 * 9. program D reads from adam and writes to stdout.
 */
typedef enum stream_t {
    ST_NONE = -1,
    ST_ADAM = 0,
    ST_SETH = 1
} stream_t;


typedef enum stream_flags {
    SF_ADAM_VACANT         = 1 << 0,
    SF_ADAM_CLOSE          = 1 << 1,
    SF_SETH_VACANT         = 1 << 2,
    SF_SETH_CLOSE          = 1 << 3
    //NEXT_IS_PIPE        = 1 << 4,
    //CAME_FROM_PIPE      = 1 << 5,
    //NEXT_IS_REDIRECT    = 1 << 6,
    //CAME_FROM_REDIRECT  = 1 << 7
} stream_flags;


/*
 * keeps track of streams and that determine where program reads and writes input and output.
 * keeps track of flags that determine what streams are active and will be active, and will close.
 */
typedef struct exec_ctx {
    int streams[2][2];
    int flags;
    stream_t read_stream;
    stream_t write_stream;
} exec_ctx;


/* functions */
/*
 * forks the process and attempts to executes the given program.
 * calls the builtin 'which' (builtin/which.c) to get the full program path.
 * returns 0 if succesfull, else 1.
 */
int valery_exec_program(char *program_name, char *argv[], int argc, struct env_t *env, struct exec_ctx *e_ctx);

/*
 * if program_name is builtin, executes matching builtin, and returns true.
 * else returns false.
 */
bool valery_eval_token(char *program_name, char *argv[], int argc, struct env_t *env, struct hist_t *hist);

/* parses the tokens and calls eval on them accordingly */
int valery_parse_tokens(struct tokenized_str_t *ts, struct env_t *env, struct hist_t *hist);

int str_to_argv(char *str, char **argv, int *argv_cap);

void new_pipe(struct exec_ctx *e_ctx);

void terminate_pipe(struct exec_ctx *e_ctx);

void update_exec_flags(struct exec_ctx *e_ctx, operands_t type, operands_t next_type);

/*
 * how the three aformentioned functions relate to each other:
 *
 * [tokenized buffer] -> valery_parse_tokens() -> valery_eval_tokens() -> valery_exec_program();
 */

#endif
