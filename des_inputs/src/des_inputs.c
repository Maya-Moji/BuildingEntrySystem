/*
 * MAYA JAFFARY - SHAUNESSEE GREEN
 * des_inputs.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "../../des_display/des-mva.h"

void clean_stdin(void);

/*******************************************************************************
 * main( ) - Program starts from here
 * des_inputs is the client.
 * Prompt User for next DES input event.
 * Prepare and send a Person object to controller for processing. (controller acting as server)
 ******************************************************************************/
int main(int argc, char *argv[]) {
	// check number of command line arguments
	if (argc != 2) {
		printf("Usage: Incorrect number of arguments.\n");
		exit(EXIT_FAILURE);
	}

	// set process ID of the owner of the channel (controller)
	pid_t controllerPID = atoi(argv[1]); //string to int - no error checking required

	// struct for person object to be sent to controller
	Person personObj;
	personObj.person_state = WAITING_STATE; // set person object's state as READY
	int myState = WAITING_STATE;
	personObj.person_weight = 0;
	personObj.person_id = 0;
	personObj.person_direction = 0;

	char userInput[10]; //to store user input - can be states or inputs of id and weight

	int replyFromController; //A pointer to a buffer where the reply can be stored

	bool willSendMsg = false;

	// connect to server
	int coid;
	coid = ConnectAttach(0, controllerPID, 1, 0, 0);
	if (coid == -1) {
		printf("Cannot Connect to Server. Please try again\n");
		return EXIT_FAILURE;
	}

	while (1) {
		willSendMsg = false;
		//show wait message if system is not being used
		if (personObj.person_direction == 0) {
			printf("\nWaiting for a person...\n");
		}

		// menu
		printf(
				"\nEnter the event type: \n(ls = left scan,\n rs = right scan,\n ws = weight scale,\n lo = left open,\n ro = right open,\n lc = left closed,\n rc = right closed,\n gru = guard right unlock,\n grl = guardright lock,\n gll = guard left lock,\n glu = guard left unlock,\n exit = exit programs)\n");
		if (scanf(" %s", userInput) == -1) {
			printf("\nReading from user failed... \n");
			return EXIT_FAILURE;
		}

		//setting the value of myState and error messages
		if ((strcmp(userInput, "ls") == 0)) {
			if (myState == WAITING_STATE) {
				myState = LEFT_SCAN_STATE;
				// read id from user
				char continueLoop = '1';
				while (continueLoop == '1') {
					printf("\nEnter your ID number: \n");
					// set person object's id
					scanf("%d", &personObj.person_id);
					if (personObj.person_id >= INT_MAX) {
						printf("Enter a smaller number...\n");
						clean_stdin();
					} else {
						//printf("\nPerson's ID is = %d\n", personObj.person_id);
						continueLoop = '0';
						break;
					}
				}
				// set person object's direction
				personObj.person_direction = 1; // inbound direction (ls)
				willSendMsg = true;
			} else {
				printf("\nError--> The DES is already in use. \n");
			}
		} else if ((strcmp(userInput, "rs") == 0)) {
			if (myState == WAITING_STATE) {
				myState = RIGHT_SCAN_STATE;
				// read id from user
				char continueLoop = '1';
				while (continueLoop == '1') {
					printf("\nEnter your ID number: \n");
					// set person object's id
					scanf("%d", &personObj.person_id);
					if (personObj.person_id >= INT_MAX) {
						printf("Enter a smaller number...\n");
						clean_stdin();
					} else {
						//printf("\nPerson's ID is = %d\n", personObj.person_id);
						continueLoop = '0';
						break;
					}
				}
				// set person object's direction
				personObj.person_direction = 2; // outbound direction (rs)
				willSendMsg = true;
			} else {
				printf("\nError--> The DES is already in use. \n");
			}
		} else if ((strcmp(userInput, "ws") == 0)) {
			if (myState == LEFT_OPEN_STATE || myState == RIGHT_OPEN_STATE) {
				myState = WEIGHT_SCALE_STATE;
				// read weight from user
				char continueLoop = '1';
				while (continueLoop == '1') {
					printf("\nEnter your weight: \n");
					// set person object's weight
					scanf("%d", &personObj.person_weight);
					if (personObj.person_weight >= INT_MAX) {
						printf("Enter a smaller number...\n");
						clean_stdin();
					} else {
						//printf("\nPerson's weight is = %d\n",	personObj.person_weight);
						continueLoop = '0';
						break;
					}
				}
				willSendMsg = true;
			} else {
				printf(
						"\nError--> You need to open the door before you can weight yourself. \n");
			}

		} else if ((strcmp(userInput, "lo") == 0)) {
			if (myState == GUARD_LEFT_UNLOCK_STATE) {
				myState = LEFT_OPEN_STATE;
				willSendMsg = true;
			} else {
				printf("\nError-->Door needs to be unlocked first. \n");
			}
		} else if ((strcmp(userInput, "ro") == 0)) {
			if (myState == GUARD_RIGHT_UNLOCK_STATE) {
				myState = RIGHT_OPEN_STATE;
				willSendMsg = true;
			} else {
				printf("\nError-->Door needs to be unlocked first. \n");
			}
		} else if ((strcmp(userInput, "lc") == 0)) {
			if ((myState == WEIGHT_SCALE_STATE && personObj.person_direction == 1) || (myState == LEFT_OPEN_STATE && personObj.person_direction == 2)) {
				myState = LEFT_CLOSED_STATE;
				willSendMsg = true;
			} else {
				printf("\nError-->Door is already closed. \n");
			}
		} else if ((strcmp(userInput, "rc") == 0)) {
			if ((myState == WEIGHT_SCALE_STATE && personObj.person_direction == 2) || (myState == RIGHT_OPEN_STATE && personObj.person_direction == 1)) {
				myState = RIGHT_CLOSED_STATE;
				willSendMsg = true;
			} else {
				printf("\nError-->Door is already closed. \n");
			}
		} else if ((strcmp(userInput, "gru") == 0)) {
			if ((myState == RIGHT_SCAN_STATE && personObj.person_direction == 2) || (myState == GUARD_LEFT_LOCK_STATE && personObj.person_direction == 1)) {
				myState = GUARD_RIGHT_UNLOCK_STATE;
				willSendMsg = true;
			} else {
				printf(
						"\nError--> You haven't scanned your id yet or another door is unlocked at the moment.\n");
			}
		} else if ((strcmp(userInput, "glu") == 0)) {
			if ((myState == LEFT_SCAN_STATE && personObj.person_direction == 1) || (myState == GUARD_RIGHT_LOCK_STATE && personObj.person_direction == 2)) {
				myState = GUARD_LEFT_UNLOCK_STATE;
				willSendMsg = true;
			} else {
				printf(
						"\nError--> You haven't scanned your id yet or another door is unlocked at the moment.\n");
			}
			if (personObj.person_direction == 2) { //if outbound, this would be the last state => reset
				personObj.person_state = WAITING_STATE;
				myState = WAITING_STATE;
				personObj.person_weight = 0;
				personObj.person_id = 0;
				personObj.person_direction = 0;
			}

		} else if ((strcmp(userInput, "grl") == 0)) {
			if (myState == RIGHT_CLOSED_STATE) {
				myState = GUARD_RIGHT_LOCK_STATE;
				willSendMsg = true;
			} else {
				printf("\nError--> You need to close the door first.\n");
			}
			if (personObj.person_direction == 1) { //if inbound, this would be the last state => reset
				personObj.person_state = WAITING_STATE;
				myState = WAITING_STATE;
				personObj.person_weight = 0;
				personObj.person_id = 0;
				personObj.person_direction = 0;
			}
		} else if ((strcmp(userInput, "gll") == 0)) {
			if (myState == LEFT_CLOSED_STATE) {
				myState = GUARD_LEFT_LOCK_STATE;
				willSendMsg = true;
			} else {
				printf("\nError--> You need to close the door first.\n");
			}
		} else if ((strcmp(userInput, "exit") == 0)) {
			myState = EXIT_STATE; // you can exit the system at any time
			personObj.person_state = myState;
			int myMsgSend = MsgSend(coid, &personObj, sizeof(personObj) + 1,
					&replyFromController, sizeof(replyFromController));
			if (myMsgSend == -1) {
				fprintf(stderr, "MsgSend failed... (in inputs)\n");
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			printf("Exiting Inputs...\n");
			printf("Exit successful.\n\n");
			exit(EXIT_FAILURE);
		} else {
			printf(
					"\nError--> Incorrect input. Choose an item from the menu.\n");
		}

		//printf("mystate is : %d\n",myState); //for testing

		//setting states for the person object
		if (willSendMsg) {
			personObj.person_state = myState;

			// send struct message to display (Controller)
			int myMsgSend = MsgSend(coid, &personObj, sizeof(personObj) + 1,
					&replyFromController, sizeof(replyFromController));
			if (myMsgSend == -1) {
				fprintf(stderr, "MsgSend failed... (in inputs)\n");
				perror(NULL);
				exit(EXIT_FAILURE);
			}

			if (replyFromController == -1) {
				printf("\nINPUTS: Receiving reply from Controller failed.");
			}
		}
	}

	// disconnect from server
	ConnectDetach(coid);

	// exit success
	return EXIT_SUCCESS;
} // end of main

/*******************************************************************************
 * clean_stdin(int ) - This function clears the data stream for user input
 ******************************************************************************/
void clean_stdin(void) {
	int c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}
