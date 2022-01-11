/* 
 * avatar.c - 'avatar' module
 *
 * see avatar.h for more information.
 *
 */


#include <stdio.h>
#include <stdlib.h>	
#include <stdbool.h>		
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>		  // socket-related structures
#include <pthread.h>	  // threading library
#include <curses.h>		  // ASCII graphics
#include "amazing.h"	  // client/server communication spec
#include "avatar.h"		  // avatar header file for module use
#include "mazeSolver.h"	  // maze representation/move logic functions
#include "graphics.h"	  // ASCII graphics/maze rendering


// ***************************** STRUCTS *********************************

/*
 *	Allows passing of necessary info to thread via void *
 */
typedef struct startupInfo {
	int avatarID;
	int nAvatars;
	int difficulty;
	char *hostname;
	int mazePort;
	char *logFile;
	int comm_sock;
	pthread_mutex_t *lock;
	pthread_mutex_t *lock2;
	avatar_t **avatars;
	int *lastTurnID;
	mazeTile_t ***maze;
	int *solved;
	int height;
	int width;
	WINDOW *window;
	FILE *log;
	int *moveCount;
} startupInfo_t;

// ***********************************************************************
// ************************** STRUCT FUNCTIONS ***************************


// ------------ startupInfo_t ---------------

/*
 *	The following are "getter" functions to allow extracting info 
 *	from startup struct more easily:
 */
int getID(startupInfo_t* s) {
	return s->avatarID;
}
int getNumAvatars(startupInfo_t* s) {
	return s->nAvatars;
}
int getDiff(startupInfo_t* s) {
	return s->difficulty;
}
char* getHostname(startupInfo_t* s) {
	return s->hostname;
}
int getMazePort(startupInfo_t* s) {
	return s->mazePort;
}
char* getLogFileName(startupInfo_t* s) {
	return s->logFile;
}
pthread_mutex_t* getMutexLock(startupInfo_t* s) {
	return s->lock;
}
pthread_mutex_t* getMutexLock2(startupInfo_t* s) {
	return s->lock2;
}
avatar_t **getAvatars(startupInfo_t* s) {
	return s->avatars;
}
int *getLastTurnID(startupInfo_t* s) {
	return s->lastTurnID;
}
mazeTile_t ***getMaze(startupInfo_t* s) {
	return s->maze;
}
int* getSolved(startupInfo_t *s) {
	return s->solved;
}
int getHeight(startupInfo_t *s) {
	return s->height;
}
int getWidth(startupInfo_t *s) {
	return s->width;
}
WINDOW *getWindow(startupInfo_t *s) {
	return s->window;
}
FILE* getLog(startupInfo_t *s) {
	return s->log;
}
int* getMoveCount(startupInfo_t *s) {
	return s->moveCount;
}

/*
 *	Takes all attributes of a startupInfo_t as paramaters & creates an instance & assigns attributes
 */
startupInfo_t* loadStartupStruct(pthread_mutex_t *lock, int avatarID, int nAvatars, int difficulty, char *hostname, int mazePort, char *logFile, avatar_t **avatars, int *lastTurnID, mazeTile_t ***maze, int *solved, int height, int width, WINDOW *window, FILE *log, int *moveCount) {
	// set values
	startupInfo_t *startup = malloc(sizeof(startupInfo_t));
	startup->avatarID = avatarID;
	startup->nAvatars = nAvatars;
	startup->difficulty = difficulty;
	startup->mazePort = mazePort;
	startup->logFile = logFile;
	startup->lock = lock;
	startup->avatars = avatars;
	startup->lastTurnID = lastTurnID;
	startup->maze = maze;
	startup->solved = solved;
	startup->height = height;
	startup->width = width;
	startup->window = window;
	startup->log = log;
	startup->moveCount = moveCount;

	// Copy hostname
	char* hostname_copy = malloc(strlen(hostname) + 1);
	strcpy(hostname_copy, hostname);
	startup->hostname = hostname_copy;

	// return struct
	return startup;
}

/*
 *	Frees all memory held within a startupInfo_t, including itself.
 */
void deleteStartupStruct(startupInfo_t *s) {
	free(s->hostname);
	pthread_mutex_destroy(s->lock);
	free(s);
}

// --------------------------------------------------------

// ---------------------- avatar_t ------------------------

/*
 *	Creates & returns 'numAvatars' avatar_t's in an array
 */
avatar_t **createAvatars(int numAvatars) {
	avatar_t **avatars = malloc(numAvatars * sizeof(avatar_t));
	// iterate through 'numAvatars' time
	for (int id = 0; id < numAvatars; id++) {
		avatar_t *avatar = avatarNew(id);
		avatars[id] = avatar;
	}
	return avatars;
}

