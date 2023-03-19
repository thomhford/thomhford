/**
 * \file Expr.cpp
 * \brief Expression class
 *
 * Where class methods are defined for all Expression classes
 *
 * \author Thomas Ford
 * \date 02-07-2023
 * */

#include "Expr.h"
#include <utility>
#include <stdexcept>

#include "Val.h"

/**
 * Expr Class to_string()
 *
 * Used by all inheriting classes to pull string values from stringstream
 * for use in comparison, mainly for testing
 * Creates strings from the output of the print() method
 * @return a string value of calling expression
 */
std::string Expr::to_string() {
    std::stringstream st("");
    this->print(st);
    return st.str();
}

/**
 * Like print, pretty_print takes a std::ostream& argument and prints there.
 * Unlike print, pretty_print includes a space around + or *, and it avoids unnecessary parentheses
 * by relying the usual precedence rules for multiplication and addition. It can also avoid parentheses
 * by assuming that operators associate to the right.
 * -Calls pretty_print_at() where specific printing for methods takes place
 * letParenth determines whether let expressions will contain parenthesis. Initially set to false
 * @param ot where pretty_print is printing to
 */
void Expr::pretty_print(std::ostream &ot) {
    std::streampos sp = 0;
    bool letParenth = false;
    this->pretty_print_at(ot, prec_none, letParenth, sp);
}

/**
 * Expr Class to_pretty_string()
 *
 * Used by all inheriting classes to pull string values from stringstream
 * for use in comparison, mainly for testing
 * Creates strings from the output of pretty_print method
 * @return a string value of calling expression
 */
std::string Expr::to_pretty_string() {
    std::stringstream st("");
    this->pretty_print(st);
    return st.str();
}
/////////////////////////////////////////////--NumExpr--//////////////////////////////////////////////////////////////
// Constructor for number expression
NumExpr::NumExpr(int val) : val(val){}
/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool NumExpr::equals(Expr *e) {
    auto *num = dynamic_cast<NumExpr*>(e);
    if (num == nullptr){
        return false;
    }
    else {
        return (num->val == this->val);
    }
}
/**
 * The value of a number is the number
 * @return an int for the value of the expression
 */
Val * NumExpr::interp() {
    return new NumVal(this->val);
}
/**
 * Num is not a variable so return false
 * @return true if the expression is a variable or contains a variable, false otherwise.
 */
bool NumExpr::has_variable() {
    return false;
}
/**
 * Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
 * matching the string, the result Expr* should have the given replacement, instead.
 * @param string variable to match
 * @param e Expression to replace variable with
 * @return new expression consisting of swapped variables if variables match string
 */
Expr *NumExpr::subst(std::string, Expr *) {
    return new NumExpr(this->val);
}

/**
 * Converts NumExpr to its string value and adds this value to an ostream for printing.
 * @param ot an ostream where expressions and supporting characters are are stored for printing.
 */
void NumExpr::print(std::ostream &ot) {
    ot<<std::to_string(val);
}

/**
 * Prints the string value of NumExpr to std::ostream
 * @param ot where we will print to
 * @param prec determines the level of formatting for pretty_print() used. Is never used for NumExpr
 * @param letParenth used by let expressions
 * @param streampos used by let expressions
 */
void NumExpr::pretty_print_at(std::ostream &ot, precedence_t prec, bool letParenth, std::streampos& streampos) {
    ot<<std::to_string(val);
}

/////////////////////////////////////////////--VarExpr--//////////////////////////////////////////////////////////////
// Variable Expression Constructor
VarExpr::VarExpr(std::string val) {
    this->val = std::move(val);
}
/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool VarExpr::equals(Expr *e) {
    auto *variable = dynamic_cast<VarExpr*>(e);
    if (variable == nullptr){
        return false;
    }
    else{
        return (variable->val == this->val);
    }
}
/**
 * A variable has no value, so interp for a variable should throw a std::runtime_error exception
 * @return an int for the value of the expression
 */
Val * VarExpr::interp() {
    throw std::runtime_error("variable expression has no value");
}
/**
 * Var is a variable so always return true
 * @return true if the expression is a variable or contains a variable, false otherwise.
 */
bool VarExpr::has_variable() {
    return true;
}
/**
 * Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
 * matching the string, the result Expr* should have the given replacement, instead.
 * @param string variable to match
 * @param e Expression to replace variable with
 * @return new expression consisting of swapped variables if variables match string
 */
Expr *VarExpr::subst(std::string variable, Expr * e) {
    if (this->val == variable){
        return e;
    }
    return new VarExpr(this->val);
}
/**
 * Adds the VarExpr value to an ostream for printing.
 * @param ot an ostream where expressions and supporting characters are are stored for printing.
 */
void VarExpr::print(std::ostream &ot) {
    ot<<val;
}

