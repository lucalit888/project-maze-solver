# THE AMAZING CHALLENGE
# CS50 Winter 2020, Final Project

## Team name: Bing
## Team Members: Mack Reiferson, Sean Simons, Luca Lit and Connor Davis

GitHub usernames: mackerson123, srsimons98, lucalit888, connormdavis

This README.md contains a high level description of the implementation specifics of our amazing project. There are Four Main Parts which will be explored in depth: 

	(1) AMStartup.c 
	(2) avatar.c 
	(3) graphics.c 
	(4) mazeSolver.c

Each of the above will be explained through detailed pseudocode, descriptions of abstract data structures, definitions of APIs / interfaces / function / prototypes and parameters, and error handling + recovery. 


### Overview of Application

Our maze-solving application simulates players (i.e. avatars) searching for each other in a maze generated and managed by a server application. A random maze of certain dimensions is generated by the server, and the caller determines the number of avatars and the difficulty of the game. The avatars will move around and discover the maze. The game ends when all players converge at one common point. 


### Project Tree:
```
├── .gitignore
├── amazing.h
├── AMStartup.c 
├── avatar.c 
├── avatar.h
├── designTest.c
├── graphics.c 
├── graphics.h
├── graphicstest.c
├── graphics.c 
├── graphics.h
├── log.out/    		# containing logs for test runs
├── Makefile
├── mazeSolver.c
├── mazeSolver.h 
├── testing.sh
├── README.md
├── DESIGN.md
├── TESTING.md
```

### User interface

The maze solver’s only interface with the user is on the command-line; it must always be provided 3 arguments, like in the following example: 

```
./AMStartup -n <NUM_OF_AVATARS> -d <DIFFICULTY_LEVEL> -h <HOST_NAME>
```
e.g. ./AMStartup -n 3 -d 3 -h flume.cs.dartmouth.edu


## Detailed parameter description + pseudocode for objects/components/functions:

### AMStartup.c:

```c
int main(const int argc, char *argv[]);
```

**Parameters:**

* argc = number of arguments passed
* argv = array of argument strings

**Pseudocode**

	1. Validate command line arguments 
	2. Create/connect socket to server 
	3. Send init message w/ diff. & numAvatars from passed args
	4. Receive response from server,
	5. If we've received an error, exit.
	6. If we receive INIT_OK message, 
	7. Create maze, log file, mutex_lock, avatars, and all shared values & create 'numAvatars' threads, passing in above values via a startupStruct
	8. Join threads to main thread and wait until they're done
	9. When the threads return, delete avatars, logfile string, maze, socket, & exit main process with code 0.

### avatar.c:

```c
int getID(startupInfo_t* s);
```

**Parameters:**

* s = startupInfo_t struct

**Pseudocode**

	1. return ID from startup struct 's' 
	2. (*All other "getters" follow this structure. Refer to avatar.h for more information)

```c
startupInfo_t* loadStartupStruct(pthread_mutex_t *lock, int avatarID, int nAvatars, int difficulty, char *hostname, int mazePort, char *logFile, avatar_t **avatars, int *lastTurnID, mazeTile_t ***maze, int *solved, int height, int width, WINDOW *window, FILE *log, int *moveCount, pthread_mutex_t *lock2);
```

**Parameters:**

* s = startupInfo_t struct
* avatarID = passes the avatar its assigned ID
* nAvatars = allows the avatar to know how many avatars there are
* difficulty = the difficulty of the current maze
* hostname = hostname of maze's host for socket creation
* mazePort =  port number for the maze
* logFile = name of file to write progress
* lock = used for mutex locking for threading
* avatars = array of pointers to avatars for avatar "communication"
* lastTurnID = allows all avatars to know if they made the last turn on next loop iteration
* maze = 2D array of pointers to mazeTile_t structs, for shared knowledge of the maze
* solved = main loop condition for ending loop on failure/solution
* height = height of maze
* width = width of maze
* window = where graphics are drawn for shared drawing
* log = file pointer to log file for progress logging
* moveCount = for tracking total moves across threads 

**Pseudocode**

	1. Create/allocate memory for a new startup struct
	2. Set all parameters to corresponding startupInfo_t attribute
	3. For hostname, make a copy and store the copied version
	4. Return configured startup struct

