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

#include <stdint.h>

#include "../valery.h"

#ifndef LOAD_CONFIG
#define LOAD_CONFIG

/* functions */

int set_home_dir(struct env_t *env);

int get_config_path(struct env_t *env, char config_path[MAX_ENV_LEN]);

void unwrap_paths(struct env_t *env);

int parse_config(struct env_t *env);

#endif
