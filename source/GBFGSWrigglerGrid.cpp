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

class GridHash
{
public:

	std::size_t operator()(const std::vector<std::string>& grid) const
	{
		std::size_t h = 101;
		for(const auto& iter : grid)
		{
			h += std::hash<std::string>()(iter);
		}

		return h;
	}
};

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

	 Node* pFinalNode = GBFGS([this](const uvec2& pos) -> int
	 {
		uvec2 goal = { m_uiWidth - 1, m_uiHeight - 1 };
		unsigned int dx = abs((int)pos.x - (int)goal.x);
		unsigned int dy = abs((int)pos.y - (int)goal.y);
		return (dx + dy);
	 });

	 auto wallTime = theTimer.GetTime();

	 std::deque<Node*> path;

	 while(pFinalNode != nullptr && pFinalNode->pPrevious != nullptr)
	 {
		path.emplace_front(pFinalNode);
		pFinalNode = pFinalNode->pPrevious;
	 }

	 for(const auto& iter : path)
	 {
		 uvec2 pos = iter->move.h ? iter->state.m_wrigglers[iter->move.w].positions.front() : iter->state.m_wrigglers[iter->move.w].positions.back();
		 cout << iter->move.w << " " << !iter->move.h << " " << pos.x << " " << pos.y << endl;

		 MoveWriggler(iter->move);
	 }

	 cout << *this;

	 cout << wallTime << endl;

	 cout << path.size() << endl;

}

Node* GBFGSWrigglerGrid::GBFGS(const std::function<int(const uvec2&)>& heuristic) const
{
	cds::PriorityQueue<Node*, std::vector<Node*>, NodeComparer> frontier;
	std::unordered_map<std::vector<std::string>, Node*, GridHash> frontierList;
	std::unordered_map<std::vector<std::string>,Node*, GridHash> closedList;
	bool bFoundSolution = false;

	frontier.Push(new Node{nullptr, *this,{},0,0});

	Node* pFinalNode = nullptr;

	while(!bFoundSolution && !frontier.Empty())
	{
		Node* pNode = pFinalNode = frontier.Top();
		frontier.Pop();

		frontierList.erase(pNode->state.m_grid);
		closedList.insert({pNode->state.m_grid, pNode});

		// If goal test pNode is true
		// return solution
		bFoundSolution = pNode->state.FinalStateCheck();
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
						Direction dir = pNode->state.GetGetWrigglerTailDir(w,h);
						if(pNode->state.MoveWriggler({w,h,d}))
						{
							int hCost = std::min(heuristic(pNode->state.m_wrigglers[0].positions.front()),
												 heuristic(pNode->state.m_wrigglers[0].positions.back()));

							if(closedList.find(pNode->state.m_grid) == closedList.end() && frontierList.find(pNode->state.m_grid) == frontierList.end())
							{
								auto* pNewNode = new Node(pNode, pNode->state, {w,h,d}, hCost, pNode->gCost + 1);

								frontier.Push(pNewNode);
								frontierList.insert({pNewNode->state.m_grid, pNewNode});
							}
							else
							{
								auto frontierIter = frontierList.find(pNode->state.m_grid);
								if(frontierIter != frontierList.end())
								{
									if((hCost) < (frontierIter->second->hCost))
									{
										frontierIter->second->pPrevious = pNode;
										frontierIter->second->move = {w,h,d};
										frontierIter->second->hCost = hCost;
										frontierIter->second->gCost = pNode->gCost + 1;

										frontier.Push(frontierIter->second);
									}
								}
							}

							// Move the wriggler back as we are backtracking,
							// if this wriggler fails to move, then something is really wrong
							bool bMovedBack = pNode->state.MoveWriggler({w,!h,dir});
							assert("Cannot move wriggler back" && bMovedBack);
						}
					}
				}
			}

		}

	}

	cout << closedList.size() << endl;

	return pFinalNode;
}


