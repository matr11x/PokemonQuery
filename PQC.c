#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "PQC.h"

#define SERVER_PORT 80
#define SERVER_IP "127.0.0.1"

int main(){
	int clientSocket;
	struct sockaddr_in serverAddr;
	int status, bytesRcv;
	char buffer[100]; //stores user input

	pthread_t saveThread;
	
	//initaliaze the struct
        PokemonData pokemonList;
        pokemonList.pokemons = NULL;
        pokemonList.num_matches = 0;
        pokemonList.totalQueries = 0;
        pthread_mutex_init(&pokemonList.mutex, NULL);
        pokemonList.type = NULL; //initialize type field
        pokemonList.savedFiles = NULL;
        pokemonList.totalSavedFiles = 0;
	
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0){
		printf("Client ERROR: Could not open socket.\n");
		exit(-1);
	}
	
	//Setup the server address
	memset(&serverAddr, 0, sizeof(serverAddr)); //zeros the struct
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons((unsigned short) SERVER_PORT);
	
	//bind the server socket
	status = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	
	if (status < 0){
		printf("Unable to establish connection to the PPS!\n");
		exit(-1);
	}

	while(1){
		int menuOption;
		
		do {
		displayMenu();
		scanf("%d", &menuOption);
			switch (menuOption) {
				case 1: {
				    printf("Enter the pokemon type 1: ");
				    scanf("%ms", &pokemonList.type);
			            
			            strcpy(buffer, pokemonList.type);
			            send(clientSocket, buffer, strlen(buffer),0);			    
					
				    int total;

				    bytesRcv = recv(clientSocket, &total, sizeof(total), 0);
				    
				    for (int i = 0; i < total; i++) {
				    	    char lineBuffer[100];
					    bytesRcv = recv(clientSocket, lineBuffer, sizeof(lineBuffer), 0);
					    lineBuffer[bytesRcv] = '\0';
					    pokemonList.num_matches++;
					    pokemonList.pokemons = (char**)realloc(pokemonList.pokemons, pokemonList.num_matches * sizeof(char*));
					    pokemonList.pokemons[pokemonList.num_matches - 1] = strdup(lineBuffer);
			            }
				    pokemonList.totalQueries++;
				    
				    free(pokemonList.type);
				    break;
				}
				case 2: {
				    //create a thread for saving query results
				    pthread_create(&saveThread, NULL, saveQueryResults, (void*)&pokemonList);
				    //wait for the save thread to finish
				    pthread_join(saveThread, NULL);
				    break;
				}
			}

		} while (menuOption != 3); //loop until the user enters a 3 to exit the program	
		
		send(clientSocket, "3", strlen("3"),0); //send the server "3" so it knows that the client is exiting    
		break;			
	}
	
    	//display new files created during the session
	printf("\nDisplaying names of new files created during the session:\n");
    	for (int i = 0; i < pokemonList.totalSavedFiles; i++) {
            	printf("%s\n", pokemonList.savedFiles[i]);
    	}
 
    	printf("\nThe total number of successful queries are: %d\n", pokemonList.totalQueries);
    	   	
    	//free the dynamiclally allocated memory
    	for (int i = 0; i < pokemonList.totalSavedFiles; i++) {
        	free(pokemonList.savedFiles[i]);
    	}
    	free(pokemonList.savedFiles);
    	   	
	for (int i = 0; i < pokemonList.num_matches; i++) {
        	free(pokemonList.pokemons[i]);
    	}
    	free(pokemonList.pokemons);
    	
    	//close client socket
	close(clientSocket);	
}
