//
// Created by Thomas Ford on 3/3/23.
//

#include "Val.h"
#include "expr.h"

std::string Val::to_string() {
    std::stringstream st("");
    this->print(st);
    return st.str();
}

/////////////////////////////////////////////--NumVal--//////////////////////////////////////////////////////////////
// Constructor for number value
NumVal::NumVal(int val) : rep(val){}

/**
 * Compares two values
 * - We need to cast the checked Val to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Val are equal
 */
bool NumVal::equals(Val *e) {
    auto *numVal = dynamic_cast<NumVal*>(e);
    if (numVal == nullptr){
        return false;
    }
    else {
        return (numVal->rep == this->rep);
    }
}

/**
 * Needed by AddExpr to add values. Must ensure that values being added are NumVal. CANNOT ADD OTHER VALUE TYPES
 * @param e what is being added
 * @return sum of added values
 */
Val *NumVal::add_to(Val *e) {
    auto *other_num = dynamic_cast<NumVal*>(e);
    if (other_num == nullptr) throw std::runtime_error("add of non-number");
    return new NumVal(this->rep + other_num->rep);
}

/**
 * Needed by MultExpr to multiply values. Must ensure that values being multiplied are NumVal. CANNOT MULTIPLY OTHER VALUE TYPES
 * @param e what is being multiplied
 * @return product of multiplied values
 */
Val *NumVal::mult_with(Val *e) {
    auto *other_num = dynamic_cast<NumVal*>(e);
    if (other_num == nullptr) throw std::runtime_error("mult of non-number");
    return new NumVal(this->rep * other_num->rep);
}

/**
 * Converts values to Expr
 * @return an Expr representing converted value
 */
Expr *NumVal::to_expr(){
    return new NumExpr(this->rep);
}

/**
 * Used by to string to convert value to string
 * @param ot where string will be written
 */
void NumVal::print(std::ostream &ot) {
    ot << std::to_string(rep);
}
/**
 * Not a boolean
 * @return none
 */
bool NumVal::is_true() {
    throw std::runtime_error("Not a boolean");
}

/////////////////////////////////////////////--BoolVal--//////////////////////////////////////////////////////////////
// Constructor for boolean value
BoolVal::BoolVal(bool val) : rep(val){}
/**
 * Compares two values
 * - We need to cast the checked Val to determine if they type is correct. If the type is incorrect, return false
 * - If type comparison is matching, return if the value stored is the same
 * @param e what we are comparing
 * @return if Val are equal
 */
bool BoolVal::equals(Val *e) {
    auto *boolVal = dynamic_cast<BoolVal*>(e);
    if (boolVal == nullptr){
        return false;
    }
    else {
        return (boolVal->rep == this->rep);
    }
}
/**
 * Cannot add booleans
 * @return none
 */
Val *BoolVal::add_to(Val *e) {
    throw std::runtime_error("add of non-number");
}
/**
 * Cannot mult booleans
 * @return none
 */
Val *BoolVal::mult_with(Val *e) {
    throw std::runtime_error("mult of non-number");
}

/**
 * Converts values to Expr
 * @return an Expr representing converted value
 */
Expr *BoolVal::to_expr() {
    return new BoolExpr(this->rep);
}
/**
 * Used by to string to convert value to string
 * @param ot where string will be written
 */
void BoolVal::print(std::ostream &ot) {
    if (this->rep){
        ot << "_true";
    }
    else
        ot << "_false";
}

/**
 * Helper method to pull value of boolean
 * @return value of boolean
 */
bool BoolVal::is_true() {
    return this->rep;
}