/**
 * Prints the value of VarExpr to std::ostream
 * @param ot where we will print to
 * @param prec determines the level of formatting for pretty_print() used. Is never used for VarExpr
 * @param letParenth used by let expressions
 * @param streampos used by let expressions
 */
void VarExpr::pretty_print_at(std::ostream &ot, precedence_t prec, bool letParenth, std::streampos& streampos) {
    ot<<val;
}

/////////////////////////////////////////////--AddExpr--//////////////////////////////////////////////////////////////
// Addition Expression Constructor
AddExpr::AddExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}
/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool AddExpr::equals(Expr *e) {
    auto *add = dynamic_cast<AddExpr *>(e);
    if (add == nullptr) {
        return false;
    } else
        return (this->lhs->equals(add->lhs) && this->rhs->equals( add->rhs));
}
/**
 * The value of an addition expression is the sum of the subexpression values
 * @return an int for the value of the expression
 */
Val * AddExpr::interp() {
    return this->lhs->interp()->add_to(this->rhs->interp());
}
/**
 * Need to call has variable on parts to determine if Expression contains var
 * @return true if the expression is a variable or contains a variable, false otherwise.
 */
bool AddExpr::has_variable() {
    return this->lhs->has_variable() || this->rhs->has_variable();
}
/**
 * Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
 * matching the string, the result Expr* should have the given replacement, instead.
 * @param string variable to match
 * @param e Expression to replace variable with
 * @return new expression consisting of swapped variables if variables match string
 */
Expr *AddExpr::subst(std::string string, Expr * e) {
    return new AddExpr(this->lhs->subst(string, e), this->rhs->subst(string, e));
}
/**
 * Prints addition expressions with an infix +. Every addition expression have parentheses around it,
 * with no extra space — not even a terminating newline.
 * @param ot an ostream where expressions and supporting characters are are stored for printing.
 */
void AddExpr::print(std::ostream &ot) {
    ot << "(";
    this->lhs->print(ot);
    ot << "+" ;
    this->rhs->print(ot);
    ot << ")";
}

/**
 * Prints the value of an AddExpr to std::ostream
 * @param ot where we will print to
 * @param prec determines the level of formatting for pretty_print() used.
 * @param letParenth used by let expressions. Will always be true on lhs
 * @param streampos used by let expressions
 * Precedence always starts at no precedence when pretty_print is initially called and is updated
 * by each recursive call. If precedence is at prec_add or above, special formatting for AddExpr is
 * used for "Pretty Printing" of the expression
*/
void AddExpr::pretty_print_at(std::ostream &ot, precedence_t prec, bool letParenth, std::streampos& streampos) {
    if (prec > prec_none) {
        ot << "(";
    }
    this->lhs->pretty_print_at(ot, prec_add, true, streampos);
    ot << " + ";
    this->rhs->pretty_print_at(ot, prec_none, letParenth, streampos);
    if (prec > prec_none) {
        ot << ")";
    }
}

/////////////////////////////////////////////--MultExpr--//////////////////////////////////////////////////////////////
// Multiplication Expression Constructor
MultExpr::MultExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}
/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool MultExpr::equals(Expr *e) {
    auto *mult = dynamic_cast<MultExpr*>(e);
    if (mult == nullptr){
        return false;
    }
    else {
        return (this->lhs ->equals(mult->lhs) && this->rhs ->equals( mult->rhs));
    }
}
/**
 * The value of a multiplication expression is the product of the subexpression values
 * @return an int for the value of the expression
 */
Val * MultExpr::interp() {
    return this->lhs->interp()->mult_with(this->rhs->interp());
}
/**
 * Need to call has_variable on parts to determine if Expression contains var
 * @return true if the expression is a variable or contains a variable, false otherwise.
 */
bool MultExpr::has_variable() {
    return this->lhs->has_variable() || this->rhs->has_variable();
}

/**
 * Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
 * matching the string, the result Expr* should have the given replacement, instead.
 * @param string variable to match
 * @param e Expression to replace variable with
 * @return new expression consisting of swapped variables if variables match string
 */
Expr *MultExpr::subst(std::string string, Expr * e) {
    return new MultExpr(this->lhs->subst(string, e), this->rhs->subst(string, e));
}

/**
 * Prints multiplication expressions with an infix *. Every multiplication expression have parentheses around it,
 * with no extra space — not even a terminating newline.
 * @param ot an ostream where expressions and supporting characters are are stored for printing.
 */
void MultExpr::print(std::ostream &ot) {
    ot << "(";
    this->lhs->print(ot);
    ot << "*" ;
    this->rhs->print(ot);
    ot << ")";
}

