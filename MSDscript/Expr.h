/**
 * \file Expr.h
 * \brief Expression class
 *
 * Where class methods are declared for all Expression classes
 *
 * \author Thomas Ford
 * \author Thomas Ford
 * */

#ifndef EXPRESSIONCLASSES_EXPR_H
#define EXPRESSIONCLASSES_EXPR_H

#include <string>
#include <ostream>
#include <sstream>

class Val;

typedef enum {
    prec_none,      // = 0
    prec_add,       // = 1
    prec_mult       // = 2
} precedence_t;

/**
 * Parent class for all represented Expression classes
 */
class Expr {
public:
    virtual bool equals(Expr *e) = 0;
    virtual Val * interp() = 0;
    virtual bool has_variable() = 0; //should return true if the expression is a variable or contains a variable, false otherwise.
    virtual Expr* subst(std::string, Expr*) = 0;
    virtual void print(std::ostream &ot) = 0;
    std::string to_string();
    virtual void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streampos) = 0;
    void pretty_print(std::ostream &ot);
    std::string to_pretty_string();
};

/**
 * Represents number values as an expression
 */
class NumExpr : public Expr {
public:
    int val;
    explicit NumExpr(int val);
    bool equals( Expr *e ) override;
    Val * interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streampos) override;
};

/**
 * Represents addition methods as an expression
 */
class AddExpr : public Expr {
public:
    Expr *lhs;
    Expr *rhs;

    AddExpr(Expr *lhs, Expr *rhs);
    bool equals( Expr *e ) override;
    Val * interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streampos) override;
};

/**
 * Represents multiplication methods as an expression
 */
class MultExpr : public Expr {
public:
    Expr *lhs;
    Expr *rhs;

    MultExpr(Expr *lhs, Expr *rhs);
    bool equals( Expr *e ) override;
    Val * interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streampos) override;
};

/**
 * Represents a variable as an expression
 */
class VarExpr : public Expr {
public:
    std::string val;

    explicit VarExpr(std::string val);
    bool equals( Expr *e ) override;
    Val * interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streampos) override;
};

/**
 * Represents a let expression
 */
class LetExpr : public Expr {
public:
    std::string lhs;
    Expr *rhs;
    Expr *body;

    LetExpr(std::string lhs, Expr *rhs, Expr *body);
    bool equals( Expr *e ) override;
    Val * interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streamPos) override;
};

class BoolExpr : public Expr{
public:
    bool val;

    explicit BoolExpr(bool val);
    bool equals( Expr *e ) override;
    Val* interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streamPos) override;
};
class IfExpr : public Expr{
public:
    Expr *_if;
    Expr *_then;
    Expr *_else;

    IfExpr(Expr *_if, Expr *_then, Expr *_else);
    bool equals( Expr *e ) override;
    Val * interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streamPos) override;

};
class EqExpr : public Expr{
public:
    Expr *lhs;
    Expr *rhs;

    EqExpr(Expr *lhs, Expr *rhs);
    bool equals( Expr *e ) override;
    Val * interp() override;
    bool has_variable() override;
    Expr* subst(std::string, Expr*) override;
    void print(std::ostream &ot) override;
    void pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos& streamPos) override;
};

#endif //EXPRESSIONCLASSES_EXPR_H
