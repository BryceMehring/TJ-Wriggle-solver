#include "IDDFTSWrigglerGrid.h"
#include "Timer.h"
#include <cassert>
#include <iostream>

using namespace std;

IDDFTSWrigglerGrid::IDDFTSWrigglerGrid()
{
}

IDDFTSWrigglerGrid::IDDFTSWrigglerGrid(const std::string& file)
{
	if(!Load(file))
	{
		throw std::string("Cannot open " + file);
	}
}

void IDDFTSWrigglerGrid::RunAI()
{
	std::deque<std::unique_ptr<Node>> path;

	Timer theTimer;
	theTimer.Start();

	// Find the solution
	IDDFTS(path);

	std::uint64_t wallTime = theTimer.GetTime();

	// Draw the path that was found
	for(const auto& iter : path)
	{
		MoveWriggler(iter->move);

		uvec2 pos = iter->move.h ? m_wrigglers[iter->move.w].positions.front() : m_wrigglers[iter->move.w].positions.back();
		cout << iter->move.w << " " << !iter->move.h << " " << pos.x << " " << pos.y << endl;
	}

	DisplayResults(wallTime, path.size());
}

void IDDFTSWrigglerGrid::IDDFTS(std::deque<std::unique_ptr<Node>>& path)
{
	int depth = 0;

	// Keep Increasing the depth and applying DLS while we are being cut off
	while(DLS(depth, path) == SearchResult::Cutoff)
	{
		++depth;
	}
}

SearchResult IDDFTSWrigglerGrid::DLS(int depth, std::deque<std::unique_ptr<Node>>& path)
{
	Node* pSolutionNode = nullptr;

	// Try to find a solution with a limited depth
	SearchResult result = DLS(&pSolutionNode, nullptr, depth);

	// Loop over the solution and build the path
	while(pSolutionNode != nullptr)
	{
		path.emplace_front(pSolutionNode);
		pSolutionNode = pSolutionNode->pPrevious;
	}

	return result;
}

SearchResult IDDFTSWrigglerGrid::DLS(Node** pSolution, Node* pNode, int depth)
{
	if(FinalStateCheck())
	{
		*pSolution = pNode;
		return SearchResult::Success;
	}

	if(depth <= 0)
	{
		return SearchResult::Cutoff;
	}

	bool bCutOffOccured = false;

	// Try to move all of the wrigglers
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
					// Construct a new node for this state

					auto* pNewNode = new Node(pNode, {w,h,d});

					// Recursively apply DLS
					SearchResult result = DLS(pSolution, pNewNode, depth - 1);

					// If we did not find a solution, then we can get rid of this node,
					// nothing else is depending on this node anymore
					if(result != SearchResult::Success)
					{
						if(result == SearchResult::Cutoff)
						{
							bCutOffOccured = true;
						}

						delete pNewNode;
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
