#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#define LATCH RPI_V2_GPIO_P1_22
#define CLOCK RPI_V2_GPIO_P1_16
#define SHLD1 RPI_V2_GPIO_P1_18
#define SHLD2 RPI_V2_GPIO_P1_15
#define M1 2,3
#define M2 1,4
#define M3 0,6
#define M4 5,7

typedef enum {
	STOP,
	FORWARD,
	BACKWARDS
} Direction;

typedef struct {
	int shield;
	uint8_t outputs[2];
} DCMotor;

typedef struct {
	int shield;
	int steps;
	uint8_t outputs[4];
} StepperMotor;

extern DCMotor dc_motor_1;
extern DCMotor dc_motor_2;
extern DCMotor dc_motor_3;
extern DCMotor dc_motor_4;

void init_gpio();
void dc_motor_run(const DCMotor *motor, Direction direction);
void stepper_step(const StepperMotor *motor, Direction direction);

#endif
