
/*
 * student.c
 * Multithreaded OS Simulation for CS 2200
 *
 * This file contains the CPU scheduler for the simulation.
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os-sim.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/** Function prototypes **/
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);

/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
unsigned int cpu_count;
static pcb_t **current;
static pthread_mutex_t current_mutex;

static pcb_t *rqHead;
static pthread_mutex_t rq_mutex;

static pthread_cond_t un_idle;

static scheduler_type sAlgorithm;
static int timeslice;

static void pushRQ(pcb_t *process) {
    process->next = NULL;
    pthread_mutex_lock(&rq_mutex);
    if (rqHead == NULL) {
        rqHead = process;
    } else {
        pcb_t *currentNode = rqHead;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }
        currentNode->next = process;
    }
    pthread_cond_broadcast(&un_idle);
    pthread_mutex_unlock(&rq_mutex);
}

static pcb_t *popRQ() {
    pthread_mutex_lock(&rq_mutex);
    pcb_t *currentNode = rqHead;

    if (rqHead != NULL) {
        rqHead = rqHead->next;
        currentNode->next = NULL;
    }
    pthread_mutex_unlock(&rq_mutex);

    return currentNode;
}
static pcb_t *popMinRQ() {
    pthread_mutex_lock(&rq_mutex);
    pcb_t *currentNode = rqHead;
    pcb_t *minNode = rqHead;

    if (rqHead != NULL) {
        while (currentNode != NULL) {
            if (currentNode->time_remaining < minNode->time_remaining) {
                minNode = currentNode;
            }
            currentNode = currentNode->next;
        }

        if (minNode == rqHead) {
            rqHead = rqHead->next;
        } else {
            currentNode = rqHead;
            while (currentNode->next != minNode) {
                currentNode = currentNode->next;
            }

            currentNode->next = minNode->next;
        }
    }

    pthread_mutex_unlock(&rq_mutex);

    return minNode;
}

/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Set the currently running process using the current array
 *
 *   4. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *
 *	The current array (see above) is how you access the currently running
 *process indexed by the cpu id. See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more
 *information about it and its parameters.
 */
static void schedule(unsigned int cpu_id) {
    pcb_t *currentNode;
    if (sAlgorithm == SCHEDULER_SRTF) {
        currentNode = popMinRQ();
    } else {
        currentNode = popRQ();
    }

    if (currentNode != NULL) {
        currentNode->state = PROCESS_RUNNING;
    }
    pthread_mutex_lock(&current_mutex);
    current[cpu_id] = currentNode;
    pthread_mutex_unlock(&current_mutex);
    context_switch(cpu_id, currentNode, timeslice);
}

/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id) {
    pthread_mutex_lock(&rq_mutex);
    while (rqHead == NULL) {
        pthread_cond_wait(&un_idle, &rq_mutex);
    }
    pthread_mutex_unlock(&rq_mutex);
    schedule(cpu_id);

    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
}

/*
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 *
 * Remember to set the status of the process to the proper value.
 */
extern void preempt(unsigned int cpu_id) {
    pthread_mutex_lock(&current_mutex);

    pcb_t *extractProcess = current[cpu_id];
    extractProcess->state = PROCESS_READY;

    pthread_mutex_unlock(&current_mutex);
    pushRQ(extractProcess);
    schedule(cpu_id);
}

/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id) {
    pthread_mutex_lock(&current_mutex);

    pcb_t *extractProcess = current[cpu_id];
    extractProcess->state = PROCESS_WAITING;

    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}

/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id) {
    pthread_mutex_lock(&current_mutex);

    pcb_t *extractProcess = current[cpu_id];
    extractProcess->state = PROCESS_TERMINATED;

    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}

/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is SRTF, wake_up() may need
 *      to preempt the CPU with the highest remaining time left to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a lower remaining time left than the one which just woke up,
 *wake_up() should not preempt any CPUs. To preempt a process, use
 *force_preempt(). Look in os-sim.h for its prototype and the parameters it
 *takes in.
 */
extern void wake_up(pcb_t *process) {
    process->state = PROCESS_READY;
    pushRQ(process);

    if (sAlgorithm == SCHEDULER_SRTF) {
        pthread_mutex_lock(&current_mutex);
        unsigned int maxCPU = 0;
        unsigned int maxRemainingTime = 0;
        int anyIdle = 0;
        for (unsigned int i = 0; i < cpu_count; i++) {
            if (current[i] == NULL) {
                anyIdle = 1;
                break;
            }
            unsigned int currentRemainingTime = current[i]->time_remaining;
            if (currentRemainingTime > maxRemainingTime) {
                maxCPU = i;
                maxRemainingTime = currentRemainingTime;
            }
        }
        pthread_mutex_unlock(&current_mutex);

        if (!anyIdle && maxRemainingTime > process->time_remaining) {
            force_preempt(maxCPU);
        }
    }
}

/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -s command-line parameters.
 */
int main(int argc, char *argv[]) {
    /*
     * Check here if the number of arguments provided is valid.
     * You will need to modify this when you add more arguments.
     */
    if (argc < 2) {
        fprintf(stderr,
                "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
                "Usage: ./os-sim <# CPUs> [ -r <time slice> | -s ]\n"
                "    Default : FIFO Scheduler\n"
                "         -r : Round-Robin Scheduler\n"
                "         -s : Shortest Remaining Time First Scheduler\n\n");
        return -1;
    }

    /* Parse the command line arguments */
    cpu_count = strtoul(argv[1], NULL, 0);

    /* FIX ME - Add support for -r and -s parameters*/
    if (argc == 2) {
        sAlgorithm = SCHEDULER_FIFO;
        timeslice = -1;
    } else if (argc == 3) {
        char *flag = argv[2];
        if (strcmp(flag, "-c") == 0) {
            sAlgorithm = SCHEDULER_SRTF;
        } else {
            fprintf(
                stderr,
                "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
                "Usage: ./os-sim <# CPUs> [ -r <time slice> | -s ]\n"
                "    Default : FIFO Scheduler\n"
                "         -r : Round-Robin Scheduler\n"
                "         -s : Shortest Remaining Time First Scheduler\n\n");
            return -1;
        }
        timeslice = -1;
    } else if (argc == 4) {
        char *flag = argv[2];
        if (strcmp(flag, "-r") == 0) {
            sAlgorithm = SCHEDULER_ROUND_ROBIN;
            timeslice = strtoul(argv[3], NULL, 0);
        } else {
            fprintf(
                stderr,
                "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
                "Usage: ./os-sim <# CPUs> [ -r <time slice> | -s ]\n"
                "    Default : FIFO Scheduler\n"
                "         -r : Round-Robin Scheduler\n"
                "         -s : Shortest Remaining Time First Scheduler\n\n");
            return -1;
        }
    } else {
        fprintf(stderr,
                "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
                "Usage: ./os-sim <# CPUs> [ -r <time slice> | -s ]\n"
                "    Default : FIFO Scheduler\n"
                "         -r : Round-Robin Scheduler\n"
                "         -s : Shortest Remaining Time First Scheduler\n\n");
        return -1;
    }

    /* Allocate the current[] array and its mutex */
    current = calloc(cpu_count, sizeof(pcb_t *));
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);
    pthread_mutex_init(&rq_mutex, NULL);
    pthread_cond_init(&un_idle, NULL);

    rqHead = NULL;

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}

#pragma GCC diagnostic pop
