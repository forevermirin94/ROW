#include  <stdio.h>
#include  <unistd.h>
#include  <wiringPi.h>
#include  <wiringPiI2C.h>
//#include  <softPwm.h>

typedef unsigned char   uint8_t;

int fd;

int main(void)
{
    uint8_t TxData1[2] = {00,0x51}; //add second array element; 
    uint8_t TxData2[1] = {0x03}; //What is the function of memory location 3?
    uint8_t low=50;

    wiringPiSetup(); 
    fd=wiringPiI2CSetup(0x70);

    // set servo in mid position
    // servo pwm control wire must me connected to wiring pi       GPIO number 1
    
    //pinMode(1,output);
    //digitalWrite(1,LOW);
    //pwmSetClock(500);  //add  explanation

    //softPwmCreate(1,0,500); //add explanation
    //softPwmWrite(1,14); //add explination

    usleep(1000);

    while(1)
    {
       // start a new measurement in centimeters
       write(fd,&TxData1[0],2);
       usleep(100000);  //give the sensor time for measurement

       write(fd,&TxData2[0],1);  //ask for the lower order byte of the range
       read(fd,&low,1);

       printf("Distance is %d   \n",low);
       usleep(2000000);

     }


    return 0;
}


