#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 4096*3

int main(int argc, char *argv[]) 
{
	int last_status = 0;
    char line[MAX_LINE];
    
    while (1) {
        printf("Pico shell prompt > ");
        fflush(stdout); // Force prompt to print before reading input

        // Read user input
        if (fgets(line, MAX_LINE, stdin) == NULL) {
            	break;
        }

        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Dynamic allocation for arguments array
        int bufsize = 16;
        int position = 0;
        char **tokens = (char**)malloc(bufsize * sizeof(char *));
        
        if (tokens == NULL) {
            write(2,"malloc failed",strlen("malloc failed"));
            exit(-1);
        }

        // Parse line into space-separated arguments
        char *token = strtok(line, " \t");
        while (token != NULL) {
            tokens[position] = token;
            position++;

            // Reallocate if we exceed the current buffer size
            if (position >= bufsize) {
                bufsize *= 2;
                tokens = (char**)realloc(tokens, bufsize * sizeof(char *));
                if (tokens == NULL) {
                    write(2,"realloc failed",strlen("realloc failed"));
                    exit(-2);
                }
            }
            token = strtok(NULL, " \t");
        }
        tokens[position] = NULL; // NULL-terminate the arguments list

        // Empty command (user just pressed Enter)
        if (position == 0) {
            free(tokens);
            continue;
        }

        // 1. Built-in: exit
        if (strcmp(tokens[0], "exit") == 0) {
            printf("Good Bye\n");
            free(tokens);
            break;
        } 
        // 2. Built-in: echo
        else if (strcmp(tokens[0], "echo") == 0) {
            for (int i = 1; i < position; i++) {
                printf("%s", tokens[i]);
                if (i < position - 1) {
                    printf(" ");
                }
            }
            printf("\n");
        } 
        // 3. Built-in: pwd
        else if (strcmp(tokens[0], "pwd") == 0) {
            char cwd[MAX_LINE];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
		 last_status = 1;
               	 write(2,"pwd failed",strlen("pwd failed"));
            }
        } 
        // 4. Built-in: cd
        else if (strcmp(tokens[0], "cd") == 0) {
            if (tokens[1] != NULL) {
                if (chdir(tokens[1]) != 0) {
			
                fprintf(stderr,"cd: %s: No such file or directory\n",tokens[1]);
			    last_status =1;
                }
		else last_status = 0;
            }
	    else last_status = 1;
        } 
        // 5. External Commands
        else {
            pid_t pid = fork();
            
            if (pid == 0) {
                // Child process executes the command
                if (execvp(tokens[0], tokens) == -1) {
			last_status = 127;
                    fprintf(stderr, "%s: command not found\n", tokens[0]);
                    exit(127);
		    
                }
            } else if (pid < 0) {
                // Fork failed
                write(2,"fork failed",strlen("fork failed"));
            } else {
                // Parent process waits for child to finish
		int status;
                waitpid(pid,&status,0);
		if (WIFEXITED(status)) {
                    last_status = WEXITSTATUS(status);
                } else {
                    last_status = 1;
                }
            }
        }

        
        free(tokens);
    }

    return last_status;
}
