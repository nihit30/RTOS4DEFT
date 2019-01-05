// Credits : Dr. Jason Losh
// References :
// https://en.wikibooks.org/wiki/Embedded_Systems/RTOS_Implementation
// https://en.wikibooks.org/wiki/Embedded_Control_Systems_Design/Real_Time_Operating_systems

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C1294 Evaluation Board
// Target uC:       TM4C1294NCPDT
// System Clock:    120 MHz

// Hardware configuration:

// PB4 : RED LED
// PK0 : Input interrupt for start event

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "driverlib/sysctl.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/pin_map.h"
#include "include/wait.h"
#include "include/initHw.h"
#include "include/kernel.h"
#include "include/tasks.h"

#define MAX_CHARS 20
uint8_t semaphoreCount = 0;
uint32_t tick = 0;
uint64_t totalTicks;
uint32_t time;
char temp[20];
char shellBuffer[MAX_CHARS];
uint32_t pidMatch;
uint8_t taskNo;
uint8_t counter = 0;
uint8_t field_posi[10];
char field_type[10];
uint8_t bitCount=0;


void StartPulseISR()
{

    GPIO_PORTK_ICR_R = 0x01;                // Clear the interrupt flag for next start event
    RED_LED = 0;

}


void StopPulseISR()
{


}
//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    bool ok;

    // Initialize hardware
    initHw();

    // Power-up flash
    RED_LED = 1;
    waitMicrosecond(500000);
    RED_LED = 0;

    rtosInit();
    // 0 : Highest priority
    // Add required idle process
    ok = createThread(idle, "Idle", 3);
    // Add other processes
    ok &= createThread(flash, "flash", 3);
    ok &= createThread(lengthyFn, "lengthyFn", 2);
    ok &= createThread(blinky, "blinky", 0);



    // Start up RTOS
    if (ok)

        rtosStart(); // never returns

    else
        RED_LED = 1;
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;

    return 0;
}
