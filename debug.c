/*
 * debug.h by ArcWand and Hzl
 * CS 2200 Project 4
 * v2.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "debug.h"

/* Helper functions */
static void assert_init();
// Getters
static int get_thread_num();
const char* get_name(const void* p);
static void print_pcb(const pcb_t *p);
static void print_queue(const queue_t *q);
static void print_queue_full(const queue_t *q);

// Variables
long thread_set[32] = {0};
pthread_mutex_t debug_mutex;
void** pointers;
char** names;

/* Implementation */

int dbglvl;
unsigned int *age_weight_ext;
bool DEBUG_INITIALIZED = false;

// Initialization function. Must be called before any other debug functions
// Takes the debug level and a count, followed by pairs of pointers and names
void debug_init(int level, unsigned int *age_weight, unsigned long count, ...) {
	// Prevent multiple initializations
	if (DEBUG_INITIALIZED) {
		printf("Debugging should only be initialized once.\n");
		exit(1);
	}
 
	// Initialization code
	pthread_mutex_init(&debug_mutex, NULL);
	dbglvl = level;
	age_weight_ext = age_weight;

	// Set name mappings
	va_list argp;
	va_start(argp, count);
	pointers = (void**) malloc(count * sizeof(void*));
	names    = (char**) malloc(count * sizeof(char*));
	for (unsigned long i = 0; i < count; i++) {
		pointers[i] = va_arg(argp, void*);
		names[i]    = va_arg(argp, char*);
	}
	va_end(argp);

	// Set initialized flag to prevent multiple initializations
	DEBUG_INITIALIZED = true;
}

// Helper function to ensure that debug_init has been called
static void assert_init() {
	if (!DEBUG_INITIALIZED) {
		printf("Debugging has not been initialized. Please call debug_init() "
			   "before using debug functions.\n");
		printf("For help on how to use debug.h, call debug_help()\n");
		exit(3);
	}
}

void debug_destroy() {
	assert_init();

	free(pointers);
	free(names);
	pthread_mutex_destroy(&debug_mutex);
	DEBUG_INITIALIZED = false;
}

// Get the thread number for the current thread
// Returns -1 if the thread number is not found
// Done by caching the thread number thread_set
static int get_thread_num() {
	assert_init();

	// Get id of current thread
	pthread_t self = pthread_self();

	// Search for the thread in the thread_set array
	for (int i = 0; i < 32; i++) {
		// If we encounter an empty spot in the array, then the thread is not
		// in the array and we append it.
		if (thread_set[i] == 0) {
			thread_set[i] = (long) self;
			return i;
		}

		// If we find the thread in the array, then return the index it is at.
		if (thread_set[i] == (long) self) {
			return i;
		}
	}
	return -1;
}

const char* get_name(const void* p) {
	assert_init();
	for (int i = 0; i < 32; i++) {
		if (pointers[i] == p) {
			return names[i];
		}
	}
	return "?";
}

// Print { pid, name, priority, state, enqueue, arrival, next }
void print_pcb(const pcb_t *p) {
	assert_init();
	printf(" { pid: %d, name: %s, priority: %d (%u), ",
			p->pid, p->name, p->priority,
			p->priority + (get_current_time() - p->enqueue_time)*(*age_weight_ext));
	switch (p->state) {
		case PROCESS_NEW:        printf("state: NEW"); break;
		case PROCESS_READY:      printf("state: READY"); break;
		case PROCESS_RUNNING:    printf("state: RUNNING"); break;
		case PROCESS_WAITING:    printf("state: WAITING"); break;
		case PROCESS_TERMINATED: printf("state: TERMINATED"); break;
	}
	printf(", enqueue: %d, arrival: %d, next: %s }",
			p->enqueue_time, p->arrival_time,
			p->next ? p->next->name : "NULL");
}

