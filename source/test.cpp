#include "test.h"
#include "Timer.h"
#include <iostream>

using namespace std;

void TestGrid::RunAI()
{
	Timer theTimer;
	theTimer.Start();

	std::list<Node*> path = m_tree.BreadthFirstSearch([&](const Node& n) -> bool
	{
		bool bFoundFinalState = false;
		if(!n.positions.empty())
		{
			if(m_wrigglers[n.move.w].id == 0)
			{
				uvec2 start = n.positions.front();
				uvec2 end = n.positions.back();
				uvec2 finalPos = {m_uiWidth - 1, m_uiHeight - 1};
				if(start == finalPos || end == finalPos)
				{
					bFoundFinalState = true;
				}
			}
		}

		return bFoundFinalState;
	});

	for(auto iter : path)
	{
		cout << m_wrigglers[iter->move.w].id << " " << iter->move.h << " " << iter->move.d << endl;
		MoveWriggler(iter->move.w,iter->move.h,(Direction)iter->move.d);
	}

	cout << *this << endl;
	cout << theTimer.GetTime() << endl;

	cout << path.size() << endl;
	cout << m_tree.Size() << endl;
}
