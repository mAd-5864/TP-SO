# Multiplayer Labyrinth Game
**Operating Systems ISEC 2023/24**

## Authors
- Pedro Amorim (2022157609)

## Description

A client-server multiplayer labyrinth game system, implemented in C. The system uses a "motor" process (server) to manage the game state, multiple "jogoUI" clients (user interfaces), and automated "bots". Communication is managed via FIFOs (named pipes), and the terminal-based graphical interface is built using `ncurses`.

## Components
- **motor**: The central game server. It manages the maze state, the logic for players, the registration of new clients, and the spawning of bots.
- **jogoUI**: The game client (user interface). It allows a player to connect to the motor, handles movement within the maze, and allows sending messages.
- **bot**: An autonomous process, launched by the motor, which simulates a player by dropping 'stones' (temporary obstacles) in the maze.

## Technologies

- **C**
- **ncurses** (for the terminal-based graphical interface)
- **pthreads** (for concurrency in the motor and client)
- **FIFOs (Named Pipes)** (for inter-process communication between motor and clients)
- **fork/exec** (for the motor to launch bot processes)
- **Makefile** (for build automation)

## Directory Structure

- `src/`: C source code (`motor.c`, `jogoUI.c`, `bot.c`, header files, etc.).
- `levels/`: Text files containing the maze maps (`maze1.txt`, `maze2.txt`, etc.).
- `out/`: Directory for the compiled binaries (created by the `Makefile`).
- `Makefile`: The build script.

## How to Compile and Run

### Prerequisites

1.  `gcc` compiler.
2.  `make` utility.
3.  `ncurses` library (e.g., `libncurses-dev`).

### 1. Compile the Project

To compile all source code into the `out/` folder, run the following command from the project root:

```bash
make
```

### 2. Run the Game (Multiple Terminals)

You will need to open multiple terminals in the project's root directory.

**Terminal 1: Motor (Server)**
*The motor must be started first. It will wait for players to register.*

```bash
./out/motor
```

**Terminal 2 (and others): Game Client(s)**
*While the motor is waiting, launch one or more clients, each with a unique username:*

```bash
./out/jogoUI <player_name_1>

./out/jogoUI <player_name_2>
```

After the registration period, the game will start, and the motor will automatically launch the `bot` processes defined for the level.

## Commands

### Server (motor)
Commands are entered into the motor's command window.

* `users`: Lists all currently connected players.
* `kick <username>`: Kicks the specified player from the game.
* `test_bot <interval> <duration>`: Spawns a new bot with a specific interval and stone duration.
* `end`: Shuts down the server and kicks all players.
* `bots`, `bmov`, `rbm`, `begin`: Commands reserved but not implemented.

### Client (jogoUI)
The client has two input modes.

**Game Mode (Default)**
* `Arrow Keys`: Move the player character (Up, Down, Left, Right).
* `Spacebar`: Switch to Command Mode.

**Command Mode**
* `players`: Lists all players currently in the game.
* `msg <username> <message>`: Sends a private message to the specified user.
* `exit`: Quits the game client.
* `Escape Key (Esc)`: Switch back to Game Mode.
