#include "vect2.hpp"
// Default constructor
vect2::vect2()
: x(0), y(0)
{};
// Parametric constructor
vect2::vect2(int xNew, int yNew)
: x(xNew), y(yNew)
{};
// Copy constructor
vect2::vect2(vect2& rhs)
: x(rhs.x), y(rhs.y)
{};
// Copy assignment operator
vect2& vect2::operator=(vect2& rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
};
// Destructor
vect2::~vect2()
{};


// Operator overload for []
int& vect2::operator[](int index)
{
	if (index == 0)
		return x;
	else
		return y;
};
// Operator overload for []
const int& vect2::operator[](int index) const
{
	if (index == 0)
		return x;
	else
		return y;
};


// Operator overload for +=
vect2& vect2::operator+=(const vect2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
};
// Operator overload for +
vect2 vect2::operator+(const vect2& rhs)
{
	vect2 result(*this);
	result += rhs;
	return result;
};
// Operator overload for ++ (pre)
vect2& vect2::operator++()
{
	x += 1;
	y += 1;
	return *this;
}
// Operator overload for ++ (post)
vect2 vect2::operator++(int)
{
	vect2 temp(*this);
	x += 1;
	y += 1;
	return temp;
};


// Operator overload for -=
vect2& vect2::operator-=(const vect2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
};
// Operator overload for -
vect2 vect2::operator-(const vect2& rhs)
{
	vect2 result(*this);
	result -= rhs;
	return result;
};
// Operator overload for -- (pre)
vect2& vect2::operator--()
{
	x -= 1;
	y -= 1;
	return *this;
}
// Operator overload for -- (post)
vect2 vect2::operator--(int)
{
	vect2 temp(*this);
	x -= 1;
	y -= 1;
	return temp;
};


// Operator overload for *=
vect2& vect2::operator*=(const int rhs)
{
	x *= rhs;
	y *= rhs;
	return *this;
};
// Operator overload for *
vect2 vect2::operator*(const int rhs)
{
	vect2 result(*this);
	result *= rhs;
	return result;
};


// Operator overload for ==
bool vect2::operator==(const vect2& rhs)
{
	return (x == rhs.x && y == rhs.y);
};
// Operator overload for !=
bool vect2::operator!=(const vect2& rhs)
{
	return (x != rhs.x || y != rhs.y);
};


// Operator overload for - (unary)
vect2 vect2::operator-()
{
	vect2 result(*this);
	result *= -1;
	return result;
};


// Operator overload for <<
std::ostream& operator<<(std::ostream& lhs, const vect2& rhs)
{
	lhs << "{" << rhs[0] << ", " << rhs[1] << "}";
	return lhs;
};