/*
Author: Han Jiang
Assignment: adventure 
File Name: jianghan.buildrooms.c
Class: CS344
Due date: 2019/11/1
Submission date: 2019/11/2
*/
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>

#define MIN_OUTBOUNDS (int)3
#define MAX_OUTBOUNDS (int)6
#define MAXPATHLEN (int)32
#define MaxOutboundConnections (int)6



typedef struct Room
{
  int id;
  char* name;
  int numOutboundConnections;
  struct Room* outboundConnections[6];
  char* type;
}Room;

// Hardcode ten names, seven of which will be randomly assigned to rooms
 char* ROOM_NAMES[10] = {
    "Gate",
    "Laundry",
    "Security",
    "Cafe",
    "Garage",
    "Bed",
    "Meeting",
    "Kitchen",
    "Visitor",
    "Dining"
};

// Hardcode 3 types for rooms
const char* ROOM_TYPES[3] = {
    "START_ROOM",
    "MID_ROOM",
    "END_ROOM"
};


// Hardcode 10 files' names, match them to rooms' names
char* FILE_NAMES[10] = {
    "Gate_Room",
    "Laundry_Room",
    "Security_Room",
    "Cafe_Room",
    "Garage_Room",
    "Bed_Room",
    "Meeting_Room",
    "Kitchen_Room",
    "Visitor_Room",
    "Dining_Room"
};

bool IsGraphFull(Room* rooms);
void AddRandomConnection(Room* rooms, int count); 
bool CanAddConnectionFrom(Room* x); 
bool ConnectionAlreadyExists(Room* x, Room* y);
void ConnectRoom(Room* x, Room* y); 
bool IsSameRoom(Room* x, Room* y); 
void GenerateRooms(Room* rooms, int count); 
void setTypes(Room* rooms);
void OutputRoomsToFiles(char* dirname, Room* rooms); 
void shuffle(int* arr, int size);
struct Room* getRandomRoom(struct Room* rooms, int count);



int main(void){
    char* newname[10];
    char* newFileName[10];
    srand(time(0));
 /*************************************
  * make a directory with proper name 
  * references: https://cboard.cprogramming.com/c-programming/165757-using-process-id-name-file-directory.html 
 **************************************/
    int pid = getpid();
    char fileName[MAXPATHLEN];
    char *filePrefix = "jianghan.rooms.";
    snprintf(fileName, sizeof(fileName), "%s%d", filePrefix, pid);
   // printf("%s\n",fileName);
    struct stat st = {0};
    if (stat(fileName, &st) == -1) {
        mkdir(fileName, 0755);
    }
    int count;
    count=7;
    Room rooms[count];

  


    int index[10];
    int i;
    for (i = 0; i < 10; i++) 
    {
      index[i] = i;
    }
  
    shuffle(index, 10);

    int c;
    for (c=0;c<10;c++){
        newname[c]=ROOM_NAMES[index[c]];
        newFileName[c]=FILE_NAMES[index[c]];
    /*debug test printf("ROOM TYPE: %s\n", newrooms[c].type); */
    }


    int k;
    for(k=0;k<10;k++){
        ROOM_NAMES[k]=newname[k];
         FILE_NAMES[k]=newFileName[k];
    }

    GenerateRooms(rooms, count); 

    setTypes(rooms);
    OutputRoomsToFiles(fileName,rooms);

    return 0;
}


/********************************************************************************
* Description: Returns true if all rooms have 3 to 6 outbound connections, false otherwise
* Parameters: the struct room array filled with connection
* Returns: boolean value
* Pre-Conditions: Room array has been filled with connection
* Post-Conditions: It is a helper function for GenerateRooms
*********************************************************************************/

bool IsGraphFull(Room* rooms)  
{
    assert(rooms);

    int i;
    for (i = 0; i < 7; i++) {
        int outbounds = rooms[i].numOutboundConnections;
        if (outbounds < MIN_OUTBOUNDS || outbounds > MAX_OUTBOUNDS )
            return false;
            
    }
    return true;
}






/********************************************************************************
* Description: To get a random room from room array
* Parameters: struct room pointer, integer to set range of random value
* Returns: a random struct room
* Pre-Conditions: struct room array is fulfilled with 10 elements
* Post-Conditions: It is a helper function for AddRandomConnection
*********************************************************************************/
struct Room* getRandomRoom(struct Room* rooms, int count) {
	int room_index = rand() % count;

	return (rooms + room_index);
}









/********************************************************************************
* Description: Adds a random, valid outbound connection from a Room to another Room
* Parameters: pointer to struct room array,  integer to set range of random value
* Returns: None
* Pre-Conditions: struct room array hase been built up, the struct can add connection
* Post-Conditions:the struct in rooms array has connection to a random room, and this selected 
                  random room also has a connection back
*********************************************************************************/
void AddRandomConnection(Room* rooms, int count) {
	struct Room* x;
	struct Room* y;

	while(1) {
		x = getRandomRoom(rooms, count);
		
		if(CanAddConnectionFrom(x) == 1 ) {
			break;
		}
	}

	/*a random room different from the first one, that can add a connection  */
	do {
		y = getRandomRoom(rooms, count);

	} while(CanAddConnectionFrom(y) == 0 || IsSameRoom(x, y) == 1);

	if( !ConnectionAlreadyExists(x, y) && !ConnectionAlreadyExists(y, x) ) {
		ConnectRoom(x, y);
		ConnectRoom(y, x);
	}

}







