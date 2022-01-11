# CS50 The Amazing Project - DESIGN.md
## Team members: Mack, Sean, Connor and Luca 

This document provides and in-depth design specification of the input and outputs, data flow, major data structures and pseudocode for the two key pieces of our Amazing project: 

(1) the AMStartup program
(2) the Avatar program
(3) the Graphics program 


======================================================================================

### AMStartup Program: 


#### Inputs and Outputs:

* Input: 

	The user must execute the program using the following parameters via the commandline:

	-n nAvatars: (int) the number of Avatars in the maze

	-d Difficulty: (int) the difficulty level, on the scale of 0 (easy) to 9 (hardest)

	-h Hostname: (char *) the hostname of the server. Our server will be running on flume.cs.dartmouth.edu 
	
	Note: setting the difficulty level to 7+ may result in a longer run time for the program. 

	```
	./AMStartup -n <NUM_OF_AVATARS> -d <DIFFICULTY_LEVEL> -h <HOST_NAME>
	```

* Output:
	
	If the inputted parameters are valid, the avatar program will be executed, and their outputs created. Refer to the "Output" section of the Avatar Program for more information. 

	If the inputted parameters are invalid, the AMStartup client will display the corresponding error messages. Otherwise, the program will exit with 0 status. 


#### Functional decomposition into modules:

Main function (type int):

- Takes in two parameters, **argc** - an integer representing the number of arguments provided, and **argv[]** - a string array that helps us refer to which argument in the commandline. 


#### Pseudo code for logic/algorithmic flow:

The program essentially validates the parameters that are provided by the user on the commandline, and initialises all modules in the Avatar Program. 

1. If the <NUM_OF_AVATARS> entered is invalid, then the program will print an error message and exit with non-zero status. Otherwise, it will create < NUM_OF_AVATARS> number of threads, each representing an avatar.

2. The <DIFFICULTY_LEVEL> must be an integer between 0 and 9. If the user enters an invalid input, the program prints an error message and exits with non-zero status. Otherwise, this integer will be directed to the Avatar Program, to be read and used to determine the difficulty level of the maze. 

3. The program checks for a valid <HOST_NAME>, which in our case will be flume.cs.dartmouth.edu. If the server fails to establish a connection (invalid server), it will print an error message and exit with non-zero status. Else, a response from the server, AM_INIT_OK, should be received. 

4. The program stores the information that is returned in the AM_INIT_OK, which includes information such as MazePort, MazeWidth and MazeHeight.

5. AMStartup will also create a log file, Amazing_$USER_N_D.log, where $USER is the current userid, N is the value of nAvatars and D is the value of Difficulty. 

6. AMStartup creates a 2D array representing each square of the maze. The array will hold a struct containing the following components: north_wall (bool), south_wall (bool), east_wall (bool), west_wall (bool), xCoord (int), yCoord (int), and visited (int) that tracks who discovered that tile.

7. The program will initialise threads running in the runAvatar() method of avatar.c, NUM_OF_AVATARS number of times, passing in a struct w/ the following attributes:
	- AvatarId
	- nAvatars
	- difficulty
	- hostname
	- mazePort
	- logFile (name of logFile)
	- lock (mutex lock)
	- avatars (array of all avatars)
	- lastTurnID 
	- maze (2D array of maze tile structs)
	- solved (int)
	- height (of maze)
	- width (of maze)
	- window (for graphics);
	- moveCount;


8. When the Avatar Program successfully terminates, when the maze is solved (all avatars meet at the same point, or an error exit),the corresponding status will be returned to the AMStartup program, and the corresponding success/failure procedures will be executed.

9. If everything runs successfully, the program exits with 0 status. 


#### Dataflow through modules:

Upon validation of all inputted parameters, <NUM_OF_AVATARS>, <DIFFICULTY_LEVEL> and <HOST_NAME>, the client AMStartup sends an AM_INIT message to the server at the AM_SERVER_PORT. Successful establishment of connection will have the server generate a new maze, distribute Avatars across the maze, and reply to the client with the AM_INIT_OK message. The message also contains the unique MazePort as the TCP/IP Port number that will be used to communicate with the server about this new maze. The server then begins listening on that new MazePort. In addition, the message includes the MazeWidth and MazeHeight of the new maze. AMStartup extracts the MazePort from the message, and then starts up NUM_OF_AVATARS number of threads, one for every Avatar, running the main client software and each provided with appropriate start parameters. Once the maze is solved, a AM_MAZE_SOLVED message will be given, closing the Mazeport and freeing all the data structures used in Avatar Program. To ensure that all threads are terminated before the program exits, pthread_join will be called. In addition, if there are any errors during the running of the maze, the Avatar Program will terminate & AMStartup will free all allocated memory.

#### Major data structures:

AMStartup.c does not define any data structures. However, its execution heavily depends on the data structures that are called in avatar.c. These include: 

