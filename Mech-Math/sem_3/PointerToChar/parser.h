// =====================================
// Char|Int pointer|array analyser
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

/*
 * Parser & Scanner Common Definitions
 */
#ifndef PARSER_H
#define PARSER_H

/* The type of values associated with terminals and nonterminals */

#include "str.h"

#define YYSTYPE string  

#include "calc.tab.hpp"   /* File generated automatically by bison: */
                        /* contains the definitions of terminals  */

int yylex();              /* Prototype of the scanner function "next lexeme" */
int yyerror(const char*); /* Prototype of the error diagnostics function     */

#endif
