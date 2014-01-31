#include "Grid.h"
#include <fstream>
#include <cassert>

const ivec2 Grid::s_adjacentTiles[] =
{
		{ -1, -1 }, { 0, -1 }, { 1, -1 },
		{ -1, 0 }, { 1, 0 }, { -1, 1 },
		{ 0, 1 }, { 1, 1 }
};

static const ivec2 DIRECTIONS[] =
{
	{0,-1}, // Up
	{0,1}, // Down
	{-1,0}, // Left
	{1,0} // Right
};

static const char HEAD_DIRECTION[] =
{
	'U',
	'D',
	'L',
	'R'
};

static const char FLIPED_HEAD_DIRECTION[] =
{
	'D',
	'U',
	'R',
	'L'
};

static const char INTERNAL_DIRECTION[] =
{
	'^',
	'v',
	'<',
	'>'
};

bool operator==(const uvec2& a, const uvec2& b)
{
	return ((a.x == b.x) && (a.y == b.y));
}

bool operator!=(const uvec2& a, const uvec2& b)
{
	return !::operator==(a,b);
}

bool operator==(const Wriggler& a, const Wriggler& b)
{
	return ((a.id == b.id) && (a.positions == b.positions));
}

bool operator!=(const Wriggler& a, const Wriggler& b)
{
	return !::operator==(a,b);
}

Grid::Grid() : m_uiWidth(0), m_uiHeight(0)
{
}

bool Grid::Load(const std::string &file)
{
	std::ifstream inFile(file);
	return Load(inFile);
}

unsigned int Grid::GetNumWrigglers() const
{
	return m_wrigglers.size();
}

unsigned int Grid::GetWidth() const
{
	return m_uiWidth;
}

unsigned int Grid::GetHeight() const
{
	return m_uiHeight;
}

bool Grid::CanMoveWriggler(const Move& m) const
{
	assert(m.w < m_wrigglers.size());

	bool bCanMove = false;
	const Wriggler& wriggler = m_wrigglers[m.w];
	if(!wriggler.positions.empty())
	{
		uvec2 newPos = m.h ? wriggler.positions.front() : wriggler.positions.back();

		newPos.x += DIRECTIONS[m.d].x;
		newPos.y += DIRECTIONS[m.d].y;

		if(IsValid(newPos))
		{
			bCanMove = true;
		}
	}

	return bCanMove;
}

bool Grid::MoveWriggler(const Move& m)
{
	if(CanMoveWriggler(m))
	{
		Wriggler& wriggler = m_wrigglers[m.w];
		uvec2 newPos = m.h ? wriggler.positions.front() : wriggler.positions.back();

		newPos.x += DIRECTIONS[m.d].x;
		newPos.y += DIRECTIONS[m.d].y;

		if(m.h)
		{
			ClearPos(wriggler.positions.back());
			FlipTileDirection(wriggler.positions.front());

			wriggler.positions.pop_back();
			wriggler.positions.push_front(newPos);

			SetWrigglerTail(wriggler.positions.back(),wriggler.id);
			SetWrigglerHead(newPos,m.d);
		}
		else
		{
			ClearPos(wriggler.positions.front());
			SetWrigglerDirection(wriggler.positions.back(),m.d);

			wriggler.positions.pop_front();
			wriggler.positions.push_back(newPos);

			FlipTileDirection(wriggler.positions.front());
			SetWrigglerTail(newPos,wriggler.id);
		}

		return true;
	}

	return false;
}

Direction Grid::GetGetWrigglerTailDir(unsigned int w, bool bHead)
{
	// All wrigglers must have a length of at least two for this algorithm to work
	assert(m_wrigglers[w].positions.size() >= 2);

	uvec2 tail = bHead ? m_wrigglers[w].positions.back() : m_wrigglers[w].positions.front();
	uvec2 next = bHead ? m_wrigglers[w].positions[m_wrigglers[w].positions.size() - 2] : m_wrigglers[w].positions[1];

	if(next.x > tail.x)
	{
		return Left;
	}
	else if(next.x < tail.x)
	{
		return Right;
	}
	else if(next.y > tail.y)
	{
		return Up;
	}
	else
	{
		return Down;
	}
}

bool Grid::Load(std::istream &stream)
{
	std::vector<uvec2> wrigglers = GenerateGrid(stream);

	m_wrigglers.clear();

	// Construct all the wrigglers with their paths and id
	for(uvec2 pos : wrigglers)
	{
		std::deque<uvec2> positions;
		positions.push_back(pos);

		while(!IsTail(m_grid[pos.y][pos.x]))
		{
			ivec2 dir = GetDirVector(m_grid[pos.y][pos.x]);
			pos.x += dir.x;
			pos.y += dir.y;

			assert(pos.x < GetWidth() && pos.y < GetHeight());

			positions.push_back(pos);
		}

		m_wrigglers.push_back({std::move(positions), (unsigned int)m_grid[pos.y][pos.x] - 48});
	}

	return wrigglers.empty() == false;
}

std::vector<uvec2> Grid::GenerateGrid(std::istream &stream)
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


ivec2 Grid::GetDirVector(char c) const
{
	return DIRECTIONS[GetDir(c)];
}

Direction Grid::GetDir(char c) const
{
	Direction dir;
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

bool Grid::IsHead(char c) const
{
	return (c == 'U' || c == 'D' || c == 'L' || c == 'R');
}

bool Grid::IsTail(char c) const
{
	return std::isdigit(c);
}

bool Grid::IsValid(const uvec2 &pos) const
{
	return ((pos.x < m_uiWidth) && (pos.y < m_uiHeight) && (m_grid[pos.y][pos.x] == 'e'));
}

void Grid::ClearPos(const uvec2 &pos)
{
	m_grid[pos.y][pos.x] = 'e';
}

void Grid::FlipTileDirection(const uvec2 &pos)
{
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

void Grid::SetWrigglerTail(const uvec2 &pos, unsigned int id)
{
	m_grid[pos.y][pos.x] = id + 48;
}

void Grid::SetWrigglerHead(const uvec2& newPos, Direction dir)
{
	m_grid[newPos.y][newPos.x] = FLIPED_HEAD_DIRECTION[dir];
}

void Grid::SetWrigglerDirection(const uvec2 &pos, Direction dir)
{
	m_grid[pos.y][pos.x] = INTERNAL_DIRECTION[dir];
}

std::ostream& operator <<(std::ostream& stream, const Grid& g)
{
	for(auto& height : g.m_grid)
	{
		for(auto& iter : height)
		{
			stream << iter << ' ';
		}
		stream << std::endl;
	}

	return stream;
}

std::istream& operator >>(std::istream& stream, Grid& g)
{
	g.Load(stream);
	return stream;
}
