/*                                                                          */
/* File Name    : subc.l                                                    */
/* Description  : a skeleton flex input                                     */
/*                                                                          */

%{

/* C Code Section */
/* Any code placed inside the brackets will be literally copied into the generated C file. */

#include "subc.h"
#include "subc.tab.h"
static int lineno = 1;
extern char* current_filename;

extern char* tracked_strdup(const char* str);

%}

/* Definitions Section */
/* This section contains simple name definitions and declarations of start conditions. */

letter        [A-Za-z_]
digit         [0-9]
id            {letter}({letter}|{digit})*
integer_const ([1-9]{digit}*)|"0"
whitespace    [ \t]
special_char  \\[nt]

%x COMMENT

%%

  /* Rules Section */
  /* This section contains series of rules for recognizing tokens. */

  /* Keywords */
"int"|"char"                    { 
                                  yylval.stringVal = tracked_strdup(yytext);
                                  return TYPE; 
                                }
"struct"                        { return STRUCT; } 
"NULL"                          { return SYM_NULL; }
"return"                        { return RETURN; }
"if"                            { return IF; }
"else"                          { return ELSE; }
"while"                         { return WHILE; }
"for"                            { return FOR; }
"break"                         { return BREAK; }
"continue"                      { return CONTINUE; }

  /* Identifiers */
{id}                            {
                                  yylval.stringVal = tracked_strdup(yytext);
                                  return ID;
                                }

  /* Integer constants */
{integer_const}                 {
                                  yylval.intVal = atoi(yytext);
                                  return INTEGER_CONST;
                                }

  /* String variables */
\"({special_char}|[^\\\"\n])*\" {
                                  yylval.stringVal = tracked_strdup(yytext);
                                  return STRING;
                                }

  /* Character variables */
'({special_char}|[^\\'\n])'     {    
                                  yylval.intVal = yytext[1];
                                  return CHAR_CONST;
                                }

  /* Operators */
","|";"|"("|")"|"["|"]"|"{"|"}" { return yytext[0]; }
"!"|"*"|"/"|"%"|"+"|"-"|"&"     { return yytext[0]; }
"="|"."                         { return yytext[0]; }

"&&"                            { return LOGICAL_AND; }
"||"                            { return LOGICAL_OR; }
"<"|"<="|">"|">="               { return RELOP; }
"=="|"!="                       { return EQUOP; }
"++"                            { return INCOP; }
"--"                            { return DECOP; }
"->"                            { return STRUCTOP; }

  /* Whitespaces */
{whitespace}                    { /* ignore whitespace */ }

  /* Comment blocks */
"/*"                            { BEGIN(COMMENT); }
<COMMENT>"*/"                   { BEGIN(INITIAL); }
<COMMENT>.                      { /* ignore comment content */ }

  /* Line number tracking */
<*>\n                           { ++lineno; }

%%
   
/* User Code Section */
/* The user code section is simply copied to lex.yy.c verbatim. */

int get_lineno() {
   return lineno;
}

int main(int argc, char* argv[]) {
  if(argc >= 2) {
    yyin = fopen(argv[1], "r");
    // basename 추출
    char* slash = strrchr(argv[1], '/');
    current_filename = slash ? slash + 1 : argv[1];
  } else {
    yyin = stdin;
    current_filename = "stdin";
  }

  if(!yyin) {
    printf("Can't open input stream!\n");
    exit(1);
  }
  
  yyparse();
  fclose(yyin);

  return 0;
}
