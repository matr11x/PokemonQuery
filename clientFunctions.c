#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "PQC.h"

//function to save pokemons in memory to the file;
void* saveQueryResults(void* args) {
    PokemonData* pokemonList = (PokemonData*)args;
    FILE* file;
    char* saveFile;

    printf("Enter the name of the file which will contain the query results: ");

    while (1) {

        scanf("%ms", &saveFile);

        //check to see if the file exits
        file = fopen(saveFile, "w");
        if (file == NULL) {
            printf("Unable to create the new file. Please enter the name of the file again: ");
        }
        else {
       	    //lock the mutex before accessing the shared data
            pthread_mutex_lock(&pokemonList->mutex);
              
            //allocate memory for the new file name
	    char* newFileName = malloc(strlen(saveFile) + 1);
	    strcpy(newFileName, saveFile);

	    //increase the size of the dynamically allocated array and save it 
	    char** tempFiles = realloc(pokemonList->savedFiles, (pokemonList->totalSavedFiles + 1) * sizeof(char*));
	    if (tempFiles == NULL){
	    	printf("Failled to realloc\n");
	    	exit(1);
    	    }
	    pokemonList->savedFiles = tempFiles;
	    pokemonList->savedFiles[pokemonList->totalSavedFiles] = newFileName;
	    pokemonList->totalSavedFiles++;
	    //free(tempFiles);
            
            fprintf(file, "#,Name,Type 1,Type 2,Total,HP,Attack,Defense,Sp.Atk,Sp.Def,Speed,Generation,Legendary\n"); //write the file header

            //print the lines to the file
            for (int i = 0; i < pokemonList->num_matches; i++) {
            	fprintf(file, "%s", pokemonList->pokemons[i]);
    	    }

            //unlock the mutex after accessing the shared data
            pthread_mutex_unlock(&pokemonList->mutex);
            fclose(file);
            break;
        }
    }
    free(saveFile);
    pthread_exit(NULL);
}

//displays the menu
void displayMenu() {
    printf("\n1. Type search\n");
    printf("2. Save results\n");
    printf("3. Exit the program\n");
}
