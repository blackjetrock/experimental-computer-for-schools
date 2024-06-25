// 
// Experimental Schools Computer Television
//
//

$fn = 200;

lcd_disp_x = 136.55;
lcd_disp_y = 104.9;
lcd_disp_z = 10.8;

lcd_pcb_x = 148.3;
lcd_pcb_y = 120.55;
lcd_pcb_z = 1.6;

lcd_aa_x = 115.7;
lcd_aa_y = 86.37;
lcd_aa_z = 10.1;

lcd_va_x = 120.14;
lcd_va_y = 92.14;
lcd_va_z = 0.5;

module lcd_a()
{
     difference()
     {
	  cube([lcd_va_x, lcd_va_y, lcd_va_z], center=true);
	  cube([lcd_aa_x, lcd_aa_y, lcd_aa_z], center=true);
     }
}

module lcd()
{
     cube([lcd_pcb_x, lcd_pcb_y, lcd_pcb_z], center=true);
     translate([0, 0, lcd_disp_z/2+lcd_pcb_z/2])
	  cube([lcd_disp_x, lcd_disp_y, lcd_disp_z], center=true);
    
     translate([0, 0, lcd_disp_z+lcd_pcb_z/2+lcd_va_z])
	  lcd_a();
}

big_rad_scale_1 = 15;
big_rad_scale_2 = 10;
cut_h = 50;

module round_x(offset, base_rad)
{
     rad = base_rad*big_rad_scale_1;
    
     translate([0, offset, 0])
	  cylinder(r=rad, h=cut_h);  
}

bez_x = 10;
bez_y = 10;
bez_t = 3;

module bezel_prism(len)
{

     translate([-len/2, 0, lcd_disp_z+lcd_pcb_z/2])
	  rotate([90, 0, 0])
	  rotate([0, 90, 0])
	  linear_extrude(len) 
     {
	  polygon(
	       points = 
	       [
		    [0,                0],
		    [bez_x+bez_t,      0],
		    [bez_x+bez_t,  bez_y],
		    [bez_x,        bez_y],

		    ]
	       );
     }
 
}

module round_y(offset, base_rad)
{
     rad = base_rad*big_rad_scale_2;
    
     translate([offset, 0, 0])
	  cylinder(r=rad, h=cut_h);  
}

module tv_front_cutout()
{
     translate([0, 0, lcd_disp_z+lcd_pcb_z/2+lcd_va_z-10])
	  intersection()
     {
	  round_x(-lcd_va_y*big_rad_scale_1+lcd_va_y/2, lcd_va_y);
	  round_x( lcd_va_y*big_rad_scale_1-lcd_va_y/2, lcd_va_y);
	  round_y(-lcd_va_x*big_rad_scale_2+lcd_va_x/2, lcd_va_x);
	  round_y( lcd_va_x*big_rad_scale_2-lcd_va_x/2, lcd_va_x);
     }
}

module tv_bezel()
{
     translate([0, lcd_aa_y/2, 0])
	  bezel_prism(lcd_disp_x+bez_t*2-0.8);
    
     translate([0, -lcd_aa_y/2, 0])
	  rotate([0, 0, 180])
	  bezel_prism(lcd_disp_x+bez_t*2-0.8);

     translate([-lcd_aa_x/2, 0, 0])
	  rotate([0, 0, 90])
	  bezel_prism(lcd_disp_y+bez_t*2);

     translate([lcd_aa_x/2, 0, 0])
	  rotate([0, 0, -90])
	  bezel_prism(lcd_disp_y+bez_t*2);

}

echo("X=",lcd_disp_x+bez_t*2-0.8);
echo("Y=",lcd_disp_y+bez_t*2);

module tv_front()
{
     difference()
     {
	  tv_bezel();
	  
	  tv_front_cutout();
     }
}


rh_p_x = (lcd_disp_y+bez_t)*34/116+10;
rh_p_th = 2.0;

slot_x = 9;
slot_y = 0.84;
slot_off_x = -8;
slot_off_y = 52.5;

slot_panel_xo = -14;
slot_row_w = slot_x+2;
slot_v_spacing = slot_y*2;
line_z = 1.0;

module tv_rh_panel()
{
     difference()
     {
	  cube([rh_p_x, lcd_disp_y+bez_t*2+bez_t/2, rh_p_th], center=true);
        
	  //Grill
	  for(p=[0:slot_v_spacing:20*slot_v_spacing])
	  {
	       translate([slot_panel_xo+slot_row_w+slot_off_x, p-slot_off_y, 0])
		    cube([slot_x, slot_y, 100], center=true);

	       translate([slot_panel_xo+2*slot_row_w+slot_off_x, p-slot_off_y, 0])
		    cube([slot_x, slot_y, 100], center=true);

	       translate([slot_panel_xo+3*slot_row_w+slot_off_x, p-slot_off_y, 0])
		    cube([slot_x, slot_y, 100], center=true);
	  }
      
      translate([-(lcd_aa_x/2+rh_p_x/2+bez_x)+(lcd_disp_x+bez_t*2-0.8)/2, 0, rh_p_th/2+0.01])
      cube([1.0, 400, line_z], center=true);

      translate([-(lcd_aa_x/2+rh_p_x/2+bez_x)+(lcd_disp_x+bez_t*2-0.8)/2+rh_p_x/2, -15, rh_p_th/2+0.01])
      cube([rh_p_x, 1.0, line_z], center=true);

     }
}

lcd();

tv_front();
translate([lcd_aa_x/2+rh_p_x/2+bez_x, 0, lcd_pcb_z+lcd_disp_z+bez_x-1.8])
tv_rh_panel();
