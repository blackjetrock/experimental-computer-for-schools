// Support for keyboard
//

$fn=100;


module pre()
{
 cube([40, 20, 33], center=true);
}

module rem()
{
    translate([0, 0, -5])
    cube([20, 20+0.1, 33-10+0.1], center=true);
    
    translate([0, -20/2, 33/2])
    
    rotate([-12, 0,0])
    //translate([0, 20/2+30/2-20/2, 10/2+(33-10)/2+5])
    translate([0, 30/2, 10/2])
    cube([60, 30, 10], center=true);
    
    translate([0, 0, 10])
    rotate([-12, 0, 0])
    cylinder(d=3.2, h=20, center=true);
    
    translate([-15, 0, -10])
    cylinder(d=2.8, h=20, center=true);

    translate([ 15, 0, -10])
    cylinder(d=2.8, h=20, center=true);
    
}

module support()
{
    difference()
    {
        pre();
        rem();
    }
}


support();

