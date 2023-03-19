//
// Created by Thomas Ford on 2/23/23.
//

#include "test_msdscript.h"
#include <ctime>
#include <random>
int itr = 1000;

int main(int argc, char **argv) {
    const char * const interp_argv[] = { "msdscript", "--interp" };
    const char * const interp_argv1[] = { "msdscript7", "--interp" };
    const char * const print_argv[] = { "msdscript", "--print" };
    const char * const print_argv1[] = { "msdscript3", "--print" };
    const char * const pretty_print_argv[] = { "msdscript", "--pretty-print" };
    const char * const pretty_print_argv1[] = { "msdscript6", "--pretty-print" };
    for (int i = 0; i < itr; i++) {
        std::string in = random_expr_string();
        //std::cout << "Trying "<< in << "\n";
        ExecResult interp_result = exec_program(2, interp_argv, in);
        ExecResult interp_result1 = exec_program(2, interp_argv1, in);
//        ExecResult print_result = exec_program(2, print_argv, in);
//        ExecResult print_result1 = exec_program(2, print_argv1, in);
//        ExecResult pretty_print_result = exec_program(2, pretty_print_argv, in);
//        ExecResult pretty_print_result1 = exec_program(2, pretty_print_argv1, in);

//        if (interp_result.out != interp_result1.out) {
//            std::cout << "Interp: " << interp_result.out << "!=" << interp_result1.out << "\n";
//            throw std::runtime_error("different result for interp");
//        }
//        if (print_result.out != print_result1.out) {
//            std::cout << "Print: " << print_result.out << "!=" << print_result1.out << "\n";
//            throw std::runtime_error("different result for printed");
//        }
//        if (pretty_print_result.out != pretty_print_result1.out) {
//            std::cout << "Pretty-Print: " << pretty_print_result.out << "!=" << pretty_print_result1.out << "\n";
//            throw std::runtime_error("different result for pretty-printed");
//        }
    }
    return 0;
}

std::string random_expr_string() {
    std:: minstd_rand simple_rand;
    simple_rand.seed(clock());
    if ((simple_rand() % 100) < 40)
        return std::to_string(simple_rand());
    else if ((simple_rand() % 100) < 50 && (simple_rand() % 100) >= 40 ){
        return random_var_string();
    }
    else if ((simple_rand() % 100) < 70 && (simple_rand() % 100) >= 50)
        return random_expr_string() + "+" + random_expr_string();
    else if ((simple_rand() % 100) < 90 && (simple_rand() % 100) >= 70)
        return random_expr_string() + "*" + random_expr_string();
    else if ((simple_rand() % 100) < 95 && (simple_rand() % 100) >= 90)
        return "(" + random_expr_string() + ")";
    else
        return "_let " + random_var_string() + " = " + random_expr_string() + " _in " + random_var_string() + " + " + random_expr_string();
}

std::string random_var_string(){
    std:: minstd_rand simple_rand;
    simple_rand.seed(clock());
    if ((simple_rand() % 10) <= 5)
        return "x";
    else
        return "y";
}
std::string random_string(){
    std:: minstd_rand simple_rand;
    simple_rand.seed(clock());
    if ((simple_rand() % 10) <= 5)
        return std::to_string(simple_rand());
    else
        return "-" + std::to_string(simple_rand());
}