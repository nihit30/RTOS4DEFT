
#include "include/initHw.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
// Initialize Hardware
void initHw()
{
       // SYSTEM INITIALIZATION
       // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 120 Mhz
       SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320, 120000000);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
       SysCtlDelay(100);

       // Configure on-board LED's and PB5 as outputs for test purposes
       GPIOPinTypeGPIOOutput(PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); //port N
       GPIOPinTypeGPIOOutput(PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); //port F
       GPIOPinTypeGPIOOutput(PORTB_BASE, GPIO_PIN_5 | GPIO_PIN_4);

       // Configure PK0 as input, edge triggered interrupt
       GPIOPinTypeGPIOInput(PORTK_BASE, GPIO_PIN_0);
       GPIOIntTypeSet(PORTK_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);
       GPIOIntEnable(PORTK_BASE, GPIO_PIN_0);
       IntEnable(INT_GPIOK_TM4C129);
}
