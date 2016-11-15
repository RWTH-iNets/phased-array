cabine_height = 190;

cabine_depth = 345;
ant_cabine_depth = 100;

floor_height = 10;
base_floor_height = 15;

rack_hole_spacing_x = 465;
cabine_wall_width = 16;

cabine_width = rack_hole_spacing_x - cabine_wall_width;
echo("cabine_width = ", cabine_width, "mm");
echo("floor_height = ", floor_height, "mm");
echo("wall_width = ", cabine_wall_width, "mm");

//BASE
cube([cabine_width, cabine_depth, base_floor_height]);

//COTOCLOCK
octo_floor_z = cabine_height + base_floor_height;
octo_width = 440;
octo_height = 45;
octo_depth = 150;
octo_y = cabine_depth - octo_depth - 90;
translate([0, 0, octo_floor_z]) {
    octo_spacer_width = (cabine_width - octo_width) / 2;
    cube([cabine_width, cabine_depth, floor_height]);
    translate([octo_spacer_width, octo_y, floor_height]) 
    cube([octo_width, octo_depth, octo_height]);
}

//USRPS
usrp_y = 5;
usrp_floor_z = 3 * cabine_height + 2*floor_height + base_floor_height;
usrp_width = 220;
usrp_height = 40;
usrp_depth = 255;
translate([0, 0, usrp_floor_z]) {
    usrp_spacer_width = (cabine_width - 2 * usrp_width) / 3;
    cube([cabine_width, cabine_depth, floor_height]);
    translate([usrp_spacer_width, usrp_y, floor_height])
    cube([usrp_width, usrp_depth, usrp_height]);
    translate([2*usrp_spacer_width + usrp_width, usrp_y, floor_height])
    cube([usrp_width, usrp_depth, usrp_height]);
}

//SPLITTER
splitter_y = 5;
splitter_floor_z = 2 * cabine_height + floor_height + base_floor_height;
splitter_width = 150;
splitter_height = 30;
translate([0, 0, splitter_floor_z]) {
    splitter_spacer_width = (cabine_width - splitter_width) / 2;
    cube([cabine_width, cabine_depth, floor_height]);
    translate([splitter_spacer_width, splitter_y, floor_height]) 
    cube([splitter_width, splitter_width, splitter_height]);
}

//BASE
base_y = 0;
translate([0, 0, splitter_floor_z]) {
    splitter_spacer_width = (cabine_width - splitter_width) / 2;
    cube([cabine_width, cabine_depth, floor_height]);
    translate([splitter_spacer_width, splitter_y, floor_height]) 
    cube([splitter_width, splitter_width, splitter_height]);
}

//ANTENNA
ant_y = ant_cabine_depth / 2;
ant_floor_z = 4 * cabine_height + base_floor_height + 3 * floor_height;
ant_width = 300;
ant_height = 100;
ant_depth = 3;
translate([0, 0, ant_floor_z]) {
    ant_spacer_width = (cabine_width - ant_width) / 2;
    translate([0, (cabine_depth - ant_cabine_depth) / 2, 0]) {
        cube([cabine_width, ant_cabine_depth, floor_height]);
        translate([ant_spacer_width, ant_y, floor_height]) 
        cube([ant_width, ant_depth, ant_height]);
    }
}