/*
 *	Takes an array of avatar_t's & deletes each one, and then the array
 */
void deleteAvatars(avatar_t **avatars, int numAvatars) {
	// Iterate through entire avatar array
	for (int i = 0; i < numAvatars; i++) {
		avatarDelete(avatars[i]);
	}
	free(avatars);
}

/*	
 *	Creates an avatar_t instance w/ the given ID, assigning necessary memory
 */
avatar_t *avatarNew(int avatarID) {

	// allocate memory space for avatar creation 
	avatar_t *avatar = malloc(sizeof(avatar_t));

	// check that the memory space was allocated 
	if (avatar != NULL){
		// Initialize values
		avatar->avatarID = avatarID;
		avatar->firstTurn = true;
		avatar->xCoord = 0;
		avatar->yCoord = 0;
		avatar->direction = 2;

		return avatar; 
	} 
	// if there was a failure in allocating memory space
	else {
		fprintf(stderr, "Failed to malloc for avatar\n");
		return NULL;
	}
}

/*
 *	Frees an avatar_t instance
 */
void avatarDelete(avatar_t *avatar) {
	free(avatar);
}

/*
 *	Updates an avatar_t structs x & y-coordinate to the given values
 */
void setPosition(avatar_t *avatar, int x, int y) {
	avatar->xCoord = x;
	avatar->yCoord = y;
}

/*
 *	Updates an avatar_t structs direction to the given value
 */
void setDirection(avatar_t *avatar, int direction) {
	avatar->direction = direction;
}


// ***********************************************************************
// ************************** HELPER FUNCTIONS ***************************


/*
 *	Returns the direction an avatar should move in based on its location & the existence of walls
 */
int leftHandRule(avatar_t *currentAvatar, mazeTile_t *currentTile, int *lastTurn, int numAvatars, avatar_t **avatars) {
	(*lastTurn) = currentAvatar->avatarID;

	// Last avatar should not move
	if (currentAvatar->avatarID == (numAvatars - 1)) {
		currentAvatar->direction = 8;
		return M_NULL_MOVE;
	}
	// If currentAvatar is on the same tile as the "goal" avatar, don't move
	if ((currentAvatar->xCoord == avatars[numAvatars - 1]->xCoord) && (currentAvatar->yCoord == avatars[numAvatars - 1]->yCoord)) {
		currentAvatar->direction = 8;
		return M_NULL_MOVE;
	}

	// if we are facing up
	if (currentAvatar->direction == M_NORTH) {
		// if there is not a wall to the left, go left
		if (currentTile->westWall != true) {
			currentAvatar->direction = M_WEST;
			return M_WEST;
		}
		// if there is not a wall to the front, go forward
		else if (currentTile->northWall != true) {
			currentAvatar->direction = M_NORTH;
			return M_NORTH;
		}
		// if there is not a wall to the right, go right
		else if (currentTile->eastWall != true) {
			currentAvatar->direction = M_EAST;
			return M_EAST;
		}
		// if there is not a wall to the bottom, go backwards
		else if (currentTile->southWall != true) {
			currentAvatar->direction = M_SOUTH;
			return M_SOUTH;
		}
	}
	// if we are facing east
	else if (currentAvatar->direction == M_EAST) {
		// if there is not a wall to the left, go up
		if (currentTile->northWall != true) {
			currentAvatar->direction = M_NORTH;
			return M_NORTH;
		}
		// if there is not a wall in front, go forward
		else if (currentTile->eastWall != true) {
			currentAvatar->direction = M_EAST;
			return M_EAST;
		}
		// if there is not a wall to the right, go right
		else if (currentTile->southWall != true) {
			currentAvatar->direction = M_SOUTH;
			return M_SOUTH;
		}
		// if there is not a wall to the back, go backwards
		else if (currentTile->westWall != true) {
			currentAvatar->direction = M_WEST;
			return M_WEST;
		}
	}
	// if we are facing south
	else if (currentAvatar->direction == M_SOUTH) {
		// if there is not a wall to the left, go left
		if (currentTile->eastWall != true) {
			currentAvatar->direction = M_EAST;
			return M_EAST;
		}
		// if there is not a wall in front, go forward
		else if (currentTile->southWall != true) {
			currentAvatar->direction = M_SOUTH;
			return M_SOUTH;
		}
		// if there is not a wall to the right, go right
		else if (currentTile->westWall != true) {
			currentAvatar->direction = M_WEST;
			return M_WEST;
		}
		// if there is not a wall to the back, go backwards
		else if (currentTile->northWall != true) {
			currentAvatar->direction = M_NORTH;
			return M_NORTH;
		}
	}
	// if we are facing west
	else if (currentAvatar->direction == M_WEST) {
		// if there is not a wall to the left, go left
		if (currentTile->southWall != true) {
			currentAvatar->direction = M_SOUTH;
			return M_SOUTH;
		}
		// if there is not a wall in front, go forward
		else if (currentTile->westWall != true) {
			currentAvatar->direction = M_WEST;
			return M_WEST;
		}
		// if there is not a wall to the right, go right
		else if (currentTile->northWall != true) {
			currentAvatar->direction = M_NORTH;
			return M_NORTH;
		}
		// if there is not a wall to the back, go backwards
		else if (currentTile->eastWall != true) {
			currentAvatar->direction = M_EAST;
			return M_EAST;
		}
	}
	return M_NULL_MOVE;
}

