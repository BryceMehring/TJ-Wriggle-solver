#include "test.h"
#include <iostream>
#include <cassert>

using namespace std;

int main(int n, char** cmd)
{
	if(n < 2)
	{
		cout << "Invalid number of command line arguments" << endl;
		return 1;
	}

	srand(time(0));

	const char* file = cmd[1];
	TestGrid myGrid;

	assert(myGrid.Load(file));

	myGrid.RunAI();

	//cout << myGrid << endl;

	return 0;
}
