#include <stdio.h>
#include <ctype.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include "../../src/generator.h"
#include "../../src/generator.c"
#include "../../src/symtable.h"
#include "../../src/symtable.c"
#include "../../src/custom_data_types/list.c"
#include "../../src/custom_data_types/hashtable.c"
#include "../../src/error.c"
/*
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
    symtableSetDataType(table,DATA_TYPE_DOUBLE,false);
    symtableSetVariableValue(table,&testDouble);

    symtableInsert(table,"funkce27",true);

    symtableAddFunctionNextArgument(table);
    symtableSetDataType(table,DATA_TYPE_DOUBLE,false);
    symtableSetFunctionArgumentName(table,"lorem");

    symtableAddFunctionNextArgument(table);
    symtableSetDataType(table,DATA_TYPE_DOUBLE,false);
    symtableSetFunctionArgumentName(table,"ipsum");

    symtableFunctionEndOfArguments(table);

    symtableSetDataType(table,DATA_TYPE_DOUBLE,false);

    symtableInsert(table,"a",false);
    symtableSetDataType(table,DATA_TYPE_DOUBLE,false);
    symtableSetVariableValue(table,&testDouble);

    symtableInsert(table,"c",false);
    symtableSetDataType(table,DATA_TYPE_INTEGER,false);
    symtableSetVariableValue(table,&testInt);

    symtableInsert(table,"b",false);
    symtableSetDataType(table,DATA_TYPE_STRING,false);
    symtableSetVariableValue(table,&testString);

    EXPECT_EQ(symtableGetReturnTypeOfCurrentScope(table),DATA_TYPE_DOUBLE);

    symtablePrintVariables(table);

    symtableExitScope(table);
    symtablePrintVariables(table);

    symtableFree(table);
}*/
/*
TEST(SymtableTest, Test3){
    symtable *table = symtableInit();

    double testDouble = 27.27f;
    int testInt = 27;
    char *testString = "ahoj světe";

    EXPECT_EQ(symtableIsVariableDefined(table,"x"),false);
    EXPECT_EQ(symtableIsVariableInitiated(table,"x"),false);

    symtableInsert(table,"x",false);

    EXPECT_EQ(symtableGetVariableType(table,"x"),DATA_TYPE_NOTSET);

    EXPECT_EQ(symtableIsVariableInitiated(table,"x"),false);

    symtableSetDataType(table,DATA_TYPE_INTEGER,false);
    symtableSetVariableValue(table,&testInt);

    EXPECT_EQ(symtableGetVariableType(table,"x"),DATA_TYPE_INTEGER);

    EXPECT_EQ(symtableIsVariableInitiated(table,"x"),true);

    EXPECT_EQ(symtableIsVariableDefined(table,"x"),true);
    EXPECT_EQ(symtableIsVariableDefined(table,"funkce"),false);

    symtableInsert(table,"funkce",true);
    symtableFunctionEndOfArguments(table);
    symtableSetDataType(table,DATA_TYPE_VOID,false);

    EXPECT_EQ(symtableIsVariableDefined(table,"a"),false);
    symtableInsert(table,"a",false);
    EXPECT_EQ(symtableIsVariableDefined(table,"a"),true);
    EXPECT_EQ(symtableIsVariableDefined(table,"x"),true);
    EXPECT_EQ(symtableIsVariableInitiated(table,"x"),true);
    EXPECT_EQ(symtableGetVariableType(table,"x"),DATA_TYPE_INTEGER);

    symtableExitScope(table);

    EXPECT_EQ(symtableIsVariableDefined(table,"funkce"),true);
    EXPECT_EQ(symtableIsVariableDefined(table,"a"),false);

    symtableFree(table);
}*/

TEST(SymtableTest, Test5){
    generator *gen = generatorInit();
    symtable *table = symtableInit(gen);

    double testDouble = 27.27f;
    int testInt = 27;
    char *testString = "ahoj světe";

    symtableInsert(table,"funkce",true);

    symtableAddFunctionNextArgument(table);
    symtableSetDataType(table,DATA_TYPE_INTEGER,false);
    symtableSetFunctionArgumentName(table,"xd");

    symtableFunctionEndOfArguments(table);
    symtableSetDataType(table,DATA_TYPE_VOID,false);
    symtableExitScope(table);

    symtableFunctionCallStart(table,"funkce");
    symtableFunctionCallNextParameter(table);
    symtableFunctionCallSetParameterName(table,"xd");
    symtableFunctionCallSetParameterType(table,DATA_TYPE_INTEGER,false);
    symtableFunctionCallEnd(table);

    symtableFree(table);
    generatorFree(gen);

    //Kdyz meno je podrzitko pri volani ho neupouzivas
}
/*
TEST(GeneratorTest,Test1){
    generator *gen = generatorInit();
    generatorGenerateOutput(gen);
    generatorFree(gen);
}*/