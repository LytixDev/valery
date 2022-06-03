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

#include "valery.h"
#include "utils/load_config.h"
#include "utils/prompt.h"
#include "utils/lexer.h"
#include "utils/histfile.h"


static volatile int is_running = 1;


void catch_exit_signal(int signal)
{
    is_running = 0;
}

struct ENV *new_env()
{
    struct ENV *env = (ENV *) malloc(sizeof(ENV));
    env->exit_code = 0;
    env->PS1 = (char *) malloc(sizeof(char) * MAX_ENV_LEN);
    env->PATH = (char *) malloc(sizeof(char) * MAX_ENV_LEN);
    return env;
}

void free_env(struct ENV *env)
{
    if (env != NULL) {
        free(env->PS1);
        free(env->PATH);
        free(env);
    }
}

int main()
{
    struct ENV *env = new_env();
    struct HIST_FILE *hf = new_hist_file();
    struct HIST_FILE_WRITER *hfw = new_hist_file_writer();
    char *buf = "";
    char full_cmd[4096];
    char cmd[4096];
    char args[4096];

    /* trap ctrl+c */
    /* TODO: have ctrl+c only clear current prompt */
    signal(SIGINT, catch_exit_signal);

    int rc = parse_config(env);
    if (rc == 1) {
        fprintf(stderr, "error parsing .valeryrc");
        free_env(env);
        return 1;
    }

    open_hist_file(hf, "/home/nic/.valery_hist");

    /* main loop */
    while (strcmp(buf, "exit") != 0 && is_running != 0) {
        buf = prompt(env->PS1);
        split_buffer(buf, cmd, args);

        snprintf(full_cmd, 4096, "%s/%s %s", env->PATH, cmd, args);

        /* save command to memory. Write to hist file on max saved or on exit. */
        save_command(hfw, hf, full_cmd);

        rc = system(full_cmd);
        env->exit_code = rc;
    }

    /* free and write to file before exiting */
    free(buf);
    write_commands_to_hist_file(hf->fp, hfw->commands, hfw->total_commands);
    free_env(env);
    free_hist_file(hf);
    free_hist_file_writer(hfw);
    printf("Exiting ...\n");

    return 0;
}
