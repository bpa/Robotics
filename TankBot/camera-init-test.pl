#! /usr/bin/perl

use strict;
use warnings;

use Math::Trig;
use Text::Table;

use constant FOOT   => 30.48;
use constant INCH   => 2.54;
use constant camera => 6 * INCH;
use constant D      => 20.5;

my @targets = (
    { height => 2 * INCH,  distance => 10 * FOOT },
    { height => 12 * INCH, distance => 15 * FOOT },
    { height => 8 * INCH,  distance => 20 * FOOT },
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
        push @scenes, Scene->new(calc_pixels( $t, $r ), $r);
    }
}

my @keys = sort keys %{$scenes[0]};
my $table = Text::Table->new(@keys);
for my $s (@scenes) {
	$table->add(map { $s->{$_} } @keys);
}
print $table;

sub calc_pixels {
    my ( $target, $mode ) = @_;
    return (
        project( $target->{height},     $target->{distance}, $mode ),
        project( $target->{height} + D, $target->{distance}, $mode )
    );
}

sub project {
    my ( $h, $d, $mode ) = @_;
    my $angle = atan2( abs( $h - camera ), $d );
    my $px   = $mode->{pixels} / deg2rad( $mode->{fov} ) * $angle;
    my $half = $mode->{pixels} / 2;
    return $h < camera ? int( $half - $px ) : int( $half + $px );
}

package Scene;

use Math::Trig;

use constant D => 20.5;

sub new {
    my ( $pkg, $y1, $y2, $mode ) = @_;
    my $self = bless {
        y1  => $y1,
        y2  => $y2,
        px  => $mode->{pixels},
        fov => $mode->{fov}
    }, $pkg;
    return $self;
}

