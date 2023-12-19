# Building Entry System

## Description
QNX RTOS (C language)\
A two-door system with card scanners on each door, a scale that weighs the person after they have entered the space between the two doors, and a human guard that uses a switch to open each of the doors.\
People enter the entry control space from the left to enter the building and enter the control space from the right to exit the building.
Each person requesting to enter or exit the building scans their unique personal id card containing an integer person_id number.
The scanner sends a message to the controller with the input information (which door opening is being requested, and which person_id is being used (e.g., “left 12345”).
Only 1 person at a time can be inside the lock.
An event will be sent to the controller when the status of the door changes.\
Uses multiple processes, QNX message passing, and function pointers.
| Process  | Role |
| ------------- | ------------- |
| Display  | Server  |
| Controller  | Client for Display / Server to Inputs  |
| Inputs | Client |

## State Machine
![image](https://github.com/Maya-Moji/BuildingEntrySystem/assets/106558260/b63423e7-d6bb-4920-914e-f8ed912b0ce5)
