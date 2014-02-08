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

	unsigned int uiAlgorithm = 0;
	if(n >= 3)
	{
		uiAlgorithm = strtol(cmd[2],nullptr,10);
		if(uiAlgorithm > 1)
		{
			uiAlgorithm = 0;
		}
	}

	try
	{
		const char* file = cmd[1];
		WrigglerGrid* pWrigglerPuzzle = nullptr;
		switch(uiAlgorithm)
		{
		case 0:
			pWrigglerPuzzle = new IDDFTSWrigglerGrid(file);
			break;
		case 1:
			pWrigglerPuzzle = new BFTSWrigglerGrid(file);
			break;
		default:
			cout << "Invalid algorithm specified";
			break;
		}

		if(pWrigglerPuzzle != nullptr)
		{
			pWrigglerPuzzle->RunAI();
		}
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
