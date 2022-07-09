Proposed solution for the implementation of locks and condition variables (LAB 3).

In order to be used, the attached files must be copied respectively to:
- kern/include/synch.h
- kern/thread/synch.c
So both replace existing files and were written assuming the existence of the "synch" option.
ATTENTION: the option must be defined in kern/conf/conf.kern and ENABLED in the configuration file used (could
be SYNCH, but it is not mandatory, the option can be enabled in DUMBVM, in SYSCALLS, GENERIC or other).

Within synch.h, an additional constant (USE_SEMAPHORE_FOR_LOCK) has
been used (it must be defined explicitly).
This constant decides whether the locks are made by means of a
semaphore, or with a wait_channel.
I inserted comments, that should be understandable, to explain the choices made.

The sy1, sy2, sy3, sy4 tests should not give problems. I tried them with different configurations in sys161.conf
(1 and 4 cpu), with modified dumbvm and active syscalls. Beware that there may be problems
related to other parts of the kernel (e.g. dumbvm and/or syscalls).

G.Cabodi
