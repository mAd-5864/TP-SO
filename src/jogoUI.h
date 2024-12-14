#ifndef JOGOUI_H
#define JOGOUI_H

#include "estruturas.h"

// Declarar funções
void validateCommands(char *command, Player user, WINDOW *janela_comandos);
void playersCommand(WINDOW *janela_comandos);
void msgCommand(ChatMessage chat, WINDOW *janela_comandos);
void printMaze(WINDOW *win, char maze[ROWS][COLS]);
void createFIFOs(Player *this);
void cleanupFIFOs(Player *this);
void registarMotor(const char *username);
void esperarInicioMotor(Player this);
void deserializeGameInfo(const char *buffer, Game *gameInfo);
void *gameStateListener(void *arg);
void *listenForMessages(void *arg);
void enviarMovimentoJogador(char direction, const char *fifo);
void *updateMaze(void *arg);
#endif
