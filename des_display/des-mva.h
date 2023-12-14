/*
 * MAYA JAFFARY
 * des-mva.h
 * header file
 */

#ifndef PROJ_H_
#define PROJ_H_

#define NUM_STATES 13
typedef enum
{
	LEFT_SCAN_STATE = 0,
	RIGHT_SCAN_STATE = 1,
	WEIGHT_SCALE_STATE = 2,
	LEFT_OPEN_STATE = 3,
	RIGHT_OPEN_STATE = 4,
	LEFT_CLOSED_STATE = 5,
	RIGHT_CLOSED_STATE = 6,
	GUARD_RIGHT_UNLOCK_STATE = 7,
	GUARD_RIGHT_LOCK_STATE = 8,
	GUARD_LEFT_LOCK_STATE = 9,
	GUARD_LEFT_UNLOCK_STATE = 10,
	WAITING_STATE = 11,
	EXIT_STATE = 12
} State;

#define NUM_INPUTS 4
typedef enum
{
	ID_INPUT = 0,
	WEIGHT_INPUT = 1,
	OCCUPIED_INPUT = 2,
	FREE_INPUT = 3
} Input;

#define NUM_OUTPUTS 13
typedef enum
{
	LEFT_SCAN_MSG = 0,
	RIGHT_SCAN_MSG = 1,
	WEIGHT_SCALE_MSG = 2,
	LEFT_OPEN_MSG = 3,
	RIGHT_OPEN_MSG = 4,
	LEFT_CLOSED_MSG = 5,
	RIGHT_CLOSED_MSG = 6,
	GUARD_RIGHT_UNLOCK_MSG = 7,
	GUARD_RIGHT_LOCK_MSG = 8,
	GUARD_LEFT_LOCK_MSG = 9,
	GUARD_LEFT_UNLOCK_MSG = 10,
	WAITING_MSG = 11,
	EXIT_STATE_MSG = 12
} Output;

const char *outMessage[NUM_STATES] = {
		"Left Scan Completed ",
		"Right Scan Completed ",
		"Weight Scale Completed ",
		"Person opened left door",
		"Person opened right door",
		"Left Closed (automatically)",
		"Right Closed (automatically)",
		"Right door unlocked by guard",
		"Right door locked by guard",
		"Left door locked by guard",
		"Left door unlocked by guard",
		"Waiting for person.",
		"You're exiting the program."
};

const char *inMessage[NUM_INPUTS] = {
	"Enter your ID",
	"Enter your weight",
	"Someone is using the DES.",
	"DES is free to use."
};

// inputs client sends a Person struct to its server, the controller
typedef struct {
int person_id;
int person_weight;
int person_direction; //0 for not set, 1 for inbound (left scan), and 2 for outbound (right scan)
int person_state; //e.g. "Left Scan"
} Person;

// controller client sends a Display struct to its server, the display
typedef struct {
int display_message; //index of outMessages array
Person display_person; //messages that display the Person's ID and weight need Person
} Display;

#endif /* DES_MVA_H_ */
