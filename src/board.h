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

//Game winner, or NONE if there is none
enum Winner{
    WHITE_WINS = 0, BLACK_WINS = 1,
    DRAW, NONE
};

struct Coord{
    int x;
    int y;
};

struct Board{
    //The coordinate planes for pins and paths
    enum Entity pinBoard[7][7];
    enum Entity pathBoard[6][6];
    
    //Keep specific track of pin locations, for quick reference
    //2D for easy color access, i.e. pins[WHITE] or pins[BLACK]
    //Coord.x == -1 indicates a captured pin
    struct Coord pins[2][8];
    
    //Current turn
    enum Color turn;
};

struct Board newBoard();

void printBoard(const struct Board*);