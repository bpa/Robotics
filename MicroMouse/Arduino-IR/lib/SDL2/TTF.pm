use NativeCall;
use SDL2;
use SDL2::Raw;

my Str $lib;
BEGIN {
    $lib = 'SDL2_ttf';
}

class Text {
	has $.texture;
	has $.w;
	has $.h;

	method draw(SDL_Renderer $render, $x, $y) {
		my $rect = SDL_Rect.new($x, $y, $.w, $.h);
    	SDL_RenderCopy($render, $.texture, Any, $rect);
	}

	method DESTROY() {
		SDL_DestroyTexture($.texture);
	}
}

sub TTF_Init() is native($lib) is export returns int32 {*}
sub TTF_Quit() is native($lib) is export {*}

sub TTF_OpenFont(Str, int32) is native($lib) is export returns Pointer {*}

sub TTF_RenderText_Solid(Pointer, Str, SDL_Color) is native($lib) is export returns SDL_Surface {*}

sub TTF_Text(SDL_Renderer $r, Pointer $p, Str $text, SDL_Color $color) is export returns Text {
	my $surface = TTF_RenderText_Solid($p, $text, $color);
	my $texture = SDL_CreateTextureFromSurface($r, $surface);
	SDL_FreeSurface($surface);
	my (uint32 $f, int32 $a, int32 $w, int32 $h);
	SDL_QueryTexture($texture, $f, $a, $w, $h);
	return Text.new(:$texture, :$w, :$h);
}

# vim: syn=perl6
