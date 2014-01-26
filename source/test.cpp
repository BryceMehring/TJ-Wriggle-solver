#include "test.h"
#include <ctime>

void TestGrid::RunAI()
{	
	for(int i = 0; i < 100000; ++i)
	{
		MoveWriggler(0,rand() % 2,(Direction)(rand() % 4));
	}
}
