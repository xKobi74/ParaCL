#pragma once

#include "leaf.hpp"

#include <typeinfo>

TEST(PTree, MainTest) {
	ptree::PTree pt;
	int id = pt.getid();
	std::string name = pt.getname();
}

TEST(Leaf, MainTest) {
	ptree::Leaf leaf;
	ASSERT_TRUE(leaf.isLeaf());
	ASSERT_EQ(leaf.execute(), &leaf);
}

TEST(LeafImidiate, ConstructorTestInt) {
	ptree::Imidiate<int> x(10);
	ASSERT_STREQ(typeid(x.getvalue()).name(), typeid(static_cast<int>(10)).name());
	ASSERT_EQ(x.getvalue(), 10);
}

TEST(LeafImidiate, DumpTest) {
	ptree::PTree pt;
	ptree::Imidiate<int> im(10);
	im.parent = &pt;
	std::string dump = im.dump();
}


TEST(LeafReserved, ConstructorTestDefault) {
	ptree::Reserved r;
	ptree::Reserved::Types type = ptree::Reserved::Types::None;
	ASSERT_EQ(r.type, type);
}
TEST(LeafReserved, ConstructorTestNone) {
	ptree::Reserved r(ptree::Reserved::Types::None);
	ptree::Reserved::Types type = ptree::Reserved::Types::None;
	ASSERT_EQ(r.type, type);
}
TEST(LeafReserved, ConstructorTestInput) {
	ptree::Reserved r(ptree::Reserved::Types::Input);
	ptree::Reserved::Types type = ptree::Reserved::Types::Input;
	ASSERT_EQ(r.type, type);
}

TEST(LeafReserved, DumpTest) {
	ptree::PTree pt;
	ptree::Reserved r(ptree::Reserved::Types::Input);
	r.parent = &pt;
	std::string dump = r.dump();
}

TEST(LeafName, ConstructorTestInt1) {
	ptree::Name<int> v(7);
	ASSERT_STREQ(typeid(v.value).name(), typeid(static_cast<int>(7)).name());
	ASSERT_EQ(v.value, 7);
}
TEST(LeafName, ConstructorTestInt2) {
	ptree::Name<int> v(7, 11, 9);
	ASSERT_STREQ(typeid(v.value).name(), typeid(static_cast<int>(7)).name());
	ASSERT_EQ(v.value, 7);
	ASSERT_EQ(v.nameid, 11);
	ASSERT_EQ(v.offset, 9);
}

TEST(LeafName, DumpTest) {
	ptree::PTree pt;
	ptree::Imidiate<int> var(10);
	var.parent = &pt;
	std::string dump = var.dump();
}