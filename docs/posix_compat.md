# POSIX compatibility

This tree provides a very small subset of POSIX threads.  Only thread
creation, joining, detaching and basic mutex operations are available.
The implementation wraps the Mach cthreads library and uses the simple
spin locks from `kern/spinlock.h`.  Condition variables and other
pthreads features are not implemented.
