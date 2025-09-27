




module small_esc_pcb()
{
  rotate([0,0,90])
    {
      translate([0, 0, 0])
        {
          scale([1,1,1])
            {
              //linear_extrude(height = 1, center = true, convexity = 10,twist=0)
              import("small_esc.stl", center=true);
            }
          // Cylindrical spike that can be used to check poisitions of 
          // components on the PCB
          
        }
    }
}



hull($fn = 50)
{
    
    translate([-20, -40, 30])
    {
        rotate([0, 30, 0])
        {
   cube([30, 25, 20], center=true);
        }   
    }
small_esc_pcb();
}
