
#ifndef INCLUDE_KERNEL_H_
#define INCLUDE_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "include/initHw.h"
#include "include/wait.h"

// function pointer
typedef void (*_fn)();


int rtosScheduler();
void rtosInit();
bool createThread(_fn fn, char name[], int priority);
uint8_t pidToTask(uint32_t pgId);
void destroyThread(_fn fn);
void sleep(uint32_t tick);
void systickISR();
void pendSvIsr();
void rtosStart();
void idle();
void* fetchSP();
void writeSP(void* z);
void* getR0();
void setThreadPriority(_fn fn, uint8_t priority);
void yield();

#endif /* INCLUDE_KERNEL_H_ */
