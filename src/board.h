#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>

//Maximum number of moves is all eight pins moving in all eight different directions
#define MAX_MOVES 8*8

//Enums
//White and black constants have the same integer value across enums
//This allows easy comparison between enums, so don't feel bad about
//using WHITE or BLACK from Color instead of something more specific

//Generic colors
enum Color{
    WHITE = 0, BLACK = 1
};

//Things that can take up space on the board
//WHITE_ENTITY and BLACK_ENTITY represent pins or paths, depending on board
enum Entity{
    WHITE_ENTITY = 0, BLACK_ENTITY = 1,
    WHITE_BASE, BLACK_BASE,
    EMPTY, DISABLED
};

//Outcomes of a game
enum Outcome{
    WHITE_WINS = 0, BLACK_WINS = 1, DRAW
};

//Structs

struct Coord{
    int x;
    int y;
};

struct Move{
    struct Coord start;
    struct Coord end;
};

struct Board{
    //The coordinate planes for pins and paths
    enum Entity pinBoard[7][7];
    enum Entity pathBoard[6][6];
    
    //Keep specific track of pin locations, for quick reference
    //2D for easy color access, i.e. pins[WHITE] or pins[BLACK]
    //Coord.x == -1 indicates a captured pin
    struct Coord pins[2][8];
    
    //Convenience/caching storage for pin and path counts for each color
    int pinCount[2];
    int pathCount[2];
    
    //Current turn
    enum Color turn;
};

struct Board newBoard();

//Functions

//Gets moves from current board into passed array
//If checkForWin is enabled, each move will be checked to see if it wins
//The board is not modified permanently, but changed for purposes of testing for winner
//Returns the number of moves found, or -1 if a winning move is found
//If a winning move is found, it will be the first in the array
int getMoves(struct Board*, struct Move[], const int checkForWin);

//Make a move on a board
//Note: does not ensure the move is valid
void makeMove(struct Board*, const struct Move*);

//Checks to see if the given color has won, returns 1 for win, 0 for loss
int checkForWinner(const struct Board*, const enum Color);


//Returns 1 if move is a capture move, else 0
int isCapture(const struct Move*);
//Searches allMoves (up to moveCount), playing capture moves in capturesMoves
//Returns the number of capture moves found
int getCaptureMoves(struct Move allMoves[], int moveCount, struct Move captureMoves[]);

//Compares two moves, returning 0 if they are the same and 1 if not (mimicking strcmp)
int moveCompare(const struct Move*, const struct Move*);

//Converts strings to various structs
struct Board stringToBoard(const char*);
struct Move stringToMove(const char[]);

//Serialize the various structs and write to stdout (with no newline)
void printBoard(const struct Board*);
void printMove(const struct Move*);
void printMoveStdErr(const struct Move*); //Same as above, but to stderr

#endif