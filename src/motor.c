#include "motor.h"

pthread_mutex_t gameStateMutex;
Game gameInfo;
int gameChangeFlag = 0, infoWinFlag = -1;

int main(int argc, char *argv[], char *envp[])
{
  // Inicializar estado de jogo
  gameInfo.nPlayers = 0;
  gameInfo.nivel = 1;
  gameInfo.podio = 1;
  char filename[] = "../levels/maze1.txt";
  initMaze(filename);

  // Initialize ncurses
  initscr();
  erase();
  cbreak();
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);

  // Iniciar registo de jogadores
  memset(gameInfo.players, 0, sizeof(gameInfo.players)); // Clear the player list
  pthread_t thread_registo;
  setupMotor(thread_registo);

  // Create windows
  WINDOW *maze_win = newwin(18, 80, 2, 5);
  WINDOW *cmd_win = newwin(10, 80, 19, 5);
  WINDOW *info_win = newwin(27, 28, 2, 87);
  pthread_t updateInfoWinThread;
  pthread_create(&updateInfoWinThread, NULL, updateInfoWin, (void *)info_win);

  // Preparar labirinto inicial
  pthread_mutex_init(&gameStateMutex, NULL);
  pthread_t updateMazeThread;
  pthread_create(&updateMazeThread, NULL, updateMaze, (void *)maze_win);
  iniciarBots(gameInfo.nivel);
  sendGameStateToAllPlayers(&gameInfo);

  usleep(100000); // 100 ms
  printPlayerList(info_win);
  char *command = (char *)malloc(sizeof(char) * MAX_COMMAND_SIZE);
  do
  {
    box(cmd_win, 0, 0);
    mvwprintw(cmd_win, 1, 1, "Introduza um comando: ");
    wgetnstr(cmd_win, command, MAX_COMMAND_SIZE);
    wclear(cmd_win);
    wrefresh(cmd_win);

    validateAdminCommands(cmd_win, command);
  } while (strcmp(command, "end"));

  terminateUsers(cmd_win);
  pthread_cancel(updateMazeThread);
  pthread_cancel(updateInfoWinThread);
  unlink("registo.fifo");
  free(command);
  endwin();
  return 0;
}

void validateAdminCommands(WINDOW *cmd_win, char *command)
{
  char *commandAux = strtok(command, " \n");
  char *arg;

  if (commandAux == NULL)
  {
    mvwprintw(cmd_win, 2, 1, "[ERRO] Comando vazio.\n");
    wrefresh(cmd_win);
    return;
  }

  mvwprintw(cmd_win, 2, 1, "Comando: [%s]\n", commandAux);
  wrefresh(cmd_win);
  if (!strcmp(commandAux, "users"))
  {
    usersCommand(cmd_win);
  }
  else if (!strcmp(commandAux, "kick"))
  {
    arg = strtok(NULL, " ");
    if (arg == NULL)
      wprintw(cmd_win, " [ERRO] Syntax: kick <username>\n");
    else
    {
      kickCommand(cmd_win, arg);
    }
  }
  else if (!strcmp(commandAux, "bots"))
  {
    botsCommand(cmd_win);
  }
  else if (!strcmp(commandAux, "bmov"))
  {
    bmovCommand(cmd_win);
  }
  else if (!strcmp(commandAux, "rbm"))
  {
    rbmCommand(cmd_win);
  }
  else if (!strcmp(commandAux, "begin"))
  {
    beginCommand(cmd_win);
  }
  else if (!strcmp(commandAux, "end"))
  {
    strcpy(command, "end");
  }
  else if (!strcmp(commandAux, "test_bot"))
  {
    char *interval = strtok(NULL, " ");
    char *duration = strtok(NULL, " ");
    if (interval == NULL || duration == NULL)
      wprintw(cmd_win, " [ERRO] Syntax: test_bot <interval> <duration>\n");
    else
    {
      wprintw(cmd_win, " interval: %s\tduration: %s\n", interval, duration);
      testBotCommand(cmd_win, interval, duration);
    }
  }
  else
    wprintw(cmd_win, " [ERRO]: Comando invalido.");
}

void setupMotor(pthread_t thread_registo)
{
  if (mkfifo("registo.fifo", 0666) == -1) // Criar FIFO registo
  {
    perror("mkfifo");
    exit(1);
  }

  pthread_create(&thread_registo, NULL, lerRegistosThread, NULL);

  // Esperar por novos jogadores
  playerQueque();
}

void *lerRegistosThread()
{
  char username[MAX_USERNAME_SIZE];

  // Abrir FIFO para leitura
  int fd = open("registo.fifo", O_RDONLY | O_NONBLOCK);
  if (fd == -1)
  {
    perror("open");
    exit(1);
  }

  while (1)
  {
    if (read(fd, username, sizeof(username)) > 0)
    {
      // Successfully read data
      strncpy(gameInfo.players[gameInfo.nPlayers].username, username, MAX_USERNAME_SIZE);
      gameInfo.players[gameInfo.nPlayers].username[MAX_USERNAME_SIZE - 1] = '\0';
      sprintf(gameInfo.players[gameInfo.nPlayers].fifo_to_motor, "%s_to_motor.fifo", username);
      sprintf(gameInfo.players[gameInfo.nPlayers].fifo_from_motor, "motor_to_%s.fifo", username);
      sprintf(gameInfo.players[gameInfo.nPlayers].message_fifo, "mensagens_%s.fifo", username);
      generateStartPos(&gameInfo, &gameInfo.players[gameInfo.nPlayers]);
      pthread_create(&gameInfo.players[gameInfo.nPlayers].movementThread, NULL, lerMovimentosJogadores, (void *)&gameInfo.players[gameInfo.nPlayers]);
      gameInfo.players[gameInfo.nPlayers].terminou = 0;
      gameInfo.nPlayers++;
    }
    usleep(100000); //100ms

    if (gameInfo.nPlayers == MAX_PLAYERS)
      break;
  }

  close(fd);
  return NULL;
}

void playerQueque()
{
  WINDOW *queque_win = newwin(20, 50, 5, 35);
  box(queque_win, 0, 0);
  curs_set(0);
  time_t startTime = time(NULL);
  time_t currentTime;
  do
  {
    currentTime = time(NULL);
    if (INSCRICAO - (currentTime - startTime) >= 0)
    {
      mvwprintw(queque_win, 1, 1, "A espera de jogadores: %ds ", INSCRICAO - (currentTime - startTime));
    }
    for (size_t i = 0; i < gameInfo.nPlayers; i++)
    {
      mvwprintw(queque_win, 2 + i, 1, " %s", gameInfo.players[i].username);
    }
    wrefresh(queque_win);
    sleep(1);
  } while (gameInfo.nPlayers < MIN_PLAYERS || currentTime - startTime < INSCRICAO);
  curs_set(1);
}

void generateStartPos(Game *gameInfo, Player *user)
{
  srand(time(NULL)); // Seed the random number generator

  int randomX, randomY;
  do
  {
    randomX = rand() % COLS;
    randomY = rand() % ROWS;
  } while (gameInfo->maze[randomY][randomX] != ' ');
  user->Position.x = randomX;
  user->Position.y = randomY;
  gameInfo->maze[randomY][randomX] = user->username[0];
}

void usersCommand(WINDOW *cmd_win)
{
  wprintw(cmd_win, " [%s, ", gameInfo.players[0].username);
  for (size_t i = 1; i < gameInfo.nPlayers; i++)
  {
    if (i == gameInfo.nPlayers - 1)
    {
      wprintw(cmd_win, "%s]", gameInfo.players[i].username);
    }
    else
    {
      wprintw(cmd_win, "%s, ", gameInfo.players[i].username);
    }
  }
}

void kickCommand(WINDOW *cmd_win, char *username)
{
  // Encontrar jogador na estrutura
  for (int i = 0; i < gameInfo.nPlayers; i++)
  {
    if (!strcmp(gameInfo.players[i].username, username))
    {
      // Enviar mensagem para kickar jogador
      int fd = open(gameInfo.players[i].message_fifo, O_WRONLY | O_NONBLOCK);
      if (fd != -1)
      {
        char kickMsg = 'K';
        write(fd, &kickMsg, sizeof(kickMsg));
        close(fd);

        // Terminar thread do jogador
        pthread_cancel(gameInfo.players[i].movementThread);
        // Remover char do jogador do jogo
        gameInfo.maze[gameInfo.players[i].Position.y][gameInfo.players[i].Position.x] = ' ';
        // Remover jogador da gameInfo
        for (int j = i; j < gameInfo.nPlayers - 1; j++)
        {
          pthread_cancel(gameInfo.players[j].movementThread);
          gameInfo.players[j] = gameInfo.players[j + 1];
          pthread_create(&gameInfo.players[j].movementThread, NULL, lerMovimentosJogadores, (void *)&gameInfo.players[j]);
        }
        gameInfo.nPlayers--;
        pthread_cancel(gameInfo.players[gameInfo.nPlayers].movementThread);

        // Apagar duplicado de ultimo jogador
        memset(&gameInfo.players[gameInfo.nPlayers], 0, sizeof(Player));

        wprintw(cmd_win, " Jogador %s foi expulso.\n", username);
        wrefresh(cmd_win);

        sendGameStateToAllPlayers(&gameInfo);
        usleep(100000); // 100 ms
      }
      else
      {
        wprintw(cmd_win, " [Erro]: Impossivel abrir FIFO para %s\n", username);
        wrefresh(cmd_win);
      }
      return;
    }
  }

  // Jogador nao foi encontrado
  wprintw(cmd_win, " Utilizador desconhecido: '%s'\n", username);
  wrefresh(cmd_win);
}

void botsCommand(WINDOW *cmd_win)
{
  wprintw(cmd_win, " Comando [bots] nao implementado.\n");
}

void bmovCommand(WINDOW *cmd_win)
{
  wprintw(cmd_win, " Comando [bmov] nao implementado.\n");
}

void rbmCommand(WINDOW *cmd_win)
{
  wprintw(cmd_win, " Comando [rbm] nao implementado.\n");
}

void beginCommand(WINDOW *cmd_win)
{
  wprintw(cmd_win, " Comando [begin] nao implementado.\n");
}

void terminateUsers(WINDOW *cmd_win)
{
  do
  {
    /* code */
    kickCommand(cmd_win, gameInfo.players[gameInfo.nPlayers - 1].username);
  } while (gameInfo.nPlayers);
}

void testBotCommand(WINDOW *cmd_win, char *interval, char *duration)
{
  int PID_bot, nBytes, numBots = 0;
  int pipeBotMotor[2];
  char *botInfo = (char *)malloc(sizeof(char) * MAX_COMMAND_SIZE);

  while (numBots < MAX_BOTS)
  {
    if (pipe(pipeBotMotor) == -1)
    {
      wprintw(cmd_win, "[ERRO]: Nao foi possível criar o pipe para comunicacao com o Bot.\n");
      wrefresh(cmd_win);
      break;
    }

    PID_bot = fork();
    if (PID_bot == -1)
    {
      wprintw(cmd_win, "[ERRO]: O fork falhou.\n");
      wrefresh(cmd_win);
      break;
    }

    if (PID_bot == 0)
    {                         // Child process
      close(pipeBotMotor[0]); // Fechar leitura do pipe

      // Redirecionar o output do bot para escrita no pipe
      close(STDOUT_FILENO);
      dup(pipeBotMotor[1]);
      close(pipeBotMotor[1]); // Fechar a escrita no pipe original
      execl("bot", "bot", interval, duration, NULL);

      exit(3);
    }
    else
    { // Parent process
      close(pipeBotMotor[1]);

      nBytes = read(pipeBotMotor[0], botInfo, MAX_COMMAND_SIZE);
      if (nBytes == -1)
      {
        wprintw(cmd_win, "[ERRO]: Erro na leitura do pipe\n");
        wrefresh(cmd_win);
        break;
      }
      if (nBytes == 0)
      {
        break;
      }

      botInfo[nBytes] = '\0';
      char *xChar = strtok(botInfo, " ");
      char *yChar = strtok(NULL, " ");
      char *dChar = strtok(NULL, "\n");

      int x = atoi(xChar), y = atoi(yChar), d = atoi(dChar);
      wmove(cmd_win, 4 + numBots, 1);
      wprintw(cmd_win, "Recebi: %d %d %d", x, y, d);
      wrefresh(cmd_win);
    }
    numBots++;
  }

  free(botInfo);
}

void initMaze(char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    perror("\n[ERRO]: Erro ao abrir o ficheiro");
    exit(1);
  }

  for (int i = 0; i < ROWS; i++)
  {
    int j = 0;
    while (j < COLS)
    {
      int ch = fgetc(file);
      if (ch == EOF)
      { // End of file
        break;
      }
      if (ch == '\n' || ch == '\r')
      { // Skip \n e \r
        continue;
      }
      gameInfo.maze[i][j] = ch;
      j++;
    }
  }

  fclose(file);
}

void printMaze(WINDOW *win, char maze[ROWS][COLS])
{
  for (int i = 0; i < ROWS; i++)
  {
    int k = 0;
    for (int j = 0; j < COLS; j++)
    {
      if (maze[i][j] != ' ' && maze[i][j] != 'X' && maze[i][j] != 'p')
      {
        wattron(win, COLOR_PAIR(1)); // Turn on color for player
        mvwaddch(win, i, j + k, maze[i][j]);
        wattroff(win, COLOR_PAIR(1)); // Turn off color
      }
      else if (maze[i][j] == 'p')
      {
        wattron(win, COLOR_PAIR(2)); // Turn on color for pedras
        mvwaddch(win, i, j + k, maze[i][j]);
        wattroff(win, COLOR_PAIR(2)); // Turn off color
      }
      else
      {
        mvwaddch(win, i, j + k, maze[i][j]); // Print a cada caracter do labirinto
      }
      mvwaddch(win, i, j + ++k, ' ');
    }
  }
  wrefresh(win);
}
void *updateMaze(void *arg)
{
  WINDOW *win = (WINDOW *)arg;
  while (TRUE)
  {
    if (gameChangeFlag)
    {
      printMaze(win, gameInfo.maze);
      gameChangeFlag = 0;
      wrefresh(win);
    }

    usleep(10000); // 10 ms
  }
  return NULL;
}
void *updateInfoWin(void *arg)
{
  WINDOW *win = (WINDOW *)arg;
  while (TRUE)
  {
    if (gameInfo.nPlayers != infoWinFlag)
    {
      printPlayerList(win);
      infoWinFlag = gameInfo.nPlayers;
      wrefresh(win);
    }
    usleep(10000); // 10 ms
  }
  return NULL;
}
void printPlayerList(WINDOW *win)
{
  wclear(win);
  box(win, 0, 0);
  mvwprintw(win, 1, 1, "Users:");
  for (size_t i = 0; i < gameInfo.nPlayers; i++)
  {
    if (gameInfo.players[i].terminou)
    {
      mvwprintw(win, 2 + i, 1, "%s - %dº Lugar", gameInfo.players[i].username, gameInfo.players[i].terminou);
    }
    else
    {
      mvwprintw(win, 2 + i, 1, "%s", gameInfo.players[i].username);
    }
  }
  wrefresh(win);
}

void serializeGameInfo(const Game *gameInfo, char *buffer, size_t bufferSize)
{
  size_t offset = 0;

  // Serializar dados
  if (offset + sizeof(gameInfo->nPlayers) <= bufferSize)
  {
    memcpy(buffer + offset, &gameInfo->nPlayers, sizeof(gameInfo->nPlayers));
    offset += sizeof(gameInfo->nPlayers);
  }

  // Serializar arrays
  for (int i = 0; i < gameInfo->nPlayers; i++)
  {
    if (offset + MAX_USERNAME_SIZE <= bufferSize)
    {
      memcpy(buffer + offset, gameInfo->players[i].username, MAX_USERNAME_SIZE);
      offset += MAX_USERNAME_SIZE;
    }
  }

  // Serializar labirinto
  if (offset + (ROWS * COLS) <= bufferSize)
  {
    memcpy(buffer + offset, gameInfo->maze, ROWS * COLS);
    offset += ROWS * COLS;
  }
}

void sendGameStateToAllPlayers(const Game *gameInfo)
{
  int bufferSize = BUFFER_SIZE;
  char buffer[bufferSize];
  gameChangeFlag = 1;
  serializeGameInfo(gameInfo, buffer, sizeof(buffer));

  for (int i = 0; i < gameInfo->nPlayers; i++)
  {
    int fd = open(gameInfo->players[i].fifo_from_motor, O_WRONLY);
    if (fd == -1)
    {
      perror("open");
    }
    else
    {
      write(fd, buffer, sizeof(buffer)); // Enviar estado do jogo serializado
      close(fd);
    }
  }
}

