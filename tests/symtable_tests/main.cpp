#include <stdio.h>
#include <ctype.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include "../../src/symtable.h"
#include "../../src/symtable.c"
#include "../../src/custom_data_types/list.c"
#include "../../src/custom_data_types/hashtable.c"

TEST(SymtableTest, Test1) {
    
    symtable *table = symtableInit();

    symtableInsert(table,"test",false);
    symtablePrintVariables(table);

    symtableEnterScope(table,"myScope");

    symtableInsert(table,"flex27",false);
    symtablePrintVariables(table);
    symtableExitScope(table);

    symtableFree(table);
}