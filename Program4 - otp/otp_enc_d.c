/***********************************************************************************************************************************************
 * Author: Han Jiang
 * Assignment: Prorgam 4 OTP
 * File name: otp_enc_d.c
 * Submission date: Dec 8 2019
 * **************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#define BUFFERSIZE 100000

void error(const char *msg, int exitStatus)
{
	perror(msg);
	exit(exitStatus);
} // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, childExitMethod, informRead;//a pool for file descriptor and other counting stuff
	socklen_t sizeOfClientInfo;
	char buffer[100000], keybuffer[100000];			 //buffers for transmission
	struct sockaddr_in serverAddress, clientAddress; //socket address
	pid_t spawnPid;									 //child process id
	//buffers for transmission
	char plainTextBuffer[BUFFERSIZE];
	char encryptedMess[BUFFERSIZE];
	char encryptedKey[BUFFERSIZE];
	//char informBuffer[3];
	if (argc < 2)
	{
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	} // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]);									 // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET;							 // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber);					 // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY;					 // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0)
		error("ERROR opening socket", 1);

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding", 2);
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	if (listen(listenSocketFD, 5) < 0)
	{
		fprintf(stderr, "ERROR on accept\n");
	}
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	while (1)
	{ //live forever
		// Accept a connection, blocking if one is not available until one connects
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
		// accept() is going to block until something tries to connect.
		if (establishedConnectionFD < 0)
		{
			error("ERROR on accept", 1);
		}

		spawnPid = fork();
		switch (spawnPid)
		{
		case -1:
			fprintf(stderr, "Hull breach!\n");
			close(establishedConnectionFD);
			break;
		//child Process
		case 0:
			// Get the message from the client and display it

			
			

			/********************************* This is my text receiving loop! ***********************************/

			memset(plainTextBuffer, '\0', BUFFERSIZE);
			memset(encryptedMess, '\0', BUFFERSIZE);
			int charsRead = 0;
			while (strstr(plainTextBuffer, "\n") == NULL) // As long as we haven't found the terminal...
			{
				memset(buffer, '\0', sizeof(buffer));								  // Clear the buffer
				charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0); // Get the next chunk

				strcat(plainTextBuffer, buffer); // Add that chunk to what we have so far
				if (charsRead == 0)
				{
					break;
				}
				if (charsRead == -1)
					error("ERROR reading from socket", 2);
			}
			int terminalLocation = strstr(plainTextBuffer, "\n") - plainTextBuffer; // Where is the terminal
			plainTextBuffer[terminalLocation] = '\0';								// End the string early to wipe out the terminal
			/********************************** This is end of my text receiving loop! ************************************************/

			/********************discussion from piazza question 534 *************************************/
			//send to client and inform client it is from enc_d 
			int charsWritten = send(establishedConnectionFD, "enc_d", 5, 0); // Write to the server
			if (charsWritten < 0)
				error("CLIENT: ERROR writing to socket", 1);
			if (charsWritten < 2)
				error("CLIENT: WARNING: Not all data written to socket!\n", 2);
			/********************************* This is my key receiving loop! ***********************************/
			memset(encryptedKey, '\0', BUFFERSIZE);
			//https://stackoverflow.com/questions/13557705/socket-programming-connect-fails-the-second-time-i-run-the-loop
			int charsRead1 = 0;

			while (strstr(encryptedKey, "\n") == NULL) // As long as we haven't found the terminal...
			{

				memset(keybuffer, '\0', sizeof(keybuffer)); // Clear the buffer

				charsRead1 = recv(establishedConnectionFD, keybuffer, sizeof(keybuffer), 0); // Get the next chunk

				strcat(encryptedKey, keybuffer); // Add that chunk to what we have so far

				if (charsRead1 == 0)//if nothing to read, exit the recv loop
				{

					break;
				}
				if (charsRead1 == -1)
				{

					error("ERROR reading from socket", 2);
				}
			}

			terminalLocation = strstr(encryptedKey, "\n") - encryptedKey; // Where is the terminal
			encryptedKey[terminalLocation] = '\0';
			/********************************* This is end of my key receiving loop! ***********************************/

			/************************************************************/
			int i; // index integer
			//encrypt the plaintext message from client, follow the homework hint
			for (i = 0; i < strlen(plainTextBuffer); i++)
			{

				// typecast for  ASCII operation
				int inputChar = (int)plainTextBuffer[i];
				int keyChar = (int)keybuffer[i];
				//shift the space ASCII range  
				if (inputChar == 32)
				{
					inputChar = 91;
				}
				if (keyChar == 32)//if the character is space
				{
					keyChar = 91;//change ASCII value that helps operation
				}
				// change ASCII range
				inputChar = inputChar - 65;
				keyChar = keyChar - 65;

				// Sum and then mod by 26
				int cipherText = (inputChar + keyChar) % 27;

				// change characters to capital letters
				cipherText = cipherText + 65;
				//change back to space ASCII
				if (cipherText == 91)
				{
					cipherText = 32;
				}
				// and typecast back to characters
				encryptedMess[i] = (char)cipherText; 
			}
			encryptedMess[strlen(encryptedMess)] = '@';//add @ at the encryptedMess, let client know it's the end 
		
			/************************************************************/

			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, encryptedMess, strlen(encryptedMess), 0); // Send success back
			if (charsRead < 0)
				error("ERROR writing to socket", 2);

			// Verify that the data has actually left the system
			int checkSend = -5; // Bytes remaining in send buffer
			do
			{
				// Check the send buffer for this socket
				ioctl(establishedConnectionFD, TIOCOUTQ, &checkSend);
			} while (checkSend > 0); // Loop forever until send buffer for this socket is empty

			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			close(listenSocketFD);
			exit(0);
			break;

		default:
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			break;
		}
	}

	return 0;
}
