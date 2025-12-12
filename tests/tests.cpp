#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../helpers/external/doctest.h"

#ifdef SINGLE_HEADER
    #include "../single_include/bmssp.hpp"
#else
    #include "../include/bmssp.hpp"
#endif

#include "tests-wc.cpp"
#include "tests-expected.cpp"