#include "WrigglerGrid.h"
#include <fstream>
#include <cctype>
#include <cassert>
#include <iostream>

#ifdef SOLVE_BFACTOR
#include "PolySolver/PolynomialSolver.h"
#endif

using namespace std;

const ivec2 WrigglerGrid::s_adjacentTiles[] =
{
		{ -1, -1 }, { 0, -1 }, { 1, -1 },
		{ -1, 0 }, { 1, 0 }, { -1, 1 },
		{ 0, 1 }, { 1, 1 }
};

const ivec2 WrigglerGrid::s_directions[] =
{
	{0,-1}, // Up
	{0,1}, // Down
	{-1,0}, // Left
	{1,0} // Right
};

const char WrigglerGrid::s_headDirection[] =
{
	'U',
	'D',
	'L',
	'R'
};

const char WrigglerGrid::s_flipedHeadDirection[] =
{
	'D',
	'U',
	'R',
	'L'
};

const char WrigglerGrid::s_internalDirection[] =
{
	'^',
	'v',
	'<',
	'>'
};


WrigglerGrid::WrigglerGrid() : m_uiWidth(0), m_uiHeight(0)
{
}

bool WrigglerGrid::Load(const std::string& file)
{
	std::ifstream inFile(file);
	return Load(inFile);
}

unsigned int WrigglerGrid::GetNumWrigglers() const
{
	return m_wrigglers.size();
}

unsigned int WrigglerGrid::GetWidth() const
{
	return m_uiWidth;
}

unsigned int WrigglerGrid::GetHeight() const
{
	return m_uiHeight;
}

bool WrigglerGrid::CanMoveWriggler(const WrigglerMove& m) const
{
	assert(m.w < m_wrigglers.size());

	bool bCanMove = false;
	const Wriggler& wriggler = m_wrigglers[m.w];
	if(!wriggler.positions.empty())
	{
		// Get the position of the wriggler that we are moving
		uvec2 newPos = m.h ? wriggler.positions.front() : wriggler.positions.back();

		newPos.x += s_directions[m.d].x;
		newPos.y += s_directions[m.d].y;

		if(IsValid(newPos))
		{
			bCanMove = true;
		}
	}

	return bCanMove;
}

bool WrigglerGrid::MoveWriggler(const WrigglerMove& m)
{
	if(CanMoveWriggler(m))
	{
		// Wriggler to move
		Wriggler& wriggler = m_wrigglers[m.w];

		// New position to add to the wriggler
		uvec2 newPos = m.h ? wriggler.positions.front() : wriggler.positions.back();

		newPos.x += s_directions[m.d].x;
		newPos.y += s_directions[m.d].y;

		if(m.h)
		{
			// Move the head

			ClearPos(wriggler.positions.back());
			FlipTileDirection(wriggler.positions.front());

			wriggler.positions.pop_back();
			wriggler.positions.push_front(newPos);

			SetWrigglerTail(wriggler.positions.back(),m.w);
			SetWrigglerHead(newPos,m.d);
		}
		else
		{
			// Move the tail

			ClearPos(wriggler.positions.front());
			SetWrigglerDirection(wriggler.positions.back(),m.d);

			wriggler.positions.pop_front();
			wriggler.positions.push_back(newPos);

			FlipTileDirection(wriggler.positions.front());
			SetWrigglerTail(newPos,m.w);
		}

		return true;
	}

	return false;
}

bool WrigglerGrid::FinalStateCheck() const
{
	if(m_wrigglers.empty())
		return false;

	// Check if the blue wriggler has moved to the bottom right corner of the grid
	uvec2 finalPos = { m_uiWidth - 1, m_uiHeight - 1 };
	return (m_wrigglers[0].positions.front() == finalPos || m_wrigglers[0].positions.back() == finalPos);
}

Direction WrigglerGrid::GetGetWrigglerTailDir(unsigned int w, bool bHead) const
{
	// All wrigglers must have a length of at least two for this algorithm to work
	assert((w < m_wrigglers.size()) && (m_wrigglers[w].positions.size() >= 2));

	uvec2 tail = bHead ? m_wrigglers[w].positions.back() : m_wrigglers[w].positions.front();
	uvec2 next = bHead ? m_wrigglers[w].positions[m_wrigglers[w].positions.size() - 2] : m_wrigglers[w].positions[1];

	Direction dir = Down;
	if(next.x > tail.x)
	{
		dir = Left;
	}
	else if(next.x < tail.x)
	{
		dir = Right;
	}
	else if(next.y > tail.y)
	{
		dir = Up;
	}

	return dir;
}

void WrigglerGrid::DisplayResults(std::uint64_t time, int pathLength, int nodesGenerated) const
{
	// Draw the final grid after movement
	cout << "Final State: " << endl;
	cout << *this << endl;

	// Draw wall time
	cout << "Wall Time: " << time << endl;

	// Draw path length
	cout << "Path Length: " << pathLength << endl;

#ifdef SOLVE_BFACTOR
	if(nodesGenerated > 0)
	{
		Polynomial poly;
		poly.SetEquation(nodesGenerated, pathLength);
		poly.Solve();

		// Draw number of states generated
		cout << "Branching Factor: " << poly.GetRoot() << endl;
		cout << "Nodes generated: " << nodesGenerated << endl;
	}
#endif // SOLVE_BFACTOR

}

