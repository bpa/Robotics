#include "mocks.h"

#if !defined(ARDUINO)
#include <stdio.h>
#include <stdlib.h>

SerialImpl Serial;
uint16_t analogPins[7];

void SerialImpl::begin(int) {}
void SerialImpl::print(const char *m) { printf("%s", m); }
void SerialImpl::print(int i) { printf("%d", i); }
void SerialImpl::println() { printf("\n"); }
void SerialImpl::println(int i) { printf("%d\n", i); }
void SerialImpl::println(const char *m) { printf("%s\n", m); }

AF_DCMotor::AF_DCMotor(int) { }

void AF_DCMotor::run(int) { }

void AF_DCMotor::setSpeed(int) { }

Encoder::Encoder(int, int) { }

long Encoder::read() {
    return 10;
}

void Encoder::write(long v) { }

int analogRead(int i) {
    return analogPins[i];
}

void analogWrite(int i, uint16_t val) {
    analogPins[i] = val;
}

int randomSeed(int x) {
    srandom(x);
}

int random(int x) {
    long i = random();
    return i % x;
}

#endif
