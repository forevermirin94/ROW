#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>
#include <sys/time.h>

typedef  unsigned char uint8_t;

struct timeval lastKeyPress;

void* Motorcontrol (void* param);  //thread function
void  Motorinit();
void  setMotors(int speedLeft, int dirLeft, int speedRight, int dirRight);
float timedifference_msec(struct timeval t0, struct timeval t1);

int fd;

void Motorinit()
{
    uint8_t Totalpower[2]= {4,230};    // power between 0 and 255
    uint8_t Softstart[3]= {0x91,23,0}; // add explanation


    wiringPiSetup () ;
    pullUpDnControl(0,PUD_DOWN);

    fd=wiringPiI2CSetup(0x32);

    write(fd,&Totalpower[0], 2);
    write(fd,&Softstart[0],3);

    //number of bytes = 3
    //What is a soft start?
}

int main()
{
    //struct timeval currentTime;
    pthread_t tid1;
    gettimeofday(&lastKeyPress, 0);
    Motorinit();

    pthread_create(&tid1,NULL,Motorcontrol,0);
    //Create and start a new posix thread
    pthread_join(tid1,NULL); // add an explanation

    return 0;
}
// The function Motorcontrol runs in a posix thread

void* Motorcontrol(void* param)
{
    initscr();
    timeout(0);
    while(1)
    {
        int key = getch();
        switch(key)
        {
            case 119 :
                setMotors(1023, 2, 1023, 2);
                gettimeofday(&lastKeyPress, 0);
                break;

            case 115 :
                setMotors(1023, 1, 1023, 1);
                gettimeofday(&lastKeyPress, 0);
                break;

            case 97 :
                setMotors(1023, 1, 1023, 2);
                gettimeofday(&lastKeyPress, 0);
                break;

            case 100 :
                setMotors(1023, 2, 1023, 1);
                gettimeofday(&lastKeyPress, 0);
                break;
            default : {
                struct timeval currentTime;
                gettimeofday(&currentTime, 0);
                if (timedifference_msec(currentTime, lastKeyPress) > 100) {
                    setMotors(0, 0, 0, 0);
                }
            }
        }
    }
    endwin();
}

void setMotors(int speedLeft, int dirLeft, int speedRight, int dirRight)
{
    uint8_t temp[7] = {7,(speedLeft >> 8) & 0xFF, speedLeft & 0xFF, dirLeft,(speedRight >> 8) & 0xFF,speedRight & 0xFF, dirRight};
    write(fd,&temp[0],7);
}


float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t0.tv_sec - t1.tv_sec) * 1000.0f + (t0.tv_usec - t1.tv_usec) / 1000.0f;
}
