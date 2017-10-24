#! /usr/bin/env perl6

use XML::Writer;

constant SIZE = 8;
constant MM = 0.001;
constant INCH = 0.0254;
constant IN_2 = INCH / 2;
constant IN_4 = INCH / 4;
constant WALL = INCH * 2;
constant UNIT_SIZE = 7 * INCH;
constant BLACK = ['0 0 0 1'];
constant WALL_Z = WALL / 2 + IN_2;

sub write_maze(@walls) is export {
    mkdir "maze";
    write_config();
    write_model(@walls);
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

sub write_model(@walls) {
    spurt "maze/model.sdf", XML::Writer.serialize(
        :sdf[
            :version('1.6'),
            model => [
                :name("{SIZE}x{SIZE} Maze"),
                static => ['1'],
                link => links(@walls),
            ]
        ]
    )
}

sub links(@maze) {
    my @l = :name('Maze');
    @l.append(base(SIZE * UNIT_SIZE + IN_2));
	@l.append(post(SIZE/2, SIZE/2));

	for ^@maze.elems -> $y {
		for ^@maze.elems -> $x {
			if !@maze[$x][$y] {
				next;
			}

			if $x %% 2 {
				if $y % 2 {
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

sub base ($size) {
    return (
    collision=>[
        :name("Base_Collision"),
        pose=>["{$size/2} {$size/2} {IN_4} 0 0 0"],
        geometry=>[box=>[size=>["$size $size {IN_2}"]]],
    ],
    visual=>[
        :name("Base_Visual"),
        pose=>["{$size/2} {$size/2} {IN_4} 0 0 0"],
        geometry=>[box=>[size=>["$size $size {IN_2}"]]],
        material=>[
            script=>[
                uri=>["file://media/materials/scripts/gazebo.material"],
                name=>["Gazebo/FlatBlack"],
            ],
        ],
        meta=>[layer=>["0"]],
    ]);
}

sub post ($x, $y) {
    my $name = "Post{$x}_{$y}";
    return (
    collision=>[
        :name("{$name}_Collision"),
        pose=>["{$x * UNIT_SIZE + IN_4} {$y * UNIT_SIZE + IN_4} {WALL_Z} 0 0 0"],
        geometry=>[box=>[size=>["{IN_2} {IN_2} {WALL}"]]],
    ],
    visual=>[
        :name("{$name}_Visual"),
        pose=>["{$x * UNIT_SIZE + IN_4} {$y * UNIT_SIZE + IN_4} {WALL_Z} 0 0 0"],
        geometry=>[box=>[size=>["{IN_2} {IN_2} {WALL - MM}"]]],
        material=>[
            script=>[
                uri=>["file://media/materials/scripts/gazebo.material"],
                name=>["Gazebo/White"],
            ],
        ],
        meta=>[layer=>["0"]],
    ],
    visual=>[
        :name("{$name}_Visual_Cap"),
        pose=>["{$x * UNIT_SIZE + IN_4} {$y * UNIT_SIZE + IN_4} {WALL+IN_2} 0 0 0"],
        geometry=>[box=>[size=>["{IN_2} {IN_2} {MM}"]]],
        material=>[
            script=>[
                uri=>["file://media/materials/scripts/gazebo.material"],
                name=>["Gazebo/Red"],
            ],
        ],
        meta=>[layer=>["0"]],
    ]);
}

sub wall ($dir, $x, $y, $w, $h) {
    my $name = "{$dir}Wall{$x}_{$y}";
    return (
    collision=>[
        :name("{$name}_Collision"),
        pose=>["{$x * UNIT_SIZE + $w/2} {$y * UNIT_SIZE + $h/2} {WALL_Z} 0 0 0"],
        geometry=>[box=>[size=>["$w $h {WALL}"]]],
    ],
    visual=>[
        :name("{$name}_Visual"),
        pose=>["{$x * UNIT_SIZE + $w/2} {$y * UNIT_SIZE + $h/2} {WALL_Z} 0 0 0"],
        geometry=>[box=>[size=>["$w $h {WALL - MM}"]]],
        material=>[
            script=>[
                uri=>["file://media/materials/scripts/gazebo.material"],
                name=>["Gazebo/White"],
            ],
        ],
        meta=>[layer=>["0"]],
    ],
    visual=>[
        :name("{$name}_Visual_Cap"),
        pose=>["{$x * UNIT_SIZE + $w/2} {$y * UNIT_SIZE + $h/2} {WALL+IN_2} 0 0 0"],
        geometry=>[box=>[size=>["$w $h {MM}"]]],
        material=>[
            script=>[
                uri=>["file://media/materials/scripts/gazebo.material"],
                name=>["Gazebo/Red"],
            ],
        ],
        meta=>[layer=>["0"]],
    ]);
}

sub hwall($x, $y) {
    return wall('H', $x, $y, UNIT_SIZE, IN_2);
}

sub vwall($x, $y) {
    return wall('V', $x, $y, IN_2, UNIT_SIZE);
}

# vim: ft=perl6 sw=4 ts=4 expandtab
