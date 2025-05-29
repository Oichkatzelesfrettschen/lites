# POSIX Implementation Roadmap

This document outlines the high level milestones for bringing up a usable POSIX environment on top of the modern Lites sources. Each item links to the relevant page in the included copy of the POSIX.1‑2017 specification under `docs/books/susv4-2018-main/`.

- **Build infrastructure** – follow the requirements in [c99](books/susv4-2018-main/utilities/c99.html) when compiling the userland components.
- **Process control** – implement [fork](books/susv4-2018-main/functions/fork.html) and the related `execve` family.
- **Memory management** – provide [mmap](books/susv4-2018-main/functions/mmap.html) and `mprotect` for anonymous and file‑backed mappings.
- **Scheduling** – expose [sched_yield](books/susv4-2018-main/functions/sched_yield.html) and other simple scheduler controls.
- **Threads** – bring up [pthread_create](books/susv4-2018-main/functions/pthread_create.html) using Mach cthreads underneath.
- **Interprocess communication** – support [mq_open](books/susv4-2018-main/functions/mq_open.html) and named semaphores.
- **File system calls** – implement [open](books/susv4-2018-main/functions/open.html), `read`, `write` and associated metadata queries.
- **Networking** – wire up [connect](books/susv4-2018-main/functions/connect.html) to the capability backed socket layer.

These stages can be developed independently and validated against the test suites located in `tests/`.  The roadmap will expand as additional parts of the specification become necessary.
