#include "think.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#define POS_INFINITY INT_MAX
#define NEG_INFINITY INT_MIN

#define UCTK 1.0 //Parameter controlling exploration vs. exploitation
#define AK 1 //Parameter for final move selection

struct Node{
    struct Node* parent;
    struct Move move; //Move to get to this node
    
    struct Node* children[MAX_MOVES];
    int childrenCount;
    
    int visits;
    float value;
};

struct Node* makeNode(struct Node* parent, struct Move move){
    struct Node* node = (struct Node*) malloc(sizeof(struct Node));

    node->parent = parent;
    node->move = move;
    node->childrenCount = -2;
    
    return node;
}

float calculateUCT(const struct Node* node){
    //Check for infinity first, to avoid wrapping around in equation
    if(node->value == POS_INFINITY) return POS_INFINITY;
    if(node->value == NEG_INFINITY) return NEG_INFINITY;
    
    return node->value + sqrt((UCTK * log(node->parent->visits)) / node->visits);
}

struct Node* select(const struct Node* node){
    int i;

    float bestUCT = NEG_INFINITY;
    struct Node* bestNode = NULL;
    for(i = 0; i < node->childrenCount; i++){
        if(node->children[i]->visits == 0){
            return node->children[i];
        }
    
        float uct = calculateUCT(node->children[i]);
        if(uct >= bestUCT){
            bestUCT = uct;
            bestNode = node->children[i];
        }
    }
    
    return bestNode;
}

static int simulate(struct Board* board){
    struct Move moves[MAX_MOVES];
    int moveCount = getMoves(board, moves, 1);
    enum Color turn = board->turn;
    
    while(moveCount > 0){
        //TODO: switch to Mersenne twister RNG
        makeMove(board, &moves[rand() % moveCount]);
        moveCount = getMoves(board, moves, 1);
    }
    
    if(moveCount == -1){
        if(board->turn == turn){
            return 1;
        }else{
            return -1;
        }
    }else{
        return 0;
    }
}

void updateValue(struct Node* node, int r){
    if(r == POS_INFINITY){
        node->value = POS_INFINITY;
        return;
    }else if(r == NEG_INFINITY){
        node->value = NEG_INFINITY;
        return;
    }

    node->value = ((node->value * (node->visits - 1)) + r) / node->visits;
}

static int solver(struct Board* board, struct Node* node){
    int i;
    
    //If node's children haven't been generated, generate them
    if(node->childrenCount == -2){
        struct Move moves[MAX_MOVES];
        int moveCount = getMoves(board, moves, 1);

        node->childrenCount = moveCount;
        for(i = 0; i < moveCount; i++){
            struct Node* child = makeNode(node, moves[i]);
            node->children[i] = child;
        }
    }

    //If there's a win, take it
    if(node->childrenCount == -1){
        return POS_INFINITY;
    }
    
    struct Node* bestChild = select(node);
    makeMove(board, &bestChild->move);

    node->visits++;
    
    int r;
    
    if(bestChild->value != POS_INFINITY && bestChild->value != NEG_INFINITY){
        //If the nodes has no visits, simulate it
        if(bestChild->visits == 0){
            r = -simulate(board);
            updateValue(node, r);
            bestChild->visits++;
            return r;
        }else{
            //Else go farther down the tree
            r = -solver(board, bestChild);
        }
    }else{
        //Game-theoretical value
        r = -bestChild->value;
    }

    //Game-theoretical logic 
    if(r == NEG_INFINITY){
        node->value = NEG_INFINITY;
        return r;
    }else if(r == POS_INFINITY){
        for(i = 0; i < node->childrenCount; i++){
            if(node->children[i]->value != NEG_INFINITY){
                r = -1;
                break;
            }
            
            if(r == POS_INFINITY){
                node->value = POS_INFINITY;
                return r;
            }
        }
    }
    
    updateValue(node, r);
    return r;
}

void freeNode(struct Node* node){
    int i;
    for(i = 0; i < node->childrenCount; i++){
        freeNode(node->children[i]);
    }
    free(node);
}

struct Move think(struct Board* board){
    struct Move moves[MAX_MOVES];
    int moveCount = getMoves(board, moves, 1);
    int i;
    
    //Take a win without thinking
    if(moveCount == -1){
        return moves[0];
    }
        
    struct Node* root = makeNode(NULL, moves[0]);
    for(i = 0; i < 100000 ; i++){
        struct Board testBoard = *board;
        
        int winner = solver(&testBoard, root);
        if(winner == POS_INFINITY || winner == NEG_INFINITY){
            break;
        }
    }

    //Final move selection
    float bestScore = NEG_INFINITY;
    struct Node* bestChild = NULL;
    for(i = 0; i < root->childrenCount; i++){
        struct Node* child = root->children[i];
        float score = child->value * (AK / sqrt(child->visits));

        if(score >= bestScore){
            bestScore = score;
            bestChild = child;
        }
    }

    struct Move finalMove = bestChild->move;
    
    freeNode(root);
    
    return finalMove;
}
