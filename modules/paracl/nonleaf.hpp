#pragma once

#include "ptree.hpp"


#include <string>
#include <sstream>
//CPP20 is not needed because most compilators doesn`t accept format lib
#ifdef CPP20
#include <format>
#endif

namespace ptree {

class NonLeaf: public PTree {
  public:
  
  NonLeaf(PTree* parent_ = nullptr, PTree* left_ = nullptr, PTree* right_ = nullptr): PTree(parent_, left_, right_) {};
  
  std::string get_links() const {
    std::string res{""};
    if (left != nullptr) res += getname() + " -> " + left->getname() + "\n";
    if (right != nullptr) res += getname() + " -> " + right->getname() + "\n";
    return res;
  }
  
  inline std::string get_chld_dump() const {
    std::string res;
    if (left != nullptr) res += left->dump();
    if (right != nullptr) res += right->dump();
    return res;
  }

  bool isLeaf() const override {
    return false;
  } 
  
  PTree* execute() override {
    return this;
  }
};

class Operation : public NonLeaf {
  public:
  Operation(PTree* parent_ = nullptr, PTree* left_ = nullptr, PTree* right_ = nullptr): NonLeaf(parent_, left_, right_) {};
};
//TODO: maybe I should point it like BinOpType: std::string
enum class BinOpType {
  ADDITION,
  SUBTRACTION,
  MULTIPLICATION,
  DIVISION,
  UNDEF,
};

class BinOp: public Operation {
  public:
  BinOpType operation_;
  BinOp(BinOpType operation = BinOpType::UNDEF, PTree* parent_ = nullptr, PTree* left_ = nullptr, PTree* right_ = nullptr): 
        Operation(parent_, left_, right_), operation_(operation) {};
  
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
      default:
        return "?";  
    }

  }
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    res += getname() + "[shape = record, label=\"{Binary operation(" + get_op() + ")|" +
    "{ " + get_addr(left) + " | " + get_addr(right) + "}}\"]\n";
    res += get_links();  
    return res;
  } 
};

enum class UnOpType {
  ADDITION,
  SUBTRACTION,
  UNDEF
};

class UnOp: public Operation {
  public:
  UnOpType operation_;
  //HACK: right pointer depricated because unary operation has only one operand to count value
  //FIXME: Now we can easily make ++i operator, but i++ op needs some more modifications
  UnOp(UnOpType operation = UnOpType::UNDEF, PTree* parent_ = nullptr, PTree* left_ = nullptr): 
        Operation(parent_, left_, nullptr), operation_(operation) {};
  
  std::string get_op() const {
    switch (operation_) {
      case UnOpType::ADDITION:
        return "++";
      case UnOpType::SUBTRACTION:
        return "--";
      default:
        return "?";  
    }

  }
  
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    res += getname() + "[shape = record, label=\"{Unary operation(" + get_op() + ")|" +
    "{ " + get_addr(left) + " | " + get_addr(right) + "\\n(depricated)}}\"]\n";
    res += get_links();  
    return res;
  } 
};
/* left pointer - process inside block, right pointer - outer process, continuing of main programm
*/
class Block: public NonLeaf {
  public:
  using offset_t = unsigned long;
  using block_id = int;
  offset_t offset_;
  block_id id_;

  Block(offset_t offset = 0x0, block_id id = -1, PTree* parent_ = nullptr, PTree* left_ = nullptr, PTree* right_ = nullptr):
  NonLeaf(parent_, left_, right_), offset_(offset), id_(id) {};

  std::string get_blk_info() const {
    std::string res;
#ifdef CPP20
    res += std::format("offset = {:X} \\n block id = {}", offset_, id_);
#else
    res += "offset = " + std::to_string(offset_) + "\\n block id = " + std::to_string(id_);
#endif
  return res;
  }
  
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    res += getname() + "[shape = record, label=\"{Block \\n" + get_blk_info() +"|" +
    "{ " + get_addr(left) + " | " + get_addr(right) + "\\n(depricated)}}\"]\n";
    res += get_links();
    return res;
  }

};

class Branch: public NonLeaf {
  public:
  PTree* condition_;
  //FIXME: specify condition type as Immidiate value, or leave this specialization to execute module
  //HACK: as condition used immidiate int value like a pointer to similar block, it should be available to count result and return > 0(true) or <= 0(false)
  Branch(PTree* condition = nullptr, PTree* parent_ = nullptr, PTree* left_ = nullptr, PTree* right_ = nullptr): 
  NonLeaf(parent_, left_, right_), condition_(condition) {};
};

class IfBlk: public Branch {
  public:
  //HACK: if left pointer == nullptr it means that else clause not exist, otherwise right pointer is true case, left pointer is else case
  
  
  IfBlk(PTree* condition = nullptr, PTree* parent_ = nullptr, PTree* left_ = nullptr, PTree* right_ = nullptr): 
  Branch(condition, parent_, left_, right_) {};
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    if (condition_ != nullptr) res += condition_->dump();

    res += getname() + "[shape = record, label=\"{If clause \\n |" +
    "{ " + get_addr(left) + "\\n (false case) | " + get_addr(right) + "\\n(true case)}}\"]\n";

    //HACK: no get_links() here, because nodes should be colorized and with caption
    if (left != nullptr) res += getname() + " -> " + left->getname() + "[color=\"red\", label=\"false\"]\n";
    if (right != nullptr) res += getname() + " -> " + right->getname() + "[color=\"green\", label=\"true\"]\n";
    
    if (condition_ != nullptr) res += getname() + " -> " + condition_->getname() + " [style=dotted]\n";
    return res;
  }
};

class WhileBlk: public Branch {
  public:
  WhileBlk(PTree* condition = nullptr, PTree* parent_ = nullptr, PTree* left_ = nullptr): Branch(condition, parent_, left_, nullptr) {};
  std::string dump() const override {
    std::string res;
    res += get_chld_dump();
    if (condition_ != nullptr) res += condition_->dump();

    res += getname() + "[shape = record, label=\"{While clause \\n |" +
    "{ " + get_addr(left) + "\\n (cycle) | " + get_addr(right) + "\\n(depricated)}}\"]\n";

    //HACK: no get_links() here, because nodes should be colorized and with caption
    if (left != nullptr) res += getname() + " -> " + left->getname() + "\n";
    
    if (condition_ != nullptr) res += getname() + " -> " + condition_->getname() + " [style=dotted]\n";
    return res;
  }
};

};