#ifndef _WRIGGLER_GRID_
#define _WRIGGLER_GRID_

#include <string>
#include <vector>
#include <deque>
#include "vec2.h"
#include "Wriggler.h"


// Defines a grid for TJ-wriggle puzzles which stores the location of all the objects
// and manages basic wriggler operations
class WrigglerGrid
{
public:

	// Builds an empty grid
	WrigglerGrid();

	virtual ~WrigglerGrid() {}

	// Loads puzzle description from the specified file
	// The old state of the grid is discarded
	bool Load(const std::string& file);

	// Returns the number of wrigglers on the grid
	unsigned int GetNumWrigglers() const;

	// Returns the width of the grid
	unsigned int GetWidth() const;

	// Returns the height of the grid
	unsigned int GetHeight() const;

	// This method should do all the work to solve the puzzle
	virtual void RunAI() = 0;

protected:

	// Returns true if move m is valid, false otherwise
	bool CanMoveWriggler(const WrigglerMove& m) const;

	// Moves a wriggler specified by m. Returns true if the move is valid, false otherwise
	bool MoveWriggler(const WrigglerMove& m);

	// Checks if the blue wriggler has made it to the final state
	// Returns true if successful, false otherwise
	bool FinalStateCheck() const;

	// Returns the direction that the wriggler will move away from, the opposite side of the wriggler that is set to move
	// w: index of the wriggler
	// bHead: true to move the head, false to move the tail
	Direction GetGetWrigglerTailDir(unsigned int w, bool bHead) const;

private:

	// Loads the grid from the specified stream and returns true if successful, false otherwise
	bool Load(std::istream& stream);

	// Fills the grid from the file and returns the location of all the heads of the wrigglers
	std::vector<uvec2> GenerateGrid(std::istream& stream);

	// Returns the direction specified by the current tile character c in array space, which can either be 'U', '^', 'D', 'v', 'L', '<', 'R', '>'
	ivec2 GetDirVector(char c) const;

	// Returns the direction specified by the current tile character c,  which can either be 'U', '^', 'D', 'v', 'L', '<', 'R', '>'
	Direction GetDir(char c) const;

	// Returns true if c is the head of a wriggler, false otherwise
	bool IsHead(char c) const;

	// Returns true if c is the tail of a wriggler, false otherwise
	bool IsTail(char c) const;

	// Returns true if pos is a valid position to move to, false otherwise
	bool IsValid(const uvec2& pos) const;

	// Sets the position specified by pos to empty
	void ClearPos(const uvec2& pos);

	// Inverts the direction at the specified position in the grid
	// 'U' <--> '^'
	// 'D' <--> 'v'
	// 'L' <--> '<'
	// 'R' <--> '>'
	void FlipTileDirection(const uvec2& pos);


	// Sets the tail of the wriggler
	// pos: the position of the wriggler's tail
	// id: id of the wriggler
	void SetWrigglerTail(const uvec2& pos, unsigned int id);

	// Sets the direction of the head of the wriggler
	// pos: the position of the head
	// dir: the direction that the head should be facing
	void SetWrigglerHead(const uvec2& pos, Direction dir);

	// Sets the direction of a specific part of the wriggler
	// pos: the position of the wriggler to update the direction
	// dir: direction of the wriggler
	void SetWrigglerDirection(const uvec2& pos, Direction dir);

	// Grid I/O
	friend std::ostream& operator <<(std::ostream& stream, const WrigglerGrid& grd);
	friend std::istream& operator >>(std::istream& stream, WrigglerGrid& grd);

protected:

	// Grid containing all of the tiles
	std::vector<std::string> m_grid;

	// List containing all of the wrigglers
	std::vector<Wriggler> m_wrigglers;

	// Width and height of the grid
	unsigned int m_uiWidth;
	unsigned int m_uiHeight;

private:

	static const ivec2 s_adjacentTiles[];
	static const ivec2 s_directions[];
	static const char s_headDirection[];
	static const char s_flipedHeadDirection[];
	static const char s_internalDirection[];
};

std::ostream& operator <<(std::ostream& stream, const WrigglerGrid&);
std::istream& operator >>(std::istream& stream, WrigglerGrid&);

#endif // _WRIGGLER_GRID_
