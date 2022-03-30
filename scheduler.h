/*
 * scheduler.h
 *
 *  Created on: Sep 14, 2020
 *      Author: richa
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

/* Pointer to function typedef */
typedef void (*taskfunc)();

/* Node struct for task queue. Elements: burst time, pointer to task function
 * pointer to previous node, pointer to next node, and task name. */
struct task_node {
  uint8_t priority;
  taskfunc mytask;
  uint8_t runFlag;
  char name[6];
};

/* Add task to queue */
void add_task(char name[], uint8_t priority, void *task_func);

/* Pre-defined tasks */
void task_one();
void task_two();
void task_three();
void task_four();
void task_five();
void task_six();

/* Default prototype for new tasks */
void default_task();

/* Scheduler of type FIFO */
void fifo_scheduler();

/* Sorting algorithm (bubble sort) */
struct task_node* sort_tasks();

/* Function to clean run flags of tasks */
void clean_flags();

#endif /* SCHEDULER_H_ */
