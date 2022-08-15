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

#ifndef LOAD_CONFIG
#define LOAD_CONFIG

#include <stdint.h>

#include "valery/env.h"

/* functions */

int parse_config(struct env_vars_t *env_vars, struct paths_t *p);

/*
 * copies the config path into the result argument if it is found.
 * returns 0 if config was found, else 1.
 */
int get_config_path(char result[MAX_ENV_LEN], char *HOME);

void unwrap_paths(struct paths_t *p, char *PATHS);


#endif
