#ifndef SYSTEM_TASKS_H
#define SYSTEM_TASKS_H

// Priorities
#define USB_TASK_PRIO      5
#define MAIN_TASK_PRIO     4
#define ICE40_LOAD_PRIO    2
#define BLINKY_TASK_PRIO   1

// Stack sizes
#define USB_TASK_STACK_SIZE        (CFG_TUSB_DEBUG?512:256)
#define MAIN_TASK_STACK_SIZE       1024
#define ICE40_LOAD_TASK_STACK_SIZE 2048
#define BLINKY_TASK_STACK_SIZE     256

#endif // SYSTEM_TASKS_H
