#Pilgrim

Pilgrim is an AI engine for Creeper, an abstract strategy game by [Graham's Games](http://www.grahams-games.co.uk/creeper.html).
It began as an attempt to give some AI testers for [Harding University's](http://www.harding.edu) senior computer science
capstone class an opponent to play against in preparing to play the student projects.

###Algorithm

Pilgrim is based on [MCTS-Solver](https://dke.maastrichtuniversity.nl/m.winands/documents/uctloa.pdf), a refinement of
Monte-Carlo Tree Search, which itself is an application of the [Monte Carlo method](http://en.wikipedia.org/wiki/Monte_carlo_method)
to game playing. For more information on MCTS, see [this excellent overview](https://project.dke.maastrichtuniversity.nl/games/files/phd/Chaslot_thesis.pdf).

###Status

Pilgrim currently plays a decent game at a beginning-intermediate level, but struggles with providing a truly challenging
game. There are several algorithm parameters that could be tweaked to potentially increase playing strength, and there are
featuers of the algorithm implementation that could be improved.

###Building and running

Pilgrim is written ANSI C, except using C99's INFINITY value. Tkreeper, a rough GUI, is written in Python 2. Any modern C
compiler should be able to compile the engine, and a short build script is provided for GCC. If you don't have Python, get
it from the [Python website](http://www.python.org). The latest version of Python 2 (not Python 3) should work.

###Tkreeper

Pilgrim is bundled with Tkreeper, a rough GUI board. Its purpose is to provide a simple way to play against the engine.
It was implemented very quickly, and its code is in a pretty messy state. With a cleanup and some fleshing out, it could
provide a complete GUI board.

###Other notes

This was my first real pure C project. I'd welcome any advice on idiomatic or effective C programming.
