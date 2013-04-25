#include "board.h"

static void addPin(struct Board *board, enum Color color, int x, int y){
    //Add the pin to the pinBoard
    board->pinBoard[y][x] = color;
    
    //Find the first available slot in the pin list, and add the pin there
    int i;
    for(i = 0; i < 8; i++){
        if(board->pins[color][i].x == -1){
            board->pins[color][i].x = x;
            board->pins[color][i].y = y;
            break;
        }
    }
    
    board->pinCount[color]++;
}

//Set up a fresh board, ready to start a new game
struct Board newBoard(){
    struct Board board;
    int x, y, i;    

    //Make sure board is empty
    for(x = 0; x < 7; x++){
        for(y = 0; y < 7; y++){
            board.pinBoard[y][x] = EMPTY;
        }
    }
    
    for(x = 0; x < 6; x++){
        for(y = 0; y < 6; y++){
            board.pathBoard[y][x] = EMPTY;
        }
    }
    
    //Clear pin lists (see Board struct in board.h)
    for(i = 0; i < 8; i++){
        board.pins[WHITE][i].x = -1;
        board.pins[BLACK][i].x = -1;    
    }
    
    //Clear count caches
    board.pinCount[WHITE] = 0;
    board.pinCount[BLACK] = 0;
    board.pathCount[WHITE] = 0;
    board.pathCount[BLACK] = 0;

    //Add base and disabled entities
    board.pinBoard[6][0] = DISABLED;
    board.pinBoard[0][6] = DISABLED;
    board.pinBoard[0][0] = DISABLED;
    board.pinBoard[6][6] = DISABLED;
    board.pathBoard[5][0] = WHITE_BASE;
    board.pathBoard[0][5] = WHITE_BASE;
    board.pathBoard[0][0] = BLACK_BASE;
    board.pathBoard[5][5] = BLACK_BASE;
    
    //Add pins 
    addPin(&board, WHITE, 1, 6); //Top-left
    addPin(&board, WHITE, 2, 6);
    addPin(&board, WHITE, 0, 4);
    addPin(&board, WHITE, 0, 5);
    addPin(&board, WHITE, 4, 0); //Bottom-right 
    addPin(&board, WHITE, 5, 0);
    addPin(&board, WHITE, 6, 1);
    addPin(&board, WHITE, 6, 2);
    addPin(&board, BLACK, 1, 0); //Bottom-left
    addPin(&board, BLACK, 2, 0);
    addPin(&board, BLACK, 0, 1);
    addPin(&board, BLACK, 0, 2);
    addPin(&board, BLACK, 4, 6); //Top-right
    addPin(&board, BLACK, 5, 6);
    addPin(&board, BLACK, 6, 5);
    addPin(&board, BLACK, 6, 4);
        
    board.turn = WHITE;
    
    return board;
}

static void recordMove(struct Move moves[], int *moveCount,
                       int sx, int sy, int ex, int ey){
    moves[*moveCount].start.x = sx;
    moves[*moveCount].start.y = sy;
    moves[*moveCount].end.x = ex;
    moves[*moveCount].end.y = ey;
    *moveCount += 1;
}