/*
 *	Determines which specific error was caught by the mask and writes to log correspondingly
 */	
void runAvatarError(FILE *log, int responseType, int moveCount, int avatarID, int *solved) {
	// Check which error type ocurred & act accordingly
	if (responseType == AM_TOO_MANY_MOVES) {
		fprintf(log, "Move limit reached at turn %d\n", moveCount);
		(*solved)++;
	}
	else if (responseType == AM_NO_SUCH_AVATAR) {
		fprintf(log, "Avatar %d does not exist\n", avatarID);
	}
	else if (responseType == AM_AVATAR_OUT_OF_TURN) {
		fprintf(log, "Avatar %d moving out of turn on turn %d\n", avatarID, moveCount);
	}
	else if (responseType == AM_SERVER_TIMEOUT) {
		fprintf(log, "Server timeout occured on turn %d\n", moveCount);
		(*solved)++;
	}
	else if (responseType == AM_UNKNOWN_MSG_TYPE) {
		fprintf(log, "Unknown message received on turn %d\n", moveCount);
	}
	else if (responseType == AM_UNEXPECTED_MSG_TYPE) {
		fprintf(log, "Unexpected message of type %d received on turn %d\n", responseType, moveCount);
	}	
}

/*
 *	Converts direction int to direction string (used for logging)
 */
char* parseDirection(int direction) {
	if (direction == 0) {
		return "west";
	}
	else if (direction == 1) {
		return "north";
	}
	else if (direction == 2) {
		return "south";
	}
	else if (direction == 3) {
		return "east";
	}
	else {
		return "no direction (NULL_MOVE)";
	}
}

// ***********************************************************************
// ********************** MAIN AVATAR FUNCTION ***************************
/*
 *   Main avatar thread function
 */
