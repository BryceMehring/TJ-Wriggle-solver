#ifndef _UCGS_WRIGGLER_GRID_
#define _UCGS_WRIGGLER_GRID_

#include "WrigglerGrid.h"
#include <functional>
#include <memory>

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

	// Applies Greedy Best First Graph Search to solve the wriggler puzzle
	// Returns all of the states in the graph
	// path: If a solution is found, 'path' will be filled with the nodes in the path, else, 'path' will not be modified
	// heuristic: a function which accepts an uvec2 and returns the estimated distance to the solution
	std::vector<std::unique_ptr<GBFGSWrigglerGrid>> GBFGS(std::deque<GBFGSWrigglerGrid*>& path, const std::function<int(const uvec2&)>& heuristic);

	GBFGSWrigglerGrid* m_pPrevious;
	WrigglerMove m_move;

	int m_iHCost;
	int m_iGCost;

	friend class GBFGSWrigglerGridSorter;
	friend class GBFGSWrigglerGridHash;
	friend class GBFGSWrigglerGridEqual;
};

// Defines how states are going to be sorted in the priority queue
class GBFGSWrigglerGridSorter
{
public:

	bool operator()(const GBFGSWrigglerGrid* a, const GBFGSWrigglerGrid* b) const;
};

// Defines the hash for a Wriggler State
class GBFGSWrigglerGridHash
{
public:

	std::size_t operator()(const GBFGSWrigglerGrid* data) const;
};

// Defines the equality check for Wriggler States
class GBFGSWrigglerGridEqual
{
public:

	std::size_t operator()(const GBFGSWrigglerGrid* a, const GBFGSWrigglerGrid* b) const;
};

#endif // _UCGS_WRIGGLER_GRID_
