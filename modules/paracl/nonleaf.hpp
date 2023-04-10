#pragma once

#include "ptree.hpp"


#include <string>
#include <list>
#include <sstream>
#include <vector>
#include <iostream>
//CPP20 is not needed because most compilators doesn`t accept format lib
#ifdef CPP20
#include <format>
#endif

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
  
  std::string get_links() const {
    std::string res{""};
    if (getleft() != nullptr) res += getname() + " -> " + getleft()->getname() + "\n";
    if (getright() != nullptr) res += getname() + " -> " + getright()->getname() + "\n";
    return res;
  }
  
  inline std::string get_chld_dump() const {
    std::string res;
    if (getleft() != nullptr) res += getleft()->dump();
    if (getright() != nullptr) res += getright()->dump();
    return res;
  }

  bool isLeaf() const override {
    return false;
  } 
  
  PTree* execute() override {
    return this;
  }
};

//expression provides sequencing between strings separated with ';'
//HACK: left pointer is continuation of programm right pointer is operations made in line before ';'
class Expression : public NonLeaf {
  public:
  Expression(PTree* parent = nullptr, PTree* operations = nullptr): NonLeaf(parent, nullptr, operations) {};
  
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();

    res += getname() + "[shape = record, label=\"{Expression \\n |" +
    "{ " + get_addr(getleft()) + "\\n (depricated) | " + get_addr(getright()) + "\\n(expression)}}\"]\n";

    //HACK: no get_links() here, because nodes should be colorized and with caption
    if (getleft() != nullptr) res += getname() + " -> " + getleft()->getname() + "[color=\"black\", label=\"continuing\"]\n";
    if (getright() != nullptr) res += getname() + " -> " + getright()->getname() + "[color=\"black\", label=\"expression\"]\n";
    
    return res;
  }
};

class Operation : public NonLeaf {
  public:
  Operation(PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): NonLeaf(parent, left, right) {};
};
//TODO: maybe I should point it like BinOpType: std::string
enum class BinOpType {
  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION,
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

class BinOp: public Operation {
  public:
  BinOpType operation_;
  BinOp(BinOpType operation = BinOpType::UNDEF, PTree* parent = nullptr, PTree* l_operand = nullptr, PTree* r_operand = nullptr): 
        Operation(parent, l_operand, r_operand), operation_(operation) {};
  
  std::string get_op() const {
    switch (operation_) {
      case BinOpType::ADDITION:
        return "+";
      case BinOpType::DIVISION:
        return "/";
      case BinOpType::MULTIPLICATION:
        return "*";
      case BinOpType::SUBTRACTION:
        return "-";
      case BinOpType::EQUAL:
        return "==";
      case BinOpType::MORE_EQUAL:
        return "\\>=";
      case BinOpType::LESS_EQUAL:
        return "\\<=";
      case BinOpType::NON_EQUAL:
        return "!=";
      case BinOpType::LESS:
        return "\\<";
      case BinOpType::MORE:
        return "\\>";
      case BinOpType::LOG_AND:
        return "&&";
      case BinOpType::LOG_OR:
        return "||";
      default:
        return "?";  
    }

  }
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    res += getname() + "[shape = record, label=\"{Binary operation(" + get_op() + ")|" +
    "{ " + get_addr(getleft()) + " | " + get_addr(getright()) + "}}\"]\n";
    res += get_links();  
    return res;
  } 
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
  
  std::string get_op() const {
    switch (operation_) {
      case UnOpType::POST_ADDITION:
        return "++";
      case UnOpType::POST_SUBTRACTION:
        return "--";
      case UnOpType::MINUS:
        return "-value";
      case UnOpType::NOT:
        return "!value";
      default:
        return "?";  
    }

  }
  
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    res += getname() + "[shape = record, label=\"{Unary operation(" + get_op() + ")|" +
    "{ " + get_addr(getleft()) + " | " + get_addr(getright()) + "\\n(depricated)}}\"]\n";
    res += get_links();  
    return res;
  } 
};
/* getleft() pointer - process inside block, getright() pointer - outer process, continuing of main programm
*/
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

  void update_blk_info(offset_t offset = 0x0, block_id id = 0) {
    offset_ = offset; id_ = id;
  }

  //TODO: improve push
  void push_expression(PTree* instruction) {
    operations.push_back(instruction);
  }

  std::string get_blk_info() const {
    std::string res;
#ifdef CPP20
    res += std::format("offset = {:X} \\n block id = {}", offset_, id_);
#else
    res += "offset = " + std::to_string(offset_) + "\\n block id = " + std::to_string(id_) + "\\n";
#endif
    res += "expressions count: " + std::to_string(operations.size()) + "\\n"; 
  return res;
  }
  
  std::string dump() const override {
    std::string res;
    res += getname() + "[shape = record, label=\"{Block \\n" + get_blk_info() +"}\"]\n";
    unsigned int expr_num = 1;
    for (PTree* expr: operations) {
      res += expr->dump();
      res += getname() + " -> " + expr->getname() + "[color = \"black\", label=\"expr number: " + std::to_string(expr_num++) + "\"]\n";
    }
    return res;
  }

};


