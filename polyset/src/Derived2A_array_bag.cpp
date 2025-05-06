#include "../inc/Derived2A_array_bag.hpp"
array_bag::array_bag()
{
	data = new int[10];
}
array_bag::~array_bag()
{
	delete[] data;
}