/* 
 * mazeSolver.h - header file for mazeSolver module
 *
 * This module includes many useful functions for creating and solving a maze.
 * See function headers for in depth descriptions.
 *
 * Written by:
 * Luca Lit, Sean Simons, Connor Davis, and Mack Reiferson Winter 2020
 */

#ifndef __MAZESOLVER_H
#define __MAZESOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "amazing.h"
#include "avatar.h"

/**************** Structs ****************/

/**************** mazeTile ****************/
/*
 * Defines a mazeTile struct, which represents one coordinate pair on the maze
 */
typedef struct mazeTile {
	int xCoord;
	int yCoord;
	bool northWall;
	bool southWall;
	bool eastWall;
	bool westWall;
	int visited;	
} mazeTile_t;

/**************** Functions ****************/

/**************** mazeTileNew ****************/
/*
 * Function which initializes a new mazeTile struct.
 *
 * Input: Coordinate pair.
 *
 * Output: mazeTile struct representing that coordinate pair with walls intialized.
 *
 */
mazeTile_t *mazeTileNew(int xCoord, int yCoord);

/**************** createMaze ****************/
/*
 * Function which creates a 2D array of mazeTiles which represents the maze.
 *
 * Input: Maze dimensions, number of avatars playing.
 *
 * Output: A 2D array of mazeTiles representing the initial state of the maze.
 *
 */
mazeTile_t ***createMaze(int height, int width);

/**************** mazeDelete ****************/
/*
 * Function which frees all the memory space allocated in createMaze().
 *
 * Input: Maze dimensions, 2D array of mazeTiles representing a maze.
 *
 * Output: Frees memory, returns and prints nothing.
 *
 */
void mazeDelete(int height, int width, mazeTile_t ***tiles);

/*
 * Function which adds a wall into a maze array at a given mazeTile location.
 *
 * Intput: 2D array of mazeTiles representing a maze, coordinates of tile to add wall at, cardinal direction of the wall relative
 * to the center of the tile.
 *
 * Output: Adds a wall into the maze at the given location and orientation. Returns and prints nothing.
 *
 */
void addWall(mazeTile_t ***maze, int x, int y, int direction);

#endif // __MAZESOLVER_H