```c
void deleteStartupStruct(startupInfo_t *s);
```

**Parameters:**

* s = startupInfo_t struct

**Pseudocode**

	1. free hostname attribute
	2. destroy the mutex lock 
	3. free the struct itself

```c
xyPair_t *xyPairNew(int x, int y);
```

**Parameters:**

* x = x-coordinate
* y = y-coordinate

**Pseudocode**

	1. Allocate memory for new xyPair
	2. set new xyPair's x & y from given values
	3. return new xyPair

```c
avatar_t **createAvatars(int numAvatars);
```

**Parameters:**

* numAvatars = number of avatars for current "game"

**Pseudocode**

	1. Allocate space for numAvatars avatar structs
	2. Iterate through each spot,
	3. Create a new avatar object and add to the array
	4. return array of avatars

```c
void deleteAvatars(avatar_t **avatars, int numAvatars);
```

**Parameters:**

* avatars = array of pointers to avatar structs
* numAvatars = number of avatars for current "game"

**Pseudocode**

	1. for each avatar in the given array (numAvatars amount),
	2. delete the avatar.
	3. Then, free the array itself

```c
avatar_t *avatarNew(int avatarID);
```

**Parameters:**

* avatarID = ID given from server

**Pseudocode**

	1. Allocate space for new avatar struct
	2. If allocated correctly, assign ID & initialize other values
	3. return the new avatar.

```c
void avatarDelete(avatar_t *avatar);
```

**Parameters:**

* avatar = pointer to avatar struct

**Pseudocode**

	1. Free the struct at given pointer address

```c
void setPosition(avatar_t *avatar, int x, int y);
```

**Parameters:**

* avatar = pointer to avatar struct
* x = x-coordinate to update to
* y = y-coordinate to update to

**Pseudocode**

	1. Assign x-coordinate to avatar's stored value
	2. Assign y-coordinate to avatar's stored value

```c
void setDirection(avatar_t *avatar, int direction);
```

**Parameters:**

* avatar = pointer to avatar struct
* direction = direction to re-orient given avatar to

**Pseudocode**

	1. Assign direction to given avatar object's stored direction

```c
int leftHandRule(avatar_t *currentAvatar, mazeTile_t *currentTile, int *lastTurn, int numAvatars, avatar_t **avatars);
```

**Parameters:**

* currentAvatar = used to update direction
* currentTile = used to see where known walls are
* lastTurn = used to track which avatar made last turn
* numAvatars = used to check currentAvatar's pos. against last "goal" avatar
* avatars = array of all avatars

**Pseudocode**

	1. Store currentAvatar's ID as the lastTurn ID
	2. If current avatar is the last avatar in the array, return null move.
	3. If current avatar is at same position as last avatar, return null move.
	4. If current avatar is facing north, 
	5. If there's no wall to the west, move west.
	6. If there's no wall to the north, move north.
	7. If there's no wall to the east, move east.
	8. If there's no wall to the south, move south.
	9. Else if current avatar is facing east, 
	10. If there's no wall to the north, move north.
	11. If there's no wall to the east, move east.
	12. If there's no wall to the south, move south.
	13. If there's no wall to the west, move west.
	14. Else if current avatar is facing south, 
	15. If there's no wall to the east, move east.
	16. If there's no wall to the south, move south.
	17. If there's no wall to the west, move west.
	18. If there's no wall to the north, move north.
	19. Else if current avatar is facing west,
	20. If there's no wall to the south, move south.
	21. If there's no wall to the west, move west.
	22. If there's no wall to the north, move north.
	23. If there's no wall to the east, move east.
	24. Otherwise, return null move.

```c
void runAvatarError(FILE *log, int responseType, int moveCount, int avatarID, int *solved);
```

**Parameters:**

* log = file pointer to write to
* responseType = type of error from server
* moveCount = moveCount of avatars
* avatarID = avatar that "caused" error
* solved = used to determine if "game over"

**Pseudocode**

	1. Check responseType against all error types, sequentially.
	2. Error types are detailed in amazing.h

```c
char* parseDirection(int direction);
```

**Parameters:**

* direction = direction integer to convert to string

