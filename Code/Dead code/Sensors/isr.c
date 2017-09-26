#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <sys/time.h>

static volatile int globalCounter ;

//Interrupt service routine:
void myInterrupt0 (void) { ++globalCounter ;printf("%d ",globalCounter);fflush(stdout); }
float timedifference_msec(struct timeval t0, struct timeval t1);

int main(void)
{
  wiringPiSetup();
  pullUpDnControl(0,PUD_DOWN);

  //initialisation
  wiringPiISR (0, INT_EDGE_FALLING, &myInterrupt0) ;

  double girth = 18.84955592F;

  struct timeval start, end;
  gettimeofday(&start,0);

  while(1)

 {
     if(globalCounter >= 40){

        gettimeofday(&end,0);
	float result = timedifference_msec(end,start);
	printf ("Your calculations took %.2f seconds to run.\n", result/1000);
	double  calc = girth / 100;
	printf ("Girth wheel in meters %.2lf.\n", calc);
	float endCalc = calc / (result/1000);
	printf ("Your calculations of meters per second is %.2f.\n", endCalc);
        globalCounter = 0;

	gettimeofday(&start,0);
	//usleep(500000);
    }

     usleep(500);

     };

  return 0;
}

float timedifference_msec(struct timeval t0, struct timeval t1){
    return (t0.tv_sec - t1.tv_sec) * 1000.0f + (t0.tv_usec - t1.tv_usec) / 1000.0f;
}
