#pragma once
#include "../inc/Base_bag.hpp"
class tree_bag : virtual public bag
{
	protected:
		int* data;
	public:
		tree_bag();
		~tree_bag();
};