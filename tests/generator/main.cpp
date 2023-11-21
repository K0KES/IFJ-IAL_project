#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

//#include "../../src/expression_parser.c"
//#include "../../src/expression_parser.h"
#include "../../src/scanner.c"
#include "../../src/scanner.h"
#include "../../src/symtable.c"
#include "../../src/symtable.h"
#include "../../src/str.c"
#include "../../src/str.h"
#include "../../src/parser.c"
#include "../../src/parser.h"
#include "../../src/custom_data_types/list.c"
#include "../../src/custom_data_types/list.h"
#include "../../src/custom_data_types/hashtable.c"
#include "../../src/custom_data_types/hashtable.h"
#pragma once


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