int getMoves(struct Board* board, struct Move moves[], const int checkForWin){
    enum Color turn = board->turn;
    enum Color other = !turn;
    int moveCount = 0;
    
    int  i, x, y, tx, ty, isWin;
    enum Entity temp;
    for(i = 0; i < 8; i++){
        //x == -1 indicates captured pin, see board.h
        if(board->pins[turn][i].x == -1){
            continue;
        }

        x = board->pins[turn][i].x;
        y = board->pins[turn][i].y;
        
        //Diagonal moves
        tx = x + 1;
        ty = y + 1;
        if(tx < 7 && ty < 7 && board->pinBoard[ty][tx] == EMPTY){
            //TODO: We can probably improve how this is checked
            //      (or at least make it cleaner)
            if(checkForWin){
                temp = board->pathBoard[y][x];
                board->pathBoard[y][x] = turn;
                isWin = checkForWinner(board, turn);
                board->pathBoard[y][x] = temp;
                if(isWin){
                    moveCount = 0;
                    recordMove(moves, &moveCount, x, y, tx, ty);
                    return -1;
                }
            }
            
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        tx = x + 1;
        ty = y - 1;
        if(tx < 7 && ty >= 0 && board->pinBoard[ty][tx] == EMPTY){
            if(checkForWin){
                temp = board->pathBoard[ty][x];
                board->pathBoard[ty][x] = turn;
                isWin = checkForWinner(board, turn);
                board->pathBoard[ty][x] = temp;
                if(isWin){
                    moveCount = 0;
                    recordMove(moves, &moveCount, x, y, tx, ty);
                    return -1;
                }
            }
        
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        tx = x - 1;
        ty = y + 1;
        if(tx >= 0 && ty < 7 && board->pinBoard[ty][tx] == EMPTY){
            if(checkForWin){
                temp = board->pathBoard[y][tx];
                board->pathBoard[y][tx] = turn;
                isWin = checkForWinner(board, turn);
                board->pathBoard[y][tx] = temp;
                if(isWin){
                    moveCount = 0;
                    recordMove(moves, &moveCount, x, y, tx, ty);
                    return -1;
                }
            }
        
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        tx = x - 1;
        ty = y - 1;
        if(tx >= 0 && ty >= 0 && board->pinBoard[ty][tx] == EMPTY){
            if(checkForWin){
                temp = board->pathBoard[ty][tx];
                board->pathBoard[ty][tx] = turn;
                isWin = checkForWinner(board, turn);
                board->pathBoard[ty][tx] = temp;
                if(isWin){
                    moveCount = 0;
                    recordMove(moves, &moveCount, x, y, tx, ty);
                    return -1;
                }
            }
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        //Adjacent moves and pin captures
        tx = x + 1;
        if(tx < 7){
            if(board->pinBoard[y][tx] == EMPTY){
                recordMove(moves, &moveCount, x, y, tx, y);
            }
            else if(board->pinBoard[y][tx] == other){
                tx = x + 2;
                if(tx < 7 && board->pinBoard[y][tx] == EMPTY){
                    recordMove(moves, &moveCount, x, y, tx, y);
                }
            }
        }
        
        tx = x - 1;
        if(tx >= 0){
            if(board->pinBoard[y][tx] == EMPTY){
                recordMove(moves, &moveCount, x, y, tx, y);
            }
            else if(board->pinBoard[y][tx] == other){
                tx = x - 2;
                if(tx >= 0 && board->pinBoard[y][tx] == EMPTY){
                    recordMove(moves, &moveCount, x, y, tx, y);
                }
            }
        }
        
        ty = y + 1;
        if(ty < 7){
            if(board->pinBoard[ty][x] == EMPTY){
                recordMove(moves, &moveCount, x, y, x, ty);
            }
            else if(board->pinBoard[ty][x] == other){
                ty = y + 2;
                if(ty < 7 && board->pinBoard[ty][x] == EMPTY){
                    recordMove(moves, &moveCount, x, y, x, ty);
                }
            }
        }
        
        ty = y - 1;
        if(ty >= 0){
            if(board->pinBoard[ty][x] == EMPTY){
                recordMove(moves, &moveCount, x, y, x, ty);
            }
            else if(board->pinBoard[ty][x] == other){
                ty = y - 2;
                if(ty >= 0 && board->pinBoard[ty][x] == EMPTY){
                    recordMove(moves, &moveCount, x, y, x, ty);
                }
            }
        }
    }
        
    return moveCount;
}

void makeMove(struct Board* board, const struct Move* move){
    enum Color turn = board->turn;
    enum Color other = !turn;
    
    int sx = move->start.x;
    int sy = move->start.y;
    int ex = move->end.x;
    int ey = move->end.y;
    
    board->pinBoard[sy][sx] = EMPTY;
    board->pinBoard[ey][ex] = turn;
    
    int i;
    
    //Move pin marker
    for(i = 0; i < 8; i++){
        if(board->pins[turn][i].x == sx && board->pins[turn][i].y == sy){
            board->pins[turn][i].x = ex;
            board->pins[turn][i].y = ey;
            break;
        }
    }
    
    //Diagonal move
    if(sx != ex && sy != ey){
        //Determine coordinate of placed path
        int px = sx < ex ? sx : ex;
        int py = sy < ey ? sy : ey;
        
        if(board->pathBoard[py][px] == EMPTY || board->pathBoard[py][px] == other){
            board->pathCount[turn]++;
            if(board->pathBoard[py][px] == other){
                board->pathCount[other]--;
            }
            
            board->pathBoard[py][px] = turn;
        }
    }
    
    //Pin captures
    if(abs(sx - ex) == 2){
        int cx = (sx + ex) / 2;
        board->pinBoard[sy][cx] = EMPTY;
        
        //Remove pin from list;
        for(i = 0; i < 8; i++){
            if(board->pins[other][i].x == cx && board->pins[other][i].y == sy){
                board->pins[other][i].x = -1;
                break;
            }
        }
        
        board->pinCount[other]--;
    }
    
    if(abs(sy - ey) == 2){
        int cy = (sy + ey) / 2;
        board->pinBoard[cy][sx] = EMPTY;
        
        //Remove pin from list;
        for(i = 0; i < 8; i++){
            if(board->pins[other][i].y == cy && board->pins[other][i].x == sx){
                board->pins[other][i].x = -1;
                break;
            }
        }
        
        board->pinCount[other]--;
    }
    
    board->turn = other;
}

int checkForWinner(const struct Board* board, const enum Color color){
    //This is a basic algorithm that can probably be improved
    //It starts at the left base, and tries to walk to the right
    
    //Stack of coords to walk
    struct Coord stack[6 * 6];
    int sp = 1; //Stack position, pointing to where the next element is pushed
    
    int crumbs[6][6] = {0};
    
    stack[0].x = 0;
    stack[0].y = color == WHITE ? 5 : 0;
    
    int goalX = 5;
    int goalY = color == WHITE ? 0 : 5;
    
    int x, y, tx, ty;
    while(sp > 0){
        sp--;
        
        x = stack[sp].x;
        y = stack[sp].y;
        
        crumbs[y][x] = 1;
        
        //TODO: The goal checking can probably be improved
        tx = x + 1;
        if(tx == goalX && y == goalY){
            return 1;
        }
        if(tx < 6 && board->pathBoard[y][tx] == color && crumbs[y][tx] == 0){
            stack[sp].x = tx;
            stack[sp].y = y;
            sp++;
        }
        
        tx = x - 1;
        if(tx == goalX && y == goalY){
            return 1;
        }
        if(tx >= 0 && board->pathBoard[y][tx] == color && crumbs[y][tx] == 0){
            stack[sp].x = tx;
            stack[sp].y = y;
            sp++;
        }
        
        ty = y + 1;
        if(x == goalX && ty == goalY){
            return 1;
        }
        if(ty < 6 && board->pathBoard[ty][x] == color && crumbs[ty][x] == 0){
            stack[sp].x = x;
            stack[sp].y = ty;
            sp++;
        }
        
        ty = y - 1;
        if(x == goalX && ty == goalY){
            return 1;
        }
        if(ty >= 0 && board->pathBoard[ty][x] == color && crumbs[ty][x] == 0){
            stack[sp].x = x;
            stack[sp].y = ty;
            sp++;
        }
    }
    
    return 0;
}

int moveCompare(const struct Move* move1, const struct Move* move2){
    if(move1->start.x != move2->start.x) return 1;
    if(move1->start.y != move2->start.y) return 1;
    if(move1->end.x != move2->end.x) return 1;
    if(move1->end.y != move2->end.y) return 1;
    return 0;
}

struct Board stringToBoard(const char* string){
    struct Board board;
    
    int p = 0; //Place in string
    int i;
    
    //Clear the board pin list before we add any``
    for(i = 0; i < 8; i++){
        board.pins[WHITE][i].x = -1;
        board.pins[BLACK][i].x = -1;    
    }

    enum Entity* currentBoard = &board.pinBoard[0][0];
    for(i = 0; i < 49; i++){
        if(i == 0 || i == 6 || i == 42 || i == 48){
            currentBoard[i] = DISABLED;
            continue;
        }
        //addPin(struct Board *board, enum Color color, int x, int y)
        switch(string[p++]){
        case 'w':
            currentBoard[i] = WHITE;
            addPin(&board, WHITE, i % 7, i / 7);
            break;
        case 'b':
            currentBoard[i] = BLACK;
            addPin(&board, BLACK, i % 7, i / 7);
            break;
        default:
            currentBoard[i] = EMPTY;
        }
    }
    
    currentBoard = &board.pathBoard[0][0];
    for(i = 0; i < 36; i++){
        if(i == 5 || i == 30){
            currentBoard[i] = WHITE_BASE;
            continue;
        } else if(i == 0 || i == 35){
            currentBoard[i] = BLACK_BASE;
            continue;
        }
        
        switch(string[p++]){
        case 'w':
            currentBoard[i] = WHITE;
            break;
        case 'b':
            currentBoard[i] = BLACK;
            break;
        default:
            currentBoard[i] = EMPTY;
        }
    }
    
    board.turn = string[p++] == 'w' ? WHITE : BLACK;
    
    return board;
}

struct Move stringToMove(const char string[]){
    struct Move move;
    
    move.start.x = toupper(string[0]) - 65;
    move.start.y = toupper(string[1]) - 49;
    move.end.x = toupper(string[3]) - 65;
    move.end.y = toupper(string[4]) - 49;
    
    return move;
}

void printBoard(const struct Board *board){
    char string[79];
    int p = 0; //Place in string
    int x, y;
        
    //Pin board
    for(y = 0; y < 7; y++){
        for(x = 0; x < 7; x++){
            switch(board->pinBoard[y][x]){
            case WHITE:
                string[p++] = 'w';
                break;
            case BLACK:
                string[p++] = 'b';
                break;
            case EMPTY:
                string[p++] = '.';
                break;
            }
        }
    }
        
    //Path board
    for(y = 0; y < 6; y++){
        for(x = 0; x < 6; x++){
            switch(board->pathBoard[y][x]){
            case WHITE:
                string[p++] = 'w';
                break;
            case BLACK:
                string[p++] = 'b';
                break;
            case EMPTY:
                string[p++] = '.';
                break;
            }
        }
    }
    
    //Turn indicator
    switch(board->turn){
    case WHITE:
        string[p++] = 'w';
        break;
    case BLACK:
        string[p++] = 'b';
        break;
    }
    
    string[p++] = '\0';
    
    printf("%s", string);
}

void printMove(const struct Move* move){
    char startCol = move->start.x + 65;
    char endCol = move->end.x + 65;
    printf("%c%d-%c%d", move->start.x + 65, move->start.y + 1,
                        move->end.x + 65, move->end.y + 1);
}