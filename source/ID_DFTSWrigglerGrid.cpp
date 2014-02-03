#include "ID_DFTSWrigglerGrid.h"
#include <cassert>
#include <iostream>

using namespace std;

// Constructs an empty puzzle
ID_DFTSWrigglerWrig::ID_DFTSWrigglerWrig()
{
}

// Constructs the puzzle from the specified file
ID_DFTSWrigglerWrig::ID_DFTSWrigglerWrig(const std::string& file)
{
	if(!Load(file))
	{
		throw std::string("Cannot open " + file);
	}
}

// Loads puzzle description from the specified file
// The old state of the grid is discarded
bool ID_DFTSWrigglerWrig::Load(const std::string& file)
{
	return WrigglerGrid::Load(file);
}

// Solves the wriggler puzzle using BFTS and prints the solution
void ID_DFTSWrigglerWrig::RunAI()
{
	auto* pRoot = new IDNode();

	IDNode* pNode = nullptr;
	unsigned int depth = 1;
	while(pNode == nullptr)
	{
		pNode = DLS(pRoot,depth++);
	}

	std::deque<IDNode*> path;

	while(pNode != nullptr && pNode->pPrevious != nullptr)
	{
		path.push_front(pNode);
		pNode = pNode->pPrevious;
	}

	// Draw the path that was found
	for(const IDNode* iter : path)
	{
		uvec2 pos = iter->move.h ? iter->positions.front() : iter->positions.back();
		cout << m_wrigglers[iter->move.w].id << " " << !iter->move.h << " " << pos.x << " " << pos.y << endl;

		MoveWriggler(iter->move);

		delete iter;
	}

	delete pNode;

	// Draw the final grid after movement
	cout << *this;

	// Draw path length
	cout << path.size() << endl;
}

IDNode* ID_DFTSWrigglerWrig::DLS(IDNode* pNode, int depth)
{
	if(depth <= 0)
		return nullptr;

	// Final state check
	if(pNode->pPrevious != nullptr)
	{
		if(m_wrigglers[pNode->move.w].id == 0)
		{
			// Check if the blue wriggler has moved to the bottom right corner of the grid
			uvec2 finalPos = { m_uiWidth - 1, m_uiHeight - 1 };
			if(pNode->positions.front() == finalPos || pNode->positions.back() == finalPos)
			{
				return pNode;
			}
		}
	}

	for(unsigned int w = 0; w < GetNumWrigglers(); ++w)
	{
		// Try to move in all four directions
		for(Direction d : {Up, Down, Left, Right})
		{
			// Try to move the head and the tail
			for(bool h : {true, false})
			{
				Direction dir = GetGetWrigglerTailDir(w,h);
				if(MoveWriggler({w,h,d}))
				{
					if(pNode->pPrevious == nullptr || pNode->pPrevious->positions != m_wrigglers[w].positions)
					{
						auto* pNewNode = new IDNode();
						pNewNode->pPrevious = pNode;
						pNewNode->move = {w,h,d};
						pNewNode->positions = m_wrigglers[w].positions;
						pNewNode->pPrevious->nodes.push_back(pNewNode);

						IDNode* pSolutionNode = DLS(pNewNode, depth - 1);

						if(pSolutionNode != nullptr)
						{
							return pSolutionNode;
						}

						delete pNewNode;
					}

					// Move the wriggler back as we are backtracking,
					// if this wriggler fails to move, then something is really wrong
					bool bMovedBack = MoveWriggler({w,!h,dir});
					assert("Cannot move wriggler back" && bMovedBack);
				}
			}
		}
	}

	return nullptr;
}
