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
  Leaf(PTree* parent = nullptr);
  virtual bool isLeaf() const override;
  virtual std::unique_ptr<PTree> execute(Stack *stack = nullptr) const override;
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

std::unique_ptr<PTree> intinput();

class Reserved : public Leaf {
public:
  enum Types {
    Input,
    None
  };
private:
  Types type_;
public:
  Reserved(PTree* parent = nullptr, Types type = Types::None);
  Types gettype() const;
  std::string typetostr() const;
  virtual std::unique_ptr<PTree> execute(Stack *stack = nullptr) const override;
  virtual std::string dump() const override;
};

class NameInfo : public Leaf {
private:
  int nameid_;
  int offset_;
  std::string name_;
protected:
  NameInfo(PTree* parent = nullptr, std::string name = "");
  NameInfo(PTree* parent, int nameid, int offset, std::string name = "");
public:
  int getnameid() const;
  int getoffset() const;
  void setoffset(int offset);
  std::string getvarname() const;
};

class NameInt : public NameInfo {
private:
  int value_;
public:
  NameInt(PTree* parent = nullptr, int value = 0, std::string name_ = "");
  NameInt(PTree* parent, int value, int nameid, int offset, std::string name_ = "");
  //returns value from the field that value equals to value in stack
  int getvalue() const;
  //returns value from the stack
  int getvalue(Stack *stack) const;
  void setvalue(int value, Stack *stack);

  virtual std::string dump() const override;
  std::unique_ptr<PTree> execute(Stack *stack) const override;
};

}