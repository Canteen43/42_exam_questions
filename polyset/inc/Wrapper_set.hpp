#pragma once
#include "../inc/UnitedA_searchable_array_bag.hpp"
#include "../inc/UnitedT_searchable_tree_bag.hpp"
class set
{
	private:
		searchable_bag* inner;
	public:
		set(const searchable_bag& ref);
		~set();
};