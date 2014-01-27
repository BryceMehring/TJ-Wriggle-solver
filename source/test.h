#ifndef _TEST_
#define _TEST_

#include "grid.h"

class TestGrid : public Grid
{
public:

	using Grid::Grid;

	virtual void RunAI();

private:

	void GenerateGraph(unsigned int w, unsigned int h, unsigned int d, unsigned int& counter);

};

#endif // _TEST_
