/*
 * Amazing Project - graphics.c 
 * Printing the ASCII representation of the maze
 *
 * Team members: Connor, Luca, Sean and Mack  
 */

#include "avatar.h"
#include "graphics.h"
#include "mazeSolver.h"
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// function that draws the entire maze, calling upon all the sublevel draw functions
void drawMaze(int mazeHeight, int mazeWidth, int avatarNum, avatar_t **avatars, mazeTile_t ***maze){

	// creating colored pairs
	// outer boundaries are cyan
	init_pair(1,COLOR_CYAN,COLOR_BLACK);
	// avatar colors are white
	init_pair(2,COLOR_WHITE,COLOR_RED);
	// walls are yellow
	init_pair(3,COLOR_YELLOW,COLOR_BLACK);
	// iterate over the maze 
	for (int i = 0; i <  mazeHeight; i++){
		for (int j = 0; j < mazeWidth; j++){
			// draw the tile one at a time, in corresponding color pair
			attron(COLOR_PAIR(3));
			drawMazeTile(maze[i][j]);
			attroff(COLOR_PAIR(3));
		}
	}

	// for every single avatar in the avatar array
	for (int k = 0; k < avatarNum; k++) {
		// draw the avatar at their corresponding positions, in corresponding color pair
		attron(COLOR_PAIR(2));
		drawAvatar(avatars[k]->avatarID, avatars[k]->xCoord, avatars[k]->yCoord);
		attroff(COLOR_PAIR(2));
	}
	// draw the outer borders, in corresponding color pair
	attron(COLOR_PAIR(1));
	drawOuterBorders(mazeHeight, mazeWidth);
	attroff(COLOR_PAIR(1));

	// calling refresh to draw over last frame (thus creating the animation)
	refresh();
	// nap for 10 milliseconds
	napms(15);

	// for every avatar, clear the previous spot it was on
	for (int k = 0; k < avatarNum; k++) {
		mvaddstr(convertY(avatars[k]->yCoord), convertX(avatars[k]->xCoord), " ");	
	}
	refresh();

}


/*
 * This function draws the maze, taking in a mazeTile struct as a parameter
 * It will call upon the other helper functions which are included in this file. 
 */

void drawMazeTile(mazeTile_t *mazeTile) {

	// if the north wall of the maze tile struct is true, draw a wall in the corresponding direction
	if (mazeTile->northWall) {
		drawWall(mazeTile->xCoord, mazeTile->yCoord, "north");
	}

	// if the south wall of the maze tile struct is true, draw a wall in the corresponding direction
	if (mazeTile->southWall) {
		drawWall(mazeTile->xCoord, mazeTile->yCoord, "south");
	}

	// if the east wall of the maze tile struct is true, draw a wall in the corresponding direction
	if (mazeTile->eastWall) {
		drawWall(mazeTile->xCoord, mazeTile->yCoord, "east");
	}

	// if the west wall of the maze tile struct is true, draw a wall in the corresponding direction
	if (mazeTile->westWall) {
		drawWall(mazeTile->xCoord, mazeTile->yCoord, "west");
	}
}

/*
 * This function prints the boundaries of a maze given its dimensions.
 */
void drawOuterBorders(int mazeHeight, int mazeWidth){
	// draw the top border
	for (int i = 3; i <= mazeWidth * 4; i += 4) {
		mvaddch(0, i, '*');
	}

	// draw the bottom border
	for (int i = 3; i <= mazeWidth * 4; i += 4) {
		mvaddch(mazeHeight * 2, i, '*');
	}

	// draw the left border 
	for (int j = 1; j <= mazeHeight * 2; j += 2) {
		mvaddch(j, 1, '*');
	}

	// draw the right border 
	for (int j = 1; j <= mazeHeight * 2; j += 2) {
		mvaddch(j, mazeWidth * 4 + 1, '*');
	}
}

/*
 * This function prints a wall given its location and whether or not it is a vertical wall.
 */
void drawWall(int x, int y, char *direction) {
	// Check if wall is vertical or horizontal.
	if (strcmp("north", direction) == 0) {
		// Wall is above and horizontal, print underscore.
		mvaddch(convertY(y) - 1, convertX(x), '-');
	} else if (strcmp("east", direction) == 0) {
		// Wall is to the right and vertical, print pipe.
		mvaddch(convertY(y), convertX(x) + 2, '|');

	} else if (strcmp("south", direction) == 0) {
		// Wall is below and horizontal, print underscore.
		mvaddch(convertY(y) + 1, convertX(x), '-');

	} else if (strcmp("west", direction) == 0) {
		// Wall is to the left and vertical, print pipe.
		mvaddch(convertY(y), convertX(x) - 2, '|');

	}
}

/*
 * This function prints an avatar, represented by its avatarID, in a given location.
 */
void drawAvatar(int avatarID, int x, int y) {
	// prints the avatar 
	char * ID;

	// convert the corresponding avatarID integers to a string, which will be printed by drawMaze
	if (avatarID == 0) {
		ID = "0";
	}
	if (avatarID == 1) {
		ID = "1";
	}
	if (avatarID == 2) {
		ID = "2";
	}
	if (avatarID == 3) {
		ID = "3";
	}
	if (avatarID == 4) {
		ID = "4";
	}
	if (avatarID == 5) {
		ID = "5";
	}
	if (avatarID == 6) {
		ID = "6";
	}
	if (avatarID == 7) {
		ID = "7";
	}
	if (avatarID == 8) {
		ID = "8";
	}
	if (avatarID == 9) {
		ID = "9";
	}
	// draw the avatar's position with its ID as its object image 
	mvaddstr(convertY(y), convertX(x), ID); 
}

/*
 * Helper function which converts a value to its corresponding "pixel" location.
 */
int convertY(int num) {
	return (num * 2) + 1;
}

/*
 * Helper function which converts a value to its corresponding "pixel" location.
 */
int convertX(int num) {
	return (num * 4) + 3;
}