/**
 * Prints the value of an MultExpr to std::ostream
 * @param ot where we will print to
 * @param prec determines the level of formatting for pretty_print() used.
 * @param letParenth used by let expressions. Will always be true on lhs
 * @param streampos used by let expressions
 * Precedence always starts at no precedence when pretty_print is initially called and is updated
 * by each recursive call. If precedence is at prec_mult, special formatting for MultExpr is
 * used for "Pretty Printing" of the expression
*/
void MultExpr::pretty_print_at(std::ostream &ot, precedence_t prec, bool letParenth, std::streampos& streampos) {
    if (prec == prec_mult) {
        ot << "(";
    }
    this->lhs->pretty_print_at(ot, prec_mult, true, streampos);
    ot << " * ";
    this->rhs->pretty_print_at(ot, prec_add, letParenth, streampos);
    if (prec == prec_mult) {
        ot << ")";
    }
}

/////////////////////////////////////////////--LetExpr--//////////////////////////////////////////////////////////////
// Let Expression Constructor
LetExpr::LetExpr(std::string lhs, Expr *rhs, Expr *body) {
    this->lhs = std::move(lhs);
    this->rhs = rhs;
    this->body = body;
}
/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool LetExpr::equals(Expr *e) {
    auto *let = dynamic_cast<LetExpr*>(e);
    if (let == nullptr){
        return false;
    }
    else {
        return (this->lhs == let->lhs) && (this->rhs ->equals(let->rhs) && (this->body ->equals( let->body)));
    }
}

/**
 * The value of a let expression requires substitution of included variable with its value and call of
 * interp() on the body of the let expression to determine value of whole expression
 * @return an int for the value of the expression
 */
Val * LetExpr::interp() {
    Val * rhsInterp = rhs->interp();
    return body->subst(lhs, rhsInterp->to_expr())->interp();
}

/**
 * Need to call has_variable on rhs and body to determine if Expression contains a variable
 * @return true if the expression is a variable or contains a variable, false otherwise.
 */
bool LetExpr::has_variable() {
    return this->rhs->has_variable() || this->body->has_variable();
}

/**
 * Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
 * matching the string, the result Expr* should have the given replacement, instead. -ALWAYS SUBST RHS ALWAYS- Subst
 * in the body if lhs doest equal variable.
 * @param string variable to match
 * @param e Expression to replace variable with
 * @return new expression consisting of swapped variables if variables match string
 */
Expr *LetExpr::subst(std::string variable, Expr *replacement) {
    // ALWAYS SUBST RHS
    Expr *rhsSubst = rhs->subst(variable,replacement);
    if (this->lhs == variable){
        return new LetExpr(lhs, rhsSubst, body); // ALWAYS SUBST RHS
    }
    else {
        return new LetExpr(lhs, rhsSubst, body->subst(variable, replacement)); // ALWAYS SUBST RHS
    }
}

/**
 * Prints multiplication expressions with included "_let" for where substitution will take place and "_in" for where
 * value will be used. Every let expression have parentheses around it.
 * @param ot an ostream where expressions and supporting characters are are stored for printing.
 */
void LetExpr::print(std::ostream &ot) {
    ot << "(";
    ot << "_let " << this->lhs << "=";
    this->rhs->print(ot);
    ot << " _in ";
    this->body->print(ot);
    ot << ")";
}

/**
 * Prints the value of an LetExpr to std::ostream
 * @param ot where we will print to
 * @param prec determines the level of formatting for pretty_print() used.
 * @param letParenth determines if parenthesis will be used in LetExpr.
 * @param streamPos determines where and what has been read for print offsetting.
 * Precedence always starts at no precedence when pretty_print is initially called and is updated
 * by each recursive call. If precedence is at prec_mult, special formatting for MultExpr is
 * used for "Pretty Printing" of the expression
*/
void LetExpr::pretty_print_at(std::ostream &ot, precedence_t prec, bool letParenth, std::streampos &streamPos) {
    if (letParenth){
        ot << "(";
    }
    int numSpaces = int(ot.tellp() - streamPos); // distance from last new line to current position
    ot << "_let " << this->lhs << " = ";
    // pass precedence through, let children will not have parenthesis in the rhs
    this->rhs->pretty_print_at(ot, prec_none, false, streamPos);
    ot << '\n';
    streamPos = ot.tellp(); // set new line position
//    ot << std::string(numSpaces, ' '); // add spaces equaling offset of _in and new line
    if ( ot.tellp() > 0 ) {
        for (int i = 0; i < numSpaces; i++)
            ot << " ";
    }
    ot << "_in  ";
    // set precedent to none, let children may not have parenthesis in the body
    this->body->pretty_print_at(ot, prec_none, false, streamPos);

    if (letParenth){
        ot << ")";
    }
}

/////////////////////////////////////////////--BoolExpr--//////////////////////////////////////////////////////////////
// Bool Expression Constructor
BoolExpr::BoolExpr(bool val) : val(val){}

