#pragma once
#include <ostream>

class vect2
{
	private:
	int x;
	int y;
	public:
	// Default constructor
	vect2();
	// Parametric constructor
	vect2(int xNew, int yNew);
	// Copy constructor
	vect2(vect2& rhs);
	// Copy assignment operator
	vect2& operator=(vect2& rhs);
	// Destructor
	~vect2();
	
	// Operator overload for []
	int& operator[](int index);
	// Operator overload for []
	const int& operator[](int index) const;

	// Operator overload for +=
	vect2& operator+=(const vect2& rhs);
	// Operator overload for +
	vect2 operator+(const vect2& rhs);
	// Operator overload for ++ (pre)
	vect2& operator++();
	// Operator overload for ++ (post)
	vect2 operator++(int);

	// Operator overload for -=
	vect2& operator-=(const vect2& rhs);
	// Operator overload for -
	vect2 operator-(const vect2& rhs);
	// Operator overload for -- (pre)
	vect2& operator--();
	// Operator overload for -- (post)
	vect2 operator--(int);

	// Operator overload for *=
	vect2& operator*=(const int rhs);
	// Operator overload for *
	vect2 operator*(const int rhs);

	// Operator overload for ==
	bool operator==(const vect2& rhs);
	// Operator overload for !=
	bool operator!=(const vect2& rhs);

	// Operator overload for - (unary)
	vect2 operator-();
};
// Operator overload for <<
std::ostream& operator<<(std::ostream& lhs, const vect2& rhs);