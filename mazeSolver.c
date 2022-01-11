/* 
 * mazeSolver.c, a module containing many useful functions for creating and deleting a 2D array of mazetiles. 
 *
 * Written by:
 * Luca Lit, Sean Simons, Connor Davis, and Mack Reiferson Winter 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>           // memcpy, memset
#include <stdbool.h>
#include "amazing.h"
#include "avatar.h"
#include "mazeSolver.h"

// Function which takes in an xcoordinate, y coordinate and initilaises a new mazeTile_t struct
mazeTile_t *mazeTileNew(int xCoord, int yCoord){

	// allocate memory space for mazeTile creation 
	mazeTile_t *mazeTile = malloc(sizeof(mazeTile_t));

	// check that the memory space was allocated 
	if (mazeTile != NULL){

		// set pointers to desired initialised values 
		mazeTile->xCoord = xCoord;
		mazeTile->yCoord = yCoord; 
		mazeTile->northWall = false;
		mazeTile->southWall = false;
		mazeTile->eastWall = false;
		mazeTile->westWall = false;
		mazeTile->visited = -1;

		// return the mazetile
		return mazeTile; 
	} 

	// if there was a failure in allocating memory space
	else {
		fprintf(stderr, "Failed to malloc for mazeTile\n");
		return NULL;
	}
}

// Function that adds wall to the 2D MazeTile Array after an avatar runs into a wall
void addWall(mazeTile_t ***maze, int x, int y, int direction) {

	// if the direction that the avatar failed to move in was north, set north wall true
	if (direction == M_NORTH) {
		maze[y][x]->northWall = true;
		maze[y-1][x]->southWall = true;
	}

	// if the direction that the avatar failed to move in was south, set north wall true
	else if (direction == M_SOUTH) {
		maze[y][x]->southWall = true;
		maze[y+1][x]->northWall = true;
	}

	// if the direction that the avatar failed to move in was east, set east wall true
	else if (direction == M_EAST) {
		maze[y][x]->eastWall = true;
		maze[y][x+1]->westWall = true;
	}

	// if the direction that the avatar failed to move in was west, set west wall true
	else if (direction == M_WEST) {
		maze[y][x]->westWall = true;
		maze[y][x-1]->eastWall = true;
	}
}


// function that creates the 2D array representing the maze 
mazeTile_t ***createMaze(int height, int width) {

	// allocate space for the column of mazeTiles of a user-inputted height 
	mazeTile_t ***tiles = (mazeTile_t ***)malloc(height * sizeof(mazeTile_t **));

	// safety check
	if (tiles == NULL) {
		fprintf(stderr, "Failed to malloc for tiles\n");
		return NULL;
	}

	// for whatever height the maze is
	for (int i = 0; i < height; i++) {

		// allocate space for the row of mazeTiles of a user-inputted width 
		tiles[i] = (mazeTile_t **)malloc(width * sizeof(mazeTile_t *));

		// safety check for memory allocation
		if (tiles[i] == NULL) {
			fprintf(stderr, "Failed to malloc for tiles[i]\n");
			return NULL;
		}

		// loop through width number of times 
		for (int j = 0; j < width; j++) {

			// initialise a new mazeTile struct
			tiles[i][j] = mazeTileNew(j, i);

			// for the entire first column, set the northwall to true
			if (i == 0) {
				tiles[i][j]->northWall = true;
			}	

			// for the entire last column, set the southwall to true
			if (i == height-1) {
				tiles[i][j]->southWall = true;
			}

			// for the entire first row, set westwall to true
			if (j == 0) {
				tiles[i][j]->westWall = true;
			}

			// for the entire last row, set eastwall to true
			if (j == width-1) {
				tiles[i][j]->eastWall = true;
			}
		}
	}
	return tiles;
}


// function that frees all the memory space allocated in createMaze()
void mazeDelete(int height, int width, mazeTile_t ***tiles){
	// loop through every column of the 2D array
	for (int i = 0; i < height; i++) {
		// loop through every row of the 2D array
		for (int j = 0; j < width; j++) {
			// free memory of every tile in the row
			free(tiles[i][j]);
		}
		// free every column
		free(tiles[i]);
	}
	// free the large 2D array tile struct
	free(tiles);
}