/********************************************************************************
* Description: Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
* Parameters: struct room
* Returns: Boolean value
* Pre-Conditions: struct room is built up, not a null pointer
* Post-Conditions: It helps AddRandomConnection to detetmine if the room need connection
*********************************************************************************/

bool CanAddConnectionFrom(Room* x) 
{
   if (x->numOutboundConnections < MaxOutboundConnections)
      return true;
  else 
      return false;

}





/********************************************************************************
* Description: Returns true if a connection from Room x to Room y already exists, false otherwise
* Parameters: struct room pointer x, struct room pointer y
* Returns: Boolean value
* Pre-Conditions: None
* Post-Conditions:It helps AddRandomConnection to detetmine if the room connection exists
*********************************************************************************/

bool ConnectionAlreadyExists(Room* x, Room* y)
{

      assert(x && y);
 
    int i;
    for (i = 0; i < x->numOutboundConnections; i++) {

        if (x->outboundConnections[i] == y) {
            return true;
        }
    }

    return false;

}






/********************************************************************************
* Description: Connect 2 rooms
* Parameters: struct room pointer x, struct room pointer y
* Returns: None
* Pre-Conditions: This function does not check if this connection is valid
* Post-Conditions: room x and room y are connected
*********************************************************************************/
void ConnectRoom(Room* x, Room* y) 
{
    if (!CanAddConnectionFrom(x) || !CanAddConnectionFrom(y)) return;
    x->outboundConnections[x->numOutboundConnections] = y;//connect to y, num +1
    x->numOutboundConnections++;
}






/********************************************************************************
* Description: Returns true if Rooms x and y are the same Room, false otherwise
* Parameters: struct room pointer x, struct room pointer y; 
* Returns: Boolean value
* Pre-Conditions: two pointers are not null 
* Post-Conditions:helper function for AddRandomConnection
*********************************************************************************/

bool IsSameRoom(Room* x, Room* y) {
   assert(x && y);
   if (x == y){
        return true;
   }
   else
   {
       return false;
   }
}








/********************************************************************************
* Description: build up rooms,ste name and make connection
* Parameters: struct room pointer, integer 
* Returns: None
* Pre-Conditions: struct room array exists
* Post-Conditions:The rooms in the rooms array are completed, type is undertermined
*********************************************************************************/
void GenerateRooms(Room* rooms, int count) {
    assert(rooms);
    int i, j;

    for (i = 0; i < count; i++) {  
        rooms[i].id=i;   
        rooms[i].name= calloc(16,sizeof(char));
        strcpy(rooms[i].name, ROOM_NAMES[i]);

        rooms[i].numOutboundConnections = 0;
        for (j = 0; j < 6; j++){
          rooms[i].outboundConnections[j] = NULL;
        }


    }

    while (IsGraphFull(rooms) == false)
    {
        AddRandomConnection(rooms, count);
    }

}



/********************************************************************************
* Description: randomly pick a type for selected room
* Parameters: pointer to struct room array 
* Returns: None
* Pre-Conditions: struct room array exists, room type array exists
* Post-Conditions:rooms in the array have types
*********************************************************************************/
void setTypes(Room* rooms){
        int   startroom, endroom,i;
        startroom=rand()%7; 
        endroom=rand()%7;
        for(i=0;i<7;i++){
        if (i == startroom) {
           rooms[i].type= calloc(16,sizeof(char));
           strcpy(rooms[i].type, ROOM_TYPES[0]);
        } 
        else 
            if (i == endroom) {
             rooms[i].type= calloc(16,sizeof(char));
        strcpy(rooms[i].type, ROOM_TYPES[2]);
        }
        else {
            rooms[i].type= calloc(16,sizeof(char));
        strcpy(rooms[i].type, ROOM_TYPES[1]);
        }

        }
}



  

/********************************************************************************
* Description: generate    room files and write contents to them
* Parameters: char pointer of directory name, struct room array pointer 
* Returns: None
* Pre-Conditions: struct room array pointer is not null & char pointer is existed
* Post-Conditions:There are files with sturct room information stored in the directory
*********************************************************************************/
void OutputRoomsToFiles(char* dirname, Room* rooms) {
    assert(dirname && rooms);

    int  j;
    /* generate  7  room files and write contents to them */
    int i;
    for(i=0;i<7;i++){
        char* filename;
        filename = calloc(MAXPATHLEN * 2,sizeof(char));
        
        strcat(filename, dirname);
        strcat(filename, "/");
       
        strcat(filename, FILE_NAMES[rooms[i].id]);
       
        FILE* roomfile = fopen(filename, "w");
        assert(roomfile);
        fprintf(roomfile, "ROOM NAME: %s\n", rooms[i].name);

        for (j = 0; j < rooms[i].numOutboundConnections; j++) {
            fprintf(roomfile, "CONNECTION %d: %s\n", j + 1,
                    rooms[i].outboundConnections[j]->name);
        }
  
        fprintf(roomfile, "ROOM TYPE: %s\n", rooms[i].type);

        fclose(roomfile);
        }
    
}





/********************************************************************************
* Description: shuffle the array elements order
* Parameters: integer array pointer
* Returns: None
* Pre-Conditions: thew struct room has been built
* Post-Conditions:the array's elments'oder has been randomly changed
*********************************************************************************/
void shuffle(int* array, int num_shuffles) {
    int i;
    for (i = 0; i < num_shuffles; i++) {
        int index = i + rand() % (num_shuffles - i);
        int temp = array[i];
        array[i] = array[index];
        array[index] = temp;
    }
}








