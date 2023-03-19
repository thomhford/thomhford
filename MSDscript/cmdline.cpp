/**
* \file cmdline.cpp
* \brief Reads command line arguments and runs said commands
*
*
* This program runs at the command line and can be given program arguments. For example,

    ./msdscript --help

 This will run the program msdscript in the current directory with the argument --help.
 Arguments are passed to a program as the argc and argv arguments to main:

    int main(int argc, char* argv[]) {
    ....
    }

 The argc passed to main is always at least 1, because the first “argument” (at index 0) is the name of the program
 as it was run on the command line. If argc is 2, that means the program was given one argument, maybe as

    ./msdscript --help

 and then argv[1] is that argument, which is "--help" in this case.

 Possible commands:
    --help
    --test

 * \author Thomas Ford
 * \date 02-07-2023
*/

#define CATCH_CONFIG_RUNNER

#include "cmdline.h"
#include "catch.h"

#include <string>
#include <iostream>

run_mode_t use_arguments(int argc, char* argv[]){
    if (argc > 0){ // if no arguments, return
        for (int i = 1; i < argc; i++) { // first argument is program name, start at second argument
            if (std::strcmp(argv[i], "--help") == 0){
                std::cout << "\"Help will always be given at Hogwarts to those who ask for it.\"" << std::endl;
                std::cout << "Use \"--test\" to test" << std::endl;
                std::cout << "Use \"--interp\" to return value of entered expression" << std::endl;
                std::cout << "Use \"--print\" to print entered expression " << std::endl;
                std::cout << "Use \"--pretty-print\" to print expression with nice formatting" << std::endl;
                return do_nothing;
            }
            if (std::strcmp(argv[i], "--test") == 0){
                if (Catch::Session().run(1, argv) == EXIT_SUCCESS ){
                    std::cout << "Tests passed" << std::endl;
                    exit(EXIT_SUCCESS);
                }
                else{
                    std::cout << "Tests failed..." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            if (std::strcmp(argv[i], "--interp") == 0){
                return do_interp;
            }
            if (std::strcmp(argv[i], "--print") == 0){
                return do_print;
            }
            if (std::strcmp(argv[i], "--pretty-print") == 0){
                return do_pretty_print;
            }
            else{
                std::cerr << "This message is not allowed" << std::endl;
                std::cerr << "Enter \"--help\" for allowed quires" << std::endl;
                return do_nothing;
            }
        }
    }
    return do_nothing;
}