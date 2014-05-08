package MotorDriver;

use strict;
use warnings;

use Data::Dumper;
use Device::BCM2835;
use Exporter 'import';

our @EXPORT = qw/M1 M2 M3 M4 delay/;

sub M1 { 5, 4 };
sub M2 { 6, 3 };
sub M3 { 7, 1 };
sub M4 { 2, 0 };

my $clock = &Device::BCM2835::RPI_V2_GPIO_P1_16;
my $serial = &Device::BCM2835::RPI_V2_GPIO_P1_18;
my $latch = &Device::BCM2835::RPI_V2_GPIO_P1_22;
our @data = (0,0,0,0,0,0,0,0);

Device::BCM2835::init() || die "Could not initialize GPIO library\n";
Device::BCM2835::gpio_fsel($clock, &Device::BCM2835::BCM2835_GPIO_FSEL_OUTP);
Device::BCM2835::gpio_fsel($serial, &Device::BCM2835::BCM2835_GPIO_FSEL_OUTP);
Device::BCM2835::gpio_fsel($latch, &Device::BCM2835::BCM2835_GPIO_FSEL_OUTP);

sub update {
	Device::BCM2835::gpio_write($latch, 0);
	for my $lvl (@data) {
		Device::BCM2835::gpio_write($clock, 0);
		Device::BCM2835::gpio_write($serial, $lvl);
		Device::BCM2835::gpio_write($clock, 1);
	}
	Device::BCM2835::gpio_write($latch, 1);
}

sub delay {
	Device::BCM2835::delay(shift);
}

#for my $i ( 0 .. 7 ) {
#	@data = (0,0,0,0,0,0,0,0);
#	$data[$i] = 1;
#	update();
#	delay(200);
#}
#@data = (0,0,0,0,0,0,0,0);
update();

package MotorShield;

package DCMotor;

package StepperMotor;

my @seq = (
	[1,0,0,0],
	[1,1,0,0],
	[0,1,0,0],
	[0,1,1,0],
	[0,0,1,0],
	[0,0,1,1],
	[0,0,0,1],
	[1,0,0,1],
);

sub new {
	my $pkg = shift;
	bless { map => [@_], step => 0 }, $pkg;
}

sub release {
	my $self = shift;
	for my $i (0 .. $#{$self->{map}}) {
		$MotorDriver::data[$self->{map}[$i]] = 0;
	}
	MotorDriver::update();
}

sub step {
	my $self = shift;
	my $steps = shift || 1;
	$self->{step} = ($self->{step} + $steps + @seq) % $#seq;

	my $step = $seq[$self->{step}];
	for my $i (0 .. 3) {
		$MotorDriver::data[$self->{map}[$i]] = $step->[$i];
	}
	MotorDriver::update();
}

1;
