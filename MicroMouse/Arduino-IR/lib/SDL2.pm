use NativeCall;
use SDL2::Raw;

my Str $lib;
BEGIN {
    $lib = 'SDL2';
}

enum SDL_RenderFlip (
    'SDL_FLIP_NONE',
    'SDL_FLIP_HORIZONTAL',
    'SDL_FLIP_VERTICAL',
);

sub SDL_CreateRGBSurface(uint32 $flags, int32 $width, int32 $height, int32 $depth,
     uint32 $Rmask, uint32 $Gmask, uint32 $Bmask, uint32 $Amask) is native($lib) is export returns SDL_Surface {*}

sub SDL_FreeSurface(SDL_Surface) is native($lib) is export {*}

sub SDL_QueryTexture(SDL_Texture, uint32 $format is rw, int32 $access is rw, int32 $w is rw, int32 $h is rw) is native($lib) is export returns int32 {*}

class SDL_Color is repr('CStruct') is rw {
    has uint8 $.r = 0;
    has uint8 $.g = 0;
    has uint8 $.b = 0;
    has uint8 $.a = 255;

    method new($r=0, $g=0, $b=0, $a=255) {
        return self.bless(:$r, :$g, :$b, :$a);
    }
}

# :vim set syn=perl6:
