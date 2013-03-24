#ifndef THINK_H
#define THINK_H

#include <stdio.h>
#include "board.h"

#define SIMS 10000

struct Move think(struct Board*);

#endif