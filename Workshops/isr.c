#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>




static volatile int globalCounter ;

//Interrupt service routine:
void myInterrupt0 (void) { ++globalCounter ;printf("%d ",globalCounter);fflush(stdout); }


int main(void)
{
  wiringPiSetup();
  pullUpDnControl(0,PUD_DOWN);

  //initialisation
  wiringPiISR (0, INT_EDGE_FALLING, &myInterrupt0) ;


  while(1)
    {

     usleep(500);

     };

  return 0;
}
