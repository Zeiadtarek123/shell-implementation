# Custom C Shell Implementation Series

A progressive collection of minimalist Unix-style command-line shell implementations written in C, developed using low-level system calls (`read`, `write`, `fork`, `execvp`, `waitpid`) and standard POSIX functions.

---

## Implemented Shell Architectures

### 1. Femto Shell
A foundational shell implementation constructed to analyze core command parsing logic, prompt routing, and internal command processing entirely within a single execution context without spawning child processes.

* **Supported Built-in Utilities:**
  * `echo`: Prints user-provided text strings directly to standard output.
  * `exit`: Gracefully terminates the shell session and propagates status tracking.
* **Core Characteristics:** Custom prompt formatting, manual stream management utilizing standard file descriptors, and targeted error handlers for unsupported inputs.

### 2. Pico Shell
An advanced architectural upgrade introducing process management and dynamic memory allocation to support both built-in handlers and external system binaries.

* **Supported Built-in Utilities:**
  * `cd`: Changes the current working directory.
  * `exit`: Gracefully terminates the shell session.
* **Supported External Commands:**
  * Any valid binary executable present within the operating system environment (e.g., `ls`, `pwd`, `cat`), spawned dynamically through `fork()` and executed via `execvp()`.
* **Core Characteristics:** Dynamic argument tokenization with automatic buffer reallocation (`malloc` and `realloc`), complete exit status emulation (`$?` tracking), and precise file descriptor inheritance mirroring standard Unix specifications.

### 3. Nano Shell
An intermediate shell extending Pico Shell to support local variables, environment variables, and dynamic substitution.

* **Supported Built-in Utilities:**
  * `cd`, `exit`.
  * `export`: Adds an existing local variable into the inherited environment variables using `putenv()`.
* **Core Characteristics:** Custom key-value data structure to parse and store strict `Variable=Value` assignments, inline variable expansion parsing (e.g., substituting `$var` within strings), and dynamic memory cleanup to prevent leaks.

### 4. Micro Shell
An advanced shell extending the Nano Shell parser to support multiple file input/output redirections.

* **Supported External I/O Operators:**
  * `<`: Redirects standard input (`STDIN`) to read from a specified file.
  * `>`: Redirects standard output (`STDOUT`) to write to a specified file.
  * `2>`: Redirects standard error (`STDERR`) to write to a specified file.
* **Core Characteristics:** Stream redirection via `open()` and `dup2()` executed strictly left-to-right inside the child process, integrated with dynamic permission and file existence error handling.

---

## Compilation and Execution Instructions

### Femto Shell
```bash
gcc -o femtoshell femtoshell.c
./femtoshell
