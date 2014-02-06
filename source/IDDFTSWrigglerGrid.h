#ifndef _IDDFTS_WRIGGLER_GRID_
#define _IDDFTS_WRIGGLER_GRID_

#include "WrigglerGrid.h"
#include <memory>
#include <map>

// Defines search results from Depth Limited Search
enum class SearchResult
{
	Success, // A solution is found
	Failure, // There is no solution
	Cutoff // No solution is found yet because of the depth limit
};

// IDDFTS Wriggler AI
class ID_DFTSWrigglerWrig : public WrigglerGrid
{
public:

	// Defines a node that is part of a search tree
	struct Node
	{
		Node() : pPrevious(nullptr)
		{
		}

		// Pointer to the previous Node in the tree
		Node* pPrevious;

		// Movement from the previous state to this state
		WrigglerMove move;
	};

	// Constructs an empty puzzle
	ID_DFTSWrigglerWrig();

	// Constructs the puzzle from the specified file
	ID_DFTSWrigglerWrig(const std::string& file);

	// Loads puzzle description from the specified file
	// The old state of the grid is discarded
	bool Load(const std::string& file);

	// Solves the wriggler puzzle using IDDFTS and prints the solution
	virtual void RunAI();

private:

	// Applies Iterative deepening depth-first search to solve the wriggler puzzle
	// If a path is found, path will be filled with all of the nodes in the path
	// else, path will not be modified
	void IDDFTS(std::deque<std::unique_ptr<Node>>& path);

	// Base case of the recursive Depth Limited Search algorithm
	// depth: the detph limit
	// path: If a solution is found, path will be filled with the nodes in the path,
	// else, path will not be modified
	SearchResult DLS(int depth, std::deque<std::unique_ptr<Node>>& path);

	// Apply Depth Limited Search on the grid to solve the wriggler puzzle
	// Returns the status of the search
	// pSolution: must point to a valid Node which will be set if a path is found
	// pNode: node to apply DLS to
	// depth: the depth limit
	SearchResult DLS(Node** pSolution, Node* pNode, std::map<std::vector<std::vector<char>>,int>& states, int depth);
};

#endif // _IDDFTS_WRIGGLER_GRID_
