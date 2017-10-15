use NativeCall;
use SDL2;
use SDL2::Raw;
use SDL2::Image;

sub init is native('./solv') { * }
my $maze := cglobal('./solv', 'maze', CArray[uint8]);
my $dist := cglobal('./solv', 'dist', CArray[int32]);

class Mouse is export {
    has $!render;
    has $!width;
    has @!walls;
    has $!x;
    has $!y;
    has $!dest;
    has Num $!r;
    has $!texture;

    submethod BUILD(:$!render, :$x, :$y, :$!width, :@!walls) {
        my $surf = IMG_Load("car.png");
        $!texture = SDL_CreateTextureFromSurface($!render, $surf);
        SDL_FreeSurface($surf);
        $!dest = scaled-rect($!texture, $!width);
        $!dest.x += $x + Int($!width/2);
        $!dest.y += $y + Int($!width/2);
        $!r = Num(-90);
    }

    sub scaled-rect($texture, $size) {
	    my (uint32 $f, int32 $a, int32 $w, int32 $h);
        SDL_QueryTexture($texture, $f, $a, $w, $h);
        my $major = max($w, $h);
        my $scale = $major / $size;
        $w = Int($w/$scale);
        $h = Int($h/$scale);
        return SDL_Rect.new(($size-$w)/2, ($size-$h)/2, $w, $h);
    }

    method render() {
        SDL_RenderCopyEx($!render, $!texture, SDL_Rect, $!dest, $!r, SDL_Point, SDL_FLIP_NONE );
    }
}

# vim: syn=perl6
