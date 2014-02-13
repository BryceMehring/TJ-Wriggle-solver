#ifndef _UCGS_WRIGGLER_GRID_
#define _UCGS_WRIGGLER_GRID_

#include "WrigglerGrid.h"
#include <functional>

class GBFGSWrigglerGrid : public WrigglerGrid
{
public:

	friend class GBFGSWrigglerGridSorter;
	friend class GBFGSWrigglerGridHash;
	friend class GBFGSWrigglerGridEqual;

	// Constructs an empty puzzle
	GBFGSWrigglerGrid();

	// Constructs the puzzle from the specified file
	GBFGSWrigglerGrid(const std::string& file);

	// Solves the wriggler puzzle using GBFGS and prints the solution
	virtual void RunAI();

private:

	void GBFGS(std::deque<GBFGSWrigglerGrid*>& path, const std::function<int(const uvec2&)>& heuristic);

	GBFGSWrigglerGrid* m_pPrevious = nullptr;

	WrigglerMove m_move;

	int m_iHCost = 0;
	int m_iGCost = 0;
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
