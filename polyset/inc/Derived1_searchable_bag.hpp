#pragma once
#include "../inc/Base_bag.hpp"
class searchable_bag : virtual public bag
{
	protected:
		virtual bool has(int) = 0;
};