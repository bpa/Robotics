package MotorShield;

use strict;
use warnings;

use Device::BCM2835;

sub MOTOR_PINOUT { 5, 4, 6, 3, 7, 1, 2, 0 }
my %motors = (
    empty   => { pins => 2 },
    motor   => { pins => 2 },
    stepper => { pins => 4, motor => 'StepperMotor' }
);

our ( $clock, $latch, @shield, @data );

sub init {
	no strict 'refs';
    my $args = shift;

    die "No clock pin specified\n" unless $args->{clock};
    die "No latch pin specified\n" unless $args->{latch};

    Device::BCM2835::init() || die "Could not initialize GPIO library\n";

    $clock = &{ "Device::BCM2835::RPI_V2_GPIO_P1_" . $args->{clock} };
    $latch = &{ "Device::BCM2835::RPI_V2_GPIO_P1_" . $args->{latch} };

    die "Invalid clock pin '" . $args->{clock} . "' specified\n" unless $clock;
    die "Invalid latch pin '" . $args->{latch} . "' specified\n" unless $latch;

    Device::BCM2835::gpio_fsel( $clock, &Device::BCM2835::BCM2835_GPIO_FSEL_OUTP );
    Device::BCM2835::gpio_fsel( $latch, &Device::BCM2835::BCM2835_GPIO_FSEL_OUTP );
}

sub configure {
	no strict 'refs';
    my $pin = shift;

    my $out = &{"Device::BCM2835::RPI_V2_GPIO_P1_$pin"};
    die "Invalid motor pin '$pin' specified\n" unless $out;
    Device::BCM2835::gpio_fsel( $out, &Device::BCM2835::BCM2835_GPIO_FSEL_OUTP );

    push @shield, $out;
    push @data, [ 0, 0, 0, 0, 0, 0, 0, 0 ];

	my @motors;
    my @pins = MOTOR_PINOUT;
	while (@pins && @_) {
		my $type = shift;
		my $motor = $motors{$type};
		die "Invalid motor type '$type'\n" unless $motor;
		my @assignedPins = splice(@pins, 0, $motor->{pins});
		if (defined $motor->{motor}) {
			push @motors, $motor->{motor}->new($#shield, @assignedPins);
		}
	}

	return @motors;
}

sub update {
    Device::BCM2835::gpio_write( $latch, 0 );
    for my $lvl ( 0 .. 7 ) {
        Device::BCM2835::gpio_write( $clock,   0 );
		for my $i ( 0 .. $#shield ) {
        	Device::BCM2835::gpio_write( $shield[$i], $data[$i][$lvl] );
		}
        Device::BCM2835::gpio_write( $clock,   1 );
    }
    Device::BCM2835::gpio_write( $latch, 1 );
}

sub delay {
    Device::BCM2835::delay(shift);
}

package StepperMotor;

my @seq = (
    [ 1, 0, 0, 0 ],
    [ 1, 1, 0, 0 ],
    [ 0, 1, 0, 0 ],
    [ 0, 1, 1, 0 ],
    [ 0, 0, 1, 0 ],
    [ 0, 0, 1, 1 ],
    [ 0, 0, 0, 1 ],
    [ 1, 0, 0, 1 ],
);

sub new {
    my $pkg    = shift;
    my $shield = shift;
    bless { map => [@_], step => 0, shield => $shield }, $pkg;
}

sub release {
    my $self = shift;
    for my $i ( 0 .. $#{ $self->{map} } ) {
        $MotorShield::data[ $self->{shield} ][ $self->{map}[$i] ] = 0;
    }
}

sub step {
    my $self = shift;
    my $steps = shift || 1;
    $self->{step} = ( $self->{step} + $steps + @seq ) % @seq;

    my $step = $seq[ $self->{step} ];
    for my $i ( 0 .. 3 ) {
        $MotorShield::data[ $self->{shield} ][ $self->{map}[$i] ] = $step->[$i];
    }
}

1;
