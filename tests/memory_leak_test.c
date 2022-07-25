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

#include <stdlib.h>
#include <string.h>

#include "../utils/lexer.h"
#include "../valery.h"
#include "../utils/load_config.h"
#include "../utils/histfile.h"
#include "../utils/exec.h"
#include "../utils/lexer.h"

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


int main()
{
    struct env_t *env = malloc_env();
    struct hist_t *hist;
    struct tokenized_str_t *ts;
    char hist_file_path[MAX_ENV_LEN];
    int rc;
    int rc_env;

    rc = parse_config(env);
    if (rc == 1) {
        fprintf(stderr, "error parsing .valeryrc");
        free_env(env);
        return 1;
    }

    /* establish a connection to the hist file */
    snprintf(hist_file_path, MAX_ENV_LEN, "%s/%s", env->HOME, HISTFILE_NAME);
    hist = malloc_history(hist_file_path);

    /* create tokenized_str_t object. Will reused same object every loop. */
    ts = tokenized_str_t_malloc();
    

    char input_buffer[COMMAND_LEN] = "ls -la && echo \"hello world || ignore\" && exit 0";

    save_command(hist, input_buffer);
    /* loop enters here means ordinary command was typed in */
    rc = tokenize(ts, input_buffer);
    if (rc == 0) {
        rc_env = valery_parse_tokens(ts, env, hist);
        env->exit_code = rc_env;
    }

    tokenized_str_t_clear(ts);
    /* free and write to file before exiting */
    write_commands_to_hist_file(hist);
    free_env(env);
    free_history(hist);
    tokenized_str_t_free(ts);
    return 0;
}
