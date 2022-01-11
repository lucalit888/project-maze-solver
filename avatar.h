/* 
 * avatar.h - header file for avatar module
 *
 * This module includes many useful functions for creating an avatar and an array of avatars.
 * These functions also dexcribe the behavior of an avatar.
 * See function headers for in depth descriptions.
 *
 * Written by:
 * Luca Lit, Sean Simons, Connor Davis, and Mack Reiferson Winter 2020
 */

#ifndef __AVATAR_H
#define __AVATAR_H
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <curses.h>
#include "mazeSolver.h"
#include "amazing.h"

/**************** structs ****************/

/**************** mazeTile ****************/
/*
 * Represents a coordinate pair on the maze.
 */
typedef struct mazeTile mazeTile_t;

/**************** startupInfo ****************/
/*
 * Struct representing all necessary knowledge to pass into an avatar.
 * See avatar.c for details.
 */
typedef struct startupInfo startupInfo_t;  // opaque to users of the module

/**************** avatar ****************/
/*
 * Defines an avatar struct that holds an avatar id, x coord, y coord, direction, and whether or not
 * the avatar is on its first turn.
 */
typedef struct avatar {
	int avatarID; 
	int xCoord; 
	int yCoord;
	int direction;
	bool firstTurn;
} avatar_t;

/**************** Get methods for startupInfo_T ****************/
/*
 * All functions within this section serve as getters for the startupInfo_t struct.
 *
 * It is implied that any output is returned for all functions in this section.
 *
 */

/*
 * Input: startupInfo_t struct.
 *
 * Output: ID
 *
 */
int getID(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Number of avatars.
 *
 */
int getNumAvatars(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Current difficulty. 
 *
 */
int getDiff(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Hostname.
 *
 */
char* getHostname(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Mazeport.
 *
 */
int getMazePort(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Name of log file.
 *
 */
char* getLogFileName(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: List of avatars.
 *
 */
avatar_t **getAvatars(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: ID of last avatar which had a turn.
 *
 */
int *getLastTurnID(startupInfo_t* s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Integer representing status of the game.
 *
 */
int* getSolved(startupInfo_t *s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Maze height.
 *
 */
int getHeight(startupInfo_t *s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Maze width.
 *
 */
int getWidth(startupInfo_t *s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Window for curses to draw in.
 *
 */
WINDOW *getWindow(startupInfo_t *s);

/*
 * Input: startupInfo_t struct.
 *
 * Output: Mutex lock number 2.
 *
 */
pthread_mutex_t* getMutexLock2(startupInfo_t* s);


/**************** functions ****************/

/*
 * Function which creates an array of avatar structs initialized w/ respective ID's.
 *
 * Input: Number of avatars.
 *
 * Output: Returns an array of avatars with numAvatars avatars.
 */
avatar_t **createAvatars(int numAvatars);

/*
 * Function which frees the memory associated with each avatar struct in the avatars array.
 *
 * Input: Avatars array, number of avatars.
 *
 * Output: None.
 */
void deleteAvatars(avatar_t **avatars, int numAvatars);

/*
 * Function which initializes an avatar.
 *
 * Input: ID number for avatar.
 *
 * Output: Returns an initialized avatar struct.
 */
avatar_t *avatarNew(int avatarID);

/*
 * Function which frees memory allocated for an avatar.
 *
 * Input: Avatar struct.
 *
 * Output: None.
 *
 */
void avatarDelete(avatar_t *avatar);

/*
 * Function which describes the behavior of an avatar's movement.
 *
 * Input: Avatar, mazetile, lastTurn, number of avatars, list of avatars.
 *
 * Output: Integer representing the direction for the current avatar to move in.
 *
 */
int leftHandRule(avatar_t *currentAvatar, mazeTile_t *currentTile, int *lastTurn, int numAvatars, avatar_t **avatars);

/*
 * Function which calls the helper functions to "drive" an avatar from the start to the finish of the game.
 *
 * Input: A startup struct representing all necessary knowledge the avatars need to have to beat the game.
 *
 * Output: Graphics, print statements, and log files which act as a GUI and history for the user. Output shows the user that the game has been won.
 *
 */
void* runAvatar(void *startup);

/*
 * Function which sets the position of an avatar.
 *
 * Input: Avatar, coordinates.
 *
 * Output: Changes avatar position.
 *
 */
void setPosition(avatar_t *avatar, int x, int y);

/*
 * Function which sets the direction of an avatar.
 *
 * Input: Avatar, direction.
 *
 * Output: Changes avatar direction.
 *
 */
void setDirection(avatar_t *avatar, int direction);

/*
 * Function which loads the startup struct.
 *
 * Input: All necessary information for the avatar to know so that it can beat the game.
 *
 * Output: Returns a startupInfo_t struct with all necessary knowledge initialized inside.
 *
 */
startupInfo_t* loadStartupStruct(pthread_mutex_t *lock, int avatarID, int nAvatars, int difficulty, char *hostname, int mazePort, char *logFile, avatar_t **avatars, int *lastTurnID, mazeTile_t ***maze, int *solved, int height, int width, WINDOW *window, FILE *log, int *moveCount);

/*
 * Function which frees memory allocated for a startupInfo_t struct.
 *
 * Input: startupInfo_t struct.
 *
 * Output: None.
 *
 */
void deleteStartupStruct(startupInfo_t *s);

/*
 * Function which handles all error messages.
 *
 * Input: Log file, type of response, move count, avatar ID, solved integer.
 *
 * Output: Prints to the log file a corresponding error message.
 *
 */
void runAvatarError(FILE *log, int resposeType, int moveCount, int avatarID, int *solved);

/*
 * Function which converts an integer representing a direction to a string of that direction.
 *
 * Input: Integer 0-3 representing W N S E respectively.
 *
 * Output: "west" "north" "south" or "east", respectively.
 *
 */
char* parseDirection(int direction);

#endif // __AVATAR_H
