# CS2200-Proj4-DebuggingTools
A repository containing a set of custom tools I wrote to help debug Project 4
(multi-threaded scheduling simulation) of Georgia Tech's CS 2200 Systems and
Networks in Spring 2024. Due to Academic Integrity Policies, the rest of the
project code is not included.

## Debugging library for CS 2200 Project 4 by ArcWand and Hzl
In `student.h`, include `debug.h`.

Then, near the top of your `student.c`, make sure to have this block of code:

    #ifndef DEBUG_H
        #define debug_init(lvl, num_mutexes, ...)
        #define debug_destroy()
        #define debug1(type, p)
        #define debug2(type, p)
        #define debug3(type, p)
        #define mutex_lock(mutex)      pthread_mutex_lock(mutex)
        #define mutex_unlock(mutex)    pthread_mutex_unlock(mutex)
        #define cond_wait(cond, mutex) pthread_cond_wait(cond, mutex)
        #define cond_signal(cond)      pthread_cond_signal(cond)
        #define cond_broadcast(cond)   pthread_cond_broadcast(cond)
    #endif

This will prevent errors when submitting to GradeScope.  
Next, call `debug_init()` in main with the following parameters:

 - level: The debug level (1-3) (0 is off)
 - age_weight: A pointer to the age_weight variable
 - count: The number of pointers you want to map to names
 - pointers: A list of pointers to map to names
 - names: A list of names to map to pointers

The recommended way to do this is as follows:

First, immediately after the includes, have

    int DBGLVL =  3;

The value of DBGLVL will be the debug level. Having it at the top will allow
you to easily change the debug level.

Then, as the first line of main(), have

    debug_init(DBGLVL, &age_weight, 3, &current_mutex, "current_mutex",
        &queue_mutex, "queue_mutex", &queue_not_empty, "queue_not_empty");

After calling  debug_init(), you can use the following functions:

    debug1(char  type, void*  p) // Print a debug message at level 1
    debug2(char  type, void*  p) // Print a debug message at level 2
    debug3(char  type, void*  p) // Print a debug message at level 3
    mutex_lock(&mutex)           // Lock a mutex with debug messages
    mutex_unlock(&mutex)         // Unlock a mutex with debug messages
    cond_wait(&cond, &mutex)     // Wait on a cond variable with debug messages
    cond_signal(&cond)           // Signal a condition variable
    cond_broadcast(&cond)        // Broadcast a cond variable with debugging

The 'type' parameter in the debug functions is a character that specifies the
type of the  'p' parameter.

The following are provided for users:
-  'd': Print a data string
-  'i': Print an integer
-  'p': Print a process name
-  'p': Print a process (pcb_t)
-  'q': Print a queue
-  'Q': Print a queue in detail

The other types of debug messages are called via other functions in the
debugger. Feel free to use them but realistically you should be using the other
provided functions, which invoke these internally.
-  'l': Indicate waiting for a mutex to lock
-  'm': Indicate a mutex has been locked
-  'u': Indicate unlocking a mutex
-  'w': Indicate waiting on a cond variable
-  's': Indicate signaling a cond variable
-  'b': Indicate broadcasting a cond variable
-  'c': Indicate a cond variable has been signaled

For more information, read the following examples of using the debugger or read
the comments in debug.h.

### Using debug.h

The DBGLVL indicates the verbosity of the debug output. Level 3 is used by the
pthread wrapper functions. It is recommended to use level 2 to indicate calling
of functions, and level 1 to help with targeting specific issues.

It can be useful to print the process when context switching, or print the
cpu_id that a function is called with.

For more information about how to use debug.h, read the comments in the file
itself. Implementation is in debug.c.

# Local Autograder
An autograder was provided via GradeScope. However, this solution ran slowly,
and would give often useless errors. Thus, I created `local-autograder.py`.

## Running
Depends on python 3. Should be placed in the same directory as the compiled
`os-sim` binary, or you can modify the `BIN` variable to include a path to the
binary.

Run using the command `python3 local-autograder.py`. It will generate a
directory called `output`, which contains logs of each run, and write the stats
to both stdout and a file called `stats.txt`.

