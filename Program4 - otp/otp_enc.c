/***********************************************************************************************************************************************
 * Author: Han Jiang
 * Assignment: Prorgam 4 OTP
 * File name: otp_enc.c
 * Submission date: Dec 8 2019
 * *************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define BUFFERSIZE 100000

void error(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
} // Error function used for reporting issues, more about value EXIT_FAILURE:https://www.geeksforgeeks.org/error-handling-c-programs/

int findSize(FILE *FD) //reference: https://www.geeksforgeeks.org/c-program-find-size-file/
{
	fseek(FD, 0, SEEK_END);
	// calculating the size of the file
	int res = ftell(FD);
	fseek(FD, 0, SEEK_SET); //reset file pointer to beginning
	return res;
}

void checkBadChar(char *input)
{
	int i;
	int j = strlen(input);
	for (i = 0; i < j; i++)
	{
		if ((int)input[i] > 90 || ((int)input[i] < 65 && (int)input[i] != 32))
		{ //cast char to ASCII number
			perror("ERROR your input file includes bad character!\n");
			exit(1);
		}
	}
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead, informRead;
	struct sockaddr_in serverAddress; // Getting the actual address into a form connect()
	struct hostent *serverHostInfo;
	char encryptBuffer[BUFFERSIZE]; //need to change to 100000
	char *inputFile;
	char *keyFile;
	inputFile = argv[1];
	keyFile = argv[2];
	char informBuffer[7];

	if (argc < 3)
	{
		fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
		exit(0);
	} // Check usage & args

	// Set up the server address struct
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	// wait later portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string, the syntax of otp_enc is “ otp_enc plaintext key port”
	portNumber = atoi(argv[3]);					//convert input to integer
	serverAddress.sin_family = AF_INET;			// Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number, htons():host-to-network-short, converts from host/PC byte order(LSB) to network byte order(MSB)
												//PCs store bytes with smallest digit first, but networks expect largest digit first

	/************************* here I need to check which argv is for name of host ******************************/
	//serverHostInfo = gethostbyname(argv[1]);
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address, gethostbyname() will do a DNS lookup and return address info
	if (serverHostInfo == NULL)
	{
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}
	memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket, type is TCP
	if (socketFD < 0)
	{
		error("CLIENT: ERROR opening socket");
	}

	// Connect to server
	if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address, (struct sockaddr*) casting the type of serverAddress
		error("CLIENT: ERROR connecting");

	// Get input message from user
	/**********************************  I need to change to get input message from filename ****************************/

	FILE *inputFP, *keyFP;
	inputFP = fopen(inputFile, "r"); //open file with filename
	keyFP = fopen(keyFile, "r");
	if ((!inputFP) || (!keyFP))
	{															  //if file doesn't exist
		error("check your input file or key file if they exist"); //print message
	}
	int inputLen = findSize(inputFP); //find fiels size

	char *inputBuffer = malloc(inputLen * sizeof(char)); //set up input text array
	memset(inputBuffer, '\0', inputLen * sizeof(char));  // Clear out the buffer array
	fgets(inputBuffer, inputLen, inputFP); // Get input from the input file, trunc to buffer - 1 chars, leaving \0
	//strcspn will look where the \n is in buffer

	inputBuffer[inputLen - 1] = '\0'; // Remove the trailing \n that fgets adds

	// closing the file
	fclose(inputFP);

	long int keyLen = findSize(keyFP);
	char *keyBuffer = malloc(keyLen * sizeof(char)); //set up input text array
	memset(keyBuffer, '\0', keyLen * sizeof(char));  // Clear out the buffer array
	fgets(keyBuffer, keyLen, keyFP);				 // Get input from the input file, trunc to buffer - 1 chars, leaving \0
	keyBuffer[keyLen - 1] = '\0';
	// closing the file
	fclose(keyFP);

	/*If otp_enc receives key or plaintext files with ANY bad characters in them, 
	or the key file is shorter than the plaintext */
	if (keyLen < inputLen)
	{
		fprintf(stderr,"ERROR GET LONGER KEY\n"); // send appropriate error text to stderr, and set the exit value to 1
		exit(1);
	}

	checkBadChar(inputBuffer); //check buffers if they have any bad character
	checkBadChar(keyBuffer);

	char forSend[inputLen + 2];

	memset(forSend, '\0', inputLen + 2);   //clear out forSend buffer
	sprintf(forSend, "%s\n", inputBuffer); //add line at the end of forSend

	

	// Send message to server
	charsWritten = send(socketFD, forSend, inputLen + 1, 0); // Write to the server
	if (charsWritten < 0)
		error("CLIENT: ERROR writing to socket");
	if (charsWritten < inputLen + 1)
		fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
	// Verify that the data has actually left the system
	int checkSend = -5; // Bytes remaining in send buffer
	do
	{
		// Check the send buffer for this socket
		ioctl(socketFD, TIOCOUTQ, &checkSend);
	} while (checkSend > 0); // Loop forever until send buffer for this socket is empty

	if (checkSend < 0)
	{
		error("ioctl error"); // Check if we actually stopped the loop because of an error
	}

	memset(inputBuffer, '\0', sizeof(inputBuffer)); // Clear out the buffer again for reuse

	char forSendKey[keyLen + 2];
	memset(forSendKey, '\0', keyLen + 2);
	sprintf(forSendKey, "%s\n", keyBuffer);//add new line to the key buffer, so it can be received by Server

	//recv from server
	informRead = 0;
	memset(informBuffer, '\0', sizeof(informBuffer));
	recv(socketFD, informBuffer, sizeof(informBuffer), 0);
	if (strcmp(informBuffer,"enc_d")!=0){//if the port connection is not enc_d
		fprintf(stderr, "ERROR WRONG SERVER\n");
		exit(2);
	}

	char keysentBuff[10];

	// Send key to server
	charsWritten = send(socketFD, forSendKey, keyLen + 1, 0); // Write to the server
	if (charsWritten < 0)
		error("CLIENT: ERROR writing to socket");
	if (charsWritten < keyLen + 1)
		fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
	// Verify that the data has actually left the system
	checkSend = -5; // Bytes remaining in send buffer
	do
	{
		// Check the send buffer for this socket
		ioctl(socketFD, TIOCOUTQ, &checkSend);
	} while (checkSend > 0); // Loop forever until send buffer for this socket is empty

	if (checkSend < 0)
	{
		error("ioctl error"); // Check if we actually stopped the loop because of an error
	}

	memset(encryptBuffer, '\0', sizeof(encryptBuffer));//clean the result buffer
	char readbuff[100000]; 

	charsRead = 0;
	while (strstr(encryptBuffer, "@") == NULL) // As long as we haven't found the terminal...
	{
		memset(readbuff, '\0', sizeof(readbuff));				   // Clear the buffer
		charsRead = recv(socketFD, readbuff, sizeof(readbuff), 0); // Get the next chunk
		strcat(encryptBuffer, readbuff);						   // Add that chunk to what we have so far

		if (charsRead == 0)
		{
			break;//Nothing left break the loop 
		}
		if (charsRead == -1)
		{
			error("CLIENT: ERROR reading from socket");//send error message
			break;
		}
	}
	int terminalLocation = strstr(encryptBuffer, "@") - encryptBuffer; // Where is the terminal
	encryptBuffer[terminalLocation] = '\0';							   // End the string early to wipe out the terminal

	
	checkBadChar(encryptBuffer);//check if there is any bad character in buffer
	printf("%s\n", encryptBuffer);//print the buffer and add newline at the end

	close(socketFD); // Close the socket
	return 0;
}
