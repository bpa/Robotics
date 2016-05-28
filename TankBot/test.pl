use strict;
use warnings;

use MotorDriver;

my $motor = StepperMotor->new(M1,M2);

for my $i (1 .. 512) {
	$motor->step(2);
	delay(5)
}
for my $i (1 .. 512) {
	$motor->step(-2);
	delay(5)
}
$motor->release;

#for my $i ( 0 .. 7 ) {
#	@MotorDriver::data = (0,0,0,0,0,0,0,0);
#	$MotorDriver::data[$i] = 1;
#	MotorDriver::update();
#	my $x = <STDIN>;
#}
@MotorDriver::data = (0,0,0,0,0,0,0,0);
MotorDriver::update();
