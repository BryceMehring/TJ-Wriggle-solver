#ifndef _BFTS_GRID_
#define _BFTS_GRID_

#include "WrigglerGrid.h"
#include "Tree.h"
#include "vec2.h"
#include <unordered_set>
#include <cinttypes>

// Defines a node that is part of a search tree
struct Node
{
	Node() : pPrevious(nullptr)
	{
	}

	// A list of all children of this node
	std::vector<Node*> nodes;

	// Pointer to the previous Node in the tree
	Node* pPrevious;

	// Head of the wriggler
	uvec2 head;

	// Tail of the wriggler
	uvec2 tail;

	// Movement from the previous state to this state
	WrigglerMove move;
};

// Breadth First Tree Search Wriggler AI
class BFTSWrigglerGrid : public WrigglerGrid
{
public:

	// Constructs an empty puzzle
	BFTSWrigglerGrid();

	// Constructs the puzzle from the specified file
	BFTSWrigglerGrid(const std::string& file);

	// Loads puzzle description from the specified file
	// The old state of the grid is discarded
	bool Load(const std::string& file);

	// Solves the wriggler puzzle using BFTS and prints the solution
	virtual void RunAI();

private:

	Tree<Node> m_tree;
	std::uint64_t m_wallTime;

	// Builds a tree of valid moves
	// closedList: used by the method to avoid getting stuck in loops
	// pTree: the root of the tree
	void GenerateTree(std::unordered_set<Wriggler, WrigglerHash>& closedList, Node* pTree);

};

#endif // _BFTS_GRID_
