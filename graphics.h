/* 
 * graphics.h - header file for graphics module
 *
 * This module includes many useful functions for visually displaying a maze.
 *
 * Written by:
 * Luca Lit, Sean Simons, Connor Davis, and Mack Reiferson Winter 2020
 */

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdio.h>
#include <stdbool.h>
#include "mazeSolver.h"
#include "avatar.h"

/**************** functions ****************/

/**************** draw_maze ****************/
/*
 * Function that draws the entire maze (excluding avatars), calling upon all the sublevel draw functions
 *
 * Input: Maze dimensions, number of avatars, list of avatars, maze.
 *
 * Output: a visualized maze via the curses library.
 *
 */
void drawMaze(int mazeHeight, int mazeWidth, int avatarNum, avatar_t **avatars, mazeTile_t ***maze);

/**************** drawMazeTile ****************/
/*
 * Function that draws an individual maze tile.
 *
 * Input: mazeTile struct representing a single coordinate on the maze.
 *
 * Output: a visualized mazeTile via the curses library.
 *
 */
void drawMazeTile(mazeTile_t *mazeTile);

/**************** draw_outer_borders ****************/
/*
 * Function that draws the outer borders of a maze.
 *
 * Input: Dimensions of a maze.
 *
 * Output: a visualized border wall, represented by '*'s, via the curses library.
 *
 */
void drawOuterBorders( int mazeHeight, int mazeWidth);

/**************** draw_wall ****************/
/*
 * Function that draws a wall at a given coordinate and orientation.
 *
 * Input: Coordinate pair and cardinal direction (N S E W) of the wall.
 *
 * Output: a visualized wall, represented by '-' if N or S or '|' if E or W, via the curses library.
 *
 */
void drawWall(int x, int y, char *direction);

/**************** draw_avatar ****************/
/*
 * Function that draws an avatar given its ID and coordinates.
 *
 * Input: ID, coordinates.
 *
 * Output: a visualized avatar, represented by its ID, via the curses library.
 *
 */
void drawAvatar(int avatarID, int x, int y);

/**************** convertY ****************/
/*
 * Helper function which converts a value to its corresponding "pixel" location.
 *
 * Input: Integer representing a 1D coordinate on the maze.
 *
 * Output: A scaled version of that integer representing the corresponding 1D coordinate to print at in the window.
 *
 */
int convertY(int num);

/**************** convertX ****************/
/*
 * Helper function which converts a value to its corresponding "pixel" location.
 *
 * Input: Integer representing a 1D coordinate on the maze.
 *
 * Output: A scaled version of that integer representing the corresponding 1D coordinate to print at in the window.
 *
 */
int convertX(int num);

#endif // __GRAPHICS_H
