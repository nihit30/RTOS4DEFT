

#ifndef INCLUDE_INITHW_H_
#define INCLUDE_INITHW_H_


#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#define PORTN_BASE 0x40064000
#define PORTF_BASE 0x4005D000
#define PORTB_BASE 0x40059000
#define PORTK_BASE 0x40061000

// On board LED's and pushbuttons
#define LED1 (*((volatile uint32_t *)(0x42000000 + (0x400643FC-0x40000000)*32 + 1*4)))
#define LED2 (*((volatile uint32_t *)(0x42000000 + (0x400643FC-0x40000000)*32 + 0*4)))
#define LED3 (*((volatile uint32_t *)(0x42000000 + (0x4005D3FC-0x40000000)*32 + 4*4)))
#define LED4 (*((volatile uint32_t *)(0x42000000 + (0x4005D3FC-0x40000000)*32 + 0*4)))
#define RED_LED (*((volatile uint32_t *)(0x42000000 + (0x400593FC-0x40000000)*32 + 4*4)))
#define PUSHBUTTON1    (*((volatile uint32_t *)(0x42000000 + (0x400603FC-0x40000000)*32 + 0*4)))
#define PUSHBUTTON2    (*((volatile uint32_t *)(0x42000000 + (0x400603FC-0x40000000)*32 + 1*4)))



void initHw();


#endif /* INCLUDE_INITHW_H_ */
