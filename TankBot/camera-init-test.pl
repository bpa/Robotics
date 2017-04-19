#! /usr/bin/perl

use strict;
use warnings;

use Math::Trig;
use Text::Table;
use Scalar::Util::Numeric 'isint';

my $FOOT   = 30.48;
my $INCH   = 2.54;
my $camera = 6 * $INCH;
my $D      = 20.5;

my @targets = (
    { height => 2 * $INCH,  distance => 10 * $FOOT },
    { height => 12 * $INCH, distance => 15 * $FOOT },
    { height => 8 * $INCH,  distance => 20 * $FOOT },
);

my @modes = (
    { pixels => 480,  fov => 41.41 },
    { pixels => 640,  fov => 53.5 },
    { pixels => 720,  fov => 41.41 },
    { pixels => 1080, fov => 41.41 },
    { pixels => 1280, fov => 53.5 },
    { pixels => 1920, fov => 53.5 },
);

my @scenes;
for my $t (@targets) {
    for my $r (@modes) {
        my $s = Scene->new(calc_pixels( $t, $r ), $r);
        $s->{distA} = $t->{distance};
        $s->{distE} = abs($t->{distance} - $s->{dist});
        $s->{A} = rad2deg(atan2( abs( $t->{height} - $camera ), $t->{distance} ));
        push @scenes, $s;
    }
}

my @keys = sort keys %{$scenes[0]};
my $table = Text::Table->new(@keys);
for my $s (@scenes) {
	$table->add(map { isint($_) ? $_ : sprintf("%.2f", $_) } map { $s->{$_} } @keys)
}
print $table;

sub calc_pixels {
    my ( $target, $mode ) = @_;
    return (
        project( $target->{height},      $target->{distance}, $mode ),
        project( $target->{height} + $D, $target->{distance}, $mode )
    );
}

sub project {
    my ( $h, $d, $mode ) = @_;
    my $angle = atan2( abs( $h - $camera ), $d );
    my $px   = $mode->{pixels} / deg2rad( $mode->{fov} ) * $angle;
    my $half = $mode->{pixels} / 2;
    return $h < $camera ? int( $half - $px ) : int( $half + $px );
}

package Scene;

use Math::Trig;

sub new {
    my ( $pkg, $y1, $y2, $mode ) = @_;
    my $self = bless {
        y1  => $y1,
        y2  => $y2,
        px  => $mode->{pixels},
        fov => $mode->{fov}
    }, $pkg;
    $self->calc_distance;
    $self->calc_distance_w_height;
    return $self;
}

sub calc_distance_w_height {
    my $self = shift;
    my $closest = { d => 2000 };
    for my $t (@targets) {
        my $diff = abs($self->{est_dist} - $t->{distance});
        if ($diff < $closest->{d}) {
            $closest->{d} = $diff;
            $closest->{h} = $t->{height};
        }
    }
    $self->{x} = abs($closest->{h} - $camera);
    $self->{dist} = abs($closest->{h} - $camera) * cot($self->{r1});
}

sub calc_distance {
    my $self = shift;
    my $half = $self->{px} / 2;
    my $span = abs($half - $self->{y1});
    my $angle = deg2rad( $self->{fov} ) / $self->{px} * $span;
    my $height = $D / ($self->{y2} - $self->{y1}) * $span;
    $self->{est_dist} = cot($angle) * $height;
    $self->{r1} = $angle;
    $self->{angle} = rad2deg($angle);
}
