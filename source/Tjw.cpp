#include "BFTSWrigglerGrid.h"
#include "IDDFTSWrigglerGrid.h"
#include <iostream>
#include <cassert>

using namespace std;

int main(int n, char** cmd)
{
	if(n < 2)
	{
		cout << "Invalid number of command line arguments" << endl;
		cout << "Example usage: ./tjw puzzle.txt" << endl;
		return 1;
	}

	try
	{
		const char* file = cmd[1];
		ID_DFTSWrigglerWrig puzzle(file);
		puzzle.RunAI();
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
