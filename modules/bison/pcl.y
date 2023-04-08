%{
    #include <iostream>
    #include <list>
    #include <string>
    #include <vector>
    #include <typeinfo>
    #include <type_traits>
    #include "../paracl/leaf.hpp"
    #include "../paracl/nonleaf.hpp"
    #include "../paracl/ptree.hpp"
    extern int yylineno;
    extern int yylex();
    void yyerror(char *s) {
        std::cerr << s << ", line " << yylineno << std::endl;
        exit(1);
    }
    std::vector<ptree::PTree*> blocks;
    ptree::Block* tmp;
    unsigned long offset = 0;
    int blk_num = 1;

    typedef struct {
        std::string str;
        ptree::PTree* oper;
        ptree::Block* blk;
        
    } YYSTYPE;
    #define YYSTYPE YYSTYPE

%}

%token IF ELSE WHILE PRINT
%token EQ LE GE NE
%token NUM ID INPUT

%type<str> NUM ID
 //%type<oper> ID
%type<oper> OPS OP1 OP2 OP
%type<oper> EXPR EXPR1 EXPR2 TERM VAL VAR
%type<blk> BLOCK


%%

PROGRAM: BLOCK                            // обработка дерева программы
;
                                                                                                      //move with *block very bad variant
BLOCK: OPS                              { /*std::cout << std::boolalpha << std::is_same_v<decltype($1), ptree::PTree *> << std::endl;*/
                                            tmp = new ptree::Block(std::move(*((ptree::Block*)$1))); tmp->update_blk_info(offset++, blk_num++); blocks.push_back(tmp); $$ = tmp;}//unique pointer wiil fit good
;

OPS:    OP                              {tmp = new ptree::Block(); tmp->push_expression($1); $$ = tmp;}
|       OPS OP                          {tmp = new ptree::Block(std::move(*((ptree::Block*)$1))); tmp->push_expression($2); $$ = tmp;} //here just a version of block, which provides compilation
;

OP1:    '{' BLOCK '}'                   { $$ = $2; }
|       EXPR ';'                        { $$ = new ptree::Expression(nullptr, nullptr, $1);}
|       IF '(' EXPR ')' OP1 ELSE OP1    { $$ = new ptree::IfBlk($3, nullptr, $7, $5);}
|       WHILE '(' EXPR ')' OP1          { $$ = new ptree::WhileBlk($3, nullptr, $5);}
;

OP2:    IF '(' EXPR ')' OP              { $$ = new ptree::IfBlk($3, nullptr, nullptr, $5); }
|       IF '(' EXPR ')' OP1 ELSE OP2    { $$ = new ptree::IfBlk($3, nullptr, $7, $5); }
|       WHILE '(' EXPR ')' OP2          { $$ = new ptree::WhileBlk($3, nullptr, $5); }
;

OP:     OP1 | OP2 ;                     // inherit

EXPR:   EXPR1                           // inherit
|       VAR '=' EXPR                     { $$ = new ptree::Assign(nullptr, $1, $3); }
|       PRINT EXPR                       { $$ = new ptree::Output(nullptr, $2);}

EXPR1:  EXPR2                           // inherit
|       EXPR1 EQ EXPR2                  { $$ = new ptree::BinOp(ptree::BinOpType::EQUAL, nullptr, $1, $3);}
|       EXPR1 LE EXPR2                  { $$ = new ptree::BinOp(ptree::BinOpType::LESS_EQUAL, nullptr, $1, $3); }
|       EXPR1 GE EXPR2                  { $$ = new ptree::BinOp(ptree::BinOpType::MORE_EQUAL, nullptr, $1, $3);}
|       EXPR1 NE EXPR2                  { $$ = new ptree::BinOp(ptree::BinOpType::NON_EQUAL, nullptr, $1, $3);}
|       EXPR1 '>' EXPR2                 { $$ = new ptree::BinOp(ptree::BinOpType::MORE, nullptr, $1, $3); }
|       EXPR1 '<' EXPR2                 { $$ = new ptree::BinOp(ptree::BinOpType::LESS, nullptr, $1, $3); }
;

EXPR2:  TERM                            // inherit
|       EXPR2 '+' TERM                  { $$ = new ptree::BinOp(ptree::BinOpType::ADDITION, nullptr, $1, $3); }
|       EXPR2 '-' TERM                  { $$ = new ptree::BinOp(ptree::BinOpType::SUBTRACTION, nullptr, $1, $3); }
;

TERM:   VAL                             // inherit
|       TERM '*' VAL                    { $$ = new ptree::BinOp(ptree::BinOpType::MULTIPLICATION, nullptr, $1, $3); }
|       TERM '/' VAL                    { $$ = new ptree::BinOp(ptree::BinOpType::DIVISION, nullptr, $1, $3); }
;

VAR:    ID                              { $$ = new ptree::Variable($1);}//temporary solution

VAL:    NUM                             { $$ = new ptree::Imidiate<int>(nullptr, std::stoi($1)); /*std::cout << $1 << std::endl;*/}
|       INPUT                           { $$ = new ptree::Reserved(nullptr, ptree::Reserved::Types::Input);}
|       '-' VAL                         { $$ = new ptree::UnOp(ptree::UnOpType::MINUS, nullptr, $2);}
|       '!' VAL                         { $$ = new ptree::UnOp(ptree::UnOpType::NOT, nullptr, $2); }
|       '(' EXPR ')'                    { $$ = $2; }
|       VAR

;




%%
int main() { 
    int res = yyparse();
    std::string out = "digraph G {\n";
    out += (blocks.back())->dump();
    out += "}\n";
    std::cout << out << std::endl;
    return res;
}