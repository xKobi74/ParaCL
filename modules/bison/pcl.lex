%option noyywrap
%{
   #include <string>
   #define YYSTYPE std::string
   #include "pcl.tab.h"
   void yyerror(char *s);
   std::string yylval;
%}

%option yylineno



%%

[/][/].*\n      ; // comment
if              return IF;
else            return ELSE;
while           return WHILE;
exit            return EXIT;
==              return EQ;
[<]=            return LE;
>=              return GE;
!=              return NE;
[0-9]+          { yylval = yytext;
                  return NUM;
                }
[a-zA-Z_][a-zA-Z0-9_]* { yylval = yytext;
                  return ID;
                }

[ \t\r\n]       ; // whitespace
[-{};()=<>+*/!,] { return *yytext; }
.               yyerror("Invalid character");
%%