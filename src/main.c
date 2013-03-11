#include <stdio.h>
#include <string.h>
#include "board.h"

int main(){
	//Turn output buffering off
	setbuf(stdout, NULL);

	//Input buffer and associated size variables
	char *input = NULL;
	size_t inputLen = 0;
	ssize_t inputRead;
	
	//Pointers to the command and args in the input buffer
	char *command;
	char *args;
	
	struct Board board = newBoard();
	
	int i;
		
	puts("Pilgrim v.3a ready");
	
	//Engine read-eval-print loop
	for(;;){
		//Get input, skip if any errors, and trim newline off the end
		inputRead = getline(&input, &inputLen, stdin);
		if(inputRead <= 1){
			continue;
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

		//Invalid command
		else{
			printf("Invalid command: %s\n", command);
		}
	}
	
	free(input);
	return 0;
}