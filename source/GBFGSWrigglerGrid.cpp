#include "GBFGSWrigglerGrid.h"
#include "PriorityQueue.h"
#include "Timer.h"
#include <cassert>
#include <map>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using namespace std;

GBFGSWrigglerGrid::GBFGSWrigglerGrid()
{
}

GBFGSWrigglerGrid::GBFGSWrigglerGrid(const std::string& file)
{
	if(!Load(file))
	{
		throw std::string("Cannot open " + file);
	}
}

void GBFGSWrigglerGrid::RunAI()
{
	 Timer theTimer;
	 theTimer.Start();

	 std::deque<GBFGSWrigglerGrid*> path;

	 GBFGS(path,[this](const uvec2& pos) -> int
	 {
		uvec2 goal = { m_uiWidth - 1, m_uiHeight - 1 };
		unsigned int dx = abs((int)pos.x - (int)goal.x);
		unsigned int dy = abs((int)pos.y - (int)goal.y);
		return (dx + dy) * 4;
	 });

	 auto wallTime = theTimer.GetTime();

	 for(const auto& iter : path)
	 {
		 uvec2 pos = iter->m_move.h ? iter->m_wrigglers[iter->m_move.w].positions.front() : iter->m_wrigglers[iter->m_move.w].positions.back();
		 cout << iter->m_move.w << " " << !iter->m_move.h << " " << pos.x << " " << pos.y << endl;

		 MoveWriggler(iter->m_move);
	 }

	 cout << *this;

	 cout << wallTime << endl;

	 cout << path.size() << endl;

}

void GBFGSWrigglerGrid::GBFGS(std::deque<GBFGSWrigglerGrid*>& path, const std::function<int(const uvec2&)>& heuristic)
{
	cds::PriorityQueue<GBFGSWrigglerGrid*, GBFGSWrigglerGridSorter, GBFGSWrigglerGridHash, GBFGSWrigglerGridEqual> frontier;
	std::unordered_set<GBFGSWrigglerGrid*, GBFGSWrigglerGridHash, GBFGSWrigglerGridEqual> closedList;
	bool bFoundSolution = false;

	frontier.Push(this);

	GBFGSWrigglerGrid* pFinalNode = nullptr;

	while(!bFoundSolution && !frontier.Empty())
	{
		GBFGSWrigglerGrid* pNode = pFinalNode = frontier.Top();
		frontier.Pop();

		closedList.insert(pNode);

		// If goal test pNode is true
		// return solution
		bFoundSolution = pNode->FinalStateCheck();
		if(!bFoundSolution)
		{
			// Try to move all of the wrigglers
			for(unsigned int w = 0; w < GetNumWrigglers(); ++w)
			{
				// Try to move in all four directions
				for(Direction d : {Up, Down, Left, Right})
				{
					// Try to move the head and the tail
					for(bool h : {true, false})
					{
						Direction dir = pNode->GetGetWrigglerTailDir(w,h);
						if(pNode->MoveWriggler({w,h,d}))
						{
							int hCost = std::min(heuristic(pNode->m_wrigglers[0].positions.front()),
												 heuristic(pNode->m_wrigglers[0].positions.back()));

							GBFGSWrigglerGrid* pFrontierNode = nullptr;

							// If the node is not in the closed list or the frontier
							if(closedList.find(pNode) == closedList.end() && !frontier.Find(pNode,pFrontierNode))
							{
								auto* pNewNode = new GBFGSWrigglerGrid(*pNode);
								pNewNode->m_pPrevious = pNode;
								pNewNode->m_move = {w,h,d};
								pNewNode->m_iGCost += 1;
								pNewNode->m_iHCost = hCost;

								frontier.Push(pNewNode);
							}
							// If the node is in the frontier
							else if(pFrontierNode != nullptr || frontier.Find(pNode, pFrontierNode))
							{
								// If this is a better path
								if(hCost < pFrontierNode->m_iHCost)
								{
									// Switch path to this node

									pFrontierNode->m_pPrevious = pNode;
									pFrontierNode->m_move = {w,h,d};
									pFrontierNode->m_iHCost = hCost;
									pFrontierNode->m_iGCost = pNode->m_iGCost + 1;

									frontier.Update(pFrontierNode);
								}
							}

							// Move the wriggler back as we are backtracking,
							// if this wriggler fails to move, then something is really wrong
							bool bMovedBack = pNode->MoveWriggler({w,!h,dir});
							assert("Cannot move wriggler back" && bMovedBack);
						}
					}
				}
			}

		}

	}

	cout << closedList.size() << endl;

	while(pFinalNode != nullptr && pFinalNode->m_pPrevious != nullptr)
	{
	   path.push_front(pFinalNode);
	   pFinalNode = pFinalNode->m_pPrevious;
	}
}

bool GBFGSWrigglerGridSorter::operator()(const GBFGSWrigglerGrid* a, const GBFGSWrigglerGrid* b) const
{
	return (a->m_iHCost) > (b->m_iHCost);
}

std::size_t GBFGSWrigglerGridHash::operator()(const GBFGSWrigglerGrid* data) const
{
	std::size_t h = 101;
	for(const auto& iter : data->m_grid)
	{
		h += std::hash<std::string>()(iter);
	}

	return h;
}

std::size_t GBFGSWrigglerGridEqual::operator()(const GBFGSWrigglerGrid* a, const GBFGSWrigglerGrid* b) const
{
	return a->m_grid == b->m_grid;
}


