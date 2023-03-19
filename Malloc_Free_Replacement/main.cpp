//
// Created by Thomas Ford on 3/17/23.
//
#define CATCH_CONFIG_RUNNER
#include "catch.h"

int main( int argc, char* argv[] ) {
    if (argc > 0){
        if (std::strcmp(argv[1], "--test") == 0){
            if (Catch::Session().run(1, argv) == EXIT_SUCCESS ){
                std::cout << "Tests passed" << std::endl;
                exit(EXIT_SUCCESS);
            }
            else{
                std::cout << "Tests failed..." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return EXIT_SUCCESS;
}