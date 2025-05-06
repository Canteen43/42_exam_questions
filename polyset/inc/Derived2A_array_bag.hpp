#pragma once
#include "../inc/Base_bag.hpp"
class array_bag : virtual public bag
{
	protected:
		int* data;
	public:
		array_bag();
		~array_bag();
};