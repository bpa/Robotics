#include <string.h>
#include <stdlib.h>
#include <bcm2835.h>
#include "gpio.h"

uint8_t output[2][8];
RPiGPIOPin pins[] = { SHLD1, SHLD2 }; 
DCMotor dc_motor_1 = { 0, M1 };
DCMotor dc_motor_2 = { 0, M2 };
DCMotor dc_motor_3 = { 0, M3 };
DCMotor dc_motor_4 = { 0, M4 };

void init_gpio() {
    if (!bcm2835_init())
		exit(1);

    bcm2835_gpio_fsel(LATCH, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(CLOCK, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SHLD1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SHLD2, BCM2835_GPIO_FSEL_OUTP);
    bzero(&output[0][0], sizeof(output));
}

void update(void) {
	bcm2835_gpio_write(LATCH, LOW);
	for (int i=7; i>=0; i--) {
		bcm2835_gpio_write(CLOCK, LOW);
		bcm2835_gpio_write(pins[0], output[0][i] ? HIGH : LOW);
		bcm2835_gpio_write(pins[1], output[1][i] ? HIGH : LOW);
		bcm2835_gpio_write(CLOCK, HIGH);
	}
	bcm2835_gpio_write(LATCH, HIGH);
}

void dc_motor_run(const DCMotor *motor, Direction direction) {
	uint8_t l = 0;
	uint8_t r = 0;
	if (direction == FORWARD) {
		l = 1;
	}
	else if (direction == BACKWARDS) {
		r = 1;
	}
	output[motor->shield][motor->outputs[0]] = l;
	output[motor->shield][motor->outputs[1]] = r;
	update();
}

void stepper_step(const StepperMotor *motor, Direction direction) {
}
