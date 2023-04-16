#pragma once

#include "ptree.hpp"
#include "stack.hpp"

#include <string>
#include <memory>
#include <iostream>
#include <memory>

namespace ptree {
class Leaf : public PTree {
public:
  Leaf(PTree* parent = nullptr): PTree(parent, nullptr, nullptr) {};
  virtual bool isLeaf() const override {
    return true;
  }
  virtual std::unique_ptr<PTree> execute(Stack *stack = nullptr) const override {
    return std::unique_ptr<PTree>{};
  }
};

template <typename T> class Imidiate : public Leaf {
private:
  T value_;
public:
  Imidiate(PTree* parent = nullptr, T value = T()) : Leaf(parent), value_(value) {}
  Imidiate(T value) : Leaf(nullptr), value_(value) {}
  T getvalue(Stack *stack = nullptr) const { //why stack?
    return value_;
  }
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = getparent()->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + std::to_string(value_) + '"' + "];\n";
    return res;
  }

  std::unique_ptr<PTree> execute(Stack *stack = nullptr) const override {
    return std::unique_ptr<PTree>{new Imidiate<int>(*this)}; 
  }
};

std::unique_ptr<PTree> intinput() {
  int x;
  std::cin >> x;
#ifdef DBG_CALL
  std::cout << "Input called" << std::endl;
#endif
  return std::unique_ptr<PTree>(new Imidiate<int>(x)); 
}

class Reserved : public Leaf {
public:
  enum Types {
    Input,
    None
  };
private:
  Types type_;
public:
  Reserved(PTree* parent = nullptr, Types type = Types::None) : Leaf(parent), type_(type) {}
  Types gettype() const {
    return type_;
  }
  std::string typetostr() const {
    switch(gettype()) {
      case Types::None:
        return "None";
        break;
      case Types::Input:
        return "Input";
        break;
    }
    return "Smth strange";
  }
  virtual std::unique_ptr<PTree> execute(Stack *stack = nullptr) const override {
#ifdef DBG_CALL
    std::cout << "Resrved called" << std::endl;
#endif
    switch(gettype()) {
      case Types::None:
        return std::unique_ptr<PTree>{};
      case Types::Input:
        return intinput();
    }
    return std::unique_ptr<PTree>{};
  }
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = getparent()->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + typetostr() + '"' + "];\n";
    return res;
  }
};

class NameInfo : public Leaf {
private:
  int nameid_;
  int offset_;
  std::string name_;
protected:
  NameInfo(PTree* parent = nullptr, std::string name = "") : Leaf(parent), name_(name) {};
  NameInfo(PTree* parent, int nameid, int offset, std::string name = "") : Leaf(parent), nameid_(nameid), offset_(offset), name_(name) {};
public:
  int getnameid() const {
    return nameid_;
  }
  int getoffset() const {
    return offset_;
  }
  void setoffset(int offset) {
    offset_ = offset;
  }
  std::string getvarname() const {
    return name_;
  }
};

class NameInt : public NameInfo {
private:
  int value_;
public:
  NameInt(PTree* parent = nullptr, int value = 0, std::string name_ = "") : NameInfo(parent, name_), value_(value) {};
  NameInt(PTree* parent, int value, int nameid, int offset, std::string name_ = "") : NameInfo(parent, nameid, offset, name_), value_(value) {};
  int getvalue() const { //returns value from the field that value equals to value in stack
    return value_;
  }
  int getvalue(Stack *stack) const { //returns value from the stack
    int value;
    stack->read(getoffset(), value);
    return value;
  }
  void setvalue(int value, Stack *stack) {
    stack->write(getoffset(), value);
    value_ = value;
  }
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = getparent()->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + getvarname() + '=' + std::to_string(getvalue()) + '\n' + "offset: " + std::to_string(getoffset()) +'"' + "];\n";
    return res;
  }

  std::unique_ptr<PTree> execute(Stack *stack) const override {
    return std::unique_ptr<PTree>{new Imidiate<int>(getvalue(stack))};
  }
};



}