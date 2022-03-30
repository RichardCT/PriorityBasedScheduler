/*
 * scheduler.c
 *
 *  Created on: Sep 14, 2020
 *      Author: richa
 */

#include "board.h"
#include "scheduler.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_LED_GPIO     BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Priority options for new taks */
uint8_t newTasks[4] = {2,7,0,4};

/* Default name of new tasks */
char def[]   = {'N','E','W'};

/* Queue of tasks */
struct task_node* task_array[10];

/* Output gpio for red LED */
gpio_pin_config_t led_config = {
    kGPIO_DigitalOutput,
    0,
};

/* Input gpio for red LED */
gpio_pin_config_t led_configIn = {
    kGPIO_DigitalInput,
    0,
};

/* Delay function for burst time */
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 1500000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/* Task counter */
uint8_t task_num = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Pre-defined tasks */
void task_one() {
  printf("This is task ONE\n");
}

void task_two() {
  printf("This is task TWO\n");
}

void task_three() {
  printf("This is task THREE\n");
}

void task_four() {
  printf("This is task FOUR\n");
}

void task_five() {
  printf("This is task FIVE\n");
}

void task_six() {
  printf("This is task SIX\n");
}

/* Default prototype for new tasks */
void default_task(){
  printf("This is new task\n");
}

/* Function to add new tasks to queue */
void add_task(char name[], uint8_t priority, void *task_func){
	/* Create new object */
	struct task_node* new_node = (struct task_node*)malloc(sizeof(struct task_node));
	task_array[task_num] = new_node;

	/* Assign priority, task function and clear run Flag */
	task_array[task_num]->priority = priority;
	task_array[task_num]->mytask = task_func;
	task_array[task_num]->runFlag = 0;

	/* Increment task counter */
	task_num++;
}

/* Delete least priority task from queue */
void pop_task(){
	/* Sort all tasks in queue */
	sort_tasks();
	/* Set space of least prioriry task to NULL */
	task_array[task_num-1] = NULL;
	/* Decrease task counter */
	task_num--;
}

/* Clean run Flags after one complete run of scheduler */
void clean_flags(){
	struct task_node* temp;
	uint8_t i;
	for(i=0;i<task_num;i++){
		temp = task_array[i];
		temp->runFlag = 0;
	}
}

/* Sorting algorithm (bubble sort) that sorts tasks from highest priority
 * (bigger number) to lowest.
 */
struct task_node* sort_tasks(){
	/* Create two pointers to nodes */
    struct task_node* temp1;
    struct task_node* temp2;
    uint8_t i;
    uint8_t j;
    for(i=0;i<task_num-1;i++){
    	for(j=0;j<task_num-1;j++){
    		/* Assign pointers to consecutive nodes in queue */
    		temp1 = task_array[j];
    		temp2 = task_array[j+1];
    		/* If node to the left has greater priority and
    		 * hasn't been executed, shift it to the right */
    		if(temp2->priority>temp1->priority && !(temp2->runFlag)) {
    			task_array[j] = temp2;
    			task_array[j+1] = temp1;
    		}
    	}
    }
    /* Print current order of tasks in queue */
    uint8_t k;
    printf("\n Queue of tasks: \n");
    for(k=0;k<task_num;k++){
    	if(!task_array[k]->runFlag) task_array[k]->mytask();
    }
    printf("\n");
    /* Return first element of queue for execution */
    return task_array[0];
}

/* Scheduler implementation */
void fifo_scheduler(){
	struct task_node* task;
	uint8_t i;
	/* Flag for max task number */
	uint8_t ten = 0;
	/* # of tasks executed in current scheduler run */
	uint8_t task_cnt = 0;
	do{
		/* Set max task number Flag */
		if(task_num==10) ten=1;
	    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, &led_config);
	    /* Sort tasks */
	    task = sort_tasks();
	    /* Execute task function */
	    printf("\nCurrent task in execution: ");
	    task->mytask();
	    for(i=0;i<5;i++){
	    	delay();
	    	GPIO_PortToggle(BOARD_LED_RED_GPIO, 1u << BOARD_LED_RED_GPIO_PIN);
	    	delay();
	    	GPIO_PortToggle(BOARD_LED_RED_GPIO, 1u << BOARD_LED_RED_GPIO_PIN);
	    }
	    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, &led_configIn);
	    /* Set run Flag */
	    task->runFlag = 1;
	    /* Increment task counter in current scheduler run */
	    task_cnt++;

	    /* Send executed task to the end of the queue */
	    uint8_t j;
	    for(j=0;j<task_num;j++){
	    	task_array[j]=task_array[j+1];
	    }
	    task_array[task_num-1] = task;

	    /* Add a new task every complete scheduler run */
	    if(task_cnt==task_num && !ten){
	        add_task(&def, newTasks[task_num-6], default_task);
	        clean_flags();
	        task_cnt = 0;
	    }

	    /* Delete least priority task when max number of tasks is reached */
	    else if(task_cnt==task_num && ten){
	        task_cnt = 0;
	        clean_flags();
	        pop_task();
	    }
	    if(ten && task_num==6) ten=0;
	}while(1);
}
