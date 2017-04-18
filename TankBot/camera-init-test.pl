#! /usr/bin/perl

use Math::Trig;

use constant FOOT   => 30.48;
use constant INCH   => 2.54;
use constant D      => 20.5;
use constant camera => 6 * INCH;

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

for my $t (@targets) {
    for my $r (@modes) {
        my ( $bottom_px, $top_px ) = calc_y( $t, $r );
        print "$bottom_px, $top_px\n";
        last;
    }
    last;
}

sub calc_y {
    my ( $target, $mode ) = @_;
    return (
        project( $target->{height},     $target->{distance}, $mode ),
        project( $target->{height} + D, $target->{distance}, $mode )
    );
}

sub project {
    my ( $h, $d, $mode ) = @_;
    my $angle = rad2deg(atan2(abs($h - camera), $d));
    my $px = $mode->{pixels} / $mode->{fov} * $angle;
    my $half = $mode->{pixels} / 2;
    return $h < camera ? int($half - $px): int($half + $px);
}
