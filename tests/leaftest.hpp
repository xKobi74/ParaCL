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
	ASSERT_EQ(x.getparent(), &pt);
}

TEST(LeafImidiate, DumpTest) {
	ptree::PTree pt;
	ptree::Imidiate<int> im(&pt, 10);
	std::string dump = im.dump();
	ASSERT_EQ(im.getparent(), &pt);
}


TEST(LeafReserved, ConstructorTestDefault) {
	ptree::Reserved r;
	ptree::Reserved::Types type = ptree::Reserved::Types::None;
	ASSERT_EQ(r.gettype(), type);
	ASSERT_EQ(r.getparent(), nullptr);
}
TEST(LeafReserved, ConstructorTestNone) {
	ptree::PTree pt;
	ptree::Reserved r(&pt, ptree::Reserved::Types::None);
	ptree::Reserved::Types type = ptree::Reserved::Types::None;
	ASSERT_EQ(r.gettype(), type);
	ASSERT_EQ(r.getparent(), &pt);
}
TEST(LeafReserved, ConstructorTestInput) {
	ptree::PTree pt;
	ptree::Reserved r(&pt, ptree::Reserved::Types::Input);
	ptree::Reserved::Types type = ptree::Reserved::Types::Input;
	ASSERT_EQ(r.gettype(), type);
	ASSERT_EQ(r.getparent(), &pt);
}

TEST(LeafReserved, DumpTest) {
	ptree::PTree pt;
	ptree::Reserved r(&pt, ptree::Reserved::Types::Input);
	std::string dump = r.dump();
	ASSERT_EQ(r.getparent(), &pt);
}

TEST(NameInt, ConstructorTestInt1) {
	ptree::PTree pt;
	ptree::NameInt v(&pt, 7);
	ASSERT_EQ(v.getvalue(), 7);
	ASSERT_EQ(v.getparent(), &pt);
}
TEST(NameInt, ConstructorTestInt2) {
	ptree::PTree pt;
	ptree::NameInt v(&pt, 10, 11, 12);
	std::string dump = v.dump();
	ASSERT_EQ(v.getvalue(), 10);
	ASSERT_EQ(v.getnameid(), 11);
	ASSERT_EQ(v.getoffset(), 12);
	ASSERT_EQ(v.getparent(), &pt);
}

TEST(NameInt, DumpTest) {
	ptree::PTree pt;
	ptree::NameInt v(&pt, 10);
	std::string dump = v.dump();
	ASSERT_EQ(v.getparent(), &pt);
}