**Pseudocode**

	1. If direction is 0,
	2. return "west"
	3. Else if direction is 1,
	4. return "north"
	5. Else if direction is 2,
	6. return "south"
	7. Else if direction is 3,
	8. return "east"
	9. else,
	10. return "NULL MOVE"

```c
void* runAvatar(void *startup);
```

**Parameters:**

* startup = startupInfo_t object containing all necessary pieces of information

**Pseudocode**

	1. Extract all attributes of the startup struct for later use,
	2. Create socket for connecting to maze w/ given port number.
	3. Try and connect,
	4. If unsuccessful, exit.
	5. Otherwise, 
	6. Assemble avatar_ready message w/ given ID,
	7. Send the assembled message to the server.
	8. While maze not solved and no error received,
	9. Read response from server,
	10. If response type is AM_AVATAR_TURN,
	11. Parse/extract message contents to get positions,
	12. If it's the first turn, set avatar's initial position & log it.
	13. If the ID of the last avatar to move equals currentAvatar's id, 
	14. If currentAvatar hasn't received "new" coordinates & thus hasn't moved, 
	15. if currentAvatar's direction is not 8 & therefore should still be adding walls, add a wall & re-orient the avatar to where it was facing before the failed move attempt.
	16. Re-draw the maze
	17. If our coordinates are not the same, the move was a success, and we must re-draw the maze and update currentAvatar's positions to the new coordinates.
	18. For all avatars, log their current "status"
	19. Else if myID equals the turnID sent from the server,
	20. Save current direction as oldDirection
	21. Get move from leftHandRule function.
	22. Assemble move message containing ID & moveDirection
	23. Send message & increment move counts
	24. Log move.
	25. Else if type of response is an AM_ERROR,
	26. pass error type to runAvatarError & log according to type.
	27. If the error is of type AM_TOO_MANY_MOVES, close the graphics window.
	28. Else if type of response is AM_MAZE_SOLVED then,
	29. Delete startup struct & close graphics window, 
	30. Log the solution w/ avatarNum, difficulty, numberMoves, and hash.
	31. Set solved to true, to exit main while loop.
	32. Outside of loop, exit thread.


### mazeSolver.c:

```c
mazeTile_t *mazeTileNew(int xCoord, int yCoord);
```

**Parameters:**

* xCoord = x coordinate of mazetile
* yCoord = y coordinate of mazetile

**Pseudocode**

	1. Allocates memory space for mazeTile_t creation 

	2. Initialise values of the components of the mazetile struct

	3. Return the mazetile_t 


```c 
void addWall(mazeTile_t ***maze, int x, int y, int direction);
```

**Parameters:**

* maze = 2D mazetile array
* xCoord = x coordinate of mazetile
* yCoord = y coordinate of mazetile
* direction = integer direction indicating which direction to add the wall in

**Pseudocode**

	1. if the direction that the avatar failed to move in was north, set north wall of the mazetile to true

	2. Repeat for all 3 other directions (east, south, west)


```c
mazeTile_t ***createMaze(int height, int width, int avatarNum);
```

**Parameters:**

* height = height of the maze 
* width = width of the maze 
* avatarNum = number of avatars 

**Pseudocode**

	1. Allocates space for the 2D array, using the height and width provided 

	2. Create a new mazetile_t for every space / 'tile' of the maze

	3. Return the 2D array


```c
void mazeDelete(int height, int width, mazeTile_t ***tiles);
```
**Parameters:**

* height = height of the maze 
* width = width of the maze 
* tiles = 2D mazetile array

**Pseudocode**

	1. Loops through every column and row of the maze array

	2. Free every individual tile

	3. Free the 2D array struct


### graphics.c:

```c
void drawMaze(int mazeHeight, int mazeWidth, int avatarNum, avatar_t **avatars, mazeTile_t ***maze)
```
**Parameters:**

* mazeHeight = height of the maze
* mazeWidth = width od the maze
* avatarNum = number of avatars in the maze
* avatars = array of avatar_t structs
* maze = 2D array of mazetile_t structs

**Pseudocode**

	1. Iterates over every mazetile in the 2D maze array and draws the mazetiles 

	2. Iterates over every avatar in the avatar array and draws them at their current positions

	3. Draws the outer borders of the maze

	4. Clears the avatars previous printed location (to create a seamless animated movement)


