#ifndef ERROR_H
#define ERROR_H

//Odkomentaruj toto pro debug printy
// A NEZAPOMEN UDELAT make clean !!!!!!!!!!!!
#define ENABLE_PRINTF 
#ifdef ENABLE_PRINTF
    #define    DEBUG_PRINTF(f,...)    printf(f,##__VA_ARGS__)
#else
    #define    DEBUG_PRINTF(f,...)
#endif

enum errorType
{
    // chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
    ERR_LEXICAL = 1,

    // chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.).
    ERR_SYNTAX = 2,

    // sémantická chyba v programu - nedefinovaná funkce.
    ERR_UNDEFINED_FUNCTION = 3,

    // sémantická chyba v programu - špatný počet / typ parametrů u volání funkce či špatný typ návratové hodnoty z funkce.
    ERR_WRONG_NUMBER_OF_ARGUMENTS = 4,

    // sémantická chyba v programu - použití nedefinované proměnné.
    ERR_UNDEFINED_VARIABLE = 5,

    // sémantická chyba v programu - chybějící / přebývající výraz v příkazu návratu z funkce.
    ERR_WRONG_RETURN_TYPE = 6,

    // sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech.
    ERR_WRONG_TYPE = 7,
    
    // sémantická chyba odvození typu - typ proměnné nebo parametru není uveden a nelze odvodit od použitého výrazu.
    ERR_MISSING_TYPE = 8,

    // ostatní sémantické chyby.
    ERR_SEMANTIC = 9,

    // interní chyba překladače tj.neovlivněná vstupním programem(např.chyba alo - kace paměti atd.)
    ERR_INTERNAL = 99
};

void raiseError(enum errorType error);

const char* getErrorName(enum errorType tokenType);
#endif
