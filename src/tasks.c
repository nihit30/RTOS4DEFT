
#include "include/tasks.h"


void flash()
{
   while(1)
   {
    LED2 ^= 1;
    waitMicrosecond(500000);
    yield();

   }
}


void partofLengthyFn()
{
    waitMicrosecond(1000);
    yield();
}


void lengthyFn()
{

  while(1)
   {

    uint8_t i;
    LED3 ^= 1;


    for(i=0; i<2; i++)
    {
       partofLengthyFn();
    }


   }

}

void blinky()
{
    while(1)
    {

                LED4 = 1;
                waitMicrosecond(5000);
                LED4 = 0;
                yield();
    }
}
