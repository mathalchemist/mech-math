/*
 * Parser & Scanner Common Definitions
 */
#ifndef PARSER_H
#define PARSER_H

/* The type of values associated with terminals and nonterminals */
#define YYSTYPE double  

#include "calc.tab.h"   /* File generated automatically by bison: */
                        /* contains the definitions of terminals  */

int yylex();              /* Prototype of the scanner function "next lexeme" */
int yyerror(const char*); /* Prototype of the error diagnostics function     */

#endif
