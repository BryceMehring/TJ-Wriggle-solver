#include "test.h"
#include <ctime>

void TestGrid::RunAI()
{	
	for(int i = 0; i < 100000; ++i)
	{
		MoveWriggler(0,(Direction)(rand() % 4),rand() % 2);
	}
}
