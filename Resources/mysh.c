/* 
  strtokeg - skeleton shell using strtok to parse command line
  
  usage:
  
    strtokeg
          
    reads in a line of keyboard input at a time, parsing it into
    tokens that are separated by white spaces (set by #define
    SEPARATORS).

    can use redirected input
        
    if the first token is a recognized internal command, then that
    command is executed. otherwise the tokens are printed on the
    display.
    
    internal commands:
    
        clear - clears the screen
        
        quit - exits from the program
      
 ********************************************************************
   version: 1.0
   date:    December 2003
   author:  Ian G Graham
            School of Information Technology
            Griffith University, Gold Coast
            ian.graham@griffith.edu.au
            
   copyright (c) Ian G Graham, 2003. All rights reserved.
            
   This code can be used for teaching purposes, but no warranty,
   explicit or implicit, is provided.
 *******************************************************************/
//&nbsp;
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER 1024                        // max line buffer
#define MAX_ARGS 64                            // max # args
#define SEPARATORS " \t\n"                     // token sparators

int var_glb;
   
int main (int argc, char ** argv)
{
    pid_t childPID;
	char buf[MAX_BUFFER];                      // line buffer
    char * args[MAX_ARGS];                     // pointers to arg strings
    char ** arg;                               // working pointer thru args
    char * prompt = "# " ;                    // shell prompt
    
    int var_lcl = 0;

    childPID = fork();

    if(childPID >= 0) // fork was successful
    {
        if(childPID == 0) // child process
        {
            var_lcl++;
            var_glb++;
        }
        else //Parent process
        {
            var_lcl = 10;
            var_glb = 20;
        }
    }
    else // fork failed
    {
        return 1;
    }

/* keep reading input until "quit" command or eof of redirected input */
     
    while (!feof(stdin)) { 
    
/* get command line from input */
  
        fputs (prompt, stdout);                // write prompt
        if (fgets (buf, MAX_BUFFER, stdin )) { // read a line
        
/* tokenize the input into args array */

            arg = args;
            *arg++ = strtok(buf,SEPARATORS);   // tokenize input
            while ((*arg++ = strtok(NULL,SEPARATORS)));
                                               // last entry will be NULL 
 
            if (args[0]) {                     // if there's anything there
            
/* check for internal/external command */

                if (!strcmp(args[0],"clear")) { // "clear" command
                    system("clear");
                    continue;
                }
            
                if (!strcmp(args[0],"quit"))   // "quit" command
                    break;                     // break out of 'while' loop

/* else pass command onto OS (or in this instance, print them out) */

                arg = args;
                while (*arg) fprintf(stdout,"%s ",*arg++);
                fputs ("\n", stdout);
            }
        }
    }
    return 0; 
}