bool WrigglerGrid::Load(std::istream& stream)
{
	std::vector<uvec2> wrigglers = GenerateGrid(stream);

	m_wrigglers.resize(wrigglers.size());

	// Construct all the wrigglers with their paths and id
	for(uvec2 pos : wrigglers)
	{
		std::deque<uvec2> positions;
		positions.push_back(pos);

		// Follow path from the head to the tail and record each position
		while(!IsTail(m_grid[pos.y][pos.x]))
		{
			ivec2 dir = GetDirVector(m_grid[pos.y][pos.x]);
			pos.x += dir.x;
			pos.y += dir.y;

			assert(pos.x < GetWidth() && pos.y < GetHeight());

			positions.push_back(pos);
		}

		unsigned int index = (unsigned int)m_grid[pos.y][pos.x] - '0';
		assert(index < m_wrigglers.size());
		m_wrigglers[index] = {std::move(positions), index};
	}

	return (wrigglers.empty() == false);
}

std::vector<uvec2> WrigglerGrid::GenerateGrid(std::istream& stream)
{
	std::vector<uvec2> wrigglers;

	if(stream)
	{
		unsigned int numWrigglers = 0;

		stream >> m_uiWidth >> m_uiHeight;
		stream >> numWrigglers;

		wrigglers.reserve(numWrigglers);

		m_grid.resize(m_uiHeight);

		// Read all the lines from the files into the grid and store the location of the heads of all of the wrigglers
		for(unsigned int i = 0; i < m_uiHeight; ++i)
		{
			m_grid[i].resize(m_uiWidth);
			for(unsigned int j = 0; j < m_uiWidth; ++j)
			{
				stream >> m_grid[i][j];

				if(IsHead(m_grid[i][j]))
				{
					wrigglers.push_back({j, i});
				}
			}
		}
	}

	return wrigglers;
}


ivec2 WrigglerGrid::GetDirVector(char c) const
{
	return s_directions[GetDir(c)];
}

Direction WrigglerGrid::GetDir(char c) const
{
	Direction dir = Up;
	switch(c)
	{
	case 'U':
	case '^':
		dir = Up;
		break;
	case 'D':
	case 'v':
		dir = Down;
		break;
	case 'L':
	case '<':
		dir = Left;
		break;
	case 'R':
	case '>':
		dir = Right;
		break;
	default:
		assert("Invalid char" && false);
	}

	return dir;
}

bool WrigglerGrid::IsHead(char c) const
{
	return (c == 'U' || c == 'D' || c == 'L' || c == 'R');
}

bool WrigglerGrid::IsTail(char c) const
{
	return (std::isdigit(c) != 0);
}

bool WrigglerGrid::IsValid(const uvec2& pos) const
{
	return ((pos.x < m_uiWidth) && (pos.y < m_uiHeight) && (m_grid[pos.y][pos.x] == 'e'));
}

void WrigglerGrid::ClearPos(const uvec2& pos)
{
	assert((pos.x < m_uiWidth) && (pos.y < m_uiHeight));
	m_grid[pos.y][pos.x] = 'e';
}

void WrigglerGrid::FlipTileDirection(const uvec2& pos)
{
	assert((pos.x < m_uiWidth) && (pos.y < m_uiHeight));

	char& tile = m_grid[pos.y][pos.x];
	switch(tile)
	{
	case 'U':
		tile = '^';
		break;
	case 'D':
		tile = 'v';
		break;
	case 'L':
		tile = '<';
		break;
	case 'R':
		tile = '>';
		break;
	case '^':
		tile = 'U';
		break;
	case 'v':
		tile = 'D';
		break;
	case '<':
		tile = 'L';
		break;
	case '>':
		tile = 'R';
		break;
	default:
		assert("Invalid char" && false);
	}
}

void WrigglerGrid::SetWrigglerTail(const uvec2& pos, unsigned int id)
{
	assert((pos.x < m_uiWidth) && (pos.y < m_uiHeight));
	m_grid[pos.y][pos.x] = id + '0';
}

void WrigglerGrid::SetWrigglerHead(const uvec2& newPos, Direction dir)
{
	assert((newPos.x < m_uiWidth) && (newPos.y < m_uiHeight));
	m_grid[newPos.y][newPos.x] = s_flipedHeadDirection[dir];
}

void WrigglerGrid::SetWrigglerDirection(const uvec2& pos, Direction dir)
{
	assert((pos.x < m_uiWidth) && (pos.y < m_uiHeight));
	m_grid[pos.y][pos.x] = s_internalDirection[dir];
}

std::ostream& operator <<(std::ostream& stream, const WrigglerGrid& grd)
{
	for(auto& height : grd.m_grid)
	{
		for(auto& iter : height)
		{
			stream << iter << ' ';
		}
		stream << std::endl;
	}

	return stream;
}

std::istream& operator >>(std::istream& stream, WrigglerGrid& grd)
{
	grd.Load(stream);
	return stream;
}
