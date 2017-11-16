#include "mocks.h"

#ifndef ARDUINO
#include <stdio.h>
#include <stdlib.h>

SerialImpl begin(int) {}
void SerialImpl::print(const char *m) { printf("%s", m); }
void SerialImpl::print(int i) { printf("%d", i); }
void SerialImpl::println() { printf("\n"); }
void SerialImpl::println(const char *m) { printf("%s\n", m); }

void AF_DCMotor::run(int) { }

void AF_DCMotor::setSpeed(int) { }

Encoder::Encoder(int, int) { }

long Encoder::read() {
    return 0;
}

void Encoder::write(long v) { }

int analogRead(int) {
    return 0;
}

int randomSeed(int x) {
    srandom(x);
}

int random(int x) {
    long i = random();
    return i % x;
}

#endif
