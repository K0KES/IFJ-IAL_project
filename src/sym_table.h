/**
 * @file sym_table.h
 * @author Vít Slavíček
 * @brief Symbol table header
 * @date 9.10.2023
 * 
*/

/**
 * @brief enum for symbol types
*/
typedef enum {
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_FUNCTION
} symbol_type_t;

/**
 * @brief enum for data types
*/
typedef enum {
    DATA_TYPE_UNDEFINED,
    DATA_TYPE_INTEGER,
    DATA_TYPE_FLOAT,
    DATA_TYPE_STRING
} data_type_t;