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
private:
  Types type;
public:
  Reserved(PTree* _parent = nullptr, Types _type = Types::None) : Leaf(_parent), type(_type) {}
  Types gettype() const {
    return type;
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
  T getvalue() const {
    return value;
  }
  virtual std::string dump() const override {
    std::string res;
    std::string parentname = getparent()->getname();
    std::string myname = getname();
    res += myname + " [label=" + '"' + std::to_string(value) + '"' + "];\n";
    return res;
  }
};

class NameInfo : public Leaf {
private:
  int nameid;
  int offset;
protected:
  NameInfo(PTree* _parent = nullptr) : Leaf(_parent) {};
  NameInfo(PTree* _parent, int _nameid, int _offset) : Leaf(_parent), nameid(_nameid), offset(_offset) {};
public:
  int getnameid() const {
    return nameid;
  }
  int getoffset() const {
    return offset;
  }
};

class NameInt : public NameInfo {
private:
  int value;
public:
  NameInt(PTree* _parent = nullptr, int _value = 0) : NameInfo(_parent), value(_value) {};
  NameInt(PTree* _parent, int _value, int _nameid, int _offset) : NameInfo(_parent, _nameid, _offset), value(_value) {};
  int getvalue() const {
    return value;
  }
  void setvalue(int x) {
    value = x;
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