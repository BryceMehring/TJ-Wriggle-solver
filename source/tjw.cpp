#include "grid.h"
#include <iostream>
#include <cassert>

using namespace std;

int main(int n, char** cmd)
{
	if(n < 2)
	{
		cout << "Invalid number of command line arguments" << endl;
	}

	const char* file = cmd[1];
	Grid myGrid;

	assert(myGrid.Load(file));

	myGrid.RunAI();

	cout << myGrid;

	return 0;
}