//HACK: left pointer is assignable object, right pointer is objects to assign
class Assign : public Operation {
  public:
  Assign(PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): Operation(parent, left, right) {};
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();

    res += getname() + "[shape = record, label=\"{Assignation (a = b) \\n |" +
    "{ " + get_addr(getleft()) + "\\n (a) | " + get_addr(getright()) + "\\n(b)}}\"]\n";

    //HACK: no get_links() here, because nodes should be colorized and with caption
    if (getleft() != nullptr) res += getname() + " -> " + getleft()->getname() + "[color=\"black\", label=\"assignable\"]\n";
    if (getright() != nullptr) res += getname() + " -> " + getright()->getname() + "[color=\"black\", label=\"to assign\"]\n";
    
    return res;
  }
};

class Branch: public NonLeaf {
  public:
  PTree* condition_;
  //FIXME: specify condition type as Immidiate value, or leave this specialization to execute module
  //HACK: as condition used immidiate int value like a pointer to similar block, it should be available to count result and return > 0(true) or <= 0(false)
  Branch(PTree* condition = nullptr, PTree* parent = nullptr, PTree* left = nullptr, PTree* right = nullptr): 
  NonLeaf(parent, left, right), condition_(condition) {};
};

class IfBlk: public Branch {
  public:
  //HACK: if getleft() pointer == nullptr it means that else clause not exist, otherwise getright() pointer is true case, getleft() pointer is else case
  
  
  IfBlk(PTree* condition = nullptr, PTree* parent = nullptr, PTree* else_blk = nullptr, PTree* if_blk = nullptr): 
  Branch(condition, parent, else_blk, if_blk) {};
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    if (condition_ != nullptr) res += condition_->dump();

    res += getname() + "[shape = record, label=\"{If clause \\n |" +
    "{ " + get_addr(getleft()) + "\\n (false case) | " + get_addr(getright()) + "\\n(true case)}}\"]\n";

    //HACK: no get_links() here, because nodes should be colorized and with caption
    if (getleft() != nullptr) res += getname() + " -> " + getleft()->getname() + "[color=\"red\", label=\"false\"]\n";
    if (getright() != nullptr) res += getname() + " -> " + getright()->getname() + "[color=\"green\", label=\"true\"]\n";
    
    if (condition_ != nullptr) res += getname() + " -> " + condition_->getname() + " [style=dotted]\n";
    return res;
  }
};

class WhileBlk: public Branch {
  public:
  WhileBlk(PTree* condition = nullptr, PTree* parent = nullptr, PTree* while_blk = nullptr): Branch(condition, parent, while_blk, nullptr) {};
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    if (condition_ != nullptr) res += condition_->dump();

    res += getname() + "[shape = record, label=\"{While clause \\n |" +
    "{ " + get_addr(getleft()) + "\\n (cycle) | " + get_addr(getright()) + "\\n(depricated)}}\"]\n";

    res += get_links();
    if (condition_ != nullptr) res += getname() + " -> " + condition_->getname() + " [style=dotted, label=\"condition\"]\n";
    return res;
  }
};

class Output: public Operation {
  public:
  Output(PTree* parent = nullptr, PTree* to_print = nullptr): Operation(parent, nullptr, to_print) {};

  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    res += getname() + "[shape = record, label=\"{print (" + get_addr(getright()) + ")}\"]\n";
    res += get_links();
    return res;
  }
};

//TODO: remove when ast test will be finished
class Variable: public PTree {
  public:
  std::string name_;
  Variable(std::string name, PTree* parent = nullptr): PTree(parent, nullptr, nullptr), name_(name) {};
  std::string dump() const override {
    std::string res;
    res += getname() + "[label=\"" + name_ + "\"]\n";
    return res;
  }
};

};