#pragma once
#include "../inc/Derived2A_array_bag.hpp"
#include "../inc/Derived1_searchable_bag.hpp"
class searchable_array_bag final : public array_bag, public searchable_bag
{
	public:
		bool has(int) {return true;};
		searchable_array_bag();
};