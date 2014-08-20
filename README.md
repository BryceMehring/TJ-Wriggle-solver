##[TJ-Wriggle](http://www.clickmazes.com/tjwrig/ixbhwrig.htm) solver

####Building using CMake:

 1. `cmake .` or use the CMake GUI
 2. `make`

##Running the program:

 * The fist argument must be set to a puzzle file which will be solved.
 * The second argument specifies the algorithm used:
 
        0 - A* Graph Search (default)
        1 - Greedy best-first graph search
        2 - Uniform-cost graph search
        3 - Iterative deepening depth-first tree search
        4 - Breadth-first tree search

 * Example usage: `./tjw tests/puzzle1.txt 1`
