#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "PPS.h"


//function for searching with type 1 of the pokemon
void search_pokemons_by_type(FILE* file, char* buffer, char*** pokemons, int* count) {
	fseek(file, 0, SEEK_SET); //reads the file from the first line
	char line[150];
	fgets(line, sizeof(line), file); //skip the header line

	//read each line of the file
	while (fgets(line, sizeof(line), file)) {

		char* lineCopy = strdup(line); //make a temp copy of the line

		char* splitLine = strtok(lineCopy, ",");

		// We need to use strtok twice more to skip the first two fields (id and name)
		splitLine = strtok(NULL, ",");
		splitLine = strtok(NULL, ",");

		//if type 1 of the pokemon matches user input, add it to the array
		if (splitLine && strcmp(splitLine, buffer) == 0) {
		    (*count)++;
		    *pokemons = (char**)realloc(*pokemons, (*count) * sizeof(char*));
		    (*pokemons)[(*count) - 1] = strdup(line);
		}

		free(lineCopy);
	}
}

//function that sends the line from the file to the server
void sendLine(int *clientSocket, char* str){
	send(*clientSocket, str, strlen(str), 0);
	usleep(1000); //sleep so each line is sent one at a time
}
