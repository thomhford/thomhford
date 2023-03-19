/**
 * \file cmdline.h
 * \brief Declaration of cmdline.cpp
 * \author Thomas Ford
 * \date 02-07-2023
 */

#ifndef cmdline_h
#define cmdline_h
#pragma once

typedef enum {

    do_nothing,
    do_interp,
    do_print,
    do_pretty_print,

} run_mode_t;

run_mode_t use_arguments(int argc, char* argv[]);

#endif /* cmdline_h */