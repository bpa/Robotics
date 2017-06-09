use NativeCall;
use SDL2::Raw;

my $lib;
BEGIN {
    $lib = 'SDL2_image';
}

enum IMG_InitFlags (
    :IMG_INIT_JPG(0x00000001),
    :IMG_INIT_PNG(0x00000002),
    :IMG_INIT_TIF(0x00000004),
    :IMG_INIT_WEBP(0x00000008),
);

sub IMG_Init(int32 $flags) is native($lib) is export returns int32 {*}
sub IMG_Load(Str) is native($lib) is export returns SDL_Surface {*}

# :vim syn=perl6
