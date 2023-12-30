#include "motor.h"

Game gameInfo;

int main(int argc, char *argv[], char *envp[])
{
  // Inicializar estado de jogo
  gameInfo.nPlayers = 0;
  // gameInfo.nBots = 0;
  char filename[] = "../levels/maze1.txt";
  initMaze(filename);

  // Initialize ncurses
  initscr();
  erase();
  cbreak();
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);

  // Iniciar registo de jogadores
  memset(gameInfo.players, 0, sizeof(gameInfo.players)); // Clear the player list
  setupMotor();
  pthread_t thread_registo;
  pthread_create(&thread_registo, NULL, lerRegistosThread, NULL);

  // Esperar por novos jogadores
  playerQueque();

  // Create windows
  WINDOW *maze_win = newwin(18, 80, 2, 5);
  WINDOW *cmd_win = newwin(10, 80, 19, 5);
  WINDOW *info_win = newwin(27, 28, 2, 87);
  box(info_win, 0, 0);
  mvwprintw(info_win, 1, 1, "Users:");
  for (size_t i = 0; i < gameInfo.nPlayers; i++)
  {
    mvwprintw(info_win, 2 + i, 1, "%s", gameInfo.players[i].username);
  }
  wrefresh(info_win);

  if (argc != 1)
  {
    wprintw(cmd_win, "[ERRO]: Argumento invalido.\n");
    wrefresh(cmd_win);
    endwin();
    return 1;
  }

  // Preparar labirinto inicial
  generateStartPos(&gameInfo);
  sendGameStateToAllPlayers(&gameInfo);
  printMaze(maze_win, gameInfo.maze);

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

  free(command);
  pthread_cancel(thread_registo);
  unlink("registo.fifo");
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
      printf("Utilizador %s foi expulso\n", arg);
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
    endCommand(cmd_win);
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

void setupMotor()
{
  if (mkfifo("registo.fifo", 0666) == -1) // Criar FIFO registo
  {
    perror("mkfifo");
    exit(1);
  }
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
      gameInfo.nPlayers++;
    }
    else
    {
      sleep(3);
    }

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

void generateStartPos(Game *gameInfo)
{
  srand(time(NULL)); // Seed the random number generator

  for (int i = 0; i < gameInfo->nPlayers; i++)
  {
    int randomX, randomY;
    do
    {
      randomX = rand() % COLS;
      randomY = rand() % ROWS;
    } while (gameInfo->maze[randomY][randomX] != ' ');
    gameInfo->players[i].Position.x = randomX;
    gameInfo->players[i].Position.y = randomY;
    gameInfo->maze[randomY][randomX] = gameInfo->players[i].username[0];
  }
}

void usersCommand(WINDOW *cmd_win)
{
  for (size_t i = 0; i < gameInfo.nPlayers; i++)
  {
    wprintw(cmd_win, "\t%s", gameInfo.players[i].username);
  }
}

void kickCommand(WINDOW *cmd_win, char *username)
{
  wprintw(cmd_win, " Comando [kick] nao implementado.\n");
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

void endCommand(WINDOW *cmd_win)
{
  wprintw(cmd_win, " Comando [end] nao implementado.\n");
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
      break; // Exit the loop or handle error appropriately
    }

    PID_bot = fork();
    if (PID_bot == -1)
    {
      wprintw(cmd_win, "[ERRO]: O fork falhou.\n");
      wrefresh(cmd_win);
      break; // Exit the loop or handle error appropriately
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
        break; // Exit the loop or handle error appropriately
      }
      if (nBytes == 0)
      {
        break; // No more data
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
      if (maze[i][j] != ' ' && maze[i][j] != 'X')
      {
        wattron(win, COLOR_PAIR(1)); // Turn on color for player
        mvwaddch(win, i, j + k, maze[i][j]);
        wattroff(win, COLOR_PAIR(1)); // Turn off color
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
