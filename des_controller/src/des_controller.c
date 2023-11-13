/*
 * MAYA JAFFARY - SHAUNESSEE GREEN
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
#include "../../des_display/des-mva.h"

void* exitState();
void* leftScan();
void* rightScan();
void* weightState();
void* leftOpen();
void* rightOpen();
void* leftClose();
void* rightClose();
void* rightUnlock();
void* leftUnlock();
void* rightLock();
void* leftLock();
void* waiting();

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
int coidToDisplay = 0; // Used to connect to channel created by Display
int replyFromDisplay = 0; //reply from display
char sendMsgToDisplay[256] = ""; //message to display

int rcvid = 0; //receive id for message from inputs
int rplid = 0; //reply id for messages to inputs
int replyMsgToInputs = 0; //reply to inputs

typedef void* (*StateFunc)();
StateFunc statefunc = waiting;
Person personObj;
Display displayObj;

/*******************************************************************************
 * main( ) - Program starts from here
 * Process the Person message received from des_inputs.
 * Advance to next accepting state (or error state) given the input of the Person object.
 * Controller process acting as client and display process acting as a server
 ******************************************************************************/
int main(int argc, char *argv[]) {

	// check number of command line arguments
	if (argc != 2) {
		printf("Usage: Incorrect number of arguments.\n");
		exit(EXIT_FAILURE);
	}

	// print controller's process id
	printf("The controller is running as PID: %d\n", getpid());
	printf("=================================\n");

	pid_t displayPID = atoi(argv[1]); //string to int - no error checking required

	// create a channel
	int chid;
	chid = ChannelCreate(0);
	if (chid == -1) {
		perror("\nFailed to create the channel. Exiting. ");
		exit(EXIT_FAILURE);
	}

	// Connect to channel created by Display
	coidToDisplay = ConnectAttach(0, displayPID, 1, 0, 0);
	if (coidToDisplay == -1) {
		printf("\nCannot Connect to Display. Exiting ");
		return EXIT_FAILURE;
	}

	// Server remains ready in an endless loop to hear from inputs
	while (1) {

		//actionState = -1;
		// get the message, and print it
		rcvid = MsgReceive(chid, &personObj, sizeof(personObj), NULL);
		if (rcvid == -1) {
			printf("\nError receiving message from Inputs. Exiting. \n");
			return EXIT_FAILURE;
		}

		//set the function pointers
		switch (personObj.person_state) {
		case LEFT_SCAN_STATE:
			statefunc = leftScan;
			break;
		case RIGHT_SCAN_STATE:
			statefunc = rightScan;
			break;
		case WEIGHT_SCALE_STATE:
			statefunc = weightState;
			break;
		case LEFT_OPEN_STATE:
			statefunc = leftOpen;
			break;
		case RIGHT_OPEN_STATE:
			statefunc = rightOpen;
			break;
		case LEFT_CLOSED_STATE:
			statefunc = leftClose;
			break;
		case RIGHT_CLOSED_STATE:
			statefunc = rightClose;
			break;
		case GUARD_RIGHT_UNLOCK_STATE:
			statefunc = rightUnlock;
			break;
		case GUARD_RIGHT_LOCK_STATE:
			statefunc = rightLock;
			break;
		case GUARD_LEFT_LOCK_STATE:
			statefunc = leftLock;
			break;
		case GUARD_LEFT_UNLOCK_STATE:
			statefunc = leftUnlock;
			break;
		case WAITING_STATE:
			statefunc = waiting;
			break;
		case EXIT_STATE:
			statefunc = exitState;
			break;

		}

		statefunc = (StateFunc) (*statefunc)();

		// Reply to inputs
		rplid = MsgReply(rcvid, 1, &replyMsgToInputs, sizeof(replyMsgToInputs));
		if (rplid == -1) {
			printf("\nMsgRelply failed... (in controller) \n");
			return EXIT_FAILURE;
		}
	}

	// destroy the channel when done
	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}

/*******************************************************************************
 * Function Pointers
 ******************************************************************************/
