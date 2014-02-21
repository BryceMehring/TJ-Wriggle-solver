#include "BFTSWrigglerGrid.h"
#include "IDDFTSWrigglerGrid.h"
#include "GraphSearchWrigglerGrid.h"
#include <iostream>
#include <cassert>

using namespace std;

enum SearchAlgorithm
{
	ASTAR,
	GBFGS,
	UCGS,
	IDDFTS,
	BFTS
};

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
		case ASTAR:
			GraphSearchWrigglerGridSorter::SetMode(GraphSearchWrigglerGridSorter::Mode::ASTAR);
			pWrigglerPuzzle.reset(new GraphSearchWrigglerGrid(file));
			break;
		case GBFGS:
			GraphSearchWrigglerGridSorter::SetMode(GraphSearchWrigglerGridSorter::Mode::GBFGS);
			pWrigglerPuzzle.reset(new GraphSearchWrigglerGrid(file));
			break;
		case UCGS:
			GraphSearchWrigglerGridSorter::SetMode(GraphSearchWrigglerGridSorter::Mode::UCGS);
			pWrigglerPuzzle.reset(new GraphSearchWrigglerGrid(file));
			break;
		case IDDFTS:
			pWrigglerPuzzle.reset(new IDDFTSWrigglerGrid(file));
			break;
		case BFTS:
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
