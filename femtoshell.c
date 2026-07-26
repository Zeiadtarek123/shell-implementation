#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD 100000
#define MAX_ARGS 5000

int main(int argc, char *argv[]) {
    
    char input[MAX_CMD];
    char *cmd_argv[MAX_ARGS];
    int last_status = 0; 

    while (1) {
        write(1, "femto shell prompt > ", 21);
        
        int i = 0;
        char c;
        int n;

        while ((n = read(0, &c, 1)) > 0) {
            if (c == '\n') {
                break;
            }
            if (i < MAX_CMD - 1) {
                input[i++] = c;
            }
        }

        if (n <= 0 && i == 0) {
            break;
        }
        input[i] = '\0';

        int cmd_argc = 0;
        int idx = 0;

        while (idx < i) {
            while (input[idx] == ' ') {
                input[idx] = '\0';
                idx++;
            }
            if (input[idx] != '\0') {
                if (cmd_argc < MAX_ARGS) {
                    cmd_argv[cmd_argc++] = &input[idx];
                }
                while (idx < i && input[idx] != ' ') {
                    idx++;
                }
            }
        }

        if (cmd_argc == 0) {
            continue;
        }

        if (strcmp(cmd_argv[0], "echo") == 0) {
            int j;
            for (j = 1; j < cmd_argc; j++) {
                int len = 0;
                while (cmd_argv[j][len] != '\0') {
                    len++;
                }

                if (write(1, cmd_argv[j], len) < 0) {
                    exit(-1);
                }

                if (j < cmd_argc - 1) {
                    if (write(1, " ", 1) < 0) {
                        exit(-2);
                    }
                }
            }

            if (write(1, "\n", 1) < 0) {
                exit(-3);
            }
            last_status = 0; 
        } else if (strcmp(cmd_argv[0], "exit") == 0) {
            write(1, "Good Bye\n", 9); 
            break;
        } else {
            write(1, "Invalid command\n", 16);
            last_status = 1; 
        }
    }

    return last_status; 
}
