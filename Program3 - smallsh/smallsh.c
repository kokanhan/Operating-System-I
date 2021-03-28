/*
Author: HanJiang
Program 3
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

//the limitation of commands lengths and how many that user can enter
#define MAXLENOFCHAR 2048
#define MAXARGS 512

bool allowBackground = true;//for the foreground mode
int childPID_intent = 0;//a variable to check if specified pid is terminated


int backgroundPid[100];// An array to store running background pid
int nPid = 0;

// Variables store the exit status of the last terminated process
int exitStatus = 0;
int termSignal = -1;


struct Commands
{
    char *argv[MAXARGS];
    int arguNum;
    bool background;
};

// A function to create and initialize the command struct
struct Commands *CommandsCreate()
{
    struct Commands *Commands = malloc(sizeof(struct Commands));//use malloc to assign memory to struct
    int i;
    for (i = 0; i < MAXARGS; i++)
    {
        Commands->argv[i] = NULL;//make sure commands have no garbage value
    }
    Commands->arguNum = 0;
    Commands->background = false;
    return Commands;
}

//clean up the struct to prevent memorylaek
void freeCommands(struct Commands *Commands)
{
    int i;
    for (i = 0; i < MAXARGS; i++)
    {
        free(Commands->argv[i]);
    }
    free(Commands);//clean the struct pointer 
}


void setCommand(struct Commands *Commands, char *input)
{

    int index;
    for (index = 0; index < MAXARGS; index++)
    {
        Commands->argv[index] = NULL;//make sure no garbage value in the argv array
    }
    Commands->arguNum = 0;
    Commands->background = false;
	
    char *token = strtok(input, " ");
    char *back = "&";
    while (token != NULL)
    {
        Commands->argv[Commands->arguNum] = strdup(token);//if the token is not empty
        Commands->arguNum++;//copy to the commands
        token = strtok(NULL, " ");//move to next context that separated by space
    }
}


void runBuiltIn(struct Commands *Commands, char *lineEntered, char *input)
{

    // if command is "cd"
    if (strcmp(Commands->argv[0], "cd") == 0)
    {

        // target directory is specified
        if (Commands->argv[1])
        {

            // change current directory to the target directory
            if (chdir(Commands->argv[1]) < 0)
            { // If we cannot find the directory

                // If the shell couldn't find it, print an error message and set the exit status to 1
                perror("Directory not found.\n");
                
                exitStatus = 1;
                termSignal = -1;
                return;
            }

            
        }
        else// target directory is not specified
        {

            // go to home directory
            chdir(getenv("HOME"));
        }
        
    }
    else if (strcmp(Commands->argv[0], "status") == 0)// If command is "status"
    {

        // Print out the exit status of the last terminated process
        if (exitStatus >= 0)
        {
            printf("exit value %d\n", exitStatus);//print out the exitStatus and do fflush as instructions
            fflush(stdout);
        }
        else
        {
            printf("terminated by signal %d\n", termSignal);//print out the terminated sinal
            fflush(stdout);
        }
        
    }
    else if (strcmp(Commands->argv[0], "exit") == 0)// If command is "exit"
    {

        // Kill off all unfinished background processes ref:https://stackoverflow.com/questions/21602689/c-c-program-to-kill-all-child-processes-of-a-parent-process-in-unix
        int i;
        for (i = 0; i < nPid; i++)
        {
            kill(backgroundPid[i], SIGKILL);
        }

        // free up all resources to prevent memory leak
        freeCommands(Commands);
        free(lineEntered);
        free(input);
        exit(0);
    }

    // if the process terminates successfully
    // set exit status to 0 and termSignal to -1 (not terminated by signal)
    exitStatus = 0;
    termSignal = -1;
}

//convert the commands start with $$ to pid number
void convertPid(struct Commands *Commands, char *pidSigan)
{
    int i;
    for (i = 0; i < Commands->arguNum; i++)
    {
        if (strcmp(Commands->argv[i], pidSigan) == 0)//check if the command string contain "$$"
        {
            char *buffer = strdup(Commands->argv[i]);
            buffer = "%d";

            sprintf(Commands->argv[i], buffer, getpid());//change "$$" command to pid number 
        }
    }
}

char *expandDuoSigan(char *line)
{

    char *nextStr;              //returned string from strstr containing the parsed string
    const char token[3] = "$$"; //token to be used to search $$
    char buffer[MAXLENOFCHAR];          //buffer to hold the parsed string
    memset(buffer, '\0', MAXLENOFCHAR);
    char pid[10]; //pid hold the pid
    memset(pid, '\0', 10);
    sprintf(pid, "%d", getpid()); //set pid

    nextStr = strstr(line, token); //find the token in entered line

    if (nextStr != NULL) //if token is found then continue
    {
        strncpy(buffer, line, nextStr - line); //copy everything before $$ to buffer

        while (nextStr != NULL) //while nextStr is not null, parse through rest of string for more  of $$
        {
            sprintf(buffer + (nextStr - line), "%s%s", pid, nextStr + strlen(token)); //add contents after $$ in nextStr to buffer along with PID
            strcpy(line, buffer); //set buffer as the new line to be read by strstr
            nextStr = strstr(line, token); //strstr line to find next $$
        }
    }

    return line;
}




void ifBackgroundRun()//check if the child process running on the background
{
    int childExitMethod;
    pid_t childPID_actual;
    if (childPID_actual = waitpid(childPID_intent, &childExitMethod, WNOHANG) != 0)
    {
        kill(childPID_actual, SIGKILL); //kill the process if it is terminated
        if (WIFEXITED(childExitMethod) != 0)
        {
            int exitStatus = WEXITSTATUS(childExitMethod);
            printf("background pid %d is done: exit value %d\n", childPID_intent, exitStatus);//print the exit value
            fflush(stdout);
        }
        //if(WIFSIGNALED(childExitMethod) != 0){
        else
        {
            int termSignal = WTERMSIG(childExitMethod);
            printf("background pid %d is done: terminated by signal %d\n", childPID_intent, termSignal);
            fflush(stdout);
        }
        childPID_intent = 0;//source from http://web.engr.oregonstate.edu/~brewsteb/CS344Slides/3.3%20Signals.pdf
    }
}




// This function gets an input from the user
char *getInput()
{
    char *lineEntered = NULL; // Holds the user input
    size_t bufferSize = 0;    // Holds the size of the input buffer (only for requirement)
    int numCharsEntered = 0;  // Holds the number of chars entered by user
                              /*printf("start get input");
    fflush(stdout);*/
                              // Get input string from user and remove its trailing newline '\n'
    while (numCharsEntered == 0 || lineEntered[0] == '\n' || lineEntered[0] == '#')
    {

        // Before each prompt, check if any background process has terminated
        fflush(stdout);
        if (childPID_intent != 0)
        {
            ifBackgroundRun();
        }

        // Get the input from the user
        printf(": ");
        fflush(stdout);
        numCharsEntered = getline(&lineEntered, &bufferSize, stdin);

        // If user input is invalid, clear error in stdin and repeat the process
        if (numCharsEntered == -1)
        {
            clearerr(stdin);

            // If the user did not enter any input, repeat the process
            // numCharsEntered is 1 because of newline '\n'
        }
    }

    // Remove the trailing newline '\n' from our input string
    lineEntered[numCharsEntered - 1] = '\0';
    return lineEntered;
}

