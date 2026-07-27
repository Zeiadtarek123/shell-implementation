Custom C Shell Implementation Series
A progressive collection of minimalist Unix-style command-line shell implementations written in C, developed using low-level system calls (read, write, fork, execvp, waitpid) and standard POSIX functions.

Implemented Shell Architectures
1. Femto Shell
A foundational shell implementation constructed to analyze core command parsing logic, prompt routing, and internal command processing entirely within a single execution context without spawning child processes.

Supported Built-in Utilities:

echo: Prints user-provided text strings directly to standard output.

exit: Gracefully terminates the shell session and propagates status tracking.

Core Characteristics: Custom prompt formatting, manual stream management utilizing standard file descriptors, and targeted error handlers for unsupported inputs.

2. Pico Shell
An advanced architectural upgrade introducing process management and dynamic memory allocation to support both built-in handlers and external system binaries.

Supported Built-in Utilities:

echo, exit, pwd (print working directory), and cd (change working directory).

Supported External Commands:

Any valid binary executable present within the operating system environment (e.g., ls, cat), spawned dynamically through fork() and executed via execvp().

Core Characteristics: Dynamic argument tokenization with automatic buffer reallocation (malloc and realloc), complete exit status emulation ($? tracking), and precise file descriptor inheritance mirroring standard Unix specifications.

Compilation and Execution Instructions
Compiling and Running Femto Shell
Bash
gcc -o femtoshell femtoshell.c
./femtoshell
Compiling and Running Pico Shell
Bash
gcc -o picoshell picoshell.c
./picoshell
Usage Examples
Femto Shell Example Session
Plaintext
femto shell prompt > echo System programming is engaging
System programming is engaging
femto shell prompt > unsupported_command
Invalid command
femto shell prompt > exit
Good Bye
Pico Shell Example Session
Plaintext
Pico shell prompt > pwd
/home/ziad/embedded
Pico shell prompt > cd /tmp
Pico shell prompt > pwd
/tmp
Pico shell prompt > ls -l
total 0
Pico shell prompt > exit
Good Bye
Future Development Roadmap
To continue advancing through the principles of operating systems and advanced systems programming, future iterations of this project will incorporate:

Nano Shell: Implementation of input/output redirection operators (> and <) alongside pipeline mechanisms (|) to facilitate inter-process communication.

Micro Shell: Integration of background process execution (&), interactive job control (jobs, fg, bg), and asynchronous signal management (SIGINT, SIGTSTP).
