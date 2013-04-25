#include <stdio.h>
#include <string.h>
#include <time.h>
#include "board.h"
#include "think.h"

int main(){
	//Turn output buffering off
	setbuf(stdout, NULL);

	//Seed the RNG
	srand(time(NULL));

	//Input buffer and associated size variables
	char *input = NULL;
	size_t inputLen = 0;
	ssize_t inputRead;
	
	//Pointers to the command and args in the input buffer
	char *command;
	char *args;
	
	struct Board board = newBoard();
//	board = stringToBoard("b...wb.b...wb.....w.......w.....bw....wbww.bb.......www...w....w...www.......w");
    
	int i;
		
	puts("Pilgrim v.5a ready");
	
	
	//Engine read-eval-print loop
	for(;;){
		//Get input, skip if any errors, and trim newline off the end
		inputRead = getline(&input, &inputLen, stdin);
		if(inputRead <= 1){
			break; //TODO: figure out a better way to quit once stdin breaks
			//continue;
		}
		input[inputRead - 1] = '\0';
		
		//Convert input to lower case
		for(i = 0; input[i]; i++){
			input[i] = tolower(input[i]);
		}
		
		//Split at the first space to separate command and arguments
		//NOTE: This changes (invalidates?) the input variable
		command = strtok(input, " ");
		args = strtok(NULL, ""); 
		
		//Interpret the command and args
		
		//stop - stops the engine
		if(strcmp("stop", command) == 0){
			break;
		}
		
		//new - starts a new game, resetting 
		else if(strcmp("new", command) == 0){
			board = newBoard();
		}

		//getboard - prints the serialized board 
		else if(strcmp("getboard", command) == 0){
			printBoard(&board);
			putchar('\n');
		}
		
		//setboard - sets the board to a certain position
		else if(strcmp("setboard", command) == 0){
			board = stringToBoard(args);
		}
		
		//getmoves - prints a list of possible moves
		else if(strcmp("getmoves", command) == 0){
			struct Move moves[MAX_MOVES];
			int moveCount = getMoves(&board, moves, 0);
			for(i = 0; i < moveCount; i++){
				printMove(&moves[i]);
				putchar('\n');
			}
			puts("done");
		}
		
		//getwinner - prints the winner
		//('w' for white, 'b' for black, 'd' for draw, '.' for none)
		else if(strcmp("getwinner", command) == 0){
			if(checkForWinner(&board, WHITE) == 1){
				puts("w");
			}else if(checkForWinner(&board, BLACK) == 1){
				puts("b");
			}else{
				//TODO: implement draw checking
				puts(".");
			}
		}
		
		//move - makes a move
		else if(strcmp("move", command) == 0){
			struct Move move = stringToMove(args);
			
			//Make sure it's a legal move	
			int legal = 0;
			struct Move moves[MAX_MOVES];
			int moveCount = getMoves(&board, moves, 0);
			for(i = 0; i < moveCount; i++){
				if(moveCompare(&move, &moves[i]) == 0){
					legal = 1;
					break;
				}
			}
			
			if(legal == 0){
				printf("invalid move: ");
				printMove(&move);
				putchar('\n');
				continue;
			}
			
			makeMove(&board, &move);
		}

		//think - AI analyzes the board and gives its move
		else if(strcmp("think", command) == 0){
			struct Move move = think(&board);
			printMove(&move);
			
			putchar('\n');
		}
		
		//Invalid command
		else{
			printf("Invalid command: %s\n", command);
		}
	}
	
	free(input);
	return 0;
}
