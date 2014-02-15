#include "BFTSWrigglerGrid.h"
#include "IDDFTSWrigglerGrid.h"
#include "GraphSearchWrigglerGrid.h"
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

	// Get the algorithm specified in the command line
	unsigned int uiAlgorithm = 0;
	if(n >= 3)
	{
		uiAlgorithm = strtol(cmd[2],nullptr,10);
	}

	try
	{
		// Apply the algorithm that the user specified
		const char* file = cmd[1];
		std::unique_ptr<WrigglerGrid> pWrigglerPuzzle = nullptr;
		switch(uiAlgorithm)
		{
		case 0:
			GraphSearchWrigglerGridSorter::SetMode(GraphSearchWrigglerGridSorter::Mode::GBFGS);
			pWrigglerPuzzle.reset(new GraphSearchWrigglerGrid(file));
			break;
		case 1:
			GraphSearchWrigglerGridSorter::SetMode(GraphSearchWrigglerGridSorter::Mode::UCGS);
			pWrigglerPuzzle.reset(new GraphSearchWrigglerGrid(file));
			break;
		case 2:
			pWrigglerPuzzle.reset(new IDDFTSWrigglerGrid(file));
			break;
		case 3:
			pWrigglerPuzzle.reset(new BFTSWrigglerGrid(file));
			break;
		default:
			cout << "Invalid algorithm specified" << endl;
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
