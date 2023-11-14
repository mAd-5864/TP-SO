#include "motor.h"

int main(int argc, char* argv[], char* envp[])
{
  if(argc != 1)
  {
    printf("[ERRO]: Argumento invalido.\n");
    exit(1);
  }

  char* command = (char *) malloc(sizeof(char) * MAX_COMMAND_SIZE);
  do
  {
    printf("Introduza um comando: ");
    fgets(command, MAX_COMMAND_SIZE, stdin);
    validateAdminCommands(command);
  } while(strcmp(command, "end"));
}

void validateAdminCommands(char* command)
{
  char* commandAux = strtok(command, " \n");
  char* arg;

  if (commandAux == NULL) {
    printf("[ERRO] Comando vazio.\n");
    return;
  }

  printf("\nComando: [%s]\n", commandAux);
  if(!strcmp(commandAux, "users"))
  {
    usersCommand();
  }
  else if(!strcmp(commandAux, "kick"))
  {
    arg = strtok(NULL, " ");
    if(arg == NULL)
      printf("[ERRO] Syntax: kick <username>\n");
    else
    {
      printf("username: %s\n", arg);
      kickCommand(arg);
    }
  }
  else if(!strcmp(commandAux, "bots"))
  {
    botsCommand();
  }
  else if(!strcmp(commandAux, "bmov"))
  {
    bmovCommand();
  }
  else if(!strcmp(commandAux, "rbm"))
  {
    rbmCommand();
  }
  else if(!strcmp(commandAux, "begin"))
  {
    beginCommand();
  }
  else if(!strcmp(commandAux, "end"))
  {
    endCommand();
    strcpy(command, "end");
  }
  else if(!strcmp(commandAux, "test_bot"))
  {
    char* interval  = strtok(NULL, " ");
    char* duration = strtok(NULL, " ");
    if(interval == NULL || duration == NULL)
      printf("[ERRO] Syntax: test_bot <interval> <duration>\n");
    else
    {
      printf("interval: %s\n", interval);
      printf("duration: %s\n", duration);
      testBotCommand(interval, duration);
    }
  }
  else printf(("[ERRO]: Comando invalido.\n"));
}

void usersCommand()
{
  printf("\nComando [users] nao implementado.\n");
}

void kickCommand(char *username)
{
  printf("\nComando [kick] nao implementado.\n");
}

void botsCommand()
{
  printf("\nComando [bots] nao implementado.\n");
}

void bmovCommand()
{
  printf("\nComando [bmov] nao implementado.\n");
}

void rbmCommand()
{
  printf("\nComando [rbm] nao implementado.\n");
}

void beginCommand()
{
  printf("\nComando [begin] nao implementado.\n");
}

void endCommand()
{
  printf("\nComando [end] nao implementado.\n");
}

void testBotCommand(char* interval, char* duration) {
  int PID_bot, nBytes, state;
  int pipeBotMotor[2];
  char* botInfo = (char *) malloc(sizeof(char)*MAX_COMMAND_SIZE);

  // Verificação da correta criação do pipeBotMotor.
  if (pipe(pipeBotMotor) == -1)
  {
    printf("[ERRO]: Nao foi possível criar o pipe para comunicacao com o Bot.\n");
    exit(1);
  }

  // Verificação da correta execução do fork.
  PID_bot = fork();
  if (PID_bot == -1)
  {
    printf("[ERRO]: O fork falhou.\n");
    exit(2);
  }

    // Bot (Child Process)
  if (PID_bot == 0)
  {
    close(pipeBotMotor[0]);


    close(STDOUT_FILENO);
    dup(pipeBotMotor[1]);

    execl("bot", "bot", interval, duration, NULL);

    printf("[ERRO]: bot nao encontrado.\n");
    exit(3);
  }
  else
  {
    // Motor (Parent process)
    printf("Bot PID: %d\n\n", PID_bot);
    close(pipeBotMotor[1]);
    while(1) {
      nBytes = read(pipeBotMotor[0], botInfo, MAX_COMMAND_SIZE);
      if (nBytes == -1) {
        perror("[ERRO]: Erro na leitura do pipe");
        exit(4);
      }
      if(nBytes == 0)  break;

      printf("Recebi %d bytes do bot.\n", nBytes);
      botInfo[nBytes] = '\0';

      char* xChar = strtok(botInfo, " ");
      char* yChar  = strtok(NULL, " ");
      char* dChar = strtok(NULL, "\n");

      int x = atoi(xChar), y = atoi(yChar), d = atoi(dChar);
      printf("Recebi: %d %d %d\n\n", x, y, d);
    }
    // printf("\nO bot terminou com %d.\n", WEXITSTATUS(state));
  }
}


