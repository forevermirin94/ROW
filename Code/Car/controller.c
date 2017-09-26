#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFFER_LENGTH 512
#define PORT_IN 8888

#define PORT_OUT 4444

#define TRIGGER_PIN 4
#define ECHO_PIN 5

typedef  unsigned char uint8_t;

struct timeval lastInputTime;
struct timeval lastDistTime;

void  Motorinit();
void  setMotors(int speedLeft, int dirLeft, int speedRight, int dirRight);
float timedifference_msec(struct timeval t0, struct timeval t1);
void* socketListener(void* args);
void socketInInit();
void socketOutInit(char host[]);
void sendPacket(char message[]);
void setupDistanceSensor();
int getDistance();

int fd;

struct sockaddr_in socket_in, socket_out;
int socketfdIn, socketfdOut = -1;

int main()
{
    gettimeofday(&lastInputTime, 0);
	wiringPiSetup();
	
    Motorinit();
	
	socketInInit();
	
	setupDistanceSensor();
	
    pthread_t pth;
    pthread_create(&pth, NULL, socketListener, 0);
	
	while (1) {
		struct timeval currentTime;
		gettimeofday(&currentTime, 0);
		
		//Turn the motors off if there hasn't been a new packet since 100ms.
		if (timedifference_msec(currentTime, lastInputTime) > 100) {
			setMotors(0, 0, 0, 0);
		}
		
		if (socketfdOut != -1) {
			//Read and transmit the distance sensor value every 0.5 seconds.
			if (timedifference_msec(currentTime, lastDistTime) > 500) {
				char str[6];
				int distance = getDistance();
				sprintf(str, "%d", distance);
				//printf("Dist: %s\n", str);
				sendPacket(str);
			}
		}
	}

    return 0;
}

void* socketListener(void* args)
{
    char buffer[BUFFER_LENGTH];
    int inputLength, socketlen = sizeof(socket_in);
	struct sockaddr_in socketreceived;
	
	
	while (1)
    {
		//Reset the buffer and read an incomming packet. 
		memset(buffer, 0, sizeof(buffer));
        inputLength = recvfrom(socketfdIn, buffer, BUFFER_LENGTH, 0, (struct sockaddr *) &socketreceived, (socklen_t *) &socketlen);

        if (inputLength == -1) {
            perror("Error on receive");
        }
		
		if (socketfdOut == -1) {
			printf("IP:%s\n", inet_ntoa(socketreceived.sin_addr));
			//Use the source IP from the incomming message as the destination IP for the outgoing messages.
			socketOutInit(inet_ntoa(socketreceived.sin_addr));
		}
		
		int leftSpeed, rightSpeed, leftDirection, rightDirection, x, y;
		unsigned int temp;
		if (sscanf(buffer, "%d", &temp)) {
			//Get bits 10..19 from the variable temp. This is the x value.
			x = (temp >> 10) & 0x3FF;
			//Get bits 0..9 for the y value.
			y = temp & 0x3FF;
			
			//Subtract 400 so the values range from -200..200.
			x -= 400;
			y -= 400;
			
			//Invert the y axis and mix y and x for each motor.
			y *= -1;
			leftSpeed = y + x;
			rightSpeed =  y - x;
			
			//Make sure the values are in range.
			leftSpeed = (leftSpeed > 200) ? 200 : (leftSpeed < -200) ? -200 : leftSpeed;
			rightSpeed = (rightSpeed > 200) ? 200 : (rightSpeed < -200) ? -200 : rightSpeed;
			
			
			//0 = no movement, speed < 0 = backwards, speed > 0 = forwards.
			if (leftSpeed == 0) {
				leftDirection = 0;
				leftSpeed = 0;
			}
			else if (leftSpeed < 0) {
				leftSpeed *= -1;
				leftSpeed += 823;
				leftDirection = 1;
			} else {
				leftSpeed += 823;
				leftDirection = 2;
			}
			
			
			if (rightSpeed == 0) {
				rightDirection = 0;
				rightSpeed = 0;
			}
			else if (rightSpeed < 0) {
				rightSpeed *= -1;
				rightSpeed += 823;
				rightDirection = 1;
			} else {
				rightSpeed += 823;
				rightDirection = 2;
			}
			
			fflush(stdout);
			setMotors(leftSpeed, leftDirection, rightSpeed, rightDirection);
			gettimeofday(&lastInputTime, 0);
		}
	}
	return NULL;
}


