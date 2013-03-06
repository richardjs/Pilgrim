#include <stdio.h>
#include <string.h>

int main(){
	//Input buffer and associated size variables
	char *input = NULL;
	size_t inputLen = 0;
	ssize_t inputRead;
	
	char *command;
	char *args;
	
	int i;
	
	puts("Pilgrim v.2a ready");
	
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
		
		//Interpret the command and args:
		
		//stop - stops the engine
		if(strcmp("stop", command) == 0){
			break;
		}
		
		if(strcmp("sayhi", command) == 0){
			sayhi();
		}
		
		//Invalid command
		else{
			printf("Invalid command: %s\n", command);
		}
	}
	
	free(input);
	return 0;
}