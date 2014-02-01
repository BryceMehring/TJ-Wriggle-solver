#ifndef _WRIGGLER_
#define _WRIGGLER_

#include <deque>
#include "vec2.h"

enum Direction
{
	Up,
	Down,
	Left,
	Right
};

struct Wriggler
{
	std::deque<uvec2> positions;
	unsigned int id;
};

bool operator==(const Wriggler& a, const Wriggler& b);
bool operator!=(const Wriggler& a, const Wriggler& b);

// Defines a hash function for a wriggler
class WrigglerHash
{
public:

	std::size_t operator()(const Wriggler& a) const;
};

// Defines a movement action for a wriggler
struct WrigglerMove
{
	WrigglerMove() : w(0), h(false), d(Up)
	{
	}

	WrigglerMove(unsigned int wrigIndex, bool bHead, Direction dir) : w(wrigIndex), h(bHead), d(dir)
	{
	}

	// Wriggler index
	unsigned int w;

	// Bit indicating whether the head is being moved
	bool h;

	// Direction of movement
	Direction d;
};

#endif // _WRIGGLER_