#include <Arduino.h>

#define DIRECTION_FORWARD 1
#define DIRECTION_STOP 0
#define DIRECTION_BACKWARD -1

#define STEERING_RIGHT 1
#define STEERING_STRAIGHT 0
#define STEERING_LEFT -1

struct Control {
	int8_t direction = 0;
	uint8_t speed = 0;
};

struct Buttons {
	bool c = false;
    bool z = false;
};

void updateSteering();
void updateMotor();
void updateButton();

void debugControls();
void debugDataSent(uint8_t* data);

void sendData();

uint8_t getFormattedFlags();