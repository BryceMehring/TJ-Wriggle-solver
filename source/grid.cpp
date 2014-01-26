#include "grid.h"
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


Grid::Grid() : m_uiWidth(0), m_uiHeight(0)
{
}

Grid::Grid(const std::string &file)
{
	Load(file);
}

Grid::Grid(std::istream &file)
{
	Load(file);
}

bool Grid::MoveWriggler(unsigned int id, Direction dir, bool bHead)
{
	bool bMoved = false;
	Wriggler& wriggler = m_wrigglers[id];
	if(!wriggler.positons.empty())
	{
		uvec2 newPos = bHead ? wriggler.positons.front() : wriggler.positons.back();
		uvec2 oldPos = newPos;

		newPos.x += DIRECTIONS[dir].x;
		newPos.y += DIRECTIONS[dir].y;

		if(IsOpen(newPos))
		{
			if(bHead)
			{
				ClearPos(wriggler.positons.back());
				FlipTile(wriggler.positons.front());

				wriggler.positons.pop_back();
				wriggler.positons.push_front(newPos);

				SetExternalPos(wriggler.positons.back(),wriggler.id);
				SetFlipedExternalPos(newPos,dir);
			}
			else
			{
				ClearPos(wriggler.positons.front());
				SetInternalPos(oldPos,dir);

				wriggler.positons.pop_front();
				wriggler.positons.push_back(newPos);

				FlipTile(wriggler.positons.front());
				SetExternalPos(newPos,wriggler.id);
			}

			bMoved = true;
		}
	}

	return bMoved;
}

bool Grid::Load(const std::string &file)
{
	std::ifstream inFile(file);
	return Load(inFile);
}

bool Grid::Load(std::istream &stream)
{
	if(stream)
	{
		int numWrigglers = 0;

		stream >> m_uiWidth >> m_uiHeight;
		stream >> numWrigglers;

		m_grid.resize(m_uiHeight);

		std::vector<uvec2> wrigglers(numWrigglers);
		m_wrigglers.reserve(numWrigglers);

		unsigned int wrigglerIndex = 0;
		for(unsigned int i = 0; i < m_uiHeight; ++i)
		{
			m_grid[i].resize(m_uiWidth);
			for(unsigned int j = 0; j < m_uiWidth; ++j)
			{
				stream >> m_grid[i][j];

				if(IsHead(m_grid[i][j]))
				{
					wrigglers[wrigglerIndex] = {j, i};
					++wrigglerIndex;
				}
			}
		}

		for(uvec2 pos : wrigglers)
		{
			std::list<uvec2> positions;
			positions.push_back(pos);

			while(!IsTail(m_grid[pos.y][pos.x]))
			{
				ivec2 dir = GetDir(m_grid[pos.y][pos.x]);
				pos.x += dir.x;
				pos.y += dir.y;

				assert(pos.x < GetWidth() && pos.y < GetHeight());

				positions.push_back(pos);
			}

			m_wrigglers.push_back({std::move(positions), static_cast<unsigned int>(m_wrigglers.size())});
		}
	}

	return stream;
}

void Grid::ClearPos(const uvec2 &pos)
{
	m_grid[pos.y][pos.x] = 'e';
}

void Grid::FlipTile(const uvec2 &pos)
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
		assert(false);
	}
}

void Grid::SetExternalPos(const uvec2 &pos, int id)
{
	m_grid[pos.y][pos.x] = id + 48;
}

void Grid::SetFlipedExternalPos(const uvec2& newPos, Direction dir)
{
	m_grid[newPos.y][newPos.x] = FLIPED_HEAD_DIRECTION[dir];
}

void Grid::SetExternalPos(const uvec2& newPos, Direction dir)
{
	m_grid[newPos.y][newPos.x] = HEAD_DIRECTION[dir];
}

void Grid::SetInternalPos(const uvec2 &pos, Direction dir)
{
	m_grid[pos.y][pos.x] = INTERNAL_DIRECTION[dir];
}

bool Grid::IsOpen(const uvec2 &pos) const
{
	if(pos.x >= m_uiWidth || pos.y >= m_uiHeight)
		return false;

	char tile = m_grid[pos.y][pos.x];
	return (!IsWriggler(tile) && (tile != 'x'));
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

ivec2 Grid::GetDir(char c) const
{
	ivec2 headDir;
	switch(c)
	{
	case 'U':
	case '^':
		headDir = {0,-1};
		break;
	case 'D':
	case 'v':
		headDir = {0,1};
		break;
	case 'L':
	case '<':
		headDir = {-1,0};
		break;
	case 'R':
	case '>':
		headDir = {1,0};
		break;
	default:
		assert(false);
	}

	return headDir;
}

bool Grid::IsHead(char c) const
{
	return (c == 'U' || c == 'D' || c == 'L' || c == 'R');
}

bool Grid::IsTail(char c) const
{
	return std::isdigit(c);
}

bool Grid::IsWriggler(char c) const
{
	return (c == '^' || c == 'v' || c == '<' || c == '>' || IsHead(c) || IsTail(c));
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
