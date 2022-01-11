/* 
 * AMStartup 
 *
 * Connects to the host and creates a thread for each avatar in the game.
 * Then it runs the game.
 *
 * Usage: ./AMStartup -h hostname -d difficulty -n number of avatars
 *
 * Example: ./AMStartup -h flume.cs.dartmouth.edu -d 5 -n 4 
 * 
 * Connor Davis, Sean Simons, Luca Lit, and Mack Reiferson, Winter 2020.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <ctype.h> 
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
#include <getopt.h>	      // allows flag parsing
#include <pthread.h>
#include <time.h>
#include "amazing.h"
#include "avatar.h"
#include "mazeSolver.h"
#include "graphics.h"

/**************** file-local constants ****************/
#define BUFSIZE 1024     // read/write buffer size

/**************** main() ****************/
int main(const int argc, char *argv[]) {

	// Initialize necessary variables.
	char *program;	  // this program's name
	char *hostName;	  // server hostname
	int port = 17235;      // server port
	int difficulty;	  // maze difficulty
	int avatarNum;	  // number of avatars

	// Check & parse arguments
	program = argv[0];
	if (argc != 7) {
		// Invalid number of arguments.
		fprintf(stderr, "usage: %s -h hostname -d difficulty -n numAvatars\n", program);
		exit (1);
	} 
	else {
		// Handle flag parsing.
		int opt;
		while ((opt = getopt(argc, argv, "h:d:n:")) != -1)
			switch (opt) {
				// Handle setting the difficulty.
				case 'd':
					difficulty = atoi(optarg);
					if (difficulty > 9 || difficulty < 0) {
						fprintf(stderr, "Error, difficulty must be between 1 and 9\n");
						exit(2);
					}
					break;
				// Handle setting the number of avatars.
				case 'n':
					avatarNum = atoi(optarg);
					if (avatarNum > 10 || avatarNum < 1) {
						fprintf(stderr, "Error, number of avatars must be between 1 and 4\n");
						exit(3);
					}
					break;
				// Handle setting the hostname.
				case 'h':
					hostName = optarg;
					break;
				// Catch all other cases.
				default:
					abort();
			}	
	}

	// Create socket and perform safety check.
	int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (comm_sock < 0) {
		perror("opening socket");
		exit(4);
	}

	// Initialize the fields of the server address
	struct sockaddr_in server;  				// address of the server
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	// Look up the hostname specified on command line and perform safety check.
	struct hostent *hostp = gethostbyname(hostName); 	// server hostname
	if (hostp == NULL) {
		fprintf(stderr, "%s: unknown host '%s'\n", program, hostName);
		exit(5);
	}

	// Copy memory block.
	memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

	// Connect the socket to the server.   
	if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(6);
	}
	printf("Connected!\n");

	// Configure AM_INIT message.
	AM_Message message;
	message.type = htonl(AM_INIT);
	uint32_t diff = difficulty;
	uint32_t nAvatars = avatarNum;
	message.init.Difficulty = htonl(diff);
	message.init.nAvatars = htonl(nAvatars);

	// Send AM_INIT message.
	send(comm_sock, (void *) &message, sizeof(message), 0);
	printf("Initialization message sent.\n");

	// Store response values.
	AM_Message response;
	int bytesReceived = recv(comm_sock, (void *) &response, sizeof(AM_Message), 0);
	
	// Check for empty message, otherwise continue.
	if (bytesReceived < 0) {
		// Handle empty message.
		fprintf(stderr, "ERROR: No message recieved.\n");
		exit(7);
	} else {
		if (IS_AM_ERROR(response.type)) {
			// Handle failed initialization.
			fprintf(stderr, "ERROR: initialization failed.\n");
			exit(8);	
		} else {
			// Success, time to play the game.
			
			// Initialize avatars array.
			avatar_t **avatars;

			// Check to see type of message
			if (response.type == ntohl(AM_INIT_OK)) {
				// Create log file for all threads share
				mazeTile_t ***mazeArray = createMaze(ntohl(response.init_ok.MazeHeight), ntohl(response.init_ok.MazeWidth));
				
				// Print useful information to stdout.
				printf("\nMaze Height:	%d\n", ntohl(response.init_ok.MazeHeight));
				printf("Maze Width: 	%d\n", ntohl(response.init_ok.MazeWidth));
				printf("Maze Port: 	%d\n", ntohl(response.init_ok.MazePort));

				// Set height and width variables.
				int h = ntohl(response.init_ok.MazeHeight);
				int w = ntohl(response.init_ok.MazeWidth);
				
				// Create the logfile name.
				char *logName = malloc(sizeof(char)*100);
				strcpy(logName, "log.out/Amazing_");
				char avatarNumString[2];
				char difficultyString[2];
				strcat(logName, getenv("USER"));
				strcat(logName, "_");
				sprintf(avatarNumString, "%d", avatarNum);
				strcat(logName, avatarNumString);
				strcat(logName, "_");
				sprintf(difficultyString, "%d", difficulty);
				strcat(logName, difficultyString);
				printf("Logfile:	%s\n\n", logName);

				// Open log file.
				FILE *fp = fopen(logName, "w");

				// Initialize thread checker.
				int threadChecker;

				// Initialize window for curses to draw into.
				WINDOW *mainwindow;

				// Safety check.
				if ((mainwindow = initscr())== NULL){
					printf("failed to initialise screen\n");
					exit(EXIT_FAILURE);
				}

				start_color();

				// Check file creation.
				if (fp != NULL) {
					// If file is created, continue.
					
					// Initialize time variable and write first line of log file.
					time_t currentTime = time(NULL);
					struct tm *tm = localtime(&currentTime);
					fprintf(fp, "%s, %d, %s\n", getenv("USER"), ntohl(response.init_ok.MazePort), asctime(tm));

					// Initialize array of threads and avatar index.
					pthread_t threads[avatarNum];
					int avatarIdx;

					// Initialize mutex locks and perform safety checks.
					pthread_mutex_t lock;
					if (pthread_mutex_init(&lock, NULL) != 0) {
						printf("\n mutext init failed\n");
						free(logName);
						exit (9);
					}

					// Create avatar array for all threads to reference.
					avatars = createAvatars(avatarNum);

					// Initialize remaining game state variables.
					int lastTurnID = -1;
					int solved = 0;
					int moveCount = 0;

					// Iterate number of avatar times.
					for (avatarIdx = 0; avatarIdx < avatarNum; avatarIdx++) {
						//Initialize a startup struct.	
						startupInfo_t *initStruct = loadStartupStruct(&lock, avatarIdx, avatarNum, difficulty, 
								hostName, ntohl(response.init_ok.MazePort), logName, avatars, &lastTurnID, 
								mazeArray, &solved, ntohl(response.init_ok.MazeHeight), ntohl(response.init_ok.MazeWidth), mainwindow, fp, &moveCount);
						
						// Create the thread and perform safety check.
						threadChecker = pthread_create(&threads[avatarIdx], NULL, runAvatar, (void *)initStruct);
						if (threadChecker) {
							fprintf(stderr, "Error when creating thread for avatar number %d\n", avatarIdx);
							free(logName);
							exit(11);
						}
					}

					// Clean up threads in memory.
					for (int i = 0; i < avatarNum; i++) {
						pthread_join(threads[i], NULL);
					}

					// Close log file.
					fclose(fp);
				} else {
					// Handle a failed log file creation.
					fprintf(stderr, "Error when creating log %s\n", logName);
					free(logName);
					exit(12);
				}

				// Clean up with respect to memory.
				deleteAvatars(avatars, avatarNum);
				mazeDelete(h, w, mazeArray);
				free(logName);
			} else {
				// Handle unexpected message.
				printf("Unexpected message received.\n");
			}
		}
	}
	
	// Close and exit, return 0.
	close(comm_sock);
	printf("Exiting AMStartup\n");
	pthread_exit(NULL);
	return 0;
}
