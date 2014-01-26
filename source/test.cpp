#include "test.h"
#include <ctime>
#include <stack>

void TestGrid::RunAI()
{
	// Algorithm to build the state tree:
	// Todo: store the direction index in the stack so that when we come back up the tree, we do not loop back down the same path
	// While there are wrigglers in the queue, do the following:
	// Get the wriggler in the front of the queue and pop him off, call him top.
	// Move the wriggler that was popped off of the queue
	// For all 8 different possibilies for each wriggler at its current location
	//		If moving in this direction can be done,
	//			Add the move to the stack along with the current indicies(I need to check this out a bit more)
	// Unmove the wriggler


	for(unsigned int w = 0; w < GetNumWrigglers(); ++w)
	{
		for(unsigned int h = 0; h < 2; ++h)
		{
			for(unsigned int d = 0; d < 4; ++d)
			{
				/*if(CanMoveWriggler(w,h,(Direction)d))
				{
					// todo:
				}*/
			}
		}
	}
}
