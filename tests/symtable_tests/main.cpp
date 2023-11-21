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

    symtableEnterScope(table,"myScope",NULL);

    symtableInsert(table,"flex27",false);
    symtablePrintVariables(table);
    symtableExitScope(table);

    symtableFree(table);
}

TEST(SymtableTest, Test2){
    symtable *table = symtableInit();

    double testDouble = 27.27f;
    int testInt = 27;
    char *testString = "ahoj světe";

    symtableInsert(table,"test",false);
    symtableSetDataType(table,DATA_TYPE_DOUBLE);
    symtableSetVariableValue(table,&testDouble);

    symtableInsert(table,"funkce27",true);

    symtableAddFunctionNextArgument(table);
    symtableSetDataType(table,DATA_TYPE_DOUBLE);
    symtableSetFunctionArgumentName(table,"lorem");

    symtableAddFunctionNextArgument(table);
    symtableSetDataType(table,DATA_TYPE_DOUBLE);
    symtableSetFunctionArgumentName(table,"ipsum");

    symtableFunctionEndOfArguments(table);

    symtableSetDataType(table,DATA_TYPE_DOUBLE);

    symtableInsert(table,"a",false);
    symtableSetDataType(table,DATA_TYPE_DOUBLE);
    symtableSetVariableValue(table,&testDouble);

    symtableInsert(table,"c",false);
    symtableSetDataType(table,DATA_TYPE_INTEGER);
    symtableSetVariableValue(table,&testInt);

    symtableInsert(table,"b",false);
    symtableSetDataType(table,DATA_TYPE_STRING);
    symtableSetVariableValue(table,&testString);

    EXPECT_EQ(symtableGetReturnTypeOfCurrentScope(table),DATA_TYPE_DOUBLE);

    symtablePrintVariables(table);

    symtableExitScope(table);
    symtablePrintVariables(table);

    symtableFree(table);
}