```c
void draw_maze_tile(mazeTile_t *mazeTile);
```
**Parameters:**

* mazeTile = mazeTile struct which represents one coordinate pair of the maze

**Pseudocode:**

	1. If the north wall of the mazetile struct is true, draw a wall in the north direction

	2. If the south wall of the mazetile struct is true, draw a wall in the north direction

	3. If the east wall of the mazetile struct is true, draw a wall in the north direction

	4. If the west wall of the mazetile struct is true, draw a wall in the north direction

```c
void drawOuterBorders(int mazeHeight, int mazeWidth);
```

**Parameters:**

* mazeWidth = width of the maze
* mazeHeight = height of the maze

**Pseudocode:**

	1. Draw top borders 

	2. Draw bottom borders 

	3. Draw left borders

	4. Draw right borders

```c
void drawWall(int x, int y, char *direction);
```

**Parameters:**

* x = x coordinate of the wall
* y = y coordinate of the wall
* direction = string representing cardinal direction

**Pseudocode:**

	1. If the wall is a north wall, draw a wall to the north of the current tile position

	2. If the wall is an east wall, draw a wall to the east of the current tile position

	3. If the wall is a south wall, draw a wall to the south of the current tile position

	4. If the wall is a west wall, draw a wall to the west of the current tile position

```c
void drawAvatar(int avatarID, int x, int y);
```

**Parameters:**

* avatarId = ID of the avatar 
* x = x coordinate of the avatar
* y = y coordinate of the avatar

**Pseudocode:**

	1. Convert the integer ID into a string, which will be printed as the object image on the maze.

	2. Display the char at the given (x,y) location

```c
int convertY(int num);
```		
**Parameters:**

* int num = number to be converted to fit in the proportions of the diplayed maze

**Pseudocode:**

	1. Perform computations to convert a given point into a display point in the Y direction

```c
int convertX(int num);
```

**Parameters:**

* int num = number to be converted to fit in the proportions of the diplayed maze

**Pseudocode:**

	1. Perform computations to convert a given point into a display point in the X direction




## Data structures (e.g., struct names and members):

### AMStartup.c:

* `AM_Message` as described in amazing.h

```c
/* AM_INIT */
struct
{
    uint32_t nAvatars;
    uint32_t Difficulty;
} init;

/* AM_INIT_OK */
struct
{
    uint32_t MazePort;
    uint32_t MazeWidth;
    uint32_t MazeHeight;
} init_ok;

/* AM_INIT_FAILED */
struct
{
    uint32_t ErrNum;
} init_failed;

/* AM_AVATAR_READY */
struct
{
    uint32_t AvatarId;
} avatar_ready;

/* AM_AVATAR_TURN */
struct
{
    uint32_t TurnId;
    XYPos    Pos[AM_MAX_AVATAR];
} avatar_turn;

/* AM_AVATAR_MOVE */
struct
{
    uint32_t AvatarId;
    uint32_t Direction;
} avatar_move;

/* AM_MAZE_SOLVED */
struct
{
    uint32_t nAvatars;
    uint32_t Difficulty;
    uint32_t nMoves;
    uint32_t Hash;
} maze_solved;

/* AM_UNKNOWN_MSG_TYPE */
struct
{
    uint32_t BadType;
} unknown_msg_type;
```

### avatar.c:

* `startupInfo_t` as described in avatar.h
```c
	int avatarID;
    int nAvatars;
    int difficulty;
    char *hostname;
    int mazePort;
    char *logFile;
    int comm_sock;
    pthread_mutex_t *lock;
    avatar_t **avatars;
    int *lastTurnID;
    mazeTile_t ***maze;
    int *solved;
    int height;
    int width;
    WINDOW *window;
    FILE *log;
    int *moveCount;
```

* `xyPair_t` as described in avatar.h
```c
	int xCoord;
	int yCoord;
```

* `avatar_t` as described in avatar.h
```c
	int avatarID; 
   	int xCoord; 
	int yCoord;
	int direction;
	bool firstTurn;
```

### mazeSolver.c

* `mazeTile_t` as described in mazeSolver.h
```c
	int xCoord;
	int yCoord;
	bool northWall;
	bool southWall;
	bool eastWall;
	bool westWall;
	int visited;
```

