#pragma once

#include "../modules/paracl/leaf.hpp"

#include <typeinfo>

TEST(PTree, MainTest) {
	ptree::PTree pt;
	std::string id = pt.getid();
	std::string name = pt.getname();
}

TEST(Leaf, MainTest) {
	ptree::Leaf leaf;
	ASSERT_TRUE(leaf.isLeaf());
	ASSERT_EQ(leaf.execute(), &leaf);
}

TEST(LeafImidiate, ConstructorTestInt) {
	ptree::PTree pt;
	ptree::Imidiate<int> x(&pt, 10);
	ASSERT_STREQ(typeid(x.getvalue()).name(), typeid(static_cast<int>(10)).name());
	ASSERT_EQ(x.getvalue(), 10);
	ASSERT_EQ(x.parent, &pt);
}

TEST(LeafImidiate, DumpTest) {
	ptree::PTree pt;
	ptree::Imidiate<int> im(&pt, 10);
	std::string dump = im.dump();
	ASSERT_EQ(im.parent, &pt);
}


TEST(LeafReserved, ConstructorTestDefault) {
	ptree::Reserved r;
	ptree::Reserved::Types type = ptree::Reserved::Types::None;
	ASSERT_EQ(r.type, type);
	ASSERT_EQ(r.parent, nullptr);
}
TEST(LeafReserved, ConstructorTestNone) {
	ptree::PTree pt;
	ptree::Reserved r(&pt, ptree::Reserved::Types::None);
	ptree::Reserved::Types type = ptree::Reserved::Types::None;
	ASSERT_EQ(r.type, type);
	ASSERT_EQ(r.parent, &pt);
}
TEST(LeafReserved, ConstructorTestInput) {
	ptree::PTree pt;
	ptree::Reserved r(&pt, ptree::Reserved::Types::Input);
	ptree::Reserved::Types type = ptree::Reserved::Types::Input;
	ASSERT_EQ(r.type, type);
	ASSERT_EQ(r.parent, &pt);
}

TEST(LeafReserved, DumpTest) {
	ptree::PTree pt;
	ptree::Reserved r(&pt, ptree::Reserved::Types::Input);
	std::string dump = r.dump();
	ASSERT_EQ(r.parent, &pt);
}

TEST(LeafName, ConstructorTestInt1) {
	ptree::PTree pt;
	ptree::Name<int> v(&pt, 7);
	ASSERT_STREQ(typeid(v.value).name(), typeid(static_cast<int>(7)).name());
	ASSERT_EQ(v.value, 7);
	ASSERT_EQ(v.parent, &pt);
}
TEST(LeafName, ConstructorTestInt2) {
	ptree::PTree pt;
	ptree::Name<int> v(&pt, 7, 11, 9);
	ASSERT_STREQ(typeid(v.value).name(), typeid(static_cast<int>(7)).name());
	ASSERT_EQ(v.value, 7);
	ASSERT_EQ(v.nameid, 11);
	ASSERT_EQ(v.offset, 9);
	ASSERT_EQ(v.parent, &pt);
}

TEST(LeafName, DumpTest) {
	ptree::PTree pt;
	ptree::Imidiate<int> var(&pt, 10);
	std::string dump = var.dump();
	ASSERT_EQ(var.parent, &pt);
}
