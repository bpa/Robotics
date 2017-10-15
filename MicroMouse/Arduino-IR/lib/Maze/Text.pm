use NativeCall;
use Algorithm::MinMaxHeap;
use Terminal::ANSIColor;

constant BOX = ('·', '╵', '╶', '└', '╷', '│', '┌', '├', '╴', '┘', '─', '┴', '┐', '┤', '┬', '┼');
constant SIZE = 8;
constant EDGES = SIZE * 2 + 1;
constant U = 1;
constant R = 2;
constant D = 4;
constant L = 8;
constant KNOWN_U = 16;
constant KNOWN_R = 32;
constant KNOWN_D = 64;
constant KNOWN_L = 128;

sub init is native('./solv') { * }
my $maze := cglobal('./solv', 'maze', CArray[uint8]);
my $dist := cglobal('./solv', 'dist', CArray[int32]);
sub cell(int32, int32) returns uint32 is native('./solv') { * }
sub closed(int32, int32) returns bool is native('./solv') { * }

my @walls[EDGES];

class Edge {
    also does Algorithm::MinMaxHeap::Comparable[Edge];
    has Num $.weight;
    has $.wx;
    has $.wy;
    has $.jx;
    has $.jy;

    method new($wx, $wy, $jx, $jy) {
        return self.bless(:$wx, :$wy, :$jx, :$jy);
    }

    submethod BUILD(:$!wx, :$!wy, :$!jx, :$!jy) {
        $!weight = rand;
    }

    method compare-to(Edge $e) {
        if $!weight == $e.weight {
            return Order::Same;
        }
        if $!weight > $e.weight {
            return Order::More;
        }
        return Order::Less;
    }
}

my Algorithm::MinMaxHeap[Algorithm::MinMaxHeap::Comparable] $heap .= new;

sub add_joint($x, $y) {
    @walls[$y][$x] = True;
    if $x > 1         { $heap.insert(Edge.new($x-1, $y, $x-2, $y)) }
    if $x < EDGES - 1 { $heap.insert(Edge.new($x+1, $y, $x+2, $y)) }
    if $y > 1         { $heap.insert(Edge.new($x, $y-1, $x, $y-2)) }
    if $y < EDGES - 1 { $heap.insert(Edge.new($x, $y+1, $x, $y+2)) }
}

sub generate_maze() {
    for ^EDGES -> $y {
        for ^EDGES -> $x {
            @walls[$y][$x] = $x == 0 || $y == 0 || $x == @walls.end || $y == @walls.end;
        }
    }
    for -2,0,2 X -2,0,2 -> ($x, $y) {
        add_joint($x+SIZE, $y+SIZE);
    }
    my @seg = -2,2 X -1,1;
    for @seg -> ($x, $y) {
        @walls[$x+SIZE][$y+SIZE] = True;
        @walls[$y+SIZE][$x+SIZE] = True;
    }
    my ($x,$y) = @seg.pick;
    @walls[$y+SIZE][$x+SIZE] = False;

    @walls[1][2] = True;
    add_joint(2,2);
    add_joint(4,0);
    add_joint(0,Int(SIZE.rand)*2);
    add_joint(@walls.end,Int(SIZE.rand)*2);
    add_joint(Int(SIZE.rand)*2,@walls.end);

    my $r = SIZE+1;
    ($x, $y) = Int($r.rand), Int($r.rand);
    add_joint($x*2, $y*2);

    while (not $heap.is-empty) {
        my $edge = $heap.pop-max;
        if !@walls[$edge.jy][$edge.jx] {
            @walls[$edge.wy][$edge.wx] = True;
            add_joint($edge.jx, $edge.jy);
        }
    }
}

sub color_maze($wall, $mask, $dir, $known) {
    my $
    if $
}

sub print_wall($x, $y) {
    if @walls[$y][$x] {
        my $c = 0;
        if $y > 0 && @walls[$y-1][$x] { $c +|= U }
        if $x > 0 && @walls[$y][$x-1] { $c +|= L }
        if $x < @walls.end && @walls[$y][$x+1] { $c +|= R }
        if $y < @walls.end && @walls[$y+1][$x] { $c +|= D }
        print BOX[$c];
    }
    else {
        print ' ';
    }
}

sub print_maze() {
#    my $c;
#    for ^1 -> $y {
#        for $maze[$y*SIZE..*] Z @walls[$y].rotor(2) -> ($m, ($l, $r)) {
#            color_maze($r, $m, U, KNOWN_U);
#            print_wall($w, $x,  $y);
#            $w++;
#            print_wall($w, $x+1,$y);
#            $w++;
#        }
#        #color_maze(@walls[@walls.end][$Y], $c.walls, R, KNOWN_R);
#        #print_wall(@walls.end, $Y);
#        #say '';
#        #$Y++;
#        #for ^SIZE -> $x {
#        #    my $X = $x*2;
#        #    $c = cell($x, $y);
#        #    color_maze(@walls[$X+1][$Y], $c.walls, L, KNOWN_L);
#        #    print_wall($X,  $Y);
#        #    print_wall($X+1,$Y);
#        #}
#        #color_maze(@walls[@walls.end][$Y], $c.walls, R, KNOWN_R);
#        #print_wall(@walls.end, $Y);
#        #say '';
#    }
#    #color_maze(@walls[@walls.end][1], 0xFF, D, KNOWN_D);
#    for ^@walls.elems -> $x {
#        print_wall($x, @walls.end);
#    }
#    say '', RESET;
}

# vim: syn=perl6
