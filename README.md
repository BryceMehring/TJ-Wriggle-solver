##CS347 TJ-Wriggle solver

How to compile the code:

 1. qmake
 2. make

##Running the program:

 * The fist argument must be set to a puzzle file which will be solved.
 * The second argument specifies the algorithm used:
 
        0 - Greedy best-first graph search (default)
        1 - Iterative deepening depth-first tree search
        2 - Breadth-first tree search

 * To output the solution to a file, one must redirect it.
 * Example usage: ./tjw puzzle.txt 1 > out.txt


