#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           // read, write, close
#include <string.h>           // memcpy, memset
#include <netdb.h>            // socket-related structures
#include <pthread.h>
#include <stdbool.h>
#include <curses.h>
#include "avatar.h"
#include "mazeSolver.h"


int main(int argc, char * argv[]) {
	// Test avatar struct initialization
	int testID = 0;
	avatar_t *testAvatar = avatarNew(testID);
	if (testAvatar != NULL) {
		printf("Avatar %d initialized at (%d, %d)", testAvatar->avatarID, testAvatar->xCoord, testAvatar->yCoord);
	}
	
	// Test avatar struct deletion
	avatarDelete(testAvatar);
	printf("Testing deletion of Avatar %d, see myvalgrind for no memory leaks", testID);

	// Test avatar array creation with multiple avatars
	int avatarNum = 3;
	avatar_t **multipleAvatars = createAvatars(avatarNum);
	printf("Tried to create %d avatars in an array\n", avatarNum);
	int avatarTotal = 0;
	for (int i = 0; i < avatarNum; i++) {
		if (multipleAvatars[i] != NULL) {
			printf("Created Avatar %d\n", multipleAvatars[i]->avatarID);
			avatarTotal++;
		}
	}
	printf("Created %d avatars overall\n", avatarTotal);
	
	// Test deletion of an avatar array
	printf("Deleting avatar array, see valgrind for no memory leaks\n");
	deleteAvatars(multipleAvatars, avatarNum);
	
	// Test set position functions for avatar
	testAvatar = avatarNew(testID);
	int testX = 1;
	int testY = 1;
	printf("Chaninging Avatar %d poisition from (%d, %d) to (%d, %d)\n", testAvatar->avatarID, testAvatar->xCoord, testAvatar->yCoord, testX, testY); 
	setPosition(testAvatar, testX, testY);
	printf("Position of Avatar %d now (%d, %d)\n", testAvatar->avatarID, testAvatar->xCoord, testAvatar->yCoord);

	// Test set direction function for avatarr

	int testDir= 10;
	printf("Chaning Avatar %d direction from %d to %d\n)", testAvatar->avatarID, testAvatar->direction, testDir); 
	setDirection(testAvatar, testDir);
	printf("Direction of Avatar %d now %d)\n", testAvatar->avatarID, testAvatar->direction);

	// Test mazeTile struct initialization
	printf("Initializing tile with position (%d, %d)\n", testX, testY);
	mazeTile_t *testTile = mazeTileNew(testX, testY);
	printf("Tile created with position of (%d, %d)\n", testTile->xCoord, testTile->yCoord);
	
	// Test mazeTile struct deletion
	printf("Deleting tile, see myvalgrind for no memory leaks\n");
	free(testTile);

	// Test TWO-D mazeTile array creation
	int xSize = 2;
	int ySize = 3;
	printf("Creating 2-D array of height %d and width %d\n", xSize, ySize);
	mazeTile_t ***testMaze = createMaze(ySize, xSize);
	if (testMaze != NULL) {
		for (int i = 0; i < ySize; i++) {
			if (testMaze[i] != NULL) {
				for (int j = 0; j < xSize; j++) {
					if (testMaze[i][j] != NULL) {
						printf("Tile at (%d, %d) created\n", i, j);
					}
				}
			}
		}
	}
	
	// Test TWO-D mazeTile deletion
	mazeDelete(ySize, xSize, testMaze);	
	printf("Deleting test maze, see myvalgrind for no memory leaks\n");
	
	// Test adding wall to function to mazeTile
	testMaze = createMaze(ySize, xSize);
	printf("Maze tile created such that ");
	if (!testMaze[0][0]->eastWall) {
		printf("east wall does not exist\n");	
	}
	int east = 3;
	addWall(testMaze, 0, 0, east);
	if (testMaze[0][0]->eastWall) {
		printf("Now maze tile has east wall\n");
	}
	mazeDelete(ySize, xSize, testMaze);

	// TEST STARTUPSTRUCTS
	testMaze = createMaze(ySize, xSize);
	avatarNum = 5;
	pthread_mutex_t lock, lock2;
	pthread_mutex_init(&lock, NULL);
       	pthread_mutex_init(&lock2, NULL);	
	int difficulty = 5;
	char *hostname = "flume.cs.dartmouth.edu";
	int mazePort = 1234;
	char *logFile = "log.out/.test";
	multipleAvatars = createAvatars(avatarNum);
	int lastTurnID = 0;
	int solved = 1;
	int height = 2;
	int width = 3;
	WINDOW *window = initscr();
	FILE *log = fopen(logFile, "r");
	int moveCount = 0;
	startupInfo_t *initStruct = loadStartupStruct(&lock, testID, avatarNum, difficulty, hostname, mazePort, logFile, multipleAvatars, &lastTurnID, testMaze, &solved, height, width, window, log, &moveCount, &lock2);
      	if (initStruct != NULL) {
		printf("Startup struct initialized\n");
	}
	delwin(window);
	endwin();
	
	// Test startup struct deletion
	deleteStartupStruct(initStruct);
	printf("Deleting startup struct, see myvalgrind for no memory leaks\n");

	// Test parse direction rule
	printf("%d = %s, %d = %s, %d = %s, and %d = %s\n", 0, parseDirection(0), 1, parseDirection(1), 2, parseDirection(2), 3, parseDirection(3));

	// Test left hand turn rule
	setDirection(multipleAvatars[0], 0);
	printf("%d\n", multipleAvatars[0]->direction);
	setPosition(multipleAvatars[0], 1, 1);
	setDirection(multipleAvatars[1], 1);
	setPosition(multipleAvatars[1], 2, 2);
	setDirection(multipleAvatars[2], 2);
	setPosition(multipleAvatars[2], 3, 3);
	setDirection(multipleAvatars[3], 3);
	setPosition(multipleAvatars[3], 4, 4);
	setPosition(multipleAvatars[4], 5, 5);
	mazeDelete(ySize, xSize, testMaze);
	ySize = 6;
        xSize = 6;
	testMaze = createMaze(ySize, xSize);

	// Rule with no walls
	printf("Initially no walls in any tile\n");
	printf("Given direction %s", parseDirection(multipleAvatars[0]->direction));
        leftHandRule(multipleAvatars[0], testMaze[1][1], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[0]->direction));
	printf("Given direction %s", parseDirection(multipleAvatars[1]->direction));
        leftHandRule(multipleAvatars[1], testMaze[2][2], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[1]->direction));
	printf("Given direction %s", parseDirection(multipleAvatars[2]->direction));
        leftHandRule(multipleAvatars[2], testMaze[3][3], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[2]->direction));

	printf("Given direction %s", parseDirection(multipleAvatars[3]->direction));
        leftHandRule(multipleAvatars[3], testMaze[4][4], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[3]->direction));
	setDirection(multipleAvatars[0], 0);
        printf("%d\n", multipleAvatars[0]->direction);
        setPosition(multipleAvatars[0], 1, 1);
        setDirection(multipleAvatars[1], 1);
        setPosition(multipleAvatars[1], 2, 2);
        setDirection(multipleAvatars[2], 2);
        setPosition(multipleAvatars[2], 3, 3);
        setDirection(multipleAvatars[3], 3);
        setPosition(multipleAvatars[3], 4, 4);
        setPosition(multipleAvatars[4], 5, 5);
	addWall(testMaze, 1, 1, 0);
	addWall(testMaze, 2, 2, 0);
	addWall(testMaze, 3, 3, 0);
	addWall(testMaze, 4, 4, 0);

	// Wall to the west
	printf("Added walls in direction %s\n", parseDirection(0));
        printf("Given direction %s", parseDirection(multipleAvatars[0]->direction));
        leftHandRule(multipleAvatars[0], testMaze[1][1], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[0]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[1]->direction));
        leftHandRule(multipleAvatars[1], testMaze[2][2], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[1]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[2]->direction));
        leftHandRule(multipleAvatars[2], testMaze[3][3], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[2]->direction));

        printf("Given direction %s", parseDirection(multipleAvatars[3]->direction));
        leftHandRule(multipleAvatars[3], testMaze[4][4], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[3]->direction));
	setDirection(multipleAvatars[0], 0);
        printf("%d\n", multipleAvatars[0]->direction);
        setPosition(multipleAvatars[0], 1, 1);
        setDirection(multipleAvatars[1], 1);
        setPosition(multipleAvatars[1], 2, 2);
        setDirection(multipleAvatars[2], 2);
        setPosition(multipleAvatars[2], 3, 3);
        setDirection(multipleAvatars[3], 3);
        setPosition(multipleAvatars[3], 4, 4);
        setPosition(multipleAvatars[4], 5, 5);
        addWall(testMaze, 1, 1, 1);
        addWall(testMaze, 2, 2, 1);
        addWall(testMaze, 3, 3, 1);
        addWall(testMaze, 4, 4, 1);

	// Wall to the north
	printf("Added walls in direction %s\n", parseDirection(1));
        printf("Given direction %s", parseDirection(multipleAvatars[0]->direction));
        leftHandRule(multipleAvatars[0], testMaze[1][1], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[0]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[1]->direction));
        leftHandRule(multipleAvatars[1], testMaze[2][2], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[1]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[2]->direction));
        leftHandRule(multipleAvatars[2], testMaze[3][3], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[2]->direction));

        printf("Given direction %s", parseDirection(multipleAvatars[3]->direction));
        leftHandRule(multipleAvatars[3], testMaze[4][4], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[3]->direction));
	setDirection(multipleAvatars[0], 0);
        printf("%d\n", multipleAvatars[0]->direction);
        setPosition(multipleAvatars[0], 1, 1);
        setDirection(multipleAvatars[1], 1);
        setPosition(multipleAvatars[1], 2, 2);
        setDirection(multipleAvatars[2], 2);
        setPosition(multipleAvatars[2], 3, 3);
        setDirection(multipleAvatars[3], 3);
        setPosition(multipleAvatars[3], 4, 4);
        setPosition(multipleAvatars[4], 5, 5);
        addWall(testMaze, 1, 1, 2);
        addWall(testMaze, 2, 2, 2);
        addWall(testMaze, 3, 3, 2);
        addWall(testMaze, 4, 4, 2);

	// Wall to the south
	printf("Added walls in direction %s\n", parseDirection(2));
        printf("Given direction %s", parseDirection(multipleAvatars[0]->direction));
        leftHandRule(multipleAvatars[0], testMaze[1][1], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[0]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[1]->direction));
        leftHandRule(multipleAvatars[1], testMaze[2][2], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[1]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[2]->direction));
        leftHandRule(multipleAvatars[2], testMaze[3][3], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[2]->direction));

        printf("Given direction %s", parseDirection(multipleAvatars[3]->direction));
        leftHandRule(multipleAvatars[3], testMaze[4][4], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[3]->direction));
	setDirection(multipleAvatars[0], 0);
        printf("%d\n", multipleAvatars[0]->direction);
        setPosition(multipleAvatars[0], 1, 1);
        setDirection(multipleAvatars[1], 1);
        setPosition(multipleAvatars[1], 2, 2);
        setDirection(multipleAvatars[2], 2);
        setPosition(multipleAvatars[2], 3, 3);
        setDirection(multipleAvatars[3], 3);
        setPosition(multipleAvatars[3], 4, 4);
        setPosition(multipleAvatars[4], 5, 5);
        addWall(testMaze, 1, 1, 3);
        addWall(testMaze, 2, 2, 3);
        addWall(testMaze, 3, 3, 3);
        addWall(testMaze, 4, 4, 3);

	// Wall to the east
	printf("Added walls in direction %s\n", parseDirection(3));
        printf("Given direction %s", parseDirection(multipleAvatars[0]->direction));
        leftHandRule(multipleAvatars[0], testMaze[1][1], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[0]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[1]->direction));
        leftHandRule(multipleAvatars[1], testMaze[2][2], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[1]->direction));
        printf("Given direction %s", parseDirection(multipleAvatars[2]->direction));
        leftHandRule(multipleAvatars[2], testMaze[3][3], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[2]->direction));

        printf("Given direction %s", parseDirection(multipleAvatars[3]->direction));
        leftHandRule(multipleAvatars[3], testMaze[4][4], &lastTurnID, avatarNum, multipleAvatars);
        printf(", the rule returns %s\n", parseDirection(multipleAvatars[3]->direction));
}
	

