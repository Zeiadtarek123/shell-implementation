#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 4096 * 3

extern char **environ;

typedef struct {
    char *key;
    char *value;
} Var;

Var *vars = NULL;
int var_count = 0;
int var_capacity = 0;

void add_or_update_var(const char *key, const char *value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].key, key) == 0) {
            free(vars[i].value);
            vars[i].value = (char *)malloc(strlen(value) + 1);
            strcpy(vars[i].value, value);
            return;
        }
    }
    
    if (var_count >= var_capacity) {
        var_capacity = (var_capacity == 0) ? 16 : var_capacity * 2;
        vars = (Var *)realloc(vars, var_capacity * sizeof(Var));
    }
    
    vars[var_count].key = (char *)malloc(strlen(key) + 1);
    strcpy(vars[var_count].key, key);
    vars[var_count].value = (char *)malloc(strlen(value) + 1);
    strcpy(vars[var_count].value, value);
    var_count++;
}

char* get_var(const char *key) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].key, key) == 0) {
            return vars[i].value;
        }
    }
    return (char *)"";
}

char* expand_vars(const char *token) {
    char *result = (char *)malloc(MAX_LINE);
    result[0] = '\0';
    int res_idx = 0;
    int i = 0;
    
    while (token[i] != '\0') {
        if (token[i] == '$') {
            i++; 
            char var_name[256];
            int var_idx = 0;
            
            while (token[i] != '\0' && (
                   (token[i] >= 'a' && token[i] <= 'z') ||
                   (token[i] >= 'A' && token[i] <= 'Z') ||
                   (token[i] >= '0' && token[i] <= '9') ||
                   token[i] == '_')) {
                var_name[var_idx++] = token[i++];
            }
            var_name[var_idx] = '\0';
            
            char *val = get_var(var_name);
            strcpy(&result[res_idx], val);
            res_idx += strlen(val);
        } else {
            result[res_idx++] = token[i++];
        }
    }
    result[res_idx] = '\0';
    return result;
}

int main(int argc, char *argv[]) 
{
    (void)argc;
    (void)argv;

    int last_status = 0;
    char line[MAX_LINE];
    char original_line[MAX_LINE];
    
    while (1) {
        printf("Micro Shell Prompt > ");
        fflush(stdout); 

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            break;
        }

        line[strcspn(line, "\n")] = '\0';
        strcpy(original_line, line);

        int bufsize = 16;
        int position = 0;
        char **tokens = (char **)malloc(bufsize * sizeof(char *));
        
        if (tokens == NULL) {
            write(2, "malloc failed", strlen("malloc failed"));
            exit(-1);
        }

        char *token = strtok(line, " \t");
        while (token != NULL) {
            tokens[position] = token;
            position++;

            if (position >= bufsize) {
                bufsize *= 2;
                tokens = (char **)realloc(tokens, bufsize * sizeof(char *));
                if (tokens == NULL) {
                    write(2, "realloc failed", strlen("realloc failed"));
                    exit(-2);
                }
            }
            token = strtok(NULL, " \t");
        }
        tokens[position] = NULL; 

        if (position == 0) {
            free(tokens);
            continue;
        }

        if (strchr(original_line, '=') != NULL) {
            if (position != 1 || tokens[0][0] == '=' || tokens[0][strlen(tokens[0]) - 1] == '=') {
                write(2, "Invalid command\n", 16);
                last_status = 1;
                free(tokens);
                continue;
            }
            
            char *eq_ptr = strchr(tokens[0], '=');
            *eq_ptr = '\0'; 
            add_or_update_var(tokens[0], eq_ptr + 1);
            last_status = 0;
            free(tokens);
            continue;
        }

        char *allocated_tokens[bufsize];
        int alloc_count = 0;

        for (int i = 0; i < position; i++) {
            if (strchr(tokens[i], '$') != NULL) {
                tokens[i] = expand_vars(tokens[i]);
                allocated_tokens[alloc_count++] = tokens[i];
            }
        }

        char **exec_tokens = (char **)malloc((position + 1) * sizeof(char *));
        int exec_pos = 0;

        // Extract clean executable tokens ignoring redirections
        for (int i = 0; i < position; i++) {
            if (strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], "2>") == 0) {
                i++; // Skip the operator and the filename
            } else {
                exec_tokens[exec_pos++] = tokens[i];
            }
        }
        exec_tokens[exec_pos] = NULL;

        if (exec_pos == 0) {
            for (int i = 0; i < alloc_count; i++) free(allocated_tokens[i]);
            free(exec_tokens);
            free(tokens);
            continue;
        }

        if (strcmp(exec_tokens[0], "exit") == 0) {
            printf("Good Bye\n");
            for (int i = 0; i < alloc_count; i++) free(allocated_tokens[i]);
            free(exec_tokens);
            free(tokens);
            break;
        } 
        else if (strcmp(exec_tokens[0], "cd") == 0) {
            if (exec_tokens[1] != NULL) {
                if (chdir(exec_tokens[1]) != 0) {
                    fprintf(stderr, "cd: %s: No such file or directory\n", exec_tokens[1]);
                    last_status = 1;
                } else {
                    last_status = 0;
                }
            } else {
                last_status = 1;
            }
        } 
        else if (strcmp(exec_tokens[0], "export") == 0) {
            if (exec_pos > 1) {
                char *val = get_var(exec_tokens[1]);
                if (strcmp(val, "") != 0) {
                    char *env_str = (char *)malloc(strlen(exec_tokens[1]) + strlen(val) + 2);
                    sprintf(env_str, "%s=%s", exec_tokens[1], val);
                    putenv(env_str); 
                }
            }
            last_status = 0;
        } 
        else {
            pid_t pid = fork();
            
            if (pid == 0) {
                // Apply redirections strictly left-to-right inside the child process
                for (int i = 0; i < position; i++) {
                    if (strcmp(tokens[i], "<") == 0 && i + 1 < position) {
                        int fd_in = open(tokens[i+1], O_RDONLY);
                        if (fd_in < 0) {
                            char err_msg[MAX_LINE];
                            sprintf(err_msg, "cannot access %s", tokens[i+1]);
                            perror(err_msg);
                            exit(1);
                        }
                        dup2(fd_in, STDIN_FILENO);
                        close(fd_in);
                        i++;
                    } else if (strcmp(tokens[i], ">") == 0 && i + 1 < position) {
                        int fd_out = open(tokens[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out < 0) {
                            perror(tokens[i+1]);
                            exit(1);
                        }
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_out);
                        i++;
                    } else if (strcmp(tokens[i], "2>") == 0 && i + 1 < position) {
                        int fd_err = open(tokens[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_err < 0) {
                            perror(tokens[i+1]);
                            exit(1);
                        }
                        dup2(fd_err, STDERR_FILENO);
                        close(fd_err);
                        i++;
                    }
                }

                if (execvp(exec_tokens[0], exec_tokens) == -1) {
                    fprintf(stderr, "%s: command not found\n", exec_tokens[0]);
                    exit(127);
                }
            } else if (pid < 0) {
                write(2, "fork failed", strlen("fork failed"));
                last_status = 1;
            } else {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    last_status = WEXITSTATUS(status);
                } else {
                    last_status = 1;
                }
            }
        }

        for (int i = 0; i < alloc_count; i++) {
            free(allocated_tokens[i]);
        }
        free(exec_tokens);
        free(tokens);
    }

    for (int i = 0; i < var_count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    if (vars != NULL) free(vars);

    return last_status;
}
