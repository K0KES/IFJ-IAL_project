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

TEST(SymtableTest, Test2){
    symtable *table = symtableInit();

    float test = 27.27f;

    symtableInsert(table,"test",false);
    symtableSetVariableType(table,DATA_TYPE_FLOAT);
    symtableSetVariableValue(table,&test);

    symtableInsert(table,"funkce27",true);
    symtableSetFunctionReturnType(table,DATA_TYPE_FLOAT);

    symtableAddFunctionNextArgument(table);
    symtableSetFunctionArgumentType(table,DATA_TYPE_FLOAT);
    symtableSetFunctionArgumentName(table,"lorem");

    symtableAddFunctionNextArgument(table);
    symtableSetFunctionArgumentType(table,DATA_TYPE_FLOAT);
    symtableSetFunctionArgumentName(table,"ipsum");

    symtablePrintVariables(table);

    symtableFree(table);
}