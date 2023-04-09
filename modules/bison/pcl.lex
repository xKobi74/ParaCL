
%{
   #include <string>
   #include <iostream>
   typedef struct {
        std::string str;
        void* oper;
        void* blk;
    } YYSTYPE;
    #define YYSTYPE YYSTYPE
   #include "pcl.tab.h"
   void yyerror(char *s);
   //std::string yylval;
%}

%option noyywrap
%option yylineno



%%

[/][/].*\n      ; // comment
"if"              return IF;
"else"            return ELSE;
"while"           return WHILE;
"print"           return PRINT;
"?"               return INPUT;
"=="              return EQ;
">"               return GREAT;
"<"               return LESS;
"<="              return LE;
">="              return GE;
"!="              return NE;
"("               return LPAR;
")"               return RPAR;
"["               return LBR;
"]"               return RBR;
"{"               return LCB;
"}"               return RCB;
"="               return ASSIGN;
"+"               return PLUS;
"-"               return MINUS;
"*"               return MUL;
"/"               return DIV;
"%"               return MOD;
"!"               return NOT;
"&&"              return AND;
"||"              return OR;
"++"              return P_PLUS;
"--"              return P_MINUS;
";"               return SEQUENCE;
[0-9]+          { yylval.str = yytext;
                  //std::cout << yytext << std::endl;
                  //std::cout << yylval.str << std::endl;
                  return NUM;
                }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = yytext;
                         //std::cout << yytext << std::endl;
                         //std::cout << yylval.str << std::endl;
                         return ID;
                       }

[ \t\r\n]       ; // whitespace
.               yyerror("Invalid character");
%%