#include "jogoUI.h"

int main(int argc, char* argv[], char* envp[])
{
  if(argc != 2)
  {
    printf("[ERRO]: Introduza o nome do jogador como argumento.\n");
    exit(1);
  }

  char* playerName = argv[1];
  printf("Nome do jogador: %s\n\n", playerName);

  char* command = (char *) malloc(sizeof(char) * MAXLEN);
  do
  {
    printf("Introduza um comando: ");
    fgets(command, MAXLEN, stdin);
    commands(command);
  } while(strcmp(command, "exit"));
}

void commands(char* command)
{
  char* commandAux = strtok(command, " \n");
  char *username, *msg;

  printf("\nComando: [%s]\n", commandAux);
  if(!strcmp(commandAux, "players"))
  {
    playersCommand();
  }
  else if(!strcmp(commandAux, "msg"))
  {
    username = strtok(NULL, " ");
    msg = strtok(NULL, " ");
    if(username == NULL || msg == NULL)
      printf("[ERRO] Syntax: msg <username> <message>\n");
    else
    {
      printf("username: %s\n", username);
      printf("msg: %s\n", msg);
      msgCommand(username, msg);
    }
  }
  else if(!strcmp(commandAux, "exit"))
  {
    exitCommand();
    strcpy(command, "exit");
  }
  else printf(("[ERRO]: Comando invalido.\n"));
}

void playersCommand()
{
  printf("\nComando [players] nao implementado.\n");
}

void msgCommand(char *username, char* msg)
{
  printf("\nComando [msg] nao implementado.\n");
}

void exitCommand()
{
  printf("\nComando [exit] nao implementado.\n");
}