#include "Wriggler.h"
#include <functional>

bool operator==(const Wriggler& a, const Wriggler& b)
{
	return ((a.id == b.id) && (a.positions == b.positions));
}

bool operator!=(const Wriggler& a, const Wriggler& b)
{
	return !::operator==(a, b);
}

std::size_t WrigglerHash::operator()(const Wriggler& a) const
{
	const unsigned int p1 = 73856093;
	const unsigned int p2 = 83492791;
	std::size_t h = 5381;
	for (auto iter : a.positions)
	{
		h += ((iter.x * p1) ^ (iter.y * p2));
	}

	h += std::hash<unsigned int>()(a.id);

	return h;
}