### graphics.c: 

* No data structures defined in graphics.c


## Error handling and recovery.

### AMStartup.c:

```
|  	   Exit Status		| Description			                |
|------------------------------	|-----------------------------------------------|
|		0		| successful execution				|
|		1		| incorrect number of inputs			|
|		2		| invalid Difficulty   				|
|		3		| invalid number of avatars 			|
|		4		| error opening socket	  			|	
|		5		| unknown hostname     				|
|		6		| unable to connect stream socket  		|
|		7		| no message received     			|
|		8		| failed initialisation (IS_AM_ERROR)   	|
|		9		| mutex_init failed    				|
|		10		| mutex_init failed   				|
|		11		| error creating thread for avatar number __    |
|		12		| error creating log   				|
```

### avatar.c:

|  	   Exit Status		| Description			                |
|------------------------------	|-----------------------------------------------|
|		0		| successful execution				|
|		1		| No message received from server		|
|		2		| error opening socket   			|
|		3		| unknown hostname 				|
|		4		| unable to connect stream socket		|

### mazeSolver.c

Contains only helper functions which are called within avatar.c. If there are any errors in allocating memory for the data structures, corresponding error messages will be outputted. 

### graphics.c: 

Contains only helper functions which are called within avatar.c. If there are any errors in allocating memory for the data structures, corresponding error messages will be outputted. 

Note: The functions of graphics.c and mazeSolver are not affected by user input. As such, there are no exit-code error cases to be handled. 


## Memory-Related Issues

- To test for memory leaks, run valgrind. 

- Notice that resulted memory leaks are caused by the <curses.h> library. Example shown below. 

e.g. 

```
 3,904 bytes in 1 blocks are still reachable in loss record 33 of 37
==620454==    at 0x483980B: malloc (vg_replace_malloc.c:309)
==620454==    by 0x48EEF3F: ??? (in /usr/lib64/libtinfo.so.6.1)
==620454==    by 0x48F3BAB: _nc_setupterm (in /usr/lib64/libtinfo.so.6.1)
==620454==    by 0x48C70EC: newterm_sp (in /usr/lib64/libncurses.so.6.1)
==620454==    by 0x48C759C: newterm (in /usr/lib64/libncurses.so.6.1)
==620454==    by 0x48C35CF: initscr (in /usr/lib64/libncurses.so.6.1)		# curses.h throwing errors
==620454==    by 0x402996: main (AMStartup.c:180)
```

From internet research and TA consulting, we find that the memory errors raised by curses are not caused by our own data structures and implementation. Rather, they are harmless, raised by initscr(). 

- Notice that resulted memory errors are caused by <pthread.h>. Example shown below. 

``` 
Thread 3:

==591127== Syscall param socketcall.sendto(msg) points to uninitialised byte(s)
==591127==    at 0x48A3C8C: __libc_send (send.c:28)
==591127==    by 0x48A3C8C: send (send.c:23)
==591127==    by 0x402C90: runAvatar (avatar.c:483)
==591127==    by 0x48994E1: start_thread (pthread_create.c:479)
==591127==    by 0x4A106D2: clone (clone.S:95)

```

- To verify that the data structures and functions we implemented do not generate any leaks or errors, comment out any curse-related functions. These include: 

```c 
# creating a window instance
WINDOW *window;

# initialising the screen
if ((mainwindow = initscr())== NULL){
    printf("failed to initialise screen\n");
    exit(EXIT_FAILURE);
}

# drawing the maze
drawMaze(getHeight(initStruct), getWidth(initStruct), getNumAvatars(initStruct), avatars, maze);

# deleting the window
delwin(mainwindow);

# ending the window
endwin()
```

Here is an example of when the functions above are commented out:


```
[lucalit888@flume ~/project-maze-challenge-bing]$ valgrind ./AMStartup -h flume.cs.dartmouth.edu -d 2 -n 2
	.
	.
	.
==591127== HEAP SUMMARY:
==591127==     in use at exit: 0 bytes in 0 blocks
==591127==   total heap usage: 262 allocs, 262 frees, 59,950 bytes allocated
==591127== 
==591127== All heap blocks were freed -- no leaks are possible
```


To build, run `make`.

To clean up, run `make clean`.
