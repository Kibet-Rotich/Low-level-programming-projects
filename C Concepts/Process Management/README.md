# Process Supervision & IPC (`Lazarus`)

A lightweight, sub-100-line process supervisor in pure C. Acts as a primitive daemonizer (conceptually similar to `systemd` or `pm2`) that attaches to an unstable child process, captures its stdout stream over the wire, diagnoses its cause of death, and resurrects it infinitely.

## Core Mechanics
* **Process Anatomy:** `fork()`, `execvp()`, `waitpid()`.
* **IPC (Inter-Process Communication):** Unnamed pipes (`pipe()`) and file descriptor redirection (`dup2()`).
* **Bitmask Autopsies:** POSIX macro evaluation (`WIFEXITED`, `WIFSIGNALED`, `WTERMSIG`) to catch hardware exceptions (e.g., `SIGSEGV` / Signal 11).

## Quickstart

```bash
gcc -o lazarus.out lazarus.c
gcc -o suicide.out suicide.c

./lazarus.out ./suicide.out