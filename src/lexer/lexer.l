%{
#include <string>
#include "../parser/parser.tab.h"

#define YY_DECL int yylex()
%}

%option noyywrap
%option nounput
%option batch

/* Regex definitions */
ID          [_A-Za-z][_A-Za-z0-9]*
NUMBER      -?(0|[1-9][0-9]*)

/* States for comments */
%x C_COMMENT
%x CPP_COMMENT

%%

    /* Whitespace handling */
[ \t\r]+        { /* skip whitespace */ }
\n              { yylineno++; }

    /* C-style multi-line comments */
"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { yylineno++; }
<C_COMMENT>.    { /* skip comment content */ }

    /* C++ style single-line comments */
"//"            { BEGIN(CPP_COMMENT); }
<CPP_COMMENT>\n { BEGIN(INITIAL); yylineno++; }
<CPP_COMMENT>.  { /* skip comment content */ }

    /* Keywords */
"int"           { return INT; }
"void"          { return VOID; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"return"        { return RETURN; }

    /* Identifiers */
{ID}            {
                    yylval.str = new std::string(yytext);
                    return ID;
                }

    /* Numbers */
{NUMBER}        {
                    yylval.num = std::stoi(yytext);
                    return NUMBER;
                }

    /* Operators */
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }
"<"             { return LT; }
">"             { return GT; }
"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE; }
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }
"="             { return ASSIGN; }

    /* Punctuators */
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
","             { return COMMA; }
";"             { return SEMICOLON; }

    /* Invalid character */
.               {
                    // For now, skip invalid characters
                    // In production, should report an error
                }

%%
