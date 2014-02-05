#include "ID_DFTSWrigglerGrid.h"
#include "Timer.h"
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

	Timer theTimer;
	theTimer.Start();

	std::deque<std::unique_ptr<Node>> path;

	while(DLS(depth, path) == SearchResult::Cutoff)
	{
		++depth;
	}

	std::uint64_t wallTime = theTimer.GetTime();

	// Draw the path that was found
	for(const auto& iter : path)
	{
		MoveWriggler(iter->move);

		uvec2 pos = iter->move.h ? m_wrigglers[iter->move.w].positions.front() : m_wrigglers[iter->move.w].positions.back();
		cout << iter->move.w << " " << !iter->move.h << " " << pos.x << " " << pos.y << endl;
	}

	// Draw the final grid after movement
	cout << *this;

	// Draw wall time
	cout << wallTime << endl;

	// Draw path length
	cout << path.size() << endl;
}

SearchResult ID_DFTSWrigglerWrig::DLS(int depth, std::deque<std::unique_ptr<Node>>& path)
{
	Node* pSolutionNode = nullptr;
	std::map<std::vector<std::vector<char>>,int> states;

	// Try to find a solution with a limited depth
	SearchResult result = DLS(&pSolutionNode, nullptr, states, depth);

	// Loop over the solution and build the path
	while(pSolutionNode != nullptr)
	{
		path.emplace_front(pSolutionNode);
		pSolutionNode = pSolutionNode->pPrevious;
	}

	return result;
}

SearchResult ID_DFTSWrigglerWrig::DLS(Node** pSolution, Node* pNode, std::map<std::vector<std::vector<char>>,int>& states, int depth)
{
	if(depth <= 0)
		return SearchResult::Cutoff;

	// Final state check
	if(pNode != nullptr)
	{
		if(FinalStateCheck())
		{
			*pSolution = pNode;
			return SearchResult::Success;
		}
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
					SearchResult result = SearchResult::Failure;

					// If we have not procesed this state yet, or the path to this state is shorter
					auto iter = states.find(m_grid);
					if(iter == states.end() || depth > iter->second)
					{
						// Construct a new node for this state

						auto* pNewNode = new Node();
						pNewNode->pPrevious = pNode;
						pNewNode->move = {w,h,d};

						if(iter != states.end())
						{
							iter->second = depth;
						}
						else
						{
							states[m_grid] = depth;
						}

						// Recursively apply DLS
						result = DLS(pSolution, pNewNode, states, depth - 1);

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
