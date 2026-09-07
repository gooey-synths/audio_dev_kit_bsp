#ifndef SYSTEM_TASKS_H
#define SYSTEM_TASKS_H

// Priorities
#define USB_TASK_PRIO      5
#define MAIN_TASK_PRIO     4
#define BLINKY_TASK_PRIO   1

// Stack sizes
#if CFG_TUSB_DEBUG
#define USB_TASK_STACK_SIZE      512
#else
#define USB_TASK_STACK_SIZE      256
#endif
#define MAIN_TASK_STACK_SIZE     1024
#define BLINKY_TASK_STACK_SIZE   256

#endif // SYSTEM_TASKS_H
