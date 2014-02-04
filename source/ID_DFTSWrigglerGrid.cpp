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
	unsigned int depth = 1;

	std::deque<std::unique_ptr<IDNode>> path;
	SearchResult result = SearchResult::Cutoff;
	while(result == SearchResult::Cutoff)
	{
		result = DLS(depth++, path);
	}

	// Draw the path that was found
	for(const auto& iter : path)
	{
		uvec2 pos = iter->move.h ? iter->positions.front() : iter->positions.back();
		cout << m_wrigglers[iter->move.w].id << " " << !iter->move.h << " " << pos.x << " " << pos.y << endl;

		MoveWriggler(iter->move);
	}

	// Draw the final grid after movement
	cout << *this;

	// Draw path length
	cout << path.size() << endl;
}

SearchResult ID_DFTSWrigglerWrig::DLS(int depth, std::deque<std::unique_ptr<IDNode>>& path)
{
	IDNode* pSolutionNode = nullptr;
	SearchResult result = DLS(&pSolutionNode, nullptr, depth);

	while(pSolutionNode != nullptr)
	{
		path.emplace_front(pSolutionNode);
		pSolutionNode = pSolutionNode->pPrevious;
	}

	return result;
}

SearchResult ID_DFTSWrigglerWrig::DLS(IDNode** pSolution, IDNode* pNode, int depth)
{
	if(depth <= 0)
		return SearchResult::Cutoff;

	// Final state check
	if(pNode != nullptr)
	{
		if(m_wrigglers[pNode->move.w].id == 0)
		{
			// Check if the blue wriggler has moved to the bottom right corner of the grid
			uvec2 finalPos = { m_uiWidth - 1, m_uiHeight - 1 };
			if(pNode->positions.front() == finalPos || pNode->positions.back() == finalPos)
			{
				*pSolution = pNode;
				return SearchResult::Success;
			}
		}
	}

	bool bCutOffOccured = false;

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
					SearchResult result = SearchResult::Failure;

					if(pNode == nullptr || pNode->pPrevious == nullptr || pNode->pPrevious->positions != m_wrigglers[w].positions)
					{
						auto* pNewNode = new IDNode();
						pNewNode->pPrevious = pNode;
						pNewNode->move = {w,h,d};
						pNewNode->positions = m_wrigglers[w].positions;

						result = DLS(pSolution, pNewNode, depth - 1);

						if(result != SearchResult::Success)
						{
							if(result == SearchResult::Cutoff)
							{
								bCutOffOccured = true;
							}

							delete pNewNode;
						}
					}

					// Move the wriggler back as we are backtracking,
					// if this wriggler fails to move, then something is really wrong
					bool bMovedBack = MoveWriggler({w,!h,dir});
					assert("Cannot move wriggler back" && bMovedBack);

					if(result == SearchResult::Success)
					{
						return result;
					}
				}
			}
		}
	}

	if(bCutOffOccured)
	{
		return SearchResult::Cutoff;
	}

	return SearchResult::Failure;
}
