#ifndef _UCGS_WRIGGLER_GRID_
#define _UCGS_WRIGGLER_GRID_

#include "WrigglerGrid.h"
#include <functional>
#include <memory>

class GraphSearchWrigglerGrid : public WrigglerGrid
{
public:

	// Constructs an empty puzzle
	GraphSearchWrigglerGrid();

	// Constructs the puzzle from the specified file
	GraphSearchWrigglerGrid(const std::string& file);

	// Solves the wriggler puzzle using GBFGS and prints the solution
	virtual void RunAI();

private:

	// Applies Greedy Best First Graph Search or Uniform Cost Graph Search to solve the wriggler puzzle
	// To change the mode of operation, call GBFGSWrigglerGridSorter::SetMode()
	// Returns all of the states in the graph
	// path: If a solution is found, 'path' will be filled with the nodes in the path, else, 'path' will not be modified
	// heuristic: a function which accepts an uvec2 and returns the estimated distance to the solution
	std::vector<std::unique_ptr<GraphSearchWrigglerGrid>> GBFGS(std::deque<GraphSearchWrigglerGrid*>& path, const std::function<int(const uvec2&)>& heuristic);

	GraphSearchWrigglerGrid* m_pPrevious;
	WrigglerMove m_move;

	int m_iGCost;
	int m_iHCost;

	friend class GraphSearchWrigglerGridSorter;
	friend class GraphSearchWrigglerGridHash;
	friend class GraphSearchWrigglerGridEqual;
};

// Defines how states are going to be ordered in the priority queue
class GraphSearchWrigglerGridSorter
{
public:

	enum class Mode
	{
		UCGS, // Only use the path cost
		GBFGS // Only use the heuristic
	};

	// Sets the ordering mode
	// GBFGS is the default mode
	static void SetMode(Mode mode);

	// Returns the current ordering mode
	static Mode GetMode();

	bool operator()(const GraphSearchWrigglerGrid* a, const GraphSearchWrigglerGrid* b) const;

private:

	static Mode s_state;
};

// Defines the hash for a Wriggler State
class GraphSearchWrigglerGridHash
{
public:

	std::size_t operator()(const GraphSearchWrigglerGrid* data) const;
};

// Defines the equality check for Wriggler States
class GraphSearchWrigglerGridEqual
{
public:

	std::size_t operator()(const GraphSearchWrigglerGrid* a, const GraphSearchWrigglerGrid* b) const;
};

#endif // _UCGS_WRIGGLER_GRID_
