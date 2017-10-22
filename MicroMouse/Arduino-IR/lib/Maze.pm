use Algorithm::MinMaxHeap;
use Maze::SDF;

constant EDGES = SIZE * 2 + 1;
constant U = 1;
constant R = 2;
constant D = 4;
constant L = 8;
constant KNOWN_U = 16;
constant KNOWN_R = 32;
constant KNOWN_D = 64;
constant KNOWN_L = 128;

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

sub add_joint($x, $y, @walls) {
    @walls[$y][$x] = True;
    if $x > 1         { $heap.insert(Edge.new($x-1, $y, $x-2, $y)) }
    if $x < EDGES - 1 { $heap.insert(Edge.new($x+1, $y, $x+2, $y)) }
    if $y > 1         { $heap.insert(Edge.new($x, $y-1, $x, $y-2)) }
    if $y < EDGES - 1 { $heap.insert(Edge.new($x, $y+1, $x, $y+2)) }
}

sub generate_maze() is export {
    my @walls[EDGES];
    for ^EDGES -> $y {
        for ^EDGES -> $x {
            @walls[$y][$x] = $x == 0 || $y == 0 || $x == @walls.end || $y == @walls.end;
        }
    }
    my @goal = -2,0,2 X -2,0,2;
    for @goal -> ($x, $y) {
        @walls[$y+SIZE][$x+SIZE] = True;
    }
    for @goal.pick(2) -> ($x, $y) {
        add_joint($x+SIZE, $y+SIZE, @walls);
    }
    my @seg = -2,2 X -1,1;
    for @seg -> ($x, $y) {
        @walls[$x+SIZE][$y+SIZE] = True;
        @walls[$y+SIZE][$x+SIZE] = True;
    }
    my ($x,$y) = @seg.pick;
    @walls[$y+SIZE][$x+SIZE] = False;

    @walls[1][2] = True;
    @walls[2][2] = True;
    add_joint(4,0,@walls);
    add_joint(0,Int(SIZE.rand)*2,@walls);
    add_joint(@walls.end,Int(SIZE.rand)*2,@walls);
    add_joint(Int(SIZE.rand)*2,@walls.end,@walls);

    my $r = SIZE+1;
    ($x, $y) = Int($r.rand), Int($r.rand);
    add_joint($x*2, $y*2,@walls);

    while (not $heap.is-empty) {
        my $edge = $heap.pop-max;
        if !@walls[$edge.jy][$edge.jx] {
            @walls[$edge.wy][$edge.wx] = True;
            add_joint($edge.jx, $edge.jy,@walls);
        }
    }
    write_maze(@walls);
    return @walls;
}

# vim: ft=perl6 sw=4 ts=4 et
