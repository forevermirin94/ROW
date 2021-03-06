
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>

typedef  unsigned char uint8_t;


void* Motorcontrol (void* param);  //thread function
void  Motorinit();



uint8_t MotorHF[7] = {7,3,0xa5,2,3,0xa5,2};
// High speed forward left + right; add explanation

uint8_t MotorST[7] = {7,0,0,0,0,0,0};
// Stop left + right; add explanation

uint8_t MotorHR[7] = {7,3,0xa5,1,3,0xa5,1};
//High speed reverse left + right; add explanation

int fd;


void Motorinit()
{
    uint8_t Totalpower[2]={4,250};     // power between 0 and 255
    uint8_t Softstart[3]={0x91,23,0};  // add explanation


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
     pthread_t tid1;

     Motorinit();

     pthread_create(&tid1,NULL,Motorcontrol,0);
     //Create and start a new posix thread

     pthread_join(tid1,NULL); // add an explanation


     return 0;
}





// The function Motorcontrol runs in a posix thread

void* Motorcontrol(void* param)
{
    short motor1, motor2;
    motor1 = 500;
    motor2 = 500;
    uint8_t test[7] = {7,(motor1 >> 8), (motor1 & 0xFF) , 2, (motor2 >> 8), (motor2 & 0xFF), 2};
    uint8_t stop[7] = {7,0,0,0,0,0,0};
    write(fd, &test, 7);
    usleep(3000000);
    write(fd, &stop, 7);
}


