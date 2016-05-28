#! /usr/bin/perl

use strict;
use warnings;
use Image::Magick;

my $orig = Image::Magick->new;
$orig->Read($ARGV[0]);
$orig->Set(alpha=>'Off');
$orig->Grayscale(channel=>'Average');
#$orig->Separate;
#$orig->Modulate(hue=>1,saturation=>0);
$orig->Write('1.png');
my $copy = $orig->Clone();
$copy->SelectiveBlur(geometry=>'4x4+50%');
$copy->Write('2.png');
$copy->Negate();
$copy->Write('3.png');
#$copy->Evaluate(value=>50,channel=>'a');
#$copy->Modulate(brightness=>50);
#$orig->Modulate(brightness=>50);
$orig->Composite(image => $copy, compose => 'ColorDodge');
$orig->Write('4.png');