//Catch ^C
void catchSIGINT(int signo)
{
    char *message = "terminated by signal 2\n:"; //Can’t use printf() during a signal handler, as it’s non-reentrant; can’t use strlen(message) in place of 24, as it’s also non-entrant!
    write(STDOUT_FILENO, message, 24);
    fflush(stdout);
}

void catchSIGTSTP(int signo)
{
    if (allowBackground)
    {
        allowBackground = false; //if not in foreground only mode
        char *message = "\nEntering foreground-only mode (& is now ignored)\n:";
        write(STDOUT_FILENO, message, 51);
        fflush(stdout); //If I do this, new prompt will disappear
                        //set allowBackground bool to false
    }
    else if (!allowBackground)
    {
        allowBackground = true; //if in foreground only mode
        char *message = "\nExiting foreground-only mode\n:";
        write(STDOUT_FILENO, message, 31);
        fflush(stdout); //set allowBackground bool to true
    }
}

void ifRedirection(struct Commands *Commands)
{
    int targetFD;
    char *fileName = NULL;

    int i;
    for (i = 1; i < Commands->arguNum; i++)
    {
        if ((strcmp(Commands->argv[i], "<") == 0) || (strcmp(Commands->argv[i], ">") == 0))
        {
  
            fileName = strdup(Commands->argv[i + 1]);

            if (Commands->background == true)//if the process is running on background
            {

                targetFD = open("/dev/null", O_RDONLY);//put the output to somewhere not stdin 
                if (dup2(targetFD, STDIN_FILENO) == -1)
                {
                    fprintf(stderr, "Error redirecting");//if the redirecting occurs wrongly 
                    exit(1);//exit and set status to 1
                };
               
            }
            else
            {
                if (strcmp(Commands->argv[i], "<") == 0)// get the resource from a specified file
                {
                    targetFD = open(fileName, O_RDONLY);//open the file
                    if (targetFD == -1)
                    {
                        fprintf(stderr, "Cannot open %s for input\n", fileName);//if file doesn't exist 
                        exit(1);
                    }
                    if (dup2(targetFD, STDIN_FILENO) == -1)
                    {
                        fprintf(stderr, "Error redirecting");//if the redirecting occurs wrongly
                        exit(1);
                    }
                }
                else
                {
                    targetFD = open(fileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);//open the file, if not, create one and overwrite it 

                    if (targetFD < 0)
                    {
                        fprintf(stderr, "Cannot open %s for ouput\n", fileName);//if fail to open 
                        exit(1);
                    }
                    if (dup2(targetFD, STDOUT_FILENO) < 0)//if the redirecting occurs wrongly 
                    {
                        fprintf(stderr, "Error redirecting");
                        exit(1);//exit and set status to 1
                    };
                }
            }
            close(targetFD);
            free(fileName);
            Commands->argv[i] = NULL; //So the evecvp could understand my commands, ignore > or <
            Commands->background = false;
        }
    }
 
}

