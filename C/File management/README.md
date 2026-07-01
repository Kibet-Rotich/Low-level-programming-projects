# POSIX File Management (`safecopy`)

A zero-leak, raw POSIX file copy utility. Demonstrates manual heap buffer allocation and user-space to kernel-space I/O pumping without standard C library stream wrappers (`<stdio.h>`).

## Core Mechanics
* **Syscalls:** `open()`, `read()`, `write()`, `close()`.
* **Safety:** Strict byte-boundary write offsets to prevent ghost-data trailing from stale Kernel Page Caches.
* **Diagnostics:** POSIX `errno` mapping via `perror()`.

## Quickstart

```bash
gcc -o safecopy safecopy.c
./safecopy [source_file] [destination_file]