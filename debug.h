/*
 * debug.h by ArcWand
 * for CS 2200 Project 4
 * v2.2
 */

#pragma once

#ifndef DEBUG_H
#define DEBUG_H

#include <pthread.h>

#include "os-sim.h"
#include "student.h"

/*
 * Each of the debug functions takes two parameters; a char and a pointer.
 * Here are the valid characters and what they do:
 *  - 'd': Print a Data string.
 *     debug3('d', "Your text here");
 *  - 'i': Print an Integer.
 *     debug3('i', &your_int);
 *  - 'p': Print a Process. 'P' to print details
 *     debug3('p', &current[i]);
 *  - 'q': Print a Queue. 'Q' to print details
 *     debug3('q', rq);
 * 
 * The following are for internal use. They are automatically called by
 * functions in the next section.
 *  - 'l': Show that a mutex is waiting to Lock. Called by default if you
 *         use the debugging methods to interact with pthreads.
 *  - 'm': Indicate that a Mutex is locked. Called by default if you use
 *         the debugging methods to interact with pthreads.
 *  - 'u': Indicate that a mutex is Unlocking. Called by default if you use
 *         the debugging methods to interact with pthreads.
 * 
 *  - 'w': Indicate waiting for a condition. Called by default if you use the
 *         debugging methods to interact with pthreads.
 *  - 's': Indicate that a condition has been Signaled. Called by default if
 *         you use the debugging methods to interact with pthreads.
 *  - 'b': Indicate that a condition has been Broadcast. Called by default if
 *         you use the debugging methods to interact with pthreads.
 *  - 'c': Indicate that a Condition has been signalled and the mutex is being
 *         unlocked. Called by default if you use the debugging methods to interact
 * 		with pthreads. 
 */
#define debug1(type, p) debug_printer(__func__, 1, type, p)
#define debug2(type, p) debug_printer(__func__, 2, type, p)
#define debug3(type, p) debug_printer(__func__, 3, type, p)

/*	
 * The library provides functions for interacting with pthreads, namely for
 * locking and unlocking mutexes, and signalling, broadcasting, and waiting on
 * conditions. These are wrappers around the pthread functions, and are called
 * as follows:
 * 
 *     mutex_lock(&mutex);
 * 	   mutex_unlock(&mutex);
 *     cond_wait(&cond, &mutex);
 *     cond_signal(&cond);
 *     cond_broadcast(&cond); 
 */
#define mutex_lock(mutex)      debug_mutex_lock(__func__, mutex)
#define mutex_unlock(mutex)    debug_mutex_unlock(__func__, mutex)
#define cond_wait(cond, mutex) debug_cond_wait(__func__, cond, mutex)
#define cond_signal(cond)      debug_cond_signal(__func__, cond)
#define cond_broadcast(cond)   debug_cond_broadcast(__func__, cond)

/*
 * The library requires some initialization. This is done via debug_init().
 * debug_init's first parameter is the debug level. The preferred way of
 * setting this is to set DBGLVL immediately after including the library. You
 * should add the following two lines to the beginning of your student.c file.
 * 
 * DBGLVL can be set to the level you want depending
 * 
 *     #include "debug.h"
 *     #define DBGLVL 2 
 * 
 * The second parameter is how many pointers to name. This part is used to
 * display the name of variables when e.g. locking mutexes. If you did not add
 * any, then you should have 3: current_mutex, queue_mutex, and
 * queue_not_empty. Thus, you would name them like so:
 * 
 *     debug_init(DBGLVL, 3, &current_mutex, "current_mutex", &queue_mutex,
 *         "queue_mutex", &queue_not_empty, "queue_not_empty");
 * 
 * There are 3 provided debug levels. The functions for each are debug1, debug2,
 * and debug3. If DBGLVL is set as follows, then:
 *  - int DBGLVL = 0: No debug output at all.
 *  - int DBGLVL = 1: Only debug1 output.
 *  - int DBGLVL = 2: debug1 and debug2 output.
 *  - int DBGLVL = 3: All debug output." 
 * 
 * debug_init must be called before any other debug functions
 */
void debug_init(int level, unsigned int *age_weight, unsigned long count, ...);

/*
 * debug_destroy should be called to free resources after the program is
 * complete.
 */
void debug_destroy(void);

/* Prototypes */
void debug_printer(const char *func, int level, char type, const void *p);
void debug_mutex_lock(const char *func, pthread_mutex_t *mutex);
void debug_mutex_unlock(const char *func, pthread_mutex_t *mutex);
void debug_cond_wait(const char *func, pthread_cond_t *cond, pthread_mutex_t *mutex);
void debug_cond_signal(const char *func, pthread_cond_t *cond);
void debug_cond_broadcast(const char *func, pthread_cond_t *cond);

#endif
