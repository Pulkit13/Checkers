Checkers
========

Implemented the alpha-beta pruning algorithm to predict the next move that would maximize either player's chances of winning in a game of checkers.

========

The program consists of the following classes :

1. Point : This class stores the coordintes of a piece in the board.
2. List : This class maintains a list of possible moves and jumps for a piece. This class inherits the Point class.
3. CheckerBoard : This class contains the attributes of each piece and the methods (given in the spec) that can apply to each piece.

Initially MaxValue is called for a piece, which stores the jumps (if any), else stores the moves for a piece of side A. It recursively calls MinValue which similarly calls MaxValue.
This is carried out for a depth d which is input from the user.

Additional Functions which support the operations are :
-readBoard : which reads the board configuration input by the user.
-Prune : which prunes the moves/jumps for a player.
-alpha_beta : for printing alpha and beta values as required.
-spaces : for printing the required number of spaces, as mentioned in the sample output.
-eval_function : evaluates the evaluation function value for a CheckerBoard, and returns the current heuristic value.
-hasGameEnded : checks whether the game has ended.
-findMax, findMin : functions to find the max and min values from two integers respectively.

The user must first enter the board configuration, and then the depth value.

*******************************************************************************************************************************************************************************************

How to Compile :

There is just one class temp.cpp
Please compile it as follows on aludra :

g++ temp.cpp -o a

This generates an executable 'a'

******************************************************************************************************************************************************************************************* 
How to Execute :

After compilation, please write the following :

./a

*****************************************************************END 
