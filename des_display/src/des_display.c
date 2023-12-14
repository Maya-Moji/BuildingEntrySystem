/*
 * MAYA JAFFARY
 * des_display.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include <errno.h>

#include "../des-mva.h"

/*******************************************************************************
 * main( ) - Program starts from here
 * des_display is the server.
 * Display prints the current state of the controller's FSM to the console
 ******************************************************************************/
int main (int argc, char* argv[]) {

	// check number of command line arguments
	if (argc != 1) {
		printf("Usage: Incorrect number of arguments.\n");
		exit(EXIT_FAILURE);
	}

	// print display's process id
	printf("The display is running as PID: %d\n", getpid());
	printf("=================================\n");

	// create a channel
	int chid;
	chid = ChannelCreate(0);
	if (chid == -1) {
		perror("ChannelCreate failed.");
		exit(EXIT_FAILURE);
	} else {
		// printf("Channel created by des_display. Channel ID is %d.\n", chid);
	}

	//display object
	Display msgFromController;

	// Server remains ready in an endless loop to hear from controller
	while (1) {
		// get the message, and print it
		int rcvid = MsgReceive(chid, &msgFromController, sizeof(msgFromController), NULL); //returns receive ID. If an error occurs, -1 is returned and errno is set.
		if (rcvid == -1) {
			printf("\nMsgReceive failed.. \n");
			return EXIT_FAILURE;
		}

		switch(msgFromController.display_message) {
		case WAITING_MSG:
			printf("[Display] %s", outMessage[msgFromController.display_message]);
			break;
		case LEFT_SCAN_MSG:
			printf("[Display] %s Person's ID = %d\n", outMessage[msgFromController.display_message], msgFromController.display_person.person_id );
			break;
		case RIGHT_SCAN_MSG:
			printf("[Display] %s Person's ID = %d\n", outMessage[msgFromController.display_message], msgFromController.display_person.person_id );
			break;
		case WEIGHT_SCALE_MSG:
			printf("[Display] %s Person's Weight = %d\n", outMessage[msgFromController.display_message], msgFromController.display_person.person_weight );
			break;
		case EXIT_STATE_MSG:
			printf("[Display] %s\n", outMessage[msgFromController.display_message]);
			int rplid = MsgReply(rcvid, EOK, &msgFromController, sizeof(msgFromController)); //If successful, returns EOK. If an error occurs, it returns -1 and sets errno.
			if (rplid == -1) {
				printf("\nMsgRelply failed... \n");
				return EXIT_FAILURE;
			}
			printf("Exiting Display...\n");
			return EXIT_SUCCESS;
			break;
		default:
			printf("[Display] %s \n", outMessage[msgFromController.display_message]);
			break;
		}

		// reply to controller
		int rplid = MsgReply(rcvid, EOK, &msgFromController, sizeof(msgFromController)); //If successful, returns EOK. If an error occurs, it returns -1 and sets errno.
		if (rplid == -1) {
			printf("\nMsgRelply failed... \n");
			return EXIT_FAILURE;
		}

	}

	 // destroy the channel when done
	 ChannelDestroy(chid);

	 return EXIT_SUCCESS; //0
}
