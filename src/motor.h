#ifndef MOTOR_H
#define MOTOR_H
#include "estruturas.h"

void validateAdminCommands(WINDOW *cmd_win, char *command);
void usersCommand(WINDOW *cmd_win);
void kickCommand(WINDOW *cmd_win, char *username);
void botsCommand(WINDOW *cmd_win);
void bmovCommand(WINDOW *cmd_win);
void rbmCommand(WINDOW *cmd_win);
void beginCommand(WINDOW *cmd_win);
void endCommand(WINDOW *cmd_win);
void testBotCommand(WINDOW *cmd_win, char *interval, char *duration);
void initMaze(char *filename);
void printMaze(WINDOW *win, char maze[ROWS][COLS]);
void printPlayerList(WINDOW *win);
void terminateUsers(WINDOW *cmd_win);
void setupMotor(pthread_t thread_registo);
void *lerRegistosThread();
void playerQueque();
void generateStartPos(Game *gameInfo, Player *user);
void serializeGameInfo(const Game *gameInfo, char *buffer, size_t bufferSize);
void sendGameStateToAllPlayers(const Game *gameInfo);
void *updateMaze(void *arg);
void *updateInfoWin(void *arg);
void *lerMovimentosJogadores(void *arg);
int validarMovimento(Position *pos, char direction, char playerChar);
void iniciarBots(int nivel);
void *lerOutputBot(void *arg);
#endif