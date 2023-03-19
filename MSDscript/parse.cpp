//
// Created by Thomas Ford on 2/16/23.
//

#include "parse.h"


/**
 * Base parser. Will call additional parsers if needed including parse_addend. Handles expressions up to AddExpr
 * @param in where is being read from
 * @return an expression representing what was parsed
 */
Expr *parse_expr(std::istream &in) {
    Expr *e = parse_comparg(in);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '=') {
        consume(in, '=');
        c = in.peek();
        if (c != '='){
            throw std::runtime_error("invalid input");
        }
        consume(in, '=');
        Expr *rhs = parse_expr(in);
        return new EqExpr(e, rhs);
    } else {
        return e;
    }
}
/**
 * Parser called by parse_expr. Handles AddExpr and can call parse_addend if needed
 * @param in where is being read from
 * @return an expression representing what was parsed
 */
Expr *parse_comparg(std::istream &in){
    Expr *e = parse_addend(in);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '+') {
        consume(in, '+');
        Expr *rhs = parse_comparg(in);
        return new AddExpr(e, rhs);
    } else {
        return e;
    }
}
/**
 * Parser called by parse_comparg. Handles MultExpr and can call parse_multicand if needed
 * @param in where is being read from
 * @return an expression representing what was parsed
 */
Expr *parse_addend(std::istream &in) {
    Expr *e = parse_multicand(in);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '*') {
        consume(in, '*');
        Expr *rhs = parse_addend(in);
        return new MultExpr(e, rhs);
    } else {
        return e;
    }
}
/**
 * Parser called by parse_addend. Handles everything not handles by previous parsers including parenthesis and LetExpr
 * @param in where is being read from
 * @return an expression representing what was parsed
 */
Expr *parse_multicand(std::istream &in) {
    skip_whitespace(in);
    int c = in.peek();
    if ((c == '-') || isdigit(c))
        return parse_num(in);
    else if (isalpha(c)){
        return parse_var(in);
    }
    else if (c == '_'){
        std::string kw = parse_keyword(in);
        if (kw == "let") {
            return parse_let(in);
        }
        else if (kw == "true"){
            return new BoolExpr(true);
        }
        else if (kw == "false"){
            return new BoolExpr(false);
        }
        else if (kw == "if"){
            return parse_if(in);
        }
        else{
            throw std::runtime_error("invalid input");
        }
    }
    else if (c == '(') {
        consume(in, '(');
        Expr *e = parse_expr(in);
        skip_whitespace(in);
        c = in.get();
        if (c != ')')
            throw std::runtime_error("missing close parenthesis");
        return e;
    }
    else {
        consume(in, c);
        throw std::runtime_error("invalid input");
    }
}

/**
 * Parses NumExpr
 * @param in where is being read from
 * @return a NumExpr
 */
Expr *parse_num(std::istream &in) {
    int n = 0, count = 0;
    bool negative = false;
    if (in.peek() == '-') {
        negative = true;
        consume(in, '-');
        ++count;
    }
    while (true) {
        int c = in.peek();
        if (isdigit(c)) {
            consume(in, c);
            n = n*10 + (c - '0');
            ++count;
        } else
            break; }
    if (negative)
        n = -n;
    if (negative & (count == 1)){
        throw std::runtime_error("invalid input");
    }
    return new NumExpr(n);
}
/**
 * Parses VarExpr
 * @param in where is being read from
 * @return a VarExpr
 */
Expr *parse_var(std::istream &in) {
    std::string var;
    while (true) {
        int c = in.peek();
        if (isalpha(c)) {
            consume(in, c);
            var.append(std::string(1,char(c)));
        }
        else if (c == '_'){
            throw std::runtime_error("invalid input");
        }
        else
            break; }
    return new VarExpr(var);
}

/**
 * Parses LetExpr
 * @param in where is being read from
 * @return a LetExpr
 */
Expr *parse_let(std::istream &in) {
    std::string lhs;
    Expr *rhs;
    Expr *body;
    skip_whitespace(in);
    int var = in.peek();
    if (!isalpha(var)){
        throw std::runtime_error("invalid input");
    }
    while (isalpha(var)) {
        consume(in, var);
        lhs.append(std::string(1,char(var)));
        var = in.peek();
    }
    skip_whitespace(in);
    int equal = in.peek();
    if (equal != '='){
        throw std::runtime_error("invalid input");
    }
    consume(in, equal);
    rhs = parse_expr(in);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '_'){
        std::string kw = parse_keyword(in);
        if (kw != "in"){
            throw std::runtime_error("invalid input");
        }
    }
    else{
        throw std::runtime_error("invalid input");
    }
    skip_whitespace(in);
    body = parse_expr(in);

    return new LetExpr(lhs, rhs, body);
}
/**
 * Removes characters from stream to allow for continuation along string
 * @param in what is being consumed
 * @param expect
 */
static void consume(std::istream &in, int expect) {
    int c = in.get();
    if (c != expect) {
        throw std::runtime_error("consume mismatch");
    }
}
/**
 * Skips whitespaces (spaces or tabs) in inputted strings
 * @param in where is being read from
 */
static void skip_whitespace(std::istream &in) {
    while (true) {
        int c = in.peek();
        if (!isspace(c))
            break;
        consume(in, c);
    }
}
/**
 * used to read string representations of expressions
 * @param s what is being read
 * @return the expressions represented by the inputted string
 */
Expr *parse_str(const std::string& s){
    std::istringstream str(s);
    return parse_expr(str);
}
/**
 * A helper method to read string values to check against required values
 * @param in Where reading from
 * @return a string representing what was read
 */
std::string parse_keyword(std::istream &in){
    std::string keyword;
    consume(in, '_');
    int c = in.peek();
    while (isalpha(c)){
        consume(in, c);
        keyword.append(std::string(1,char(c)));
        c = in.peek();
    }
    return keyword;
}
/**
 * Parses IfExpr
 * @param in where is being read from
 * @return an IfExpr
 */
Expr *parse_if(std::istream &in){
    Expr *_if;
    Expr *_then;
    Expr *_else;
    std::string kw;
    skip_whitespace(in);
    _if = parse_expr(in);
    skip_whitespace(in);
    int c = in.peek();
    if (c == '_'){
        kw = parse_keyword(in);
        if (kw == "then"){
            _then = parse_expr(in);
        }
        else{
            throw std::runtime_error("invalid input");
        }
    } else{
        throw std::runtime_error("invalid input");
    }
    skip_whitespace(in);
    c = in.peek();
    if (c == '_'){
        kw = parse_keyword(in);
        if (kw == "else"){
            _else = parse_expr(in);
        }
        else{
            throw std::runtime_error("invalid input");
        }
    } else{
        throw std::runtime_error("invalid input");
    }
    return new IfExpr(_if, _then, _else);
}