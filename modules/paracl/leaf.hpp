#pragma once

#include "ptree.hpp"

#include <string>

namespace ptree {
class Leaf : public PTree {
public:
  Leaf(PTree* parent = nullptr): PTree(parent, nullptr, nullptr) {};
  bool isLeaf() const override {
    return true;
  }
  virtual PTree* execute() {
    return this;
  }
};

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
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = getparent()->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + typetostr() + '"' + "];\n";
    return res;
  }
};

template <typename T> class Imidiate : public Leaf {
private:
  T value_;
public:
  Imidiate(PTree* parent = nullptr, T value = T()) : Leaf(parent), value_(value) {}
  T getvalue() const {
    return value_;
  }
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = getparent()->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + std::to_string(value_) + '"' + "];\n";
    return res;
  }
};

class NameInfo : public Leaf {
private:
  int nameid_;
  int offset_;
protected:
  NameInfo(PTree* parent = nullptr) : Leaf(parent) {};
  NameInfo(PTree* parent, int nameid, int offset) : Leaf(parent), nameid_(nameid), offset_(offset) {};
public:
  int getnameid() const {
    return nameid_;
  }
  int getoffset() const {
    return offset_;
  }
};

class NameInt : public NameInfo {
private:
  int value_;
public:
  NameInt(PTree* parent = nullptr, int value = 0) : NameInfo(parent), value_(value) {};
  NameInt(PTree* parent, int value, int nameid, int offset) : NameInfo(parent, nameid, offset), value_(value) {};
  int getvalue() const {
    return value_;
  }
  void setvalue(int x) {
    value_ = x;
  }
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = getparent()->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + std::to_string(getnameid()) + '=' + std::to_string(getvalue()) + '"' + "];\n";
    return res;
  }
};

}