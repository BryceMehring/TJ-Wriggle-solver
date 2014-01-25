#include "grid.h"
#include <fstream>
#include <cassert>

const ivec2 Grid::s_adjacentTiles[] =
{
		{ -1, -1 }, { 0, -1 }, { 1, -1 },
		{ -1, 0 }, { 1, 0 }, { -1, 1 },
		{ 0, 1 }, { 1, 1 }
};

Wriggler::Wriggler(const std::list<uvec2> &pos) : m_positons(pos)
{
}

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

			m_wrigglers.emplace_back(std::move(positions));
		}
	}

	return stream;
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
