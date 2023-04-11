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

%token IF ELSE WHILE PRINT INPUT
%token EQ LE GE NE AND OR NOT GREAT LESS
%token NUM ID  P_PLUS P_MINUS SEQUENCE
%token LPAR RPAR LBR RBR LCB RCB
%token ASSIGN PLUS MINUS MUL DIV MOD

%type<str> NUM ID
%type<oper> OPS OP1 OP2 OP
%type<oper> EXPR EXPR1 EXPR2 EXPR3 TERM VAL VAR
%type<blk> BLOCK


%%

PROGRAM: BLOCK                            // обработка дерева программы
;
                                                                                                    
BLOCK: OPS                              { tmp = new ptree::Block(std::move(*((ptree::Block*)$1))); tmp->update_blk_info(offset++, blk_num++); blocks.push_back(tmp); $$ = tmp;}//unique pointer wiil fit good
;

OPS:    OP                              {tmp = new ptree::Block(); tmp->push_expression($1); $$ = tmp;}
|       OPS OP                          {tmp = new ptree::Block(std::move(*((ptree::Block*)$1))); tmp->push_expression($2); $$ = tmp;} //here just a version of block, which provides compilation
;

OP1:    LCB BLOCK RCB                   { $$ = $2; }
|       EXPR SEQUENCE                   { $$ = new ptree::Expression(nullptr, $1);}
|       IF LPAR EXPR RPAR OP1 ELSE OP1    { $$ = new ptree::IfBlk($3, nullptr, $7, $5);}
|       WHILE LPAR EXPR RPAR OP1          { $$ = new ptree::WhileBlk($3, nullptr, $5);}
;

OP2:    IF LPAR EXPR RPAR OP              { $$ = new ptree::IfBlk($3, nullptr, nullptr, $5); }
|       IF LPAR EXPR RPAR OP1 ELSE OP2    { $$ = new ptree::IfBlk($3, nullptr, $7, $5); }
|       WHILE LPAR EXPR RPAR OP2          { $$ = new ptree::WhileBlk($3, nullptr, $5); }
;

OP:     OP1 | OP2 ;                     // inherit to solve C problem with if block

EXPR:   EXPR1                           // inherit
|       VAR ASSIGN EXPR                  { $$ = new ptree::Assign(nullptr, $1, $3); }
|       PRINT EXPR                       { $$ = new ptree::Output(nullptr, $2);}

EXPR1: EXPR2                           //inherit
|      EXPR1 AND EXPR2                 { $$ = new ptree::BinOp(ptree::BinOpType::LOG_AND, nullptr, $1, $3); }
|      EXPR1 OR EXPR2                  { $$ = new ptree::BinOp(ptree::BinOpType::LOG_OR, nullptr, $1, $3); }
;

EXPR2:  EXPR3                           // inherit
|       EXPR2 EQ EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::EQUAL, nullptr, $1, $3);}
|       EXPR2 LE EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::LESS_EQUAL, nullptr, $1, $3); }
|       EXPR2 GE EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::MORE_EQUAL, nullptr, $1, $3);}
|       EXPR2 NE EXPR3                  { $$ = new ptree::BinOp(ptree::BinOpType::NON_EQUAL, nullptr, $1, $3);}
|       EXPR2 GREAT EXPR3               { $$ = new ptree::BinOp(ptree::BinOpType::MORE, nullptr, $1, $3); }
|       EXPR2 LESS EXPR3                { $$ = new ptree::BinOp(ptree::BinOpType::LESS, nullptr, $1, $3); }
;

EXPR3:  TERM                            // inherit
|       EXPR3 PLUS TERM                  { $$ = new ptree::BinOp(ptree::BinOpType::ADDITION, nullptr, $1, $3); }
|       EXPR3 MINUS TERM                  { $$ = new ptree::BinOp(ptree::BinOpType::SUBTRACTION, nullptr, $1, $3); }
;

TERM:   VAL                             // inherit
|       TERM MUL VAL                    { $$ = new ptree::BinOp(ptree::BinOpType::MULTIPLICATION, nullptr, $1, $3); }
|       TERM DIV VAL                    { $$ = new ptree::BinOp(ptree::BinOpType::DIVISION, nullptr, $1, $3); }
;

VAR:    ID                              { $$ = new ptree::Variable($1);}//temporary solution

VAL:    NUM                             { $$ = new ptree::Imidiate<int>(nullptr, std::stoi($1)); /*std::cout << $1 << std::endl;*/}
|       INPUT                           { $$ = new ptree::Reserved(nullptr, ptree::Reserved::Types::Input);}
|       MINUS VAL                       { $$ = new ptree::UnOp(ptree::UnOpType::MINUS, nullptr, $2);}
|       NOT VAL                         { $$ = new ptree::UnOp(ptree::UnOpType::NOT, nullptr, $2); }
|       VAR P_PLUS                      { $$ = new ptree::UnOp(ptree::UnOpType::POST_ADDITION, nullptr, $1); }
|       VAR P_MINUS                     { $$ = new ptree::UnOp(ptree::UnOpType::POST_SUBTRACTION, nullptr, $1); }
|       LPAR EXPR RPAR                  { $$ = $2; }
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