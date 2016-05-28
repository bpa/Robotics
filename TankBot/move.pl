use strict;
use warnings;

use MotorShield;

MotorShield::init({clock => 16, latch => 22});
my ( $base ) = MotorShield::configure( 18 => 'stepper' );
my ( $pitch, $trigger ) = MotorShield::configure( 15 => 'stepper', 'stepper' );

for my $i (1 .. 512) {
	map { $_->step(-1) } $trigger;
	MotorShield::update();
	MotorShield::delay(3)
}

map { $_->release() } $base, $pitch, $trigger;
MotorShield::update();
