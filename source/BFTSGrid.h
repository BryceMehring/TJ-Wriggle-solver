#ifndef _BFTS_GRID_
#define _BFTS_GRID_

#include "Grid.h"
#include "Tree.h"
#include <unordered_set>

struct Move
{
	unsigned int w;
	unsigned int h;
	unsigned int d;
};

struct Node
{
	std::vector<Node*> nodes;
	std::deque<uvec2> positions;
	Node* pPrevious;
	Move move;
};

class BFTSGrid : public Grid
{
public:

	BFTSGrid();
	BFTSGrid(const std::string& file);

	// Loads puzzle description from the specified file
	// The old state of the grid is discared
	bool Load(const std::string& file);

	virtual void RunAI();

private:

	Tree<Node> m_tree;

	void GenerateTree(std::unordered_set<Wriggler, WrigglerHash> &closedList, Node *pTree);

};

#endif // _BFTS_GRID_
