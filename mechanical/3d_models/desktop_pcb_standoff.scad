// Stand offs for the desktop version of the experimental
// schools computer

$fn = 100;

module pre()
{
    cube([8, 8, 11], center=true);
}

module rem()
{
    cylinder(d=1.5, h=11+0.1, center=true);
}


module standoff()
{
difference()
    {
        pre();
        rem();
    }
}


standoff();
