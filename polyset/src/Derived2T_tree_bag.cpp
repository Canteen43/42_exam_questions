#include "../inc/Derived2T_tree_bag.hpp"
tree_bag::tree_bag()
{
	data = new int[10];
}
tree_bag::~tree_bag()
{
	delete[] data;
}