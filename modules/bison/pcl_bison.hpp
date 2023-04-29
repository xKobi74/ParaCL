#pragma once

#include <string>

#include "../paracl/leaf.hpp"
#include "../paracl/nonleaf.hpp"
#include "../paracl/ptree.hpp"
#include "../paracl/stack.hpp"

typedef struct {
        std::string str;
        ptree::PTree* oper;
        ptree::Block* blk;
        ptree::Condition* cnd;
        ptree::NameInt* lval;
        
    } YYSTYPE;
    #define YYSTYPE YYSTYPE

void yyerror(char *s);

ptree::Block* wrap_block(ptree::PTree* statement);