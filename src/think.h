#ifndef THINK_H
#define THINK_H

#include <stdio.h>
#include "board.h"

//Algorithm parameters
//TODO: =============== ============ Why did we have >100% focus? We had lots of visits

#define ITERATIONS 300000
#define UCTK .4 //Parameter controlling exploration vs. exploitation
#define MIN_VISITS 500 //Number of moves a node needs before it's added to the tree 
#define SIM_FORCE_CAPTURE_CHANCE .3 //Chance that a given move in a simulation must be a capture
#define AK 1 //Parameter for final move selection

struct Move think(struct Board*);

#endif