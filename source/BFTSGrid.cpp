#include "BFTSGrid.h"
#include "Timer.h"
#include <iostream>
#include <cassert>

using namespace std;

BFTSGrid::BFTSGrid()
{
}

BFTSGrid::BFTSGrid(const string &file)
{
	if(!Load(file))
	{
		throw std::string("Cannot open " + file);
	}
}

bool BFTSGrid::Load(const string &file)
{
	bool bSuccess = Grid::Load(file);
	if(bSuccess)
	{
		Node* pTree = new Node();

		m_tree.Clear();
		m_tree.SetRoot(pTree);
		m_tree.AddNode(pTree);

		std::unordered_set<Wriggler,WrigglerHash> closedList;
		GenerateTree(closedList, pTree);
	}

	return bSuccess;
}

void BFTSGrid::RunAI()
{
	Timer theTimer;
	theTimer.Start();

	// Find a path that will solve the puzzle
	std::list<Node*> path;
	m_tree.BreadthFirstSearch(path,[this](const Node& n) -> bool
	{
		bool bFoundFinalState = false;

		if(m_wrigglers[n.move.w].id == 0)
		{
			uvec2 finalPos = {m_uiWidth - 1, m_uiHeight - 1};
			if(n.head == finalPos || n.tail == finalPos)
			{
				bFoundFinalState = true;
			}
		}

		return bFoundFinalState;
	});

	// Draw the path that was found
	for(Node* pNode : path)
	{
		uvec2 pos = pNode->move.h ? pNode->head : pNode->tail;
		cout << m_wrigglers[pNode->move.w].id << " " << !pNode->move.h << " " << pos.x << " " << pos.y << endl;

		MoveWriggler(pNode->move);
	}

	cout << *this;
	cout << theTimer.GetTime() << endl;
	cout << path.size() << endl;
}

void BFTSGrid::GenerateTree(std::unordered_set<Wriggler,WrigglerHash>& closedList, Node *pTree)
{
	// GenerateTree()
	// Try to move the wriggler in 8 different directions, 4 for the head and the tail
	// Try to move the wriggler if possible
	//		If unique position for the wriggler
	//			Add new node to the graph
	//			Call GenerateGraph()
	//		Unmove the wriggler

	// Try to move all the wrigglers
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
					bool bProcessOnce = closedList.insert(m_wrigglers[w]).second;

					if(bProcessOnce)
					{
						auto* pNewNode = new Node();
						pNewNode->pPrevious = pTree;
						pNewNode->move = {w,h,d};
						pNewNode->head = m_wrigglers[w].positions.front();
						pNewNode->tail = m_wrigglers[w].positions.back();
						pNewNode->pPrevious->nodes.push_back(pNewNode);

						m_tree.AddNode(pNewNode);

						GenerateTree(closedList,pNewNode);

						closedList.erase(m_wrigglers[w]);
					}

					// Move the wriggler back as we are backtracking,
					// If this wriggler fails to move, then something is really wrong
					bool bMovedBack = MoveWriggler({w,!h,dir});
					assert("Cannot move wriggler back" && bMovedBack);
				}
			}

		}

	}

}
