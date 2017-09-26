#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define HMC5883L_ADDRESS (0x1e)

#define HCM5883L_REG_CONFIG_A (0x00)
#define HCM5883L_REG_CONFIG_B (0x01)
#define HMC5883L_REG_MODE     (0x02)
#define HMC5883L_REG_MSB_X    (0x03)
#define HMC5883L_REG_LSB_X    (0x04)
#define HMC5883L_REG_MSB_Z    (0x05)
#define HMC5883L_REG_LSB_Z    (0x06)
#define HMC5883L_REG_MSB_Y    (0x07)
#define HMC5883L_REG_LSB_Y    (0x08)
#define HMC5883L_REG_STATUS   (0x09)
#define HMC5883L_REG_ID_A     (0x0a)
#define HMC5883L_REG_ID_B     (0x0b)
#define HMC5883L_REG_ID_C     (0x0c)

#define HMC5883L_MODE_CONTINUOUS (0x00)
#define HMC5883L_MODE_SINGLE     (0x01)

#define PI 3.14159265

typedef unsigned char uint8_t;

void setupCompass();
void getCompassValues(short *x, short *y, short *z, double *angle);

void setupDist();
void getDistance(uint8_t *distance);

int fdCompass;
int fdDist;

int main(int argc, char *argv[]) {
    wiringPiSetup();
    setupCompass();
    setupDist();

    while(1) {
      short x, y, z;
      double angle;
      uint8_t distance;


      getCompassValues(&x, &y, &z, &angle);
      getDistance(distance);
      printf("Compass: x=%d, y=%d, z=%d - angle=%.1f\n",x, y, z, angle);
      printf("Distance sensor: %d\n", distance);
      usleep(1000000);
    }

    return 0;
}
void setupCompass() {
    fdCompass = wiringPiI2CSetup(HMC5883L_ADDRESS);
    if (fdCompass < 0) {
        printf("Compass error\n");
        exit(-1);
    }
    wiringPiI2CWriteReg8(fdCompass, HMC5883L_REG_MODE, HMC5883L_MODE_CONTINUOUS);
}

void getCompassValues(short *x, short *y, short *z, double *angle) {
    uint8_t msb = wiringPiI2CReadReg8(fdCompass, HMC5883L_REG_MSB_X);
    uint8_t lsb = wiringPiI2CReadReg8(fdCompass, HMC5883L_REG_LSB_X);
    *x = msb << 8 | lsb;

    msb = wiringPiI2CReadReg8(fdCompass, HMC5883L_REG_MSB_Y);
    lsb = wiringPiI2CReadReg8(fdCompass, HMC5883L_REG_LSB_Y);
    *y = msb << 8 | lsb;

    msb = wiringPiI2CReadReg8(fdCompass, HMC5883L_REG_MSB_Z);
    lsb = wiringPiI2CReadReg8(fdCompass, HMC5883L_REG_LSB_Z);
    *z = msb << 8 | lsb;

    *angle = atan2((double) *y, (double) *x);

    if (*angle < 0) {
        *angle += 2*PI;
    }
    if (*angle > 2*PI) {
        *angle -= 2*PI;
    }
    *angle = *angle * (180/PI);
}

void setupDist() {
    fdDist = wiringPiI2CSetup(0x70);
}

void getDistance(uint8_t *distance) {
    uint8_t TxData1[2] = {00, 0x51};
    uint8_t TxData2[1] = {0x03};

    write(fdDist, &TxData1[0], 2);
    usleep(100000);
    write(fdDist, &TxData2[0], 1);
    read(fdDist, distance, 1);
}
