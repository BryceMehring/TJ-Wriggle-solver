#include "GBFGSWrigglerGrid.h"
#include "PriorityQueue.h"
#include "Timer.h"
#include <cassert>

#include <iostream>
#include <unordered_set>

using std::cout;
using std::endl;

GBFGSWrigglerGridSorter::Mode GBFGSWrigglerGridSorter::s_state = GBFGSWrigglerGridSorter::Mode::GBFGS;

GBFGSWrigglerGrid::GBFGSWrigglerGrid() : m_pPrevious(nullptr), m_iGCost(0), m_iHCost(0)
{
}

GBFGSWrigglerGrid::GBFGSWrigglerGrid(const std::string& file) : m_pPrevious(nullptr), m_iGCost(0), m_iHCost(0)
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

	 auto states = GBFGS(path,[this](const uvec2& pos) -> int
	 {
		uvec2 goal = { m_uiWidth - 1, m_uiHeight - 1 };
		unsigned int dx = abs((int)pos.x - (int)goal.x);
		unsigned int dy = abs((int)pos.y - (int)goal.y);
		return (dx + dy);
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

std::vector<std::unique_ptr<GBFGSWrigglerGrid>> GBFGSWrigglerGrid::GBFGS(std::deque<GBFGSWrigglerGrid*>& path, const std::function<int(const uvec2&)>& heuristic)
{
	std::vector<std::unique_ptr<GBFGSWrigglerGrid>> states;

	cds::PriorityQueue<GBFGSWrigglerGrid*, GBFGSWrigglerGridSorter, GBFGSWrigglerGridHash, GBFGSWrigglerGridEqual> frontier;
	std::unordered_set<GBFGSWrigglerGrid*, GBFGSWrigglerGridHash, GBFGSWrigglerGridEqual> closedList;

	GBFGSWrigglerGrid* pFinalNode = nullptr;

	frontier.Push(this);

	while(!frontier.Empty())
	{
		GBFGSWrigglerGrid* pNode = frontier.Top();
		frontier.Pop();

		// If goal test pNode is true
		// return solution
		if(pNode->FinalStateCheck())
		{
			pFinalNode = pNode;
			break;
		}

		closedList.insert(pNode);

		GBFGSWrigglerGrid topGrid = *pNode;

		// Try to move all of the wrigglers
		for(unsigned int w = 0; w < GetNumWrigglers(); ++w)
		{
			// Try to move in all four directions
			for(Direction d : {Up, Down, Left, Right})
			{
				// Try to move the head and the tail
				for(bool h : {true, false})
				{
					Direction dir = topGrid.GetGetWrigglerTailDir(w,h);
					if(topGrid.MoveWriggler({w,h,d}))
					{
						int hCost = std::min(heuristic(topGrid.m_wrigglers[0].positions.front()),
											 heuristic(topGrid.m_wrigglers[0].positions.back()));

						GBFGSWrigglerGrid* pFrontierNode = nullptr;

						// If the node is not in the closed list or the frontier
						if(closedList.find(&topGrid) == closedList.end() && !frontier.Find(&topGrid,pFrontierNode))
						{
							auto* pNewNode = new GBFGSWrigglerGrid(topGrid);

							pNewNode->m_pPrevious = pNode;
							pNewNode->m_move = {w,h,d};
							pNewNode->m_iGCost += 1;
							pNewNode->m_iHCost = hCost;

							frontier.Push(pNewNode);
							states.emplace_back(pNewNode);
						}
						// If the node is in the frontier
						else if(pFrontierNode != nullptr || frontier.Find(&topGrid, pFrontierNode))
						{
							// If this is a better path
							if(hCost < pFrontierNode->m_iHCost)
							{
								// Switch path to this node
								pFrontierNode->m_pPrevious = pNode;
								pFrontierNode->m_move = {w,h,d};
								pFrontierNode->m_iGCost = pNode->m_iGCost + 1;
								pFrontierNode->m_iHCost = hCost;

								frontier.Update(pFrontierNode);
							}
						}

						// Move the wriggler back as we are backtracking,
						// if this wriggler fails to move, then something is really wrong
						bool bMovedBack = topGrid.MoveWriggler({w,!h,dir});
						assert("Cannot move wriggler back" && bMovedBack);
					}
				}
			}
		}
	}

	while((pFinalNode != nullptr) && (pFinalNode->m_pPrevious != nullptr))
	{
	   path.push_front(pFinalNode);
	   pFinalNode = pFinalNode->m_pPrevious;
	}

	return states;
}

void GBFGSWrigglerGridSorter::SetMode(Mode mode)
{
	s_state = mode;
}

bool GBFGSWrigglerGridSorter::operator()(const GBFGSWrigglerGrid* a, const GBFGSWrigglerGrid* b) const
{
	bool order = false;
	switch(s_state)
	{
	case Mode::GBFGS:
		order = ((a->m_iHCost) > (b->m_iHCost));
		break;
	case Mode::UCGS:
		order = (a->m_iGCost) > (b->m_iGCost);
		break;
	default:
		assert("Unknown state selected" && false);
	}

	return order;
}

std::size_t GBFGSWrigglerGridHash::operator()(const GBFGSWrigglerGrid* data) const
{
	std::size_t h = 5381;
	for(const auto& iter : data->m_grid)
	{
		for(char c : iter)
		{
			h *= 33;
			h += c;
		}
	}

	return h;
}

std::size_t GBFGSWrigglerGridEqual::operator()(const GBFGSWrigglerGrid* a, const GBFGSWrigglerGrid* b) const
{
	return a->m_grid == b->m_grid;
}


