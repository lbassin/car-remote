#include <Arduino.h>
#include <NintendoExtensionCtrl.h>

#include <RH_ASK.h>
#include <SPI.h>

#include <MsTimer2.h>

#include "main.h"
#include "flags.h"

#define ACTIVITY_LED_PIN 9

Nunchuk nchuk;

Control motor, steering;
Buttons btn;

RH_ASK remoteDriver(2000, 7, 6);

void setup()
{
	Serial.begin(9600);
	nchuk.begin();

	if (!remoteDriver.init()) {
		Serial.println("Failed to init remote communication");
		exit(1);
	}

	while (!nchuk.connect())
	{
		Serial.println("Nunchuk not detected!");
		delay(1000);
	}

	pinMode(ACTIVITY_LED_PIN, OUTPUT);
	digitalWrite(ACTIVITY_LED_PIN, HIGH);
	delay(1000);
	digitalWrite(ACTIVITY_LED_PIN, LOW);

	MsTimer2::set(100, sendData);
  	MsTimer2::start();
}

void loop()
{
	if (nchuk.update() == false)
	{
		Serial.println("Cannot read nunchuk data");
		return;
	}

	updateSteering();
	updateMotor();
	updateButton();
}

void updateSteering()
{
	int8_t joyX = (nchuk.joyX() == 255 ? 254 : nchuk.joyX()) - 127;
	
	if (joyX > 0) {
		steering.direction = STEERING_RIGHT;
	} else if (joyX < 0) {
		steering.direction = STEERING_LEFT;
	} else {
		steering.direction = STEERING_STRAIGHT;
	}

	steering.speed = joyX * steering.direction;
	steering.speed = (uint8_t) map(steering.speed, 0, 127, 0, 255);
}

void updateMotor(){
	int8_t joyY = (nchuk.joyY() == 0 ? 1 : nchuk.joyY()) - 128;

	if (joyY > 0) {
		motor.direction = DIRECTION_FORWARD;
	} else if (joyY < 0) {
		motor.direction = DIRECTION_BACKWARD;
	} else {
		motor.direction = DIRECTION_STOP;
	}

	motor.speed = joyY * motor.direction;
	motor.speed = (uint8_t) map(motor.speed, 0, 127, 0, 255);
}

void updateButton() {
	btn.c = nchuk.buttonC();
	btn.z = nchuk.buttonZ();
}

uint8_t getFormattedFlags() {
	uint8_t output = REMOTE_ENABLED;

	if (motor.direction == DIRECTION_FORWARD) {
		output |= REMOTE_FORWARD;
	}

	if (motor.direction == DIRECTION_BACKWARD) {
		output |= REMOTE_BACKWARD;
	}

	if (steering.direction == STEERING_RIGHT) {
		output |= REMOTE_RIGHT;
	}

	if (steering.direction == STEERING_LEFT) {
		output |= REMOTE_LEFT;
	}

	if (btn.c) {
		output |= REMOTE_BTN_C;
	}

	if (btn.z) {
		output |= REMOTE_BTN_Z;
	}

	return output;
}

void sendData() {
	digitalWrite(ACTIVITY_LED_PIN, HIGH);

	uint8_t data[3];
	data[0] = getFormattedFlags();
	data[1] = motor.speed;
	data[2] = steering.speed;

	remoteDriver.send(data, sizeof(uint8_t) * 3);
	//remoteDriver.waitPacketSent();
	
	digitalWrite(ACTIVITY_LED_PIN, LOW);
}

void debugControls() {
	Serial.print("Steering : ");
	Serial.print(steering.direction, DEC);
	Serial.print(" / ");
	Serial.print(steering.speed, DEC);
	Serial.print(" | ");
	Serial.print("Motor : ");
	Serial.print(motor.direction, DEC);
	Serial.print(" / ");
	Serial.print(motor.speed, DEC);
	Serial.println("");
}

void debugDataSent(uint8_t* data) {
	Serial.print(data[0], BIN);
	Serial.print(" ");
	Serial.print(data[1], BIN);
	Serial.print(" ");
	Serial.print(data[2], BIN);
	Serial.println();
}
