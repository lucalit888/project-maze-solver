 /* 
 * graphicstest.c, a testing module that evaluates the functionality of functions in graphics.c and mazeSolver.c, and avatar.c
 *
 * Written by:
 * Luca Lit, Sean Simons, Connor Davis, and Mack Reiferson Winter 2020
 */

#include "graphics.h"
#include "mazeSolver.h"
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <ctype.h>
#include <unistd.h>
#include "avatar.h"

// Testing function
int main(int argc, char * argv[]){

	// simulating a 10 by 10 mazee
	int mazeheight = 10;
	int mazewidth = 10;
	int numAv = 4;

	// Initializing curses window
	WINDOW *mainwindow;

	// Safety check whether initscr() successfully initialised screen
	if ((mainwindow = initscr()) == NULL){
		// if it failed, print error message and exit with failure
		fprintf(stderr, "Error encountered initialising curses\n");
		exit(EXIT_FAILURE);
	}

	// Testing createMaze() with 4 avatars (each calling mazeTilenew())
	mazeTile_t ***tiles = createMaze(mazeheight, mazewidth);

	// create an array of 4 avatars (each calling avatarNew())
	avatar_t **avatararray = createAvatars(numAv);

	// resetting initialised coordinates of every avatar to be in different positions
	avatararray[0]->xCoord = 0;
	avatararray[1]->xCoord = 1;
	avatararray[2]->xCoord = 2;
	avatararray[3]->xCoord = 3;

	// testing addWall() function (drawMaze() should draw those walls in the right place)
	addWall(tiles, 5, 5, 1);
	addWall(tiles, 5, 5, 1);
	addWall(tiles, 5, 5, 2);	
	addWall(tiles, 5, 4, 3);
	addWall(tiles, 4, 5, 0);
	addWall(tiles, 6, 5, 2);
	addWall(tiles, 0, 6, 1);

	// call drawMaze(), which calls upon all helper functions in graphics.c (drawWall, drawAvatar, etc.)
	drawMaze(mazeheight, mazewidth, numAv, avatararray, tiles);

	// change the coordinates again 
	avatararray[2]->yCoord = 1;
	avatararray[3]->yCoord = 1;

	// draw again to see that the positions were updated on the window 
	drawMaze(mazeheight, mazewidth, numAv, avatararray, tiles);

	// change one more time
	avatararray[3]->yCoord = 0;

	// draw again to see updated positions 
	drawMaze(mazeheight, mazewidth, numAv, avatararray, tiles);
	sleep(5);
	refresh();

	printf("\rTest Results Successful\n\r");
	printf(" :) \n\r");

	// testing deleteAvatars() function which calls avatarDelete on every avatar
	deleteAvatars(avatararray, numAv);

	// deleting every mazetile / freeing all memory
	mazeDelete(mazeheight, mazewidth, tiles);

	// Delete the window and end the window 
	delwin(mainwindow);
	endwin();

	printf("\n");
	return 0;
}
