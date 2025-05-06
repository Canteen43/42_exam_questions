#pragma once
#include "../inc/Derived2T_tree_bag.hpp"
#include "../inc/Derived1_searchable_bag.hpp"
class searchable_tree_bag final : public tree_bag, public searchable_bag
{
	public:
		bool has(int) {return true;};
		searchable_tree_bag();
};