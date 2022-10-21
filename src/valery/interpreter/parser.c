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
 *  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#include "valery/interpreter/parser.h"
#include "valery/interpreter/parser_utils.h"

/* spec */
//static void ast_program(void);
//static void ast_complete_commands(void);
//static void complete_commands(void);
//static void complete_command(void);
//static void list(void);
//static void and_or(void);
//static void pipeline(void);
//static void pipe_sequence(void);
//static void command(void);
//static void compound_command(void);
//static void subshell(void);
//static void compound_list(void);
//static void term(void);
//static void for_clause(void);
//static void name(void);
//static void in(void);
//static void wordlist(void);
//static void case_clause(void);
//static void case_list_ns(void);
//static void case_list(void);
//static void case_item_ns(void);
//static void case_item(void);
//static void pattern(void);
//static void if_clause(void);
//static void else_part(void);
//static void while_clause(void);
//static void until_clause(void);
//static void function_definition(void);
//static void function_body(void);
//static void fname(void);
//static void brace_group(void);
//static void do_group(void);
//static void simple_command(void);
//static void cmd_name(void);
//static void cmd_word(void);
//static void cmd_prefix(void);
//static void cmd_suffix(void);
//static void redirect_list(void);
//static void io_redirect(void);
//static void io_file(void);
//static void filename(void);
//static void io_here(void);
//static void here_end(void);
//static void newline_list(void);
//static void linebreak(void);
//static void separator_op(void);
//static void separator(void);
//static void sequential_sep(void);

/* globals */
struct tokenlist_t *tokenlist;


struct ast_node_t *parse(struct tokenlist_t *tl)
{
    tokenlist = tl;
    return NULL;
}