/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool BoolExpr::equals(Expr *e) {
    auto *boolExpr = dynamic_cast<BoolExpr*>(e);
    if (boolExpr == nullptr){
        return false;
    }
    else {
        return (boolExpr->val == this->val);
    }
}

/**
* The value of a let expression requires substitution of included variable with its value and call of
* interp() on the body of the let expression to determine value of whole expression
* @return a Val for the value of the expression
*/
Val* BoolExpr::interp() {
    return new BoolVal(this->val);
}

bool BoolExpr::has_variable() {
    return false;
}

/**
 * Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
 * matching the string, the result Expr* should have the given replacement, instead. -ALWAYS SUBST RHS ALWAYS- Subst
 * in the body if lhs doest equal variable.
 * @param string variable to match
 * @param e Expression to replace variable with
 * @return new expression consisting of swapped variables if variables match string
 */
Expr* BoolExpr::subst(std::string, Expr *) {
    return new BoolExpr(this->val);
}

void BoolExpr::print(std::ostream &ot) {
    if (this->val){
        ot<<"_true";
    }
    else
        ot<<"_false";
}

void BoolExpr::pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos &streamPos) {
    if (this->val){
        ot<<"_true";
    }
    else
        ot<<"_false";
}

/////////////////////////////////////////////--IfExpr--//////////////////////////////////////////////////////////////
// If - Then - Else Expression Constructor
IfExpr::IfExpr(Expr *_if, Expr *_then, Expr *_else) {
    this->_if = _if;
    this->_then = _then;
    this->_else = _else;
}

/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool IfExpr::equals(Expr *e) {
    auto *pIfExpr = dynamic_cast<IfExpr*>(e);
    if (pIfExpr == nullptr){
        return false;
    }
    else {
        return this->_if ->equals(pIfExpr->_if) && this->_then ->equals(pIfExpr->_then)
                                               && this->_else ->equals(pIfExpr->_else);
    }
}

/**
 * The value of a let expression requires substitution of included variable with its value and call of
 * interp() on the body of the let expression to determine value of whole expression
 * @return a val for the value of the expression
 */
Val *IfExpr::interp() {
    if (_if->interp()->is_true())
        return _then->interp();
    else
        return _else->interp();
}

bool IfExpr::has_variable() {// check for if
    return this->_if->has_variable() || this->_then->has_variable() || this->_else->has_variable();
}

/**
 * Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
 * matching the string, the result Expr* should have the given replacement, instead. -ALWAYS SUBST RHS ALWAYS- Subst
 * in the body if lhs doest equal variable.
 * @param string variable to match
 * @param e Expression to replace variable with
 * @return new expression consisting of swapped variables if variables match string
 */
Expr *IfExpr::subst(std::string string, Expr *e) {
    return new IfExpr(this->_if->subst(string, e),this->_then->subst(string, e),
                      this->_else->subst(string, e));
}

void IfExpr::print(std::ostream &ot) {
    ot << "(_if " << this->_if->to_string() << " _then " << this->_then->to_string()
    << " _else " << this->_else->to_string() << ")";
}

void IfExpr::pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos &streamPos) {

}

/////////////////////////////////////////////--EqExpr--//////////////////////////////////////////////////////////////
// Equals Expression Constructor
EqExpr::EqExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

/**
 * Compares two expressions
 * - We need to cast the checked Expr to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Expr are equal
 */
bool EqExpr::equals(Expr *e) {
    auto *pEqExpr = dynamic_cast<EqExpr*>(e);
    if (pEqExpr == nullptr){
        return false;
    }
    else {
        return this->lhs->equals(pEqExpr->lhs) && this->rhs->equals(pEqExpr->rhs);
    }
}

/**
 * The value of a let expression requires substitution of included variable with its value and call of
 * interp() on the body of the let expression to determine value of whole expression
 * @return a val for the value of the expression
 */
Val *EqExpr::interp() {
    return new BoolVal(this->lhs->interp()->equals(this->rhs->interp()));
}

bool EqExpr::has_variable() {
    return this->lhs->has_variable() || this->rhs->has_variable();
}

/**
* Takes a string and an Expr* and everywhere that the expression (whose subst method is called) contains a variable
        * matching the string, the result Expr* should have the given replacement, instead. -ALWAYS SUBST RHS ALWAYS- Subst
        * in the body if lhs doest equal variable.
* @param string variable to match
* @param e Expression to replace variable with
* @return new expression consisting of swapped variables if variables match string
*/
Expr *EqExpr::subst(std::string string, Expr *e) {
    return new EqExpr(this->lhs->subst(string, e), this->rhs->subst(string, e));
}

void EqExpr::print(std::ostream &ot) {
    ot << "(" << this->lhs->to_string() << "==" << this->rhs->to_string() << ")";
}

void EqExpr::pretty_print_at(std::ostream &ot, precedence_t, bool letParenth, std::streampos &streamPos) {

}
