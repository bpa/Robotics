#include <string.h>
#include <stdlib.h>
#include <bcm2835.h>
#include "gpio.h"

uint8_t output[2][8];
RPiGPIOPin pins[] = { SHLD1, SHLD2 }; 
DCMotor dc_motor_1 = { 0, M1 };

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
	for (int i=0; i<8; i++) {
		bcm2835_gpio_write(CLOCK, LOW);
		bcm2835_gpio_write(pins[0], output[0][i] ? HIGH : LOW);
		bcm2835_gpio_write(pins[1], output[1][i] ? HIGH : LOW);
		bcm2835_gpio_write(CLOCK, HIGH);
	}
	bcm2835_gpio_write(LATCH, HIGH);
}