// Print a queue
void print_queue(const queue_t *q) {
	assert_init();

	// In the event of an infinite loop, we want to break out
	int failsafe = 0;

	// Start at the head of the queue and print each element
	printf("< ");
	const pcb_t *p = q->head;
	while (p) {
		printf("%s ", p->name);
		p = p->next;

		// Break out if we have an infinite loop
		if (failsafe++ > 100) {
			printf("ERROR: Infinite loop detected in print_queue()\n");
			break;
		}
	}
	printf("<");
}

void print_queue_full(const queue_t *q) {
	assert_init();

	// In the event of an infinite loop, we want to break out
	int failsafe = 0;

	// Start at the head of the queue and print each element
	printf("(head) < ");
	const pcb_t *p = q->head;
	while (p) {
		printf("\n");
		print_pcb(p);
		p = p->next;

		// Break out if we have an infinite loop
		if (failsafe++ > 100) {
			printf("ERROR: Infinite loop detected in print_queue_full()\n");
			break;
		}
	}
	printf("\n< (tail)");
}

// Base function for printing debug messages
// Takes in the name of the calling function, the debug level, the type of the
// message, and the pointer to the data to print
void debug_printer(const char *func, int level, char type, const void *p) {
	// Check if we should be printing
	assert_init();
	if (level > dbglvl) { return; }

	// Acquire mutex so that printing won't be interrupted
	pthread_mutex_lock(&debug_mutex);

	// Track the thread number
	printf("l%d - %x: %s: ", level, get_thread_num(), func);

	// Cast the pointer to a string, as many of the types will use it as such.
	const char *msg = (const char *) p;
	switch (type) {
		case 'd': printf("%s", msg); break;
		case 'i': printf("%d", *(const unsigned int *)p); break;

		case 'p': printf("%s", ((const pcb_t *)p)->name); break;
		case 'P': print_pcb((const pcb_t *) p); break;
		case 'q': print_queue((const queue_t *) p); break;
		case 'Q': print_queue_full((const queue_t *) p); break;

		case 'l': printf("Locking mutex:     %s", msg); break;
		case 'm': printf("Mutex locked:      %s", msg); break;
		case 'u': printf("Unlocking mutex:   %s", msg); break;

		case 'w': printf("Waiting on cond:   %s", msg); break;
		case 's': printf("Signaling cond:    %s", msg); break;
		case 'b': printf("Broadcasting cond: %s", msg); break;
		case 'c': printf("Cond signaled:     %s", msg); break;
	}
	printf("\n");

	pthread_mutex_unlock(&debug_mutex);
}

// Wrapper functions for pthread functions
// Mutex lock; has two debug messages to help catch deadlocks
void debug_mutex_lock(const char *func, pthread_mutex_t *mutex) {
	debug_printer(func, 3, 'l', get_name(mutex));
	pthread_mutex_lock(mutex);
	debug_printer(func, 3, 'm', get_name(mutex));
}
void debug_mutex_unlock(const char *func, pthread_mutex_t *mutex) {
	debug_printer(func, 3,'u', get_name(mutex));
	pthread_mutex_unlock(mutex);
}
// Condition wait; has debug messages to indicate waiting and cond signaled
// Note that the mutex is unlocked while waiting, and relocked after, so we
// want to see this in the debug output
void debug_cond_wait(const char *func,
		pthread_cond_t *cond, pthread_mutex_t *mutex) {
	debug_printer(func, 3, 'u', get_name(mutex));
	debug_printer(func, 3, 'w', get_name(cond));
	pthread_cond_wait(cond, mutex);
	debug_printer(func, 3, 'c', get_name(cond));
	debug_printer(func, 3, 'm', get_name(mutex));
}
void debug_cond_signal(const char *func, pthread_cond_t *cond) {
	debug_printer(func, 3, 's', get_name(cond));
	pthread_cond_signal(cond);
}
void debug_cond_broadcast(const char *func, pthread_cond_t *cond) {
	debug_printer(func, 3, 'b', get_name(cond));
	pthread_cond_broadcast(cond);
}

