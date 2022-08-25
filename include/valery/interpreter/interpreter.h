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

#ifndef VALERY_INTERPRETER_H
#define VALERY_INTERPRETER_H

#include "valery/interpreter/parser.h"


/* functions */

void *visit_binary(struct binary_t *expr);

void *visit_grouping(struct grouping_t *expr);

void *visit_literal(struct literal_t *expr);

void *visit_unary(struct unary_t *expr);

#endif /* VALERY_INTERPRETER_H */
