#! /bin/sh

#convert -alpha off $1 -set colorspace Gray -separate -average 0.png
#convert -selective-blur 4x4+50% 0.png 1.png
#convert -negate 1.png 2.png
#composite -compose linear-dodge 0.png 2.png 3.png
#convert $1 -canny 0x1+10%+30% 0.png
convert $1 -colorspace Gray -negate -edge 2 -negate 0.png
