#include "GraphSearchWrigglerGrid.h"
#include "PriorityQueue.h"
#include "Timer.h"

#ifdef SOLVE_BFACTOR
#include "PolynomialSolver.h"
#endif

#include <cassert>
#include <iostream>
#include <unordered_set>

using std::cout;
using std::endl;

GraphSearchWrigglerGridSorter::Mode GraphSearchWrigglerGridSorter::s_state = GraphSearchWrigglerGridSorter::Mode::GBFGS;

GraphSearchWrigglerGrid::GraphSearchWrigglerGrid() : m_pPrevious(nullptr), m_iGCost(0), m_iHCost(0)
{
}

GraphSearchWrigglerGrid::GraphSearchWrigglerGrid(const std::string& file) : m_pPrevious(nullptr), m_iGCost(0), m_iHCost(0)
{
	if(!Load(file))
	{
		throw std::string("Cannot open " + file);
	}
}

void GraphSearchWrigglerGrid::RunAI()
{
	Timer theTimer;
	theTimer.Start();

	std::deque<GraphSearchWrigglerGrid*> path;

	// Find path with custom heuristic
	auto states = FindPath(path, [this](const uvec2& pos) -> int
	{
		uvec2 goal = { m_uiWidth - 1, m_uiHeight - 1 };
		int h1 = std::max(std::abs((long long)pos.x - goal.x), std::abs((long long)pos.y - goal.y));

		int dx = abs((int)pos.x - (int)goal.x);
		int dy = abs((int)pos.y - (int)goal.y);
		int h2 = (dx + dy);

		return 3*std::max(h1,h2);
	});

	auto wallTime = theTimer.GetTime();

	// Draw the path that was found
	for(const auto& iter : path)
	{
		uvec2 pos = iter->m_move.h ? iter->m_wrigglers[iter->m_move.w].positions.front() : iter->m_wrigglers[iter->m_move.w].positions.back();
		cout << iter->m_move.w << " " << !iter->m_move.h << " " << pos.x << " " << pos.y << endl;

		MoveWriggler(iter->m_move);
	}

	// Draw the final grid after movement
	cout << *this;

	// Draw wall time
	cout << wallTime << endl;

	// Draw path length
	cout << path.size() << endl;

#ifdef SOLVE_BFACTOR

	Polynomial poly;
	poly.SetEquation(states.size(), path.size());
	poly.Solve();

	// Draw number of states generated
	cout << "Branching Factor: " << poly.GetRoot() << endl;
	cout << "Nodes generated: " << states.size() << endl;

#endif // SOLVE_BFACTOR
}

std::vector<std::unique_ptr<GraphSearchWrigglerGrid>> GraphSearchWrigglerGrid::FindPath(std::deque<GraphSearchWrigglerGrid*>& path, const std::function<int(const uvec2&)>& heuristic)
{
	std::vector<std::unique_ptr<GraphSearchWrigglerGrid>> states;

	cds::PriorityQueue<GraphSearchWrigglerGrid*, GraphSearchWrigglerGridSorter, GraphSearchWrigglerGridHash, GraphSearchWrigglerGridEqual> frontier;
	std::unordered_set<GraphSearchWrigglerGrid*, GraphSearchWrigglerGridHash, GraphSearchWrigglerGridEqual> closedList;

	GraphSearchWrigglerGrid* pFinalNode = nullptr;

	frontier.Push(this);

	while(!frontier.Empty())
	{
		GraphSearchWrigglerGrid* pNode = frontier.Top();
		frontier.Pop();

		// If goal test pNode is true
		// return solution
		if(pNode->FinalStateCheck())
		{
			pFinalNode = pNode;
			break;
		}

		closedList.insert(pNode);

		GraphSearchWrigglerGrid topGrid = *pNode;

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
						if(closedList.find(&topGrid) == closedList.end())
						{
							int hCost = std::min(heuristic(topGrid.m_wrigglers[0].positions.front()),
												 heuristic(topGrid.m_wrigglers[0].positions.back()));

							GraphSearchWrigglerGrid* pFrontierNode = nullptr;

							// If the node is not in the closed list or the frontier
							if(!frontier.Find(&topGrid,pFrontierNode))
							{
								auto* pNewNode = new GraphSearchWrigglerGrid(topGrid);

								pNewNode->m_pPrevious = pNode;
								pNewNode->m_move = {w,h,d};
								pNewNode->m_iGCost += 1;
								pNewNode->m_iHCost = hCost;

								frontier.Push(pNewNode);
								states.emplace_back(pNewNode);
							}
							// If the node is in the frontier and if this is a better path
							else if(LowerPathCheck(pFrontierNode, pNode, hCost))
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

	BuildPath(pFinalNode, path);

	return states;
}

bool GraphSearchWrigglerGrid::LowerPathCheck(const GraphSearchWrigglerGrid* pFrontierNode, const GraphSearchWrigglerGrid* pPreviousNode, int heuristic) const
{
	GraphSearchWrigglerGridSorter::Mode mode = GraphSearchWrigglerGridSorter::GetMode();
	bool bShorterPath = false;

	switch(mode)
	{
	case GraphSearchWrigglerGridSorter::Mode::GBFGS:
		bShorterPath = heuristic < pFrontierNode->m_iHCost;
		break;
	case GraphSearchWrigglerGridSorter::Mode::UCGS:
		bShorterPath = (pPreviousNode->m_iGCost + 1) < pFrontierNode->m_iGCost;
		break;
	case GraphSearchWrigglerGridSorter::Mode::ASTAR:
		bShorterPath = (heuristic + pPreviousNode->m_iGCost + 1) < (pFrontierNode->m_iGCost + pFrontierNode->m_iHCost);
		break;
	}

	return bShorterPath;
}

void GraphSearchWrigglerGrid::BuildPath(GraphSearchWrigglerGrid* pFinalNode, std::deque<GraphSearchWrigglerGrid*>& path)
{
	while((pFinalNode != nullptr) && (pFinalNode->m_pPrevious != nullptr))
	{
	   path.push_front(pFinalNode);
	   pFinalNode = pFinalNode->m_pPrevious;
	}
}

void GraphSearchWrigglerGridSorter::SetMode(Mode mode)
{
	s_state = mode;
}

GraphSearchWrigglerGridSorter::Mode GraphSearchWrigglerGridSorter::GetMode()
{
	return s_state;
}


bool GraphSearchWrigglerGridSorter::operator()(const GraphSearchWrigglerGrid* a, const GraphSearchWrigglerGrid* b) const
{
	bool order = false;
	switch(s_state)
	{
	case Mode::GBFGS:
		order = (a->m_iHCost > b->m_iHCost);
		break;
	case Mode::UCGS:
		order = (a->m_iGCost > b->m_iGCost);
		break;
	case Mode::ASTAR:
		order = ((a->m_iGCost + a->m_iHCost) > (b->m_iGCost + b->m_iHCost));
		break;
	default:
		assert("Unknown state selected" && false);
	}

	return order;
}

std::size_t GraphSearchWrigglerGridHash::operator()(const GraphSearchWrigglerGrid* data) const
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

std::size_t GraphSearchWrigglerGridEqual::operator()(const GraphSearchWrigglerGrid* a, const GraphSearchWrigglerGrid* b) const
{
	return a->m_grid == b->m_grid;
}


