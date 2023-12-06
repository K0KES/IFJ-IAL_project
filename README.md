# IFJ-IAL_project

## Code style

* Naming
  * all variables, functions and classes will be written in "camelCase" convention (way of seperating words in phrase)
  * functions and variables will start with with lowercase letter and each new word in the word will have first letter as uppercase
  * classes will start with uppercase, rest is the same
  * The word doesn't end on a capital letter
  * No dots, underscores, numbers, dashes, or any other special characters are allowed within the word
  * examples:
    * variable =>

        ```bash
        $firstVariable = 0;
        ```

    * function =>

        ```c
        protected function testVariable(&$firstVariable) {}
        ```

    * class    =>

        ```c
        class GeographyPlaceImporter extends CsvImporter {}
        ```

* Braces placement {} - at the end of line
  * in class declaration
  * in function declaration
  * in control statements (if, while, for...)
  * examples
    * function =>

        ```c
        protected function testVariable(&$firstVariable) {
            return null;                  
        }
        ```

    * if statement =>

        ```c
        if ($grade >= self::A) {
            echo "GG";
        }
        ```

* if and else statements
  * dont use a "cuddled else"
    * right example =>

      ```c
      if ($boolVar){
          return true;
      }
      else {
          return false;
      }
      ```

    * wrong example =>

      ```c
      if ($boolVar){
          return true;
      } else {
          return false;
      }
      ```

  * if "if" or "else" statement has only one line of code, still use brackets and seperate them with whitespace
    * example =>

      ```c
      if (1 > 0) { return true; }
      ```

* Comments
  * Should be written before the wanted line of code, NOT on the same level
  * example =>

     ```bash
     // creation od db object
     $newGeographyPlace = GeographyPlace::create($dbConnectionId);
     ```

## Project structure

### Lexical analysis

### Syntactic analysis (Parser)

* It controls whole compilation process.
* If there is `derivation tree` for given input, then the input is syntactically correct.
* You go from the root of the derivation tree to the leaves and try to apply rules of the grammar to the input.
* You genereate `abstract syntax tree` from `derivation tree` .
  
### Semantic analysis

### Generation of inside code

* It converts `abstract syntax tree` to `three-address code`.

### Optimalization

* It optimizes `three-address code`.

### Generation of target code
