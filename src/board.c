#include <stdio.h>
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

void printBoard(const struct Board *board){
    char string[79];
    int p = 0; //Place in string
    int x, y;
        
    //Pin board
    for(x = 0; x < 7; x++){
        for(y = 0; y < 7; y++){
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
    for(x = 0; x < 6; x++){
        for(y = 0; y < 6; y++){
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
    
    int  i, x, y, tx, ty;
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
            //TODO: check for win here and on all moves
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        tx = x + 1;
        ty = y - 1;
        if(tx < 7 && ty >= 0 && board->pinBoard[ty][tx] == EMPTY){
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        tx = x - 1;
        ty = y + 1;
        if(tx >= 0 && ty < 7 && board->pinBoard[ty][tx] == EMPTY){
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        tx = x - 1;
        ty = y - 1;
        if(tx >= 0 && ty >= 0 && board->pinBoard[ty][tx] == EMPTY){
            recordMove(moves, &moveCount, x, y, tx, ty);
        }
        
        //Adjacent moves and captures
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

void printMove(const struct Move* move){
    char startCol = move->start.x + 65;
    char endCol = move->end.x + 65;
    printf("%c%d-%c%d", move->start.x + 65, move->start.y + 1,
                        move->end.x + 65, move->end.y + 1);
}