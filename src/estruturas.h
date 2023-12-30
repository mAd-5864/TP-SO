#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

// Constantes
#define MAX_USERNAME_SIZE 30
#define MAX_COMMAND_SIZE 50
#define MAX_BOTS 10
#define MAX_PLAYERS 5
#define MIN_PLAYERS 2
#define INSCRICAO 20
#define ROWS 16
#define COLS 40
#define MAX_FIFO_NAME_SIZE 50
#define MAX_MESSAGE_SIZE 100
#define BUFFER_SIZE (sizeof(int) + (MAX_PLAYERS * MAX_USERNAME_SIZE) + (ROWS * COLS));

// Defenir Estruturas
typedef struct {
    int x;
    int y;
} Position;

typedef struct
{
    char name[MAX_USERNAME_SIZE];
} Bot;

typedef struct
{
    char username[MAX_USERNAME_SIZE];
    Position Position;
    char fifo_to_motor[MAX_FIFO_NAME_SIZE];   // FIFO para enviar para o motor
    char fifo_from_motor[MAX_FIFO_NAME_SIZE]; // FIFO para receber do motor
} Player;

typedef struct
{
    char maze[ROWS][COLS];
    Player players[MAX_PLAYERS];
    //Bot bots[MAX_BOTS];
    int nPlayers;
    //int nBots;
    //int pipegen_fd;
} Game;

typedef struct {
    char remetente[MAX_USERNAME_SIZE];
    char destinatario[MAX_USERNAME_SIZE];
    char msg[MAX_MESSAGE_SIZE];
} ChatMessage;


/* typedef struct {
    int signupWindowDurationSeconds;
    int firstLevelDurationSeconds;
    int levelDurationDecreaseSeconds;
    int minPlayers;
} GameSettings; */

#endif