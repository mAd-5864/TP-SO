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
typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    char username[MAX_USERNAME_SIZE];
    Position Position;
    char fifo_to_motor[MAX_FIFO_NAME_SIZE];   // FIFO para enviar para o motor
    char fifo_from_motor[MAX_FIFO_NAME_SIZE]; // FIFO para receber do motor
    char message_fifo[MAX_FIFO_NAME_SIZE];    // FIFO para receber mensagens
    pthread_t movementThread;                 // Thread para ler movimentos
    int terminou;
} Player;

typedef struct
{
    char maze[ROWS][COLS];
    Player players[MAX_PLAYERS];
    int nPlayers;
    int nivel;
    int podio;
} Game;

typedef struct
{
    char remetente[MAX_USERNAME_SIZE];
    char destinatario[MAX_USERNAME_SIZE];
    char msg[MAX_MESSAGE_SIZE];
} ChatMessage;

typedef struct
{
    pid_t pid;
    int pipe;
    int intervalo;
    int duracao;
} Bot;

// Configs bots
#define BOTS_NVL1 2
#define BOTS_NVL2 3
#define BOTS_NVL3 4

// Nivel 1
Bot nivel1_bots[BOTS_NVL1] = {
    {0, 0, 30, 10}, // Bot 1
    {0, 0, 25, 5}   // Bot 2
};

// Nivel 2
Bot nivel2_bots[BOTS_NVL2] = {
    {0, 0, 30, 15}, // Bot 1
    {0, 0, 25, 10}, // Bot 2
    {0, 0, 20, 5}   // Bot 3
};

// Nivel 3
Bot nivel3_bots[BOTS_NVL3] = {
    {0, 0, 30, 20}, // Bot 1
    {0, 0, 25, 15}, // Bot 2
    {0, 0, 20, 10}, // Bot 3
    {0, 0, 15, 5}   // Bot 4
};
#endif