void *lerMovimentosJogadores(void *arg)
{
  Player *player = (Player *)arg;

  // Abrir o FIFO do jogador para ler
  int fd = open(player->fifo_to_motor, O_RDONLY | O_NONBLOCK);
  if (fd == -1)
  {
    perror("Não foi possível abrir o FIFO para jogadas");
    return NULL;
  }

  char move;
  while (true)
  {
    // Ler o comando de movimento do FIFO
    ssize_t nBytes = read(fd, &move, sizeof(move));
    if (nBytes > 0)
    {
      if (validarMovimento(&(player->Position), move, player->username[0]))
      {
        sendGameStateToAllPlayers(&gameInfo);
        if ((player->Position.x == 0 || player->Position.x == COLS - 1) && !player->terminou)
        {
          player->terminou = gameInfo.podio++;
          infoWinFlag = -1;
        }
      }
    }
    usleep(100000); // 100 ms
  }

  close(fd);
  return NULL;
}
int validarMovimento(Position *pos, char direction, char playerChar) // Tenho que enviar Player
{
  switch (direction)
  {
  case 'C': // Cima
    if (pos->y > 0)
    {
      if (gameInfo.maze[pos->y - 1][pos->x] != 'X' && gameInfo.maze[pos->y - 1][pos->x] != 'p')
      {
        gameInfo.maze[pos->y - 1][pos->x] = playerChar; // Atualizar pos na matriz
        gameInfo.maze[pos->y][pos->x] = ' ';            // Apagar posição anterior
        pos->y--;                                       // Atualizar pos do Player
        return 1;
      }
    }
    break;
  case 'B': // Baixo
    if (pos->y < ROWS)
    {
      if (gameInfo.maze[pos->y + 1][pos->x] != 'X' && gameInfo.maze[pos->y - +1][pos->x] != 'p')
      {
        gameInfo.maze[pos->y + 1][pos->x] = playerChar; // Atualizar pos na matriz
        gameInfo.maze[pos->y][pos->x] = ' ';            // Apagar posição anterior
        pos->y++;                                       // Atualizar pos do Player
        return 1;
      }
    }
    break;
  case 'E': // Esquerda
    if (pos->x > 0)
    {
      if (gameInfo.maze[pos->y][pos->x - 1] != 'X' && gameInfo.maze[pos->y][pos->x - 1] != 'p')
      {
        gameInfo.maze[pos->y][pos->x - 1] = playerChar; // Atualizar pos na matriz
        gameInfo.maze[pos->y][pos->x] = ' ';            // Apagar posição anterior
        pos->x--;                                       // Atualizar pos do Player
        return 1;
      }
    }
    break;
  case 'D': // Direita
    if (pos->x < COLS)
    {
      if (gameInfo.maze[pos->y][pos->x + 1] != 'X' && gameInfo.maze[pos->y][pos->x + 1] != 'p')
      {
        gameInfo.maze[pos->y][pos->x + 1] = playerChar; // Atualizar pos na matriz
        gameInfo.maze[pos->y][pos->x] = ' ';            // Apagar posição anterior
        pos->x++;                                       // Atualizar pos do Player
        return 1;
      }
    }
    break;
  }
}

void iniciarBots(int nivel)
{
  Bot *bots;
  int numBots;

  switch (nivel)
  {
  case 1:
    bots = nivel1_bots;
    numBots = BOTS_NVL1;
    break;
  case 2:
    bots = nivel2_bots;
    numBots = BOTS_NVL2;
    break;
  case 3:
    bots = nivel3_bots;
    numBots = BOTS_NVL3;
    break;
  }

  for (int i = 0; i < numBots; i++)
  {
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
      perror("pipe");
      continue;
    }

    bots[i].pid = fork();
    if (bots[i].pid == 0) // Processo Bot
    {
      close(pipefd[0]); // fechar leitura

      // Redirect stdout to the write end of the pipe
      dup2(pipefd[1], STDOUT_FILENO); // output bot para escrever no pipe
      close(pipefd[1]);               // Fechar escrita original

      char intervaloStr[10], duracaoStr[10];
      sprintf(intervaloStr, "%d", bots[i].intervalo);
      sprintf(duracaoStr, "%d", bots[i].duracao);
      execl("bot", "bot", intervaloStr, duracaoStr, NULL);
      perror("execlp");
      exit(EXIT_FAILURE);
    }
    else if (bots[i].pid > 0) // Processo Pai
    {
      close(pipefd[1]); // fechar escrita
      bots[i].pipe = pipefd[0];

      pthread_t botThread;
      pthread_create(&botThread, NULL, lerOutputBot, (void *)&bots[i]);
    }
    else
    {
      perror("fork");
    }
  }
}

void *lerOutputBot(void *arg)
{
  Bot *bot = (Bot *)arg;

  char buffer[MAX_COMMAND_SIZE];
  int nBytes;
  Position pedra;

  // Read from the bot's pipe
  while (TRUE)
  {
    nBytes = read(bot->pipe, buffer, sizeof(buffer) - 1);
    if (nBytes > 0)
    {
      buffer[nBytes] = '\0'; // Null-terminate the string
      sscanf(buffer, "%d %d %d", &pedra.x, &pedra.y, &bot->duracao);

      pthread_mutex_lock(&gameStateMutex);
      char aux = gameInfo.maze[pedra.y][pedra.x];
      gameInfo.maze[pedra.y][pedra.x] = 'p';
      gameChangeFlag = 1;
      pthread_mutex_unlock(&gameStateMutex);

      sleep(bot->duracao);
      pthread_mutex_lock(&gameStateMutex);
      gameInfo.maze[pedra.y][pedra.x] = aux;
      gameChangeFlag = 1;
      pthread_mutex_unlock(&gameStateMutex);
    }

    if (nBytes == -1)
    {
      perror("read");
    }
  }
  close(bot->pipe);
  return NULL;
}