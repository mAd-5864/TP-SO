#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

//Constantes
#define MAX_USERNAME_SIZE 30
#define MAX_COMMAND_SIZE 50
#define MAX_PLAYERS 5
#define MAX_BOTS 10

//Defenir Estruturas
typedef struct {
    int x;
    int y;
} MobileBlock;

typedef struct {
    char name[MAX_USERNAME_SIZE];
} Bot;

typedef struct {
    char username[MAX_USERNAME_SIZE];
    int fd_pipe;
} Player;


typedef struct {
    Player players[MAX_PLAYERS];
    Bot bots[MAX_BOTS];
    int nPlayers;
    int nBots;
    int pipegen_fd; // file descriptor for general pipe
} Game;


/* typedef struct {
    int signupWindowDurationSeconds;
    int firstLevelDurationSeconds;
    int levelDurationDecreaseSeconds;
    int minPlayers;
} GameSettings; */

#endif