#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD 4096
#define MAX_ARGS 100

int main()
{
    char input[MAX_CMD];
    char *argv[MAX_ARGS];

    while (1) {
        printf("Femto Shell prompt > ");
        
        int n = read(0, input, MAX_CMD - 1);
        if (n <= 0) {
            break;
        }
        input[n] = '\0';

        int argc = 0;
        int i = 0;

        while (i < n) {
            while (input[i] == ' ' || input[i] == '\n') {
                input[i] = '\0';
                i++;
            }
            if (input[i] != '\0') {
                argv[argc++] = &input[i];
                while (i < n && input[i] != ' ' && input[i] != '\n') {
                    i++;
                }
            }
        }

        if (argc == 0) {
            continue;
        }

        if (strcmp(argv[0], "echo") == 0) {
            int j;
            for (j = 1; j < argc; j++) {
                int len = 0;
                while (argv[j][len] != '\0') {
                    len++;
                }

                if (write(1, argv[j], len) < 0) {
                    printf("Write failed\n");
                    exit(-1);
                }

                if (j < argc - 1) {
                    if (write(1, " ", 1) < 0) {
                        printf("Write failed\n");
                        exit(-2);
                    }
                }
            }

            if (write(1, "\n", 1) < 0) {
                printf("Write failed\n");
                exit(-3);
            }
        } else if (strcmp(argv[0], "exit") == 0) {
            printf("Good Bye :)\n");
            break;
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
