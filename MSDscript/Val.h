//
// Created by Thomas Ford on 3/3/23.
//

#ifndef MSDSCRIPT_COMMAND_LINE_VAL_H
#define MSDSCRIPT_COMMAND_LINE_VAL_H

#include <string>
class Expr;


class Val {
public:
    std::string to_string();
    virtual bool equals(Val *e) = 0;
    virtual Val* add_to(Val *e) = 0;
    virtual Val* mult_with(Val *e) = 0;
    virtual Expr* to_expr() = 0;
    virtual void print(std::ostream &ot) = 0;
    virtual bool is_true() = 0;

};

/**
 * Represents a Num value
 */
class NumVal : public Val{
public:
    int rep;
    explicit NumVal(int val);
    bool equals(Val *e) override;
    Val* add_to(Val *e) override;
    Val* mult_with(Val *e) override;
    Expr* to_expr() override;
    void print(std::ostream &ot) override;
    bool is_true() override;
};

/**
 * Represents a boolean value
 */
 class BoolVal : public Val{
 public:
    bool rep;
    explicit BoolVal(bool val);
    bool equals(Val *e) override;
    Val* add_to(Val *e) override;
    Val* mult_with(Val *e) override;
    Expr* to_expr() override;
    void print(std::ostream &ot) override;
    bool is_true() override;
 };


#endif //MSDSCRIPT_COMMAND_LINE_VAL_H
