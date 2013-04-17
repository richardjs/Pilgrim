#include "think.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#define UCTK 1.0 //Parameter controlling exploration vs. exploitation
#define AK 1 //Parameter for final move selection

#define NEEDS_CHILDREN -2 //Indicates that a node has not had children generated yet

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
    node->childrenCount = NEEDS_CHILDREN;
    
    return node;
}

float calculateUCT(const struct Node* node){
    return -node->value + sqrt((UCTK * log(node->parent->visits)) / node->visits);
}

struct Node* select(const struct Node* node){
    int i;

    float bestUCT = -INFINITY;
    struct Node* bestChild = NULL;
    
    for(i = 0; i < node->childrenCount; i++){
        if(node->children[i]->visits == 0){
            return node->children[i];
        }

        float uct = calculateUCT(node->children[i]);
        if(uct >= bestUCT){
            bestUCT = uct;
            bestChild = node->children[i];
        }
    }
    
    return bestChild;
}

static int playOut(struct Board* board){
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

void update(struct Node* node, float r){
    node->visits++;
    node->value = ((node->value * (node->visits - 1)) + r) / node->visits;
}

static float solver(struct Board* board, struct Node* node){
    int i;
    
    //If node's children haven't been generated, generate them
    if(node->childrenCount == NEEDS_CHILDREN){
        struct Move moves[MAX_MOVES];
        node->childrenCount = getMoves(board, moves, 1);

        for(i = 0; i < node->childrenCount; i++){
            struct Node* child = makeNode(node, moves[i]);
            node->children[i] = child;
        }
    }

    //If there's a win, take it
    if(node->childrenCount == -1){
        update(node, INFINITY);
        return INFINITY;
    }
    
    struct Node* bestChild = select(node);
    makeMove(board, &bestChild->move);
    
    float r;
    
    if(bestChild->value != INFINITY && bestChild->value != -INFINITY){
        //If the nodes has no visits, simulate it
        if(bestChild->visits < 1){
            r = -playOut(board);
            update(bestChild, -r);
        }else{
            //Else go farther down the tree
            r = -solver(board, bestChild);
        }
    }else{
        //Game-theoretical value
        r = -bestChild->value;
    }

    //Game-theoretical logic 
    if(r == INFINITY){
        update(node, INFINITY);
        return r;
    }else if(r == -INFINITY){
        for(i = 0; i < node->childrenCount; i++){
            if(-node->children[i]->value != -INFINITY){
                r = -1;
                break;
            }
        }
    }
    
    update(node, r);
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
        
    struct Node* root = makeNode(NULL, moves[0]); //Second arg meaningless here
    for(i = 0; i < 100000 ; i++){
        struct Board testBoard = *board;

        float result = solver(&testBoard, root);
        if(result == INFINITY || result == -INFINITY){
            break;
        }
    }

    //Final move selection
    float bestScore = -INFINITY;
    struct Node* bestChild = NULL;
    for(i = 0; i < root->childrenCount; i++){
        struct Node* child = root->children[i];
        float score = -child->value * (AK / sqrt(child->visits));

        if(score >= bestScore){
            bestScore = score;
            bestChild = child;
        }
    }

    struct Move finalMove = bestChild->move;

    fprintf(stderr, "Score: %f\n", -bestChild->value);

    freeNode(root);
    
    return finalMove;
}
