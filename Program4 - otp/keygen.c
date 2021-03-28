/***********************************************************************************************************************************************
 * Author: Han Jiang
 * Assignment: Prorgam 4 OTP
 * File name: keygen.c
 * Submission date: Dec 6 2019
 * Description: This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters, 
 * generated using the standard UNIX randomization methods. Do not create spaces every five characters, as has been historically done. 
 * Note that you specifically do not have to do any fancy random number generation: we’re not looking for cryptographically secure random number generation! 
 * rand() is just fine. The last character keygen outputs should be a newline. All error text must be output to stderr, if any.
 * ********************************************************************************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void random_string(char * string, size_t length)
{
    char pool[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; //the pool contains 27 allowed character, last one is space
    size_t poolsize = strlen(pool);//get the pool size, it is the range of indexs 
    size_t index;
    size_t i;

    srand((unsigned)time(NULL));// Seed random number generator

    for(i = 0; i < length && poolsize > 0; i++)
    {
        index = rand() % poolsize;       // a random index into the pool
        string[i] = pool[index];         // take that character matches the index
        
    }                                   

}

int main(int argc,char* argv[])
{
    // Check number of arguments 
    if ((argc < 2)||(argc > 4)) {
        fprintf( stderr, "at least 2  and no more 4 arguments for keygen!\n" );//error text must be out to stderr
        exit(1);
    }
    int k = atoi(argv[1]); // convert strings of numeric characters to their int
    char key[k + 1];         // adequate length so that 
    memset(key, '\0', k+1);//make sure key string is clean
    random_string(key, k); //take k(user choosed) as the length of string
    key[k] = '\n'; ////the last character  keygen output is a newline
    printf("%s", key);  
    //printf("%d\n",strlen(key));
    return 0;
}