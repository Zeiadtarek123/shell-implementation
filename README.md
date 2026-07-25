Femto Shell in C
A minimalist Unix shell implementation written in C that supports basic built-in commands (echo and exit) using low-level system calls and standard library routines.

Features
Custom Prompt: Displays femto shell prompt > to wait for user input.

Echo Command: Echoes any text entered by the user back to standard output.

Exit Command: Prints a farewell message (Good Bye :)) and terminates the shell.

Error Handling: Outputs an Invalid command message for any unrecognized input.

Compilation and Execution
Compilation Command
Compile the program using gcc:

Bash
gcc -o femtoshell femtoshell.c
Running the Shell
Execute the compiled binary:

Bash
./femtoshell
Example Usage
Plaintext
femto shell prompt > echo Hello my shell
Hello my shell
femto shell prompt > echo You are super
You are super
femto shell prompt > ls
Invalid command
femto shell prompt > exit
Good Bye :)
Implementation Notes
This utility directly parses user input read from file descriptor 0 (stdin) and uses low-level write system calls to handle output, matching standard Unix specifications without relying on complex shell parsing frameworks.
