#! /bin/sh

convert -alpha off $1 -set colorspace Gray -separate -average 0.png
convert -selective-blur 4x4+50% 0.png 1.png
convert -negate 1.png 2.png
composite -compose linear-dodge 0.png 2.png 3.png

convert $1 -colorspace gray \
  \( +clone -tile pencil_tile.gif -draw "color 0,0 reset" \
  +clone +swap -compose color_dodge -composite \) \
  -fx 'u*.2+v*.8' sketch.gif