void* waiting() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != LEFT_SCAN_STATE) {
		statefunc = leftScan;
	} else {
		displayObj.display_message = WAITING_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		if (personObj.person_direction == 1) {
			statefunc = leftScan;
		} else if (personObj.person_direction == 2) {
			statefunc = rightScan;
		}
	}
	return statefunc;
}
void* exitState() {
	strcpy(sendMsgToDisplay, outMessage[EXIT_STATE_MSG]);
	displayObj.display_message = EXIT_STATE_MSG;
	displayObj.display_person.person_id = personObj.person_id;
	int myMsgSend = MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
			&replyFromDisplay, sizeof(replyFromDisplay));
	if (myMsgSend == -1) {
		fprintf(stderr, "\nMsgSend failed (in controller) \n");
		perror(NULL);
	}
	// Reply to inputs
	rplid = MsgReply(rcvid, 1, &replyMsgToInputs, sizeof(replyMsgToInputs));
	if (rplid == -1) {
		printf("\nMsgRelply failed... (in controller) \n");
	}
	printf("Exiting Controller...\n");
	exit(EXIT_FAILURE);
}
void* leftScan() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != LEFT_SCAN_STATE) {
		statefunc = leftScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[LEFT_SCAN_MSG]);
		displayObj.display_message = LEFT_SCAN_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		statefunc = leftUnlock;
	}
	return statefunc;
}
void* rightScan() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != RIGHT_SCAN_STATE) {
		statefunc = rightScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[RIGHT_SCAN_MSG]);
		displayObj.display_message = RIGHT_SCAN_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		statefunc = rightUnlock;
	}
	return statefunc;
}
void* weightState() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != WEIGHT_SCALE_STATE) {
		statefunc = weightState;
	} else {
		strcpy(sendMsgToDisplay, outMessage[WEIGHT_SCALE_MSG]);
		displayObj.display_message = WEIGHT_SCALE_MSG;
		displayObj.display_person.person_weight = personObj.person_weight;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		if (personObj.person_direction == 1) {
			statefunc = leftClose;
		} else if (personObj.person_direction == 2) {
			statefunc = rightClose;
		}
	}
	return statefunc;
}
void* leftOpen() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != LEFT_OPEN_STATE) {
		statefunc = leftScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[LEFT_OPEN_MSG]);
		displayObj.display_message = LEFT_OPEN_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		if (personObj.person_direction == 1) {
			statefunc = weightState;
		} else if (personObj.person_direction == 2) {
			statefunc = leftClose;
		}
	}
	return statefunc;
}
void* rightOpen() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != RIGHT_OPEN_STATE) {
		statefunc = rightScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[RIGHT_OPEN_MSG]);
		displayObj.display_message = RIGHT_OPEN_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		if (personObj.person_direction == 1) {
			statefunc = rightClose;
		} else if (personObj.person_direction == 2) {
			statefunc = weightState;
		}
	}
	return statefunc;
}
void* leftClose() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != LEFT_CLOSED_STATE) {
		statefunc = leftScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[LEFT_CLOSED_MSG]);
		displayObj.display_message = LEFT_CLOSED_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		statefunc = leftLock;
	}
	return statefunc;
}
void* rightClose() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != RIGHT_CLOSED_STATE) {
		statefunc = rightScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[RIGHT_CLOSED_MSG]);
		displayObj.display_message = RIGHT_CLOSED_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		statefunc = rightLock;
	}
	return statefunc;
}
void* rightUnlock() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != GUARD_RIGHT_UNLOCK_STATE) {
		statefunc = rightScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[GUARD_RIGHT_UNLOCK_MSG]);
		displayObj.display_message = GUARD_RIGHT_UNLOCK_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		statefunc = rightOpen;
	}
	return statefunc;
}
void* leftUnlock() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != GUARD_LEFT_UNLOCK_STATE) {
		statefunc = rightScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[GUARD_LEFT_UNLOCK_MSG]);
		displayObj.display_message = GUARD_LEFT_UNLOCK_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		statefunc = leftOpen;
	}
	return statefunc;
}
void* rightLock() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != GUARD_RIGHT_LOCK_STATE) {
		statefunc = rightScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[GUARD_RIGHT_LOCK_MSG]);
		displayObj.display_message = GUARD_RIGHT_LOCK_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		if (personObj.person_direction == 1) {
			statefunc = waiting; // this would go back to wait state
		} else if (personObj.person_direction == 2) {
			statefunc = leftUnlock;
		}
	}
	return statefunc;
}
void* leftLock() {
	if (personObj.person_state == EXIT_STATE) {
		statefunc = exitState;
	} else if (personObj.person_state != GUARD_LEFT_LOCK_STATE) {
		statefunc = rightScan;
	} else {
		strcpy(sendMsgToDisplay, outMessage[GUARD_LEFT_LOCK_MSG]);
		displayObj.display_message = GUARD_LEFT_LOCK_MSG;
		displayObj.display_person.person_id = personObj.person_id;
		if (MsgSend(coidToDisplay, &displayObj, sizeof(displayObj) + 1,
				&replyFromDisplay, sizeof(replyFromDisplay)) == -1) {
			fprintf(stderr, "Error during MsgSend\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
		if (personObj.person_direction == 1) {
			statefunc = rightUnlock;
		} else if (personObj.person_direction == 2) {
			statefunc = waiting; // this would go back to wait state
		}
	}
	return statefunc;
}
