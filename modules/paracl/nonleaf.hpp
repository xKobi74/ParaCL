#pragma once

#include "ptree.hpp"
#include "leaf.hpp"


#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>

//CPP20 is not needed because most compilators doesn`t accept format lib
#ifdef CPP20
#include <format>
#endif
//TODO: implement yyerrok option to provide more complicated error catching
/*
class structure:
1) NonLeaf - not for direct call, contains all nonleafs classes
2) Expression - provides construction ';' sequencing, made program for multiple ';' programs
3) Operation - not for direct call, contains all types of operations
4) BinOp - binary operation, provides: +, -, /, *, >=, <=, ==, !=, >, <
5) UnOp - unary operations, provides ++, --
6) Assign - operation of assignation
7) Block - provides block like '{' operations '}'
8) Branch - not for direct call, provides operations like if, else, while
9) IfBlk, WhileBlk - realization of Branch deriveds

*/

namespace ptree {

class NonLeaf: public PTree {
  public:
  
  NonLeaf(PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): PTree(parent, left, right) {};
  
  std::string get_links() const;
  
  inline std::string get_chld_dump() const ;

  bool isLeaf() const override;
  
  std::unique_ptr<PTree> execute(Stack *stack) const override = 0;

  ~NonLeaf() override = default;
};

//expression provides sequencing between strings separated with ';'
//HACK: left pointer is continuation of programm right pointer is operations made in line before ';'
class Expression : public NonLeaf {
  public:
  Expression(PTree* parent = nullptr, PTree* operations = nullptr): NonLeaf(parent, nullptr, operations) {};
  
  std::string dump() const override ;

  std::unique_ptr<PTree> execute(Stack *stack) const override ;
};

class Operation : public NonLeaf {
  public:
  Operation(PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): NonLeaf(parent, left, right) {};
  std::unique_ptr<PTree> execute(Stack *stack) const override = 0;
};
//TODO: maybe I should point it like BinOpType: std::string
enum class BinOpType {
  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION,
  REMAINDER,
  EQUAL,
  MORE_EQUAL,
  LESS_EQUAL,
  NON_EQUAL,
  MORE,
  LESS,
  UNDEF,
  LOG_AND,
  LOG_OR,
};
//TODO: Split for log binop and simple binop
template <typename T>
T operate(T lhs, T rhs, BinOpType operation) ;

class BinOp: public Operation {
  public:
  BinOpType operation_;
  BinOp(BinOpType operation = BinOpType::UNDEF, PTree* parent = nullptr, PTree* l_operand = nullptr, PTree* r_operand = nullptr): 
        Operation(parent, l_operand, r_operand), operation_(operation) {};
  
  std::string get_op() const ;
  std::string dump() const override ;

  std::unique_ptr<PTree> execute(Stack *stack) const override ; 
};

enum class UnOpType {
  POST_ADDITION,
  POST_SUBTRACTION,
  MINUS,
  NOT,
  UNDEF
};



class UnOp: public Operation {
  public:
  UnOpType operation_;
  //HACK: getright() pointer depricated because unary operation has only one operand to count value
  //FIXME: Now we can easily make ++i operator, but i++ op needs some more modifications
  UnOp(UnOpType operation = UnOpType::UNDEF, PTree* parent = nullptr, PTree* operand = nullptr): 
        Operation(parent, operand, nullptr), operation_(operation) {};
  
  std::string get_op() const ;

  std::unique_ptr<PTree> execute(Stack* stack) const override;
  
  std::string dump() const override; 
};
// getleft() pointer - process inside block, getright() pointer - outer process, continuing of main programm

class Block: public NonLeaf {
  public:
  using offset_t = unsigned long;
  using block_id = int;
  offset_t offset_;
  block_id id_;
  std::vector<PTree*> operations;

  Block(offset_t offset = 0x0, block_id id = -1):
  NonLeaf(nullptr, nullptr, nullptr), offset_(offset), id_(id) {
  }; //TODO: remove nullptr initialization
  
  Block(Block&& rhs): operations(std::move(rhs.operations))  {
    offset_ = 0; id_ = 0;
    rhs.offset_ = 0; rhs.id_ = -1;

  }
  
  Block(const Block& rhs): operations(rhs.operations) {
    std::cout << "copy called" << std::endl;
    offset_ = 0; id_ = 0;
  }

  void update_blk_info(offset_t offset = 0x0, block_id id = 0);

  //TODO: improve push
  void push_expression(PTree* instruction);

  std::string get_blk_info() const;

  std::string dump() const override;

  std::unique_ptr<PTree> execute(Stack* stack) const override;

};


//HACK: left pointer is assignable object, right pointer is objects to assign
class Assign : public Operation {
  public:
  NameInt *lval;
  Assign(PTree* parent = nullptr, NameInt* left = nullptr, PTree* right = nullptr): Operation(parent, nullptr, right), lval(left) {};
  
  std::string dump() const override;


  std::unique_ptr<PTree> execute(Stack* stack) const override;
};

class Condition: public NonLeaf {
  public:
  Condition(PTree* parent = nullptr, PTree* condition = nullptr): NonLeaf(parent, condition, nullptr) {}

  std::string dump() const override;

  std::unique_ptr<PTree> execute(Stack* stack) const override;

  bool is_true(Stack* stack) const;
};

class Branch: public NonLeaf {
  public:
  Condition* condition_;
  //FIXME: specify condition type as Immidiate value, or leave this specialization to execute module
  //HACK: as condition used immidiate int value like a pointer to similar block, it should be available to count result and return > 0(true) or <= 0(false)
  Branch(Condition* condition = nullptr, PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): 
  NonLeaf(parent, left, right), condition_(condition) {};

  std::unique_ptr<PTree> execute(Stack* stack) const override  = 0;
};



class IfBlk: public Branch {
  public:
  //HACK: if getleft() pointer == nullptr it means that else clause not exist, otherwise getright() pointer is true case, getleft() pointer is else case
  
  
  IfBlk(Condition* condition = nullptr, PTree* parent = nullptr, PTree* else_blk = nullptr, PTree* if_blk = nullptr): 
  Branch(condition, parent, else_blk, if_blk) {};
  
  std::string dump() const override;

  std::unique_ptr<PTree> execute(Stack* stack) const override;
};

class WhileBlk: public Branch {
  public:
  WhileBlk(Condition* condition = nullptr, PTree* parent = nullptr, PTree* while_blk = nullptr): Branch(condition, parent, while_blk, nullptr) {};
  
  std::string dump() const override;

  std::unique_ptr<PTree> execute(Stack* stack) const override;
};

class Output: public Operation {
  public:
  Output(PTree* parent = nullptr, PTree* to_print = nullptr): Operation(parent, nullptr, to_print) {};

  std::string dump() const override;

  std::unique_ptr<PTree> execute(Stack* stack) const override;
};



};