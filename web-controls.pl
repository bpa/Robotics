#! /usr/bin/env perl

use threads;
use threads::shared;
use Thread::Semaphore;
use MotorShield;

my $pitch : shared  = 0;
my $rotate : shared = 0;
my $firing : shared = 0;
my $commands        = Thread::Semaphore->new;
my $motor_thread    = threads->create(
    sub {
        my $commands = shift;
		my $trigger_distance = 128;
        MotorShield::init( { clock => 16, latch => 22 } );
        my ($base_motor) = MotorShield::configure( 18 => 'stepper' );
        my ( $pitch_motor, $trigger_motor ) = MotorShield::configure( 15 => 'stepper', 'stepper' );

        while (1) {
			$commands->down();
			my @trigger_states = ( -1, 1 );
			my $trigger_steps_remaining = 512;
            while ( $pitch || $rotate || $firing ) {
                if ($firing) {
					if ($trigger_steps_remaining) {
						$trigger_steps_remaining--;
						$trigger_motor->step($trigger_states[0]);
					}
					else {
						shift @trigger_states;
						if (@trigger_states) {
							$trigger_steps_remaining = 512;
						}
						else {
							$firing = 0;
						}
					}
				}
                if ($pitch) {
                    my $dir = $pitch > 0 ? -1 : 1;
                    $pitch += $dir;
                    $pitch_motor->step($dir);
                    $trigger_motor->step($dir);
                }
                if ($rotate) {
                    my $dir = $rotate > 0 ? -1 : 1;
                    $rotate += $dir;
                    $base_motor->step($dir);
                }
                MotorShield::update();
                MotorShield::delay(3);
            }
        }
    },
    $commands
);

use Mojolicious::Lite;
use Mojolicious::Static;

get '/' => sub {
    shift->render('index');
};

post '/api/fire' => sub {
    my $self = shift;
    $firing = 1;
    $commands->up();
    $self->render( json => [] );
};

post '/api/pitch' => sub {
    my $self = shift;
    $pitch = $self->param('steps');
    $commands->up();
    $self->render( json => [ $self->param('steps') ] );
};

post '/api/rotate' => sub {
    my $self = shift;
    $rotate = $self->param('steps');
    $commands->up();
    $self->render( json => [ $self->param('steps') ] );
};

app->static->paths->[0] = app->home;
app->start;

