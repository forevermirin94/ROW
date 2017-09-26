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

void checkRC(int rc, char *text) {
  if (rc < 0) {
    printf("Error: %s - %d\n", text, rc);
    exit(-1);
  }
}

int main(int argc, char *argv[]) {
  printf("HMC5883L starting\n");
  // Setup Wiring Pi
  wiringPiSetup();

  // Open an I2C connection
  int fd = wiringPiI2CSetup(HMC5883L_ADDRESS);
  checkRC(fd, "wiringPiI2CSetup");

  // Perform I2C work
  wiringPiI2CWriteReg8(fd, HMC5883L_REG_MODE, HMC5883L_MODE_CONTINUOUS);

  while(1) {
      uint8_t msb = wiringPiI2CReadReg8(fd, HMC5883L_REG_MSB_X);
      uint8_t lsb = wiringPiI2CReadReg8(fd, HMC5883L_REG_LSB_X);
      short x = msb << 8 | lsb;

      msb = wiringPiI2CReadReg8(fd, HMC5883L_REG_MSB_Y);
      lsb = wiringPiI2CReadReg8(fd, HMC5883L_REG_LSB_Y);
      short y = msb << 8 | lsb;

      msb = wiringPiI2CReadReg8(fd, HMC5883L_REG_MSB_Z);
      lsb = wiringPiI2CReadReg8(fd, HMC5883L_REG_LSB_Z);
      short z = msb << 8 | lsb;

      double angle = atan2((double) y, (double)x); 
      
      if (angle < 0) {
        angle += 2*PI;
      }
      if (angle > 2*PI) {
        angle -= 2*PI;
      }
      angle = (angle / PI) * 180;
      printf("x=%d, y=%d, z=%d - angle=%f\n",x, y, z, angle);
      sleep(1);
  }

  return 0;
}
