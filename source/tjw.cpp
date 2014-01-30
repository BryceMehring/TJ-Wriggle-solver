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

	const char* file = cmd[1];

	try
	{
		TestGrid myGrid(file);
		myGrid.RunAI();
	}
	catch(std::exception& exc)
	{
		cout << exc.what() << endl;
	}
	catch(std::string error)
	{
		cout << error << endl;
	}

	return 0;
}