1. A 2D array of data structures representing each square of the maze. 

2. "Avatar" data structure representing a single avatar. An array of these avatar structs will be made. Its size will be the number of avatars. 

3. "Startup" struct that can hold all info needed by an avatar thread but be passed as a void pointer to the thread from AMStartup. 

4. "AM_Message" struct that contains the different AM messages that will be sent and received between the server.


Note: for more information on each data structure, refer to the **Major data structures** of the avatar program section. For AM_Message, refer to amazing.h.


======================================================================================

### Avatar Program: 


#### Inputs and Outputs

* Input: 

	The avatar program takes in the following inputs:

	1. (int) avatarID - passes the avatar its assigned ID
	2. (int) nAvatars - allows the avatar to know how many avatars there are
	3. (int) difficulty - the difficulty of the current maze
	4. (char *) hostname - hostname of maze's host for socket creation
	5. (int) mazePort - port number for the maze
	6. (char *) logFile - name of file to write progress
	7. (pthread_mutex_t) lock - used for mutex locking for threading
	8. (avatar_t **) avatars - array of pointers to avatars for avatar "communication"
	9. (int *) lastTurnID - allows all avatars to know if they made the last turn on next loop iteration
	10. (mazeTile_t ***) - 2D array of pointers to mazeTile_t structs, for shared knowledge of the maze
	11. (int *) solved - main loop condition for ending loop on failure/solution
	12. (int) height - height of maze
	13. (int) width - width of maze
	14. (WINDOW *) window - where graphics are drawn for shared drawing
	15. (FILE *) log - file pointer to log file for progress logging
	16. (int *) moveCount - for tracking total moves across threads 


* Output:
	
	Each avatar will write into the same log file, inputted as an attribute of a startup structure (refer above). This will be in the form Amazing_$USER_N_D.log, where $USER is the current userID, N is the value of nAvatars and D is the value of Difficulty. The first line of the log file will write out the MazePort, date and time. 

	Whenever a new connection is established between the client and the server, a directory is created and named after the specific MazePort given to AMStartup, e.g., /var/tmp/23928. This directory will contain two log files, log.out and log.err. Users can refer to these two files for tracking avatar movement and debugging, respectively.

	In addition, the server will generate a series of PNG images for every “round” of Avatar turns. These files accumulate throughout the course of the program. If the program crashes, or the maze is successfully solved (where all avatars converge at one position), there is a window of 5 minutes to collect these files; for any time after that, the directory will be automatically removed.

	We will also implement a seamless animated graphics module, in which the avatars will update their positions, draw walls, etc. in real time running of our program. 


#### Functional decomposition into modules

* xyPairNew() - takes x & y coordinate and creates an xyPair struct

* createAvatars() - takes numAvatars & creates an array of avatars of the corresponding length with assigned ID's

* deleteAvatars() - takes an array of avatars and deletes each avatar, and then the array itself

* avatarNew() - takes an ID and creates an avatar struct w/ that ID

* appendCrumb() - takes an avatar pointer and a coordinate pair and adds that location to the avatar's list of breadcrumbs

* followAvatar() - takes two avatars and returns the coordinates that avatar 2 should take to "follow" avatar 1 according to avatar 1's breadcrumbs

* avatarDelete() - deletes a single avatar & all stored information

* leftHandRule() - takes a reference to an avatar & its current maze tile & decides what direction to move in to follow the "left hand rule"

* runAvatar() - takes the "startup struct" containing all necessary information for a maze to be connected to and solved. Runs continously until an error case or the maze has been solved.

* setPosition() - updates an avatar's coordinate position with passed coordinates

* setDirection() - re-orients the given avatar in the given direction

* loadStartupStruct() - takes all components for an avatar's startup struct & creates the struct

* deleteStartupStruct() - deletes all information contained within a startup struct, freeing memory

* runAvatarError() - catch-all error system that determines the specific error & writes a corresponding message to our log, then sets a value that's used as main loop condition

* parseDirection() - converts integer direction to "string version" (i.e. 0 -> "west")
 

#### Pseudo code for logic/algorithmic flow

The avatar program is called by the AMStartup.c, and performs the following functions: 

1. Places the avatars in their starting locations. The last avatar (with the greatest avatarID) is set to not move at all. All other avatars will attempt to search for it. 

2. Marks the starting position/mazetile in the 2D maze array as visited

3. Check if each avatar has visited the starting location (in the event that avatars are initialised in the same position).

4. Set solution boolean to false (it is only turned true when all avatars are in the same common point)

5. While solution boolean is false or below move limit:
	- Move according to the “left-hand rule,” where we prioritize left turns first, straight paths second, right turns third, and backwards paths last.
	- At every stage, we will send a move message to the server, which informs which direction we want the avatar to move in. 
	- If the move was unsuccessful, add a wall to the mazetile struct, so that the wall is marked and known for all avatars.
	- If the move was successful, proceed to move to that tile 

6. If any avatar that is **NOT** the last one runs into the last avatar (the one with the largest ID who remains fixed in position), it will stop there. 

7. When all avatars have reached the position of the last avatar i.e. all avatars have converged on a common spot, then the maze has been solved, and the game ends. 

8. Data structures are freed correspondingly, threads are terminated using pthread_join(), and the program exits with 0 status back to AMStartup.c, which should also terminate successfully with 0 status. 


#### Dataflow through modules

Called before each pthread_create(), which creates a thread for every single avatar within the number of avatars specified by the user, AMStartup.c loads a startup struct which contains informatiion such as avatarID, difficulty, hostname, mazePort, logfile, etc. (Refer to the major data structures section below to find out more).

This is the main link between the AMStartup and avatar module. When loadStartupStruct() is called, avatar.c performs the following:
- Creates a socket
- Initializes the fields of the server address
- Look up the hostname specified on command line
- Connects the socket to that server 
- Configure & send AM_INIT Message
- Check if initialization was successful & store response values
- If the response type is AM_INIT_OK, it will run the pseudocode described in the previous section. 
- If at some point, an AM_ERROR_ message is received, the program will print the corresponding error messages to the logfile, which the user can look into to keep track of the success of the program. 
- For instance, if the avatars exceed the move limit determined by AM_MAX_MOVES * (1 + difficulty) * num_of_avatars, then the program exits by printing an "move limit exceeded message"
- At the end, once an AM_MAZE_SOLVED message is returned, or there are any AM_ERROR messages that cause the termination of the avatar.c program, the dataflow will return to AMStartup.c. 


#### Major data structures

1. A 2D array of data structures representing each square of the maze. 
	- Within each index pair of the array, we have a data structure which contains 5 components:
	- (bool) north_wall - initialised as NULL. TRUE if a wall is encountered, else FALSE.
	- (bool) south_wall -  initialised as NULL. TRUE if a wall is encountered, else FALSE.
	- (bool) east_wall -  initialised as NULL. TRUE if a wall is encountered, else FALSE.
	- (bool) west_wall - initialised as NULL. TRUE if a wall is encountered, else FALSE.
	- (int) visited - stores the ID of the Avatar that discovered this tile
	- (int) xCoord - stores the x-coordinate of the maze tile
	- (int) yCoord = stores the y-coordinate of the maze tile

2. "Avatar" data structure representing a single avatar.
	- Each avatar stores information that allows it to keep track of certain state variables:
	- (int) avatarID - stores the avatar's assigned ID
	- (int) xCoord - stores the avatar's current x-coordinate
	- (int) yCoord - storest the avatar's current y-coordinate 
	- (int) direction - stores the avatar's orientation 
	- (bool) firstTurn - used initially to recognize an avatar's first turn and initialize some values

3. "Startup" struct that can hold all info needed by an avatar thread but be passed as a void pointer to the thread from AMStartup
	- (int) avatarID - passes the avatar its assigned ID
	- (int) nAvatars - allows the avatar to know how many avatars there are
	- (int) difficulty - the difficulty of the current maze
	- (char *) hostname - hostname of maze's host for socket creation
	- (int) mazePort - port number for the maze
	- (char *) logFile - name of file to write progress
	- (pthread_mutex_t) lock - used for mutex locking for threading
	- (avatar_t **) avatars - array of pointers to avatars for avatar "communication"
	- (int *) lastTurnID - allows all avatars to know if they made the last turn on next loop iteration
	- (mazeTile_t ***) - 2D array of pointers to mazeTile_t structs, for shared knowledge of the maze
	- (int *) solved - main loop condition for ending loop on failure/solution
	- (int) height - height of maze
	- (int) width - width of maze
	- (WINDOW *) window - where graphics are drawn for shared drawing
	- (FILE *) log - file pointer to log file for progress logging
	- (int *) moveCount - for tracking total moves across threads 



### Graphics Program: 

For the graphics program, we chose to use the <curses.h> library to display our maze. 

#### Inputs and Outputs:

* Inputs: 

	The graphics requires a window initialisation which is called by initscr()

* Outputs: 

	The output is a newly initialised window that is created to only display the maze and all its inner components (avatars, walls).

#### graphics.c

We will implement the graphics using a graphics.c module, which contains within functions for drawing walls and avatars. The walls will be represented by stars, pipes and dashes, while avatars are represented as their corresponding numbers (IDs). 

A drawMaze() will be implemented, such that it takes a 2D maze array and an avatars array, so that it can draw the tiles and avatars with the correct positions and information. 

#### Dataflow through modules:

The initialisation of window will be called in AMStartup.c. However, the drawMaze() function which call upon curses functions such as mvaddstr(), will be called within avatar.c. When the maze is successfully solved, avatar.c deletes and ends the window to allow return to the terminal. 


