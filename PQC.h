#include <stdio.h>

//struct to hold the pokemons
typedef struct {
    char ** pokemons; //struct to hold the file lines
    int num_matches; //total type 1 matches 
    int totalQueries;
    pthread_mutex_t mutex;
    char* type; //save the pokemon type
    char** savedFiles; //store the users files created in an array
    int totalSavedFiles;
} PokemonData;


void* saveQueryResults(void* args);
void displayMenu();
