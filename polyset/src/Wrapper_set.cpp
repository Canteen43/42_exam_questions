#include "../inc/Wrapper_set.hpp"
#include <iostream>
set::set(const searchable_bag& ref)
{
	if (dynamic_cast<const searchable_array_bag*>(&ref))
	{
		inner = new searchable_array_bag;
		std::cout << "Array!\n";
	}
	else
	{
		inner = new searchable_tree_bag;
		std::cout << "Tree!\n";
	}

}
set::~set()
{
	searchable_array_bag* tmp1 = dynamic_cast<searchable_array_bag*>(inner);
	searchable_tree_bag* tmp2 = dynamic_cast<searchable_tree_bag*>(inner);
	if (tmp1)
		delete tmp1;
	else
		delete tmp2;
}