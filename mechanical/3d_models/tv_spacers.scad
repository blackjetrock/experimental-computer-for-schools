// Spacers for LCD in TV replica
//

show_straight = 0;
show_corner = 1;

$fn=100;


module pre_st()
{
    cube([8, 40, 10], enter=true);
    

}

module rem_st()
{
    translate([-0.05, -0.05, 3])
    cube([8+0.1, 30, 10+0.1], enter=true);
    
    translate([8/2, 20, -10])
    cylinder(d=1.5, h=50);

    translate([8/2, 10, -10])
    cylinder(d=1.5, h=50);

    translate([8/2, 35, -10])
    cylinder(d=1.5, h=50);

}

module straight()
{
    difference()
    {
        pre_st();
        rem_st();
    }
}



// Corner spacer

module pre_cor()
{
    cube([8, 40, 10], enter=true);

    rotate([0, 0, 90])
    cube([8, 40, 10], enter=true);

}

module rem_cor()
{
    translate([-0.05, -0.05, 3])
    cube([8+0.1, 30, 10+0.1], enter=true);
    
    translate([8/2, 20, -10])
    cylinder(d=1.5, h=50);

    translate([8/2, 10, -10])
    cylinder(d=1.5, h=50);

    translate([8/2, 35, -10])
    cylinder(d=1.5, h=50);

}

module corner1()
{
    difference()
    {
        pre_st();
        rem_st();
    }
}

module corner()
{
translate([0, 40, 0])
rotate([0, 0, -180])
%corner1();
    
translate([40-0.1, 0, 0])
rotate([0, 0, 90])
corner1();
}

if( show_corner )
{
corner();
}
