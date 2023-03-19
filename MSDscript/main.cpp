/**
* \mainpage MSDScript
* \author Thomas Ford
* \date 02-07-2023
*/

#include "cmdline.h"
#include "Expr.h"
#include "parse.h"
#include "Val.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (use_arguments(argc, argv) == do_nothing) {
            return 0;
        }
        if (use_arguments(argc, argv) == do_interp) {
            Expr *e = parse_expr(std::cin);
            std::cout << e->interp()->to_string() << "\n";
            exit(EXIT_SUCCESS);
        }

        if (use_arguments(argc, argv) == do_print) {
            Expr *e = parse_expr(std::cin);
            e->print(std::cout);
            std::cout << '\n';
            exit(EXIT_SUCCESS);
        }
        if (use_arguments(argc, argv) == do_pretty_print) {
            Expr *e = parse_expr(std::cin);
            std::cout << e->to_pretty_string() << '\n';
            exit(EXIT_SUCCESS);
        }
        return 0;
    }
    catch(std::runtime_error &exn){
        std::cerr << exn.what() << "\n";
        return EXIT_FAILURE;
    }
}