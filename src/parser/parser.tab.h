/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_HPP_INCLUDED
# define YY_YY_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INT = 258,                     /* INT  */
    VOID = 259,                    /* VOID  */
    IF = 260,                      /* IF  */
    ELSE = 261,                    /* ELSE  */
    WHILE = 262,                   /* WHILE  */
    BREAK = 263,                   /* BREAK  */
    CONTINUE = 264,                /* CONTINUE  */
    RETURN = 265,                  /* RETURN  */
    ID = 266,                      /* ID  */
    NUMBER = 267,                  /* NUMBER  */
    PLUS = 268,                    /* PLUS  */
    MINUS = 269,                   /* MINUS  */
    MUL = 270,                     /* MUL  */
    DIV = 271,                     /* DIV  */
    MOD = 272,                     /* MOD  */
    LT = 273,                      /* LT  */
    GT = 274,                      /* GT  */
    LE = 275,                      /* LE  */
    GE = 276,                      /* GE  */
    EQ = 277,                      /* EQ  */
    NE = 278,                      /* NE  */
    AND = 279,                     /* AND  */
    OR = 280,                      /* OR  */
    NOT = 281,                     /* NOT  */
    ASSIGN = 282,                  /* ASSIGN  */
    LPAREN = 283,                  /* LPAREN  */
    RPAREN = 284,                  /* RPAREN  */
    LBRACE = 285,                  /* LBRACE  */
    RBRACE = 286,                  /* RBRACE  */
    COMMA = 287,                   /* COMMA  */
    SEMICOLON = 288                /* SEMICOLON  */
  };
  typedef enum yytokentype yytoken_kind_t;
#include "ast_node.h"
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 79 "parser.y"

    int num;
    std::string* str;
    std::vector<std::unique_ptr<ExprNode>>* expr_list;
    std::vector<std::unique_ptr<ASTNode>>* param_list;
    ASTNode* node;
    ExprNode* expr;
    StmtNode* stmt;
    Block* block;
    FuncDef* func;
    Program* program;

#line 110 "parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_HPP_INCLUDED  */
