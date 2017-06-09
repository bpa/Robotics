use NativeCall;
use SDL2::Raw;

class MazeRenderer is export {
    has $.x;
    has $.y;
    has $.width;
    has @!lines;

    method new($x, $y, Int $width, @maze) {
        my @lines;

        sub line($wall, $_x, $_y, @line) {
            if @line {
                if $wall {
                    @line[2] = $x + $_x * $width;
                    @line[3] = $y + $_y * $width;
                }
                else {
                    if @line.elems == 4 {
                        @lines.push: @line.clone;
                    }
                    @line = ();
                }
            }
            elsif $wall {
                @line = $x+$_x*$width, $y+$_y*$width;
            }
        }

        for ^@maze.elems -> $_y {
            my (@h_line, @v_line);
            for ^@maze.elems -> $_x {
                line(@maze[$_y][$_x], $_x, $_y, @h_line);
                line(@maze[$_x][$_y], $_y, $_x, @v_line);
            }
            if @h_line.elems == 4 {
                @lines.push: @h_line;
            }
            if @v_line.elems == 4 {
                @lines.push: @v_line;
            }
        }
        return self.bless(:$x, :$y, :$width, :@lines);
    }

    submethod BUILD (:$!x, :$!y, :$!width, :@!lines) {
    }

    method render($renderer) {
        SDL_SetRenderDrawColor($renderer, 0, 0, 255, 0);
        for @!lines -> $l {
            SDL_RenderDrawLine($renderer, $l[0], $l[1], $l[2], $l[3]);
        }
    }
}

# vim: syn=perl6