void execommand(struct Commands *Commands)
{

    char *lineEntered = NULL; // Holds the user input
    char *input = NULL;       // Holds the expanded user input
    int childExitMethod;
    char *pidSigan = "$$";
    // Get the parent process ID and convert it to a string
    int pid = getpid();


    //^C only terminates the foreground command if one is running
    struct sigaction SIGINT_action = {{0}}; //Completely initialize this complicated struct to be empty
    SIGINT_action.sa_handler = catchSIGINT;//change SIGINT_action to catchSIGINT
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &SIGINT_action, NULL);

    //^Z hit once go only foreground mode, hit again go normal mode
    struct sigaction SIGTSTP_action = {{0}};
    SIGTSTP_action.sa_handler = catchSIGTSTP;//change SIGTSTP_action to catchSIGTSTP
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = SA_RESTART;//reset the flag 
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    while (1)
    {

        // Get input from user and expand all instances of "$$" into process ID

        lineEntered = getInput();
        lineEntered = expandDuoSigan(lineEntered);

        // Parse line entered into command struct
        setCommand(Commands, lineEntered);
        int j;
        j = Commands->arguNum;
 
	
			  if (strcmp(Commands->argv[j - 1], "&") == 0)
			  {
				  Commands->background = true;
				  Commands->argv[j - 1] = NULL;
				  Commands->arguNum--;
			  }
		    if(allowBackground == false){
          Commands->background = false;
        }
        convertPid(Commands, pidSigan);
        fflush(stdout);

        // If there's no command to be evaluated
        if (!Commands->arguNum)
        {

            // Free up memory allocated to prevent memory leak
            // Continue the while loop
            free(lineEntered);
            lineEntered = NULL;
            continue;

            // If command is a built-in command (cd, status, exit)
        }

        // If command is a built-in command (cd, status, exit)

        else if (strcmp(Commands->argv[0], "cd") == 0 || strcmp(Commands->argv[0], "exit") == 0 || strcmp(Commands->argv[0], "status") == 0)
        {
            // Run the built-in command
            runBuiltIn(Commands, lineEntered, input);

            // If command is not a built-in command
        }
        else
        {
            pid_t spawnPid = fork();
            switch (spawnPid)
            {

            // Fail to spawn a new process
            case -1:
                perror("Hull Breach!\n"); // Print out error message
                freeCommands(Commands);             // Free up memory allocated before exiting
                free(lineEntered);
                free(input);
                exit(1);
                break;

            // Child process
            case 0:
                
                ifRedirection(Commands);
                execvp(Commands->argv[0], Commands->argv);//run nonbuilt in commands
                perror(Commands->argv[0]);//if something wrong, print message
                freeCommands(Commands);
                free(lineEntered);
                exit(1);
                break;
            default:
                // If it is a background process
                if (Commands->background == true)
                {
                    backgroundPid[nPid] = spawnPid;
                    nPid++;

                    // Print out the child process id
                    printf("background pid is %d\n", spawnPid);
                    childPID_intent = spawnPid;
                    fflush(stdout);
                }

                // If it is a foreground process
                // Wait until the child process has terminated
                else
                {
                    waitpid(spawnPid, &childExitMethod, 0);

                    // If the child process terminated successfully
                    //if (WIFEXITED(childExitMethod)) {
                    if (WIFEXITED(childExitMethod))
                    {
                        ;
                        // Store the exit status of child process
                        // Set the terminating signal to -1 (process not terminated by signal)
                        exitStatus = WEXITSTATUS(childExitMethod);
                        termSignal = -1;
                    }
                    else
                    {

                        // Store the signal that terminates child process
                        // Set the exit status to -1 (process did not terminate successfully)
                        termSignal = WTERMSIG(childExitMethod);
                        exitStatus = -1;

                        // Print out the terminating signal
                        printf("terminated by signal %d\n", termSignal);
                        fflush(stdout);
                    }
                }
                break;
            }
        }
        // Free the memory allocated to prevent memory leak
        free(lineEntered);
        lineEntered = NULL;
    }
}

int main()
{
    struct Commands *Commands = CommandsCreate(); // Create a new command struct
    execommand(Commands);//run the entered commands
    return 0;
}