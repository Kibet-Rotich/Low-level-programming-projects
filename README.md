# Low-Level Systems Ledger

A tracking repository for deep-dive implementations of operating system primitives, POSIX system calls, and distributed backend mechanics written in pure C and Modern C++. 

Built strictly as a syntax vehicle to master under-the-hood systems architecture.

## Directory Structure

* `C Concepts/` — Pure C kernel-level abstractions (Zero standard library I/O where applicable).
  * `File management/` — Raw POSIX I/O, file descriptor tracking, and kernel buffer pumping.
  * `Process Management/` — Process supervision, IPC pipes, and signal-autopsy bitmasks.
* `C++ Concepts/` *(In Progress)* — Modern C++20 systems architecture.