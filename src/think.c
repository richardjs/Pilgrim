#include "think.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//Other constants (NOT to be changed to tweak algorithm)
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
    
    node->visits = 0;
    node->value = 0.0;
    
    return node;
}

float calculateUCT(const struct Node* node){
    return -node->value + sqrt(UCTK * log(node->parent->visits) / node->visits); //TODO: normally +
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
    
    int depth = 0;
    while(moveCount > 0 && depth < MAX_SIM_DEPTH){
        //TODO: switch to Mersenne twister RNG
        makeMove(board, &moves[rand() % moveCount]);
        moveCount = getMoves(board, moves, 1);
        depth++;
    }
    
    //If max depth reached
    if(depth == MAX_SIM_DEPTH){
        if(board->pinCount[turn] > board->pinCount[!turn]){
            return 1;
        } else if(board->pinCount[!turn] > board->pinCount[turn]){
            return -1;
        } else{
            return 0;
        }
    }
    
    //If a win is found
    if(moveCount == -1){
        if(board->turn == turn){
            return 1;
        }else{
            return -1;
        }
    }
    
    //Draw due to no moves
    return 0;
}

void update(struct Node* node, float r){
    node->visits++;
    node->value = ((node->value * (node->visits - 1)) + r) / node->visits;
}

static float solver(struct Board* board, struct Node* node){
    int i;

    //If the node's children haven't been generated, generate them
    if(node->childrenCount == NEEDS_CHILDREN){
        struct Move moves[MAX_MOVES];
        node->childrenCount = getMoves(board, moves, 1);

        for(i = 0; i < node->childrenCount; i++){
            struct Node* child = makeNode(node, moves[i]);
            node->children[i] = child;
        }
    }

    //If there's a win, take it. If there's no children, it's a draw.
    if(node->childrenCount == -1){
        update(node, INFINITY);
        return INFINITY;
    }else if(node->childrenCount == 0){
        //TODO: do we need to make this game-theoretical?
        update(node, 0);
        return 0;
    }

    struct Node* bestChild = select(node);
    makeMove(board, &bestChild->move);

    float r; //result for node value this iteration, *always from this node's perspective*

    if(bestChild->value != INFINITY && bestChild->value != -INFINITY){
        //If the nodes has no visits, simulate it
        if(bestChild->visits < MIN_SIMS){
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

    if(r == -INFINITY){
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
    int start = time(0);

    struct Move moves[MAX_MOVES];
    int moveCount = getMoves(board, moves, 1);
    int i;
    
    //Take a win without thinking
    if(moveCount == -1){
        return moves[0];
    }
        
    struct Node* root = makeNode(NULL, moves[0]); //Second arg meaningless here
    int iterations;
    for(iterations = 0; iterations < ITERATIONS ; iterations++){
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
        float score = -child->value + (AK / sqrt(child->visits));

        if(score >= bestScore){
            bestScore = score;
            bestChild = child;
        }
    }

    struct Move finalMove = bestChild->move;
    
    int end = time(0);
    
    int elapsed = end - start;
    
    fprintf(stderr, "Score:\t\t%f\n", -bestChild->value);
    fprintf(stderr, "Visits:\t\t%d\n", bestChild->visits);
    fprintf(stderr, "Focus:\t\t%f\n", 100.0*bestChild->visits/iterations);
    fprintf(stderr, "Iterations:\t%d\n", iterations);
    fprintf(stderr, "Elapsed:\t%ds\n", elapsed);
    fprintf(stderr, "\n");

    freeNode(root);
    
    return finalMove;
}
