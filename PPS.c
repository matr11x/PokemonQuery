#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "PPS.h"

#define SERVER_PORT 80
#define SERVER_IP "127.0.0.1"

int main(){
	
	char** pokemons = NULL;
	FILE* file;
    	char* fileName;

	int serverSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	int status, addrSize, bytesRcv;
	char buffer[30];
	
	//create the server socket
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket < 0){
		printf("SERVER ERROR: Could not open socket.\n");
		exit(-1);
	}
	
	//Setup the server address
	memset(&serverAddr, 0, sizeof(serverAddr)); //zeros the struct
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons((unsigned short) SERVER_PORT);
	
	//bind the server socket
	status = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	
	if (status < 0){
		printf("SERVER ERROR: Could not bind socket.\n");
		exit(-1);
	}
	
        printf("Enter the name of the file containing the Pokemon descriptions: ");
	
	//loop until the user enters a valid file name
    	while (1) {
        	scanf("%ms", &fileName);
        	//if user enters a 'q', the program exits
        	if (strcmp(fileName, "q") == 0) {
            		exit(0);
        	}
        	//check to see if the file exists
        	file = fopen(fileName, "r");
        	if (file == NULL) {
            		printf("Pokemon file is not found. Please enter the name of the file again or enter 'q' to quit: ");
        	}
        	else {
            		break;
        	}
    	}    	
	
	//set up the line-up to handle 1 client in line
	status = listen(serverSocket, 1);
	if (status < 0){
		printf("SERVER ERROR: Could not listen on socket.\n");
		exit(-1);
	}
	
	//waiting for clients
	while(1){
		addrSize = sizeof(clientAddr);
		clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
		if (clientSocket < 0){
			printf("Server Error: Could not accept incoming client connection.\n");
			exit(-1);
		}
		printf("SERVER: Recieved client connection.\n");
		
		//go into infinite loop to talk to client
		while(1){
			
			int count = 0;
			//get clients type 1 search
			bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
			buffer[bytesRcv] = '\0';
			
			if (strcmp(buffer, "3") == 0){
				printf("SERVER: Closing client connection.\n");
        			close(clientSocket);
				break;
			}
			
			printf("SERVER: Received client request: %s\n", buffer);
			
			printf("Sending matching pokemons back to client....\n");
		
			search_pokemons_by_type(file, buffer, &pokemons, &count); //search the file to find those pokemon records where type 1 matches gamers input
	
			send(clientSocket, &count, sizeof(count), 0); //sends the amount of pokemons where type 1 matches to the client
			 
			//sends the pokemons to the client
			for (int i = 0; i < count; i++) {
				sendLine(&clientSocket, pokemons[i]);
			} 
			 
			for (int i = 0; i < count; i++) {
        			free(pokemons[i]);
    			}
		}
	 }
	
	// Don't forget to close the sockets!
	 close(serverSocket);
	 printf("SERVER: Shutting down.\n");

	//free dynamically allocated memory
	free(pokemons);
	free(fileName);
}
