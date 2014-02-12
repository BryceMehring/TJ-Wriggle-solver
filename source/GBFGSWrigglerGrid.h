#ifndef _UCGS_WRIGGLER_GRID_
#define _UCGS_WRIGGLER_GRID_

#include "WrigglerGrid.h"
#include <functional>

class Node;

class GBFGSWrigglerGrid : public WrigglerGrid
{
public:
	// Constructs an empty puzzle
	GBFGSWrigglerGrid();

	// Constructs the puzzle from the specified file
	GBFGSWrigglerGrid(const std::string& file);

	// Solves the wriggler puzzle using GBFGS and prints the solution
	virtual void RunAI();

private:

	Node* GBFGS(const std::function<int(const uvec2&)>& heuristic) const;

};

// Defines a node that is part of a search tree
struct Node
{
	Node(Node* pPrev, const GBFGSWrigglerGrid& s, WrigglerMove m, int h, int g) : pPrevious(pPrev), state(s), move(m), hCost(h), gCost(g)
	{
	}

	// Pointer to the previous Node in the tree
	Node* pPrevious;

	GBFGSWrigglerGrid state;

	// Movement from the previous state to this state
	WrigglerMove move;

	int hCost;
	int gCost;
};

class NodeComparer
{
public:

	bool operator()(const Node* a, const Node* b) const
	{
		return (a->hCost) > (b->hCost);
	}
};

#endif // _UCGS_WRIGGLER_GRID_
