//
// Created by Thomas Ford on 2/16/23.
//

#ifndef MSDSCRIPT_COMMAND_LINE_PARSE_H
#define MSDSCRIPT_COMMAND_LINE_PARSE_H

#include "Expr.h"

static void consume(std::istream &in, int expect);
static void skip_whitespace(std::istream &in);
Expr *parse_num(std::istream &in);
Expr *parse_var(std::istream &in);
Expr *parse_let(std::istream &in);
Expr *parse_if(std::istream &in);
std::string parse_keyword(std::istream &in);
Expr *parse_expr(std::istream &in);
Expr *parse_comparg(std::istream &in);
Expr *parse_addend(std::istream &in);
Expr *parse_multicand(std::istream &in);
Expr *parse_str(const std::string& s);


#endif //MSDSCRIPT_COMMAND_LINE_PARSE_H