/**
	Allows for both motors to be set to a specific speed and direction. Motors start moving at a speed of around 800.
	@param speedLeft	The speed of the left motor, between 0 and 1023.
	@param dirLeft		The direction of the left motor where 0 is disabled, 1 backwards, and 2 forwards.
	@param speedRight	The speed of the right motor, between 0 and 1023.
	@param dirLeft		The direction of the right motor where 0 is disabled, 1 backwards, and 2 forwards.
*/
void setMotors(int speedLeft, int dirLeft, int speedRight, int dirRight)
{
	//Format {7 = number of bytes, left speed high byte, left speed low byte, left direction, right speed high bytes, right speed low bytes, right direction}.
	//The speed for each motor is given in a 10 bit value, thus two bytes are used.
    uint8_t temp[7] = {7,(speedLeft >> 8) & 0xFF, speedLeft & 0xFF, dirLeft,(speedRight >> 8) & 0xFF,speedRight & 0xFF, dirRight};
    write(fd,&temp[0],7);
}

/**
	Calculate the difference in miliseconds between two timeval structs. 
	@param t0	First timeval.
	@param t1	Second timeval.
	@returns	Differens between the two times as a float.
*/
float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t0.tv_sec - t1.tv_sec) * 1000.0f + (t0.tv_usec - t1.tv_usec) / 1000.0f;
}

/**
	Configure the motor driver boards.
*/
void Motorinit()
{
	//Power between 0 and 255.
    uint8_t Totalpower[2]= {4,230};
	//Softstart to prevent the motors from using to much current.	
    uint8_t Softstart[3]= {0x91,23,0};

    pullUpDnControl(0,PUD_DOWN); 

	//Setup I2C for the motor board
    fd=wiringPiI2CSetup(0x32);

	//Send the data to the motor driver.
    write(fd,&Totalpower[0], 2);
    write(fd,&Softstart[0],3);
}

/**
	Configure the receiving socket.
*/
void socketInInit()
{
	//Set up an UDP socket.
    socketfdIn = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socketfdIn == -1)
    {
        perror("Error on socket creation");
    }
	
	//Set port and address.
    memset((char *) &socket_in, 0, sizeof(socket_in));
    socket_in.sin_family = AF_INET;
    socket_in.sin_port = htons(PORT_IN);
    socket_in.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socketfdIn, (struct sockaddr*) &socket_in, sizeof(socket_in)) == -1)
    {
        perror("Error on bind");
    }
}

/**
	Configre the outgoing socket.
	@param host		The host to connect to.
*/
void socketOutInit(char host[]) 
{
	struct hostent *server;
	server = gethostbyname(host);
	
	socketfdOut = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socketfdOut == -1) {
        perror("ERROR opening socket");
    }
	bzero((char *) &socket_out, sizeof(socket_out));
    socket_out.sin_family = AF_INET;
	
    bcopy((char *)server->h_addr, (char *)&socket_out.sin_addr.s_addr, server->h_length);
    socket_out.sin_port = htons(PORT_OUT);

	int result = connect(socketfdOut,(struct sockaddr *)&socket_out,sizeof(socket_out));
    if (result < 0) {
		printf("Result: %d\n", result);
        perror("ERROR connecting");
    }
}

/**
	Send a packet on the outgoing socket.
	@param message	A string with data to send.
*/
void sendPacket(char message[]) {
	
	int n = write(socketfdOut,message,strlen(message));
	if (n < 0) {
		perror("ERROR on sendPacket");
		close(socketfdOut);
		socketfdOut = -1;
	}
}

/**
	Configure the distance sensor.
*/
void setupDistanceSensor() {
	gettimeofday(&lastDistTime, 0);
	
	//Set the trigger pin as an output and the echo pin as input.
	pinMode(TRIGGER_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);
	//Set the trigger pin low.
	digitalWrite(TRIGGER_PIN, LOW);
	delay(30);
}

int getDistance() {
	//Send trig pulse
	digitalWrite(TRIGGER_PIN, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIGGER_PIN, LOW);

	//Wait for echo start
	while(digitalRead(ECHO_PIN) == LOW);
	
	//Wait for echo end
	long startTime = micros();
	while(digitalRead(ECHO_PIN) == HIGH);
	long travelTime = micros() - startTime;

	//Get distance in cm
	int distance = travelTime / 58;
	if (distance > 350) {
		distance = 350;
	}
	
	gettimeofday(&lastDistTime, 0);
	
	return distance;
}
