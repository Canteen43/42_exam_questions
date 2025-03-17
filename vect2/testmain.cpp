#include "vect2.hpp"
#include <iostream>

int main()
{
	vect2 ex1;
	std::cout << ex1 << "\n";
	vect2 ex2(5, 3);
	std::cout << ex2 << "\n";
	ex1 = ex2;
	std::cout << ex1 << "\n";
	ex1 += ex1 + ex2;
	std::cout << ex1 << "\n";
	ex1 -= ex1 - ex2;
	std::cout << ex1 << "\n";
	ex1++;
	std::cout << ex1 << "\n";
	ex1--;
	std::cout << ex1 << "\n";
	std::cout << ex1++ << "\n";
	std::cout << ex1-- << "\n";
	std::cout << ex1 << "\n";
	ex1[0] = 1;
	ex1[1] = 2;
	std::cout << ex1 << "\n";
	std::cout << ex1[0] << "\n";
	ex1 *= -2;
	std::cout << ex1 << "\n";
	std::cout << ex1 * 2 << "\n";
	std::cout << (ex1 == ex2) << "\n";
	std::cout << (ex1 != ex2) << "\n";
	std::cout << -ex1 << "\n";
}