void* runAvatar(void *startup) {

	// Get init struct from void arg
	startupInfo_t *initStruct = startup;

	// Extract info from init struct
	pthread_mutex_t *lock = getMutexLock(initStruct);
	char *hostName = getHostname(initStruct);
	int myID = getID(initStruct);
	avatar_t **avatars = getAvatars(initStruct);
	int *lastTurnID = getLastTurnID(initStruct);
	mazeTile_t ***maze = getMaze(initStruct);
	int *solved = getSolved(initStruct);
	FILE *log = getLog(initStruct);
	int numAvatars = getNumAvatars(initStruct);
	int mazePort = getMazePort(initStruct);
	int *myMoveCount = getMoveCount(initStruct);

	// Initialize values for later use
	int i = 0;
	int move = 0;
	int oldDirection = avatars[myID]->direction;

	// Create socket
	int maze_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (maze_sock < 0) {
		perror("opening socket");
		exit(2);
	}

	// Initialize the fields of the server address
	struct sockaddr_in server;  // address of the server
	server.sin_family = AF_INET;
	server.sin_port = htons(mazePort);
	// Look up the hostname specified on command line
	struct hostent *hostp = gethostbyname(hostName); // server hostname

	if (hostp == NULL) {
		fprintf(stderr, "avatar: unknown host '%s'\n", hostName);
		exit(3);
	}
	memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

	// Connect the socket to that server   
	if (connect(maze_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(4);
	}	

	// Assemble avatar_ready message
	AM_Message message;
	message.type = htonl(AM_AVATAR_READY);
	uint32_t id = getID(initStruct);
	message.avatar_ready.AvatarId = htonl(id);

	// Send avatar_ready message
	send(maze_sock, (void *) &message, sizeof(message), 0);

	// Main "move loop" - ends when an error condition triggers or maze is solved
	while (*solved == 0) {

		// Wait for server's response
		AM_Message response;
		int bytesReceived = recv(maze_sock, (void *) &response, sizeof(AM_Message), 0);

		// If there was a problem getting a response, exit
		if (bytesReceived < 0) {
			fprintf(stderr, "ERROR: No message recieved.\n");
			exit(1);
		} else {
			// If we've received a turn message
			if (response.type == ntohl(AM_AVATAR_TURN)) {

				// Parse message from server
				int turnID = ntohl(response.avatar_turn.TurnId);
				XYPos *positions = response.avatar_turn.Pos;
				int newX = ntohl(positions[myID].x);
				int newY = ntohl(positions[myID].y);

				// If firstTurn, initialize position given by server
				if (avatars[myID]->firstTurn) {
					avatars[myID]->firstTurn = false;
					setPosition(avatars[myID], newX, newY);
					fprintf(log, "Initial position of Avatar %d is (%d, %d)\n", myID, avatars[myID]->xCoord, avatars[myID]->yCoord);
				} 
				// If currentAvatar made the previous move, we must check to see if it was a success
				if (*lastTurnID == myID) {
					// if old coords match new coords, we haven't moved
					if ((avatars[myID]->xCoord == newX) && (avatars[myID]->yCoord == newY)) {

						// if currentAvatar's direction is not 8, it should still be moving & adding walls
						if (avatars[myID]->direction != 8) {
							// If we couldn't move - add wall in that direction
							addWall(maze, avatars[myID]->xCoord, avatars[myID]->yCoord, avatars[myID]->direction);
							// We must re-orient. Set direction to old direction
							setDirection(avatars[myID], oldDirection);	
						}

						// Draw the maze
						pthread_mutex_lock(lock);
						drawMaze(getHeight(initStruct), getWidth(initStruct), getNumAvatars(initStruct), avatars, maze);
						pthread_mutex_unlock(lock);
					} else {
						// Move was successful, draw updated maze
						pthread_mutex_lock(lock);
						drawMaze(getHeight(initStruct), getWidth(initStruct), getNumAvatars(initStruct), avatars, maze);
						pthread_mutex_unlock(lock);
						// Update position to server's new values
						setPosition(avatars[myID], ntohl(positions[myID].x), ntohl(positions[myID].y));
					}
					// Log all avatars "statuses" in log file
					for (int idx = 0; idx < numAvatars; idx++) {
						fprintf(log, "Avatar %d at (%d,%d) on turn %d\n", idx, avatars[idx]->xCoord, avatars[idx]->yCoord, *myMoveCount+1);
					}
					// if it's currentAvatar's turn, determine new move & send it to server
				} else if (myID == turnID) {
					// store old direction in case move fails
					oldDirection = avatars[myID]->direction;
					// Determine move
					move = leftHandRule(avatars[myID], maze[avatars[myID]->yCoord][avatars[myID]->xCoord], lastTurnID, numAvatars, avatars);

					// Assemble move message
					AM_Message moveMessage;
					moveMessage.type = htonl(AM_AVATAR_MOVE);
					uint32_t id = htonl(getID(initStruct));
					uint32_t direction = htonl(move);
					moveMessage.avatar_move.AvatarId = id;
					moveMessage.avatar_move.Direction = direction;

					// Send message
					send(maze_sock, (void *) &moveMessage, sizeof(message), 0);
					// Track total move count & individual move count
					(*myMoveCount)++;
					i++;
					// Log move attempt
					char *dir = parseDirection(move);
					fprintf(log, "Avatar %d tries to move in direction %s on turn %d, which is turn %d for the  avatar\n", myID, dir, *myMoveCount, i); 
				}

				// if we've received an error
			} else if (IS_AM_ERROR(response.type)) {
				// Determine's type of error & logs accordingly
				runAvatarError(log, ntohl(response.type), *myMoveCount, myID, solved);
				// if the error was too many moves, close graphics window
				if (ntohl(response.type) == AM_TOO_MANY_MOVES) {
					printf("Move limit exceeded.\n");
					delwin(getWindow(initStruct));
					endwin();
				}

				// if maze has solved
			} else if (response.type == ntohl(AM_MAZE_SOLVED)) {
				printf("SOLVED!\n");
				printf("ID: %d \nMove Count: %d\n", myID, i);
				printf("Maze Port: %d\n", mazePort);                   
				deleteStartupStruct(initStruct);
				// if not done yet, clean up graphics & log
				if (*solved == 0 ) {
					delwin(getWindow(initStruct));
					endwin();
					int avatarNum = ntohl(response.maze_solved.nAvatars);
					int difficulty = ntohl(response.maze_solved.Difficulty);
					int numberMoves = ntohl(response.maze_solved.nMoves);
					int hash = ntohl(response.maze_solved.Hash);
					fprintf(log, "Solved!  Number of avatars: %d, difficulty: %d number of moves: %d, hash: %d\n", avatarNum, difficulty, numberMoves, hash);
				}
				(*solved)++;
			}
		}
	}
	pthread_exit(NULL);
	return NULL;
}
