#ifndef THINK_H
#define THINK_H

#include <stdio.h>
#include "board.h"

//Total number of iterations; should control overall playing strength
#define ITERATIONS 100000 

//Parameter controlling exploration vs. exploitation in tree search
#define UCTK .4 

//Number of simulations a node must have before it's added to the tree 
#define MIN_SIMS 5 

//Maximum depth a simulation can go before it stopped and scored via heuristic
#define MAX_SIM_DEPTH 150

//Parameter for final move selection; pretty technical--shouldn't need tweaking
#define AK 1

//#define SIM_FORCE_CAPTURE_CHANCE .3 //Chance that a given move in a simulation must be a capture

struct Move think(struct Board*);

#endif