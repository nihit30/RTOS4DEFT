
#include "include/kernel.h"
//-----------------------------------------------------------------------------
// RTOS Defines and Kernel Variables
//-----------------------------------------------------------------------------


// Task states
#define STATE_INVALID    0 // no task
#define STATE_UNRUN      1 // task has never been run
#define STATE_READY      2 // has run, can resume at any time
#define STATE_BLOCKED    3 // has run, but now blocked by semaphore
#define STATE_DELAYED    4 // has run, but now awaiting timer

#define MAX_TASKS 10       // maximum number of valid tasks

uint32_t stack[MAX_TASKS][256]; //   256*10(max tasks)*4 bytes = 10.240 kB stack

struct _tcb
{
    uint8_t state;                 // see STATE_ values above
    void *pid;                     // used to uniquely identify thread
    void *sp;                      // location of stack pointer for thread
    uint8_t priority;              // 0=highest, 15=lowest
    uint8_t currentPriority;       // used for priority inheritance
    uint32_t ticks;                // ticks until sleep complete
    char name[16];                 // name of task used in ps command
    void *semaphore;               // pointer to the semaphore that is blocking the thread
    uint32_t processTime;
    uint8_t skipCount;
    char* list[10];

} tcb[MAX_TASKS];


uint8_t taskCount = 0;     // total number of valid tasks
uint32_t killprog;
uint8_t taskCurrent = 0;   // index of last dispatched task



//-----------------------------------------------------------------------------
// RTOS Kernel Functions
//-----------------------------------------------------------------------------


int rtosScheduler()
{
    bool ok;
    static uint8_t task = 0xFF;
    ok = false;
    while (!ok)
    {
        task++;
        if (task >= MAX_TASKS)
            task = 0;


        if (tcb[task].state == STATE_UNRUN)
        {
            return task;
        }
        // Skip count is used to keep a track of how many times a task has been skipped by scheduler
        else if ((tcb[task].skipCount >= tcb[task].priority) && (tcb[task].state == STATE_READY))
        {
            tcb[task].skipCount = 0;
            return task;
        }
        else
            ++tcb[task].skipCount;


    }
    return task;
}

void rtosInit()
{
    uint8_t i;
    taskCount = 0;      // no tasks running

    for (i = 0; i < MAX_TASKS; i++)      // clear out tcb records
    {
        tcb[i].state = STATE_INVALID;
        tcb[i].pid = 0;
    }
    // Initializing systick Timer for 1hz (120mhz/1hz), used when task is delayed
     NVIC_ST_CTRL_R = ~NVIC_ST_CTRL_ENABLE;
     NVIC_ST_RELOAD_R = 0x07270E00;
     NVIC_ST_CURRENT_R = 0x07270E00;
     NVIC_ST_CTRL_R = 0x07;
}

// Initialize task attributes before starting RTOS
bool createThread(_fn fn, char name[], int priority)
{
    bool ok = false;
    //volatile uint8_t k = 0;
    bool found = false;
    uint8_t z;
    uint8_t i = 0;

    i = 0;
    if (taskCount < MAX_TASKS)
    {
        // make sure fn not already in list (prevent reentrancy)
        while (!found && (i < MAX_TASKS))
        {
            found = (tcb[i++].pid == fn); // if function pointer = pid then found is true and exits loop.
        }
        if (!found)
        {
            // find first available tcb record
            i = 0;

            while (tcb[i].state != STATE_INVALID)
            {
                i++;
            }
            tcb[i].state = STATE_UNRUN;
            tcb[i].pid = fn;                        // pid = PC
            tcb[i].sp = &stack[i][255];
            tcb[i].priority = priority;
            tcb[i].currentPriority = priority;
            for (z = 0; z < 16; z++)
            {
                tcb[i].name[z] = name[z];
            }
            // increment task count
            taskCount++;
            ok = true;
        }

    }

    return ok;
}

// Maps pid to task number, used in shell commands
uint8_t pidToTask(uint32_t pgId)
{       // Reference : https://people.redhat.com/anderson/extensions/ps.c
    uint8_t j;
    for (j = 0; j < taskCount; j++)
    {
        if ((uint32_t) tcb[j].pid == pgId)
            break;
    }
    return j;
}

// Destroys existing thread
void destroyThread(_fn fn)
{
    uint8_t i, t = 0;
    for (i = 0; i < taskCount; i++)
    {
        if (tcb[i].pid == fn)
        {
            tcb[i].state = STATE_INVALID;
            tcb[i].pid = 0;
            taskCount--;

            for (t = 0; t <= 5; t++) // clear out the semaphores the task has been waiting on
            {

            }

        }
    }

}

// Used in conjunction with systickISR which clears one "sleep tick" every second
void sleep(uint32_t tick)
{
    tcb[taskCurrent].ticks = tick;
    tcb[taskCurrent].state = STATE_DELAYED;
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
}

// Called every one second to decrement sleep ticks
void systickISR()
{
    uint8_t k = 0;
    for (k = 0; k < taskCount; k++)
    {
        if (tcb[k].state == STATE_DELAYED)
        {
            tcb[k].ticks--;
            if (tcb[k].ticks == 0)
            {
                tcb[k].state = STATE_READY;

            }
            break;

        }
        break;

    }
}

// This is the heart of RTOS, context switching is carried out here
void pendSvIsr()
{

    __asm(" push {r4-r11}");                // store current task's context on its stack
    tcb[taskCurrent].sp = fetchSP();    // store current task's sp
    taskCurrent = rtosScheduler();      // Get next task
    writeSP(tcb[taskCurrent].sp);       // Write next task sp to sp register

    switch (tcb[taskCurrent].state)
    {
    // If task is ready, pop off the attributes of this task from its stack space
    case STATE_READY:
    {
        __asm(" pop { r4-r11 } ");
        __asm(" mvn lr, #~0xFFFFFFF9 ");
        break;
    }

    case STATE_UNRUN:
    {
        // If task is unrun, write the values of registers XPSR, PC, SP, LR
        tcb[taskCurrent].state = STATE_READY;
        writeSP(&stack[taskCurrent][246]);          // ADJUST SP TO CURRENT TASK SP
        stack[taskCurrent][247] = 0xFFFFFFF9;
        stack[taskCurrent][253] = (uint32_t) tcb[taskCurrent].pid; // changes
        stack[taskCurrent][254] = (uint32_t) tcb[taskCurrent].pid;
        stack[taskCurrent][255] = 0x01000000;
        break;
    }

    }
}

// This routine never returns and is called only once
void rtosStart()
{
    _fn fn;
    fn = (_fn) tcb[taskCurrent].pid;
    (*fn)();                                          // CALL IDLE AS FIRST TASK
    taskCurrent = rtosScheduler();
    // Add code to initialize the SP with tcb[task_current].sp;

}

// one task must be ready at all times or the scheduler will fail
// the idle task is implemented for this purpose
void idle()
{
    // Switch processor to power saver mode
    while(1)
    {
    tcb[taskCurrent].state = STATE_READY;
    RED_LED = 1;
    waitMicrosecond(10000);
    RED_LED = 0;
    yield();
    }

}

// Fetch location of stack pointer in C variable
void* fetchSP()
{ // Reference :https://stackoverflow.com/questions/32953949/saving-value-of-stack-pointer-in-c-variable-in-code-composer-studio-for-arm-cort
    __asm(" mov r0, sp");
    __asm(" bx lr");
}

// Performs a write SP register
void writeSP(void* z)
{ // Reference : https://stackoverflow.com/questions/32953949/saving-value-of-stack-pointer-in-c-variable-in-code-composer-studio-for-arm-cort
    __asm(" mov sp, r0");
    __asm(" bx lr");
}

// Fetches first argument passed to a function
void* getR0()
{
    __asm(" bx lr ");
}

// Used to set thread priority based on external inputs (eg. Push-button)
void setThreadPriority(_fn fn, uint8_t priority)
{
    uint8_t t = pidToTask((uint32_t)fn);
    tcb[t].priority = priority;

}

// Pends service call for context switching
void yield()
{
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
}







