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
        ```
        $firstVariable = 0;
        ```
        * function =>
        ```
        protected function testVariable(&$firstVariable) {}
        ```
        * class    =>
        ```
        class GeographyPlaceImporter extends CsvImporter {}
        ```

* Braces placement {} - at the end of line
    * in class declaration
    * in function declaration
    * in control statements (if, while, for...)
    * examples
        * function =>
        ```
        protected function testVariable(&$firstVariable) {
            return null;                  
        }
        ```
        
        * if statement =>
        ```
        if ($grade >= self::A) {
            echo "GG";
        }
        ```

* if and else statements
    * dont use a "cuddled else"
      * right example =>
      ```
      if ($boolVar){
          return true;
      }
      else {
          return false;
      }
      ```
      * wrong example =>
      ```
      if ($boolVar){
          return true;
      } else {
          return false;
      }
      ```
    * if "if" or "else" statement has only one line of code, still use brackets and seperate them with whitespace
      * example =>
      ```
      if (1 > 0) { return true; }
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

## Place where to inspire yourself :D

### 2022

### 2021

### 2020

* [Demon124 _full points_](https://github.com/V4-FIT/V4-IFJ)
* [horsecz](https://github.com/horsecz/FIT-IFJ-TeamProject)
* [GargiMan](https://github.com/GargiMan/ifj)

### 2019

* [Roman3349](https://github.com/Roman3349/FIT-BIT-IFJ-2019-project)
* [TomasBeranek](https://github.com/RichardKlem/IFJ)

### 2018

* [janhavlin](https://github.com/janhavlin/VUT-FIT-IFJ-2018)
* [janchaloupka](https://github.com/janchaloupka/IFJ-Projekt)
* [youngbender](https://github.com/yungbender/ifj-projekt)
* [dvaga](https://github.com/dvagala/VUT-FIT-IFJ-Project)

### 2017

* [Harmin](https://github.com/harmim/vut-ifj-project)
* [xjuric29](https://github.com/xjuric29/ifj)

### 2016

### 2015

* [arakan94](https://github.com/arakan94/ifj15)
  
