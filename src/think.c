#include "think.h"

static enum Outcome simulate(struct Board* board){
    struct Move moves[MAX_MOVES];
    int moveCount = getMoves(board, moves, 1);
    
    while(moveCount > 0){
        //TODO: switch to Mersenne twister RNG
        makeMove(board, &moves[rand() % moveCount]);
        moveCount = getMoves(board, moves, 1);
    }
    
    if(moveCount == -1){
        return board->turn;
    }else{
        return DRAW;
    }
}

struct Move think(struct Board* board){
    struct Move moves[MAX_MOVES];
    int moveCount = getMoves(board, moves, 1);
    int i;
    
    if(moveCount == -1){
        return moves[0];
    }
    
    int wins[MAX_MOVES] = {0}; 
    
    int n;
    struct Board testBoard;
    enum Outcome outcome;
    for(i = 0; i < moveCount; i++){
        for(n = 0; n < SIMS; n++){
            testBoard = *board;
            makeMove(&testBoard, &moves[i]);
        
            outcome = simulate(&testBoard);
            if(outcome == board->turn){
                wins[i]++;
            }
        }
    }
    
    int mostWins = 0;
    int bestMove;
    for(i = 0; i < moveCount; i++){
        if(wins[i] > mostWins){
            mostWins = wins[i];
            bestMove = i;
        }
    }
    
    if(mostWins = 0){
        bestMove = rand() % moveCount;
    }
    
    return moves[bestMove];
}
