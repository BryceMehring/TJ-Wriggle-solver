#ifndef _ID_DFTS_WRIGGLER_GRID_
#define _ID_DFTS_WRIGGLER_GRID_

#include "WrigglerGrid.h"
#include <memory>
#include <map>

// Defines a node that is part of a search tree
struct IDNode
{
	IDNode() : pPrevious(nullptr)
	{
	}

	// Pointer to the previous Node in the tree
	IDNode* pPrevious;

	std::deque<uvec2> positions;

	// Movement from the previous state to this state
	WrigglerMove move;
};

enum class SearchResult
{
	Success,
	Failure,
	Cutoff
};

class ID_DFTSWrigglerWrig : public WrigglerGrid
{
public:

	// Constructs an empty puzzle
	ID_DFTSWrigglerWrig();

	// Constructs the puzzle from the specified file
	ID_DFTSWrigglerWrig(const std::string& file);

	// Loads puzzle description from the specified file
	// The old state of the grid is discarded
	bool Load(const std::string& file);

	// Solves the wriggler puzzle using BFTS and prints the solution
	virtual void RunAI();

	// Depth Limited Search
	// Returns the target node if found, else nullptr
	// pNode: node to apply dls too
	// depth: the depth limit
	SearchResult DLS(int depth, std::deque<std::unique_ptr<IDNode>>& path);
	SearchResult DLS(IDNode** pSolution, IDNode* pNode, std::map<std::vector<std::vector<char>>,int>& states, int depth);
};

#endif // _ID_DFTS_WRIGGLER_GRID_
