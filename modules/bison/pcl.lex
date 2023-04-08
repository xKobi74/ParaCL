
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
if              return IF;
else            return ELSE;
while           return WHILE;
print           return PRINT;
[?]               return INPUT;
==              return EQ;
[<]=            return LE;
>=              return GE;
!=              return NE;
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
[-{};()=<>+*/!,] { return *yytext; }
.               yyerror("Invalid character");
%%