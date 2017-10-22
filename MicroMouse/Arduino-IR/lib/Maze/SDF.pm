#! /usr/bin/env perl6

use XML::Writer;

constant SIZE = 8;
constant INCH = 0.0254;
constant IN_2 = INCH / 2;
constant WALL = INCH * 2;
constant UNIT_SIZE = 7 * INCH;
constant BLACK = ['0 0 0 1'];

class Pose {
	has $.x=0;
	has $.y=0;
	has $.z=0;
	has $.roll=0;
	has $.pitch=0;
	has $.yaw=0;

	method Str {
    	return "$.x $.y $.z $.roll $.pitch $.yaw";
	}

	method inner {
    	return "0 0 {$.z/2} 0 0 0";
	}
}

sub write_maze(@walls) is export {
    write_config();
    write_model(@walls);
}

sub write_model(@walls) {
    spurt "maze/model.sdf", XML::Writer.serialize(
        :sdf[
            :version('1.6'),
            static => ['1'],
            pose => [Pose.new()],
            model => links(@walls),
        ]
    )
}

sub box ($name, $size, $pose, :$material="White") {
    return :link[
        :name($name),
        pose=>[:frame(''), $pose.Str],
        collision=>[
            :name("{$name}_Collision"),
            pose => [$pose.inner],
            geometry=>[box=>[size=>$size]],
        ],
        visual=>[
            :name("{$name}_Visual"),
            pose => [$pose.inner],
            geometry=>[box=>[size=>$size]],
            material=>[
                script=>[
                    uri=>["file://media/materials/scripts/gazebo.material"],
                    name=>["Gazebo/$material"],
                ],
            ],
            meta=>[layer=>["1"]],
        ],
    ];
}

sub links(@maze) {
    my $floor_w = SIZE * UNIT_SIZE + IN_2;
    my @l = (
        :name("{SIZE}x{SIZE} Maze"),
        box('Floor', ["$floor_w $floor_w {IN_2}"], Pose.new(), :material('FlatBlack')),
    );
	for ^@maze.elems -> $y {
		for ^@maze.elems -> $x {
			if !@maze[$x][$y] {
				next;
			}

			if $x %% 2 {
				if $y %% 2 {
					@l.append(post($x/2, $y/2));
				}
				else {
					@l.append(vwall($x/2, ($y-1)/2));
				}
			}
			elsif $y %% 2 {
				@l.append(hwall(($x-1)/2, $y/2));
			}
		}
	}

    return @l;
}

sub write_config() {
    spurt "maze/model.config", XML::Writer.serialize(
        :model[
            name => ["maze"],
            version => ['1.0'],
            sdf => [:version(1.6), 'model.sdf'],
            :author[
                name=>['Bruce Armstrong'],
                email=>['bruce@fortressofgeekdom.org']],
            description => ['MicroMouse Maze']
        ]
    );
}

sub post($x, $y) {
	my $name = "Post{$x}_{$y}";
	return
		box($name, ["{IN_2} {IN_2} {WALL}"],
			Pose.new(:x(UNIT_SIZE * $x), :y(UNIT_SIZE*$y), :z(IN_2))),
		box("{$name}_cap", ["{IN_2} {IN_2} .001"],
			Pose.new(:x(UNIT_SIZE * $x), :y(UNIT_SIZE*$y), :z(WALL+IN_2)),
			:material("Red"));
}

sub hwall($x, $y) {
	return :link[:name("hwall{$x}_{$y}")];
}

sub vwall($x, $y) {
	return :link[:name("vwall{$x}_{$y}")];
}

# vim: ft=perl6 sw=4 ts=4 expandtab
