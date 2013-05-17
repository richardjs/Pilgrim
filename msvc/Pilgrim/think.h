#ifndef THINK_H
#define THINK_H

#include <stdio.h>
#include "board.h"

//Total number of iterations; should control overall playing strength
#define ITERATIONS 100000 

//Parameter controlling exploration vs. exploitation in tree search
#define UCTK .9

//Number of simulations a node must have before it's added to the tree 
#define MIN_SIMS 50

//Maximum depth a simulation can go before it stopped and scored via heuristic
#define MAX_SIM_DEPTH 150

//Chance that a move in a simulation must be a capture
#define SIM_FORCE_CAPTURE_CHANCE 0

//Parameter for final move selection; pretty technical, shouldn't need tweaking
#define AK 1

struct Move think(struct Board*);

#endif
