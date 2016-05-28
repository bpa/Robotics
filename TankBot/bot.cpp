#include "gpio.h"
#include <time.h>

int main() {
	init_gpio();
	struct timespec second = { 1, 0 };
	struct timespec leftover;
	DCMotor *motor = &dc_motor_3;
	dc_motor_run(motor, FORWARD);
	nanosleep(&second, &leftover);
	dc_motor_run(motor, BACKWARDS);
	nanosleep(&second, &leftover);
	dc_motor_run(motor, STOP);
	return 0;
}
