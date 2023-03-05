#pragma once

#include "ptree.hpp"

#include <string>

namespace ptree {
class Leaf : public PTree {
public:
  Leaf(PTree* parent_ = nullptr): PTree(parent_, nullptr, nullptr) {};
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
  Types type;
  Reserved(PTree* _parent = nullptr, Types _type = Types::None) : Leaf(_parent), type(_type) {}
  std::string typetostr() const {
    switch(type) {
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
    std::string parentname = parent->getname();
    std::string myname = getname();
    //TODO: res undefined here
    res += myname + " [label=" + '"' + typetostr() + '"' + "];\n";
    return res;
  }
};

template <typename T> class Imidiate : public Leaf {
private:
  T value;
public:
  Imidiate(PTree* _parent = nullptr, T _value = T()) : Leaf(_parent), value(_value) {}
  T getvalue() const{
    return value;
  }
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = parent->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + std::to_string(value) + '"' + "];\n";
    return res;
  }
};

template <typename T>
class Name : public Leaf {
public:
  T value;
  int nameid;
  int offset;
  Name(PTree* _parent = nullptr, T _value = T()) : Leaf(_parent), value(_value) {};
  Name(PTree* _parent, T _value, int _nameid, int _offset) : Leaf(_parent), value(_value), nameid(_nameid), offset(_offset) {};
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = parent->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + std::to_string(nameid) + '"' + "];\n";
    return res;
  }
};

}