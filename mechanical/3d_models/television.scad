// 
// Experimental Schools Computer Television
//
//

show_front = 1;
show_rear  = 0;
show_lcd = 0;
show_panel = 0;
show_top_panel = 1;

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

lcd_h2h_x = 140;
lcd_h2h_y = 112.5;

module lcd_a()
{
     difference()
     {
	  cube([lcd_va_x, lcd_va_y, lcd_va_z], center=true);
	  cube([lcd_aa_x, lcd_aa_y, lcd_aa_z], center=true);
     }
}

module lcd_screw()
{
     %cylinder(d=2.5, h=100, center=true);
}

module lcd()
{
     cube([lcd_pcb_x, lcd_pcb_y, lcd_pcb_z], center=true);
     translate([0, 0, lcd_disp_z/2+lcd_pcb_z/2])
	  cube([lcd_disp_x, lcd_disp_y, lcd_disp_z], center=true);
    
     translate([0, 0, lcd_disp_z+lcd_pcb_z/2+lcd_va_z])
	  lcd_a();
 
     translate([lcd_h2h_x/2, lcd_h2h_y/2, 0])
	  lcd_screw();

     translate([-lcd_h2h_x/2, lcd_h2h_y/2, 0])
	  lcd_screw();

     translate([lcd_h2h_x/2, -lcd_h2h_y/2, 0])
	  lcd_screw();

     translate([-lcd_h2h_x/2, -lcd_h2h_y/2, 0])
	  lcd_screw();
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
		    
		    //[0,                0],
		    //[bez_x+bez_t,      0],
		    //[bez_x+bez_t,  bez_y],
		    //[bez_x,        bez_y],

		    [0,                  0],
		    [bez_x,              0],
		    [bez_x,  bez_y-rh_p_th],
 		    [bez_x+bez_t,  bez_y-rh_p_th],
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
	  round_x(-lcd_va_y*big_rad_scale_1+lcd_va_y/2-1, lcd_va_y);
	  round_x( lcd_va_y*big_rad_scale_1-lcd_va_y/2+1, lcd_va_y);
	  round_y(-lcd_va_x*big_rad_scale_2+lcd_va_x/2-1, lcd_va_x);
	  round_y( lcd_va_x*big_rad_scale_2-lcd_va_x/2+1, lcd_va_x);
     }
}

module tv_bezel()
{
     translate([0, lcd_aa_y/2, 0])
	  bezel_prism(lcd_disp_x-0.8);
    
     translate([0, -lcd_aa_y/2, 0])
	  rotate([0, 0, 180])
	  bezel_prism(lcd_disp_x-0.8);

     translate([-lcd_aa_x/2, 0, 0])
	  rotate([0, 0, 90])
	  bezel_prism(lcd_disp_y+1.5);

     translate([lcd_aa_x/2, 0, 0])
	  rotate([0, 0, -90])
	  bezel_prism(lcd_disp_y+0.8);

     translate([-lcd_aa_x/2-bez_x-bez_t/2, lcd_aa_y/2+bez_x+bez_t/2, bez_y+lcd_disp_z+lcd_pcb_z/2-rh_p_th/2])
	  cube([bez_t, bez_t, rh_p_th], center=true);

     translate([-lcd_aa_x/2-bez_x-bez_t/2, -lcd_aa_y/2-bez_x-bez_t/2, bez_y+lcd_disp_z+lcd_pcb_z/2-rh_p_th/2])
	  cube([bez_t, bez_t, rh_p_th], center=true);

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
line_x = 0.5;

knob_d1 = slot_x*2;
knob_d2 = knob_d1 *1/3;
knob_h1 = 2.5;
knob_h2 = knob_h1*2;
tsz = 20;
tz = 0.5;

module k_text(a)
{
     translate([-tsz/2, -tsz/2, 30])
	  linear_extrude(tz)
	  text(a, size = tsz);
}
txt = [
     [  "1"],
     [  "2"],
     [  "3"],
     [  "4"],
     [  "5"],
     [  "6"],
     [  "7"],
     [  "8"],
     [  "9"],
     [  "10"],
     [  "11"],
     [  "12"],
     ];

module knob()
{

     translate([0, 0, knob_h1/2])    
	  cylinder(d=knob_d1, h=knob_h1, center=true);
     translate([0, 0, knob_h1/2+knob_h2/2])    
	  cylinder(d=knob_d2, h=knob_h2, center=true);

     for(rotz=[0:1:12])
     {    
	  translate([0, knob_d1*2/3,30])
	       rotate([0, 0, rotz*30])
	       translate([0, 0, knob_h1-tz+0.01])
	       k_text(txt[rotz]);
     }
}

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
      
	  translate([-(lcd_aa_x/2+rh_p_x/2+bez_x)+(lcd_disp_x+bez_t*2-0.8)/2+0.5, 0, rh_p_th/2+0.01])
	       cube([line_x, 400, line_z], center=true);

	  translate([-(lcd_aa_x/2+rh_p_x/2+bez_x)+(lcd_disp_x+bez_t*2-0.8)/2+slot_row_w, 22.5, rh_p_th/2+0.01])
	       cube([line_x, (lcd_disp_y+bez_t*2)/2+20, line_z], center=true);

	  translate([-(lcd_aa_x/2+rh_p_x/2+bez_x)+(lcd_disp_x+bez_t*2-0.8)/2+rh_p_x/2, -15, rh_p_th/2+0.01])
	       cube([rh_p_x, line_x, line_z], center=true);
      
	  translate([-2.5, 45, 0.5])
	       cube([20, 5, 1], center=1);

     }
     
     translate([7, 0, 0])
	  knob();

     translate([7, 20, 0])
	  knob();

     translate([15, 40, 0])
	  cylinder(d=5, h=3, center=1);

     translate([0, 37.5, 0])
	  cube([8, 5, 2], center=1);


}

module double_rounded_cuboid(x, y, z, rd)
{
     // Minkowski adds z dimensions of components
     scale([1, 1, 2*z/(2*rd+z)])
	  translate([-x/2, -y/2, 0])
	  minkowski()
     {  
	  cube([x-rd-rd, y-rd-rd, z], center=true);
	  translate([x/2, y/2, 0])
	       sphere(r=rd, $fn=100);
     }
}

module half_double_rounded_cuboid(x, y, z, rd)
{
     translate([0, 0, -z/4])
	  difference()
     {
	  double_rounded_cuboid(x, y, z, rd);
	  translate([0, 0, -z/2])
	       cube([x+10, y+10, z+0.5], center=true);
     }
}

sh_th = 2;

module hdrc_shell(x, y, z, rd)
{
     difference()
     {
	  half_double_rounded_cuboid(x, y, z, rd);
	  translate([0, 0, -sh_th])
	       half_double_rounded_cuboid(x-4*sh_th, y-4*sh_th, z, rd);
     }
}

module r_grill(n)
{
     rotate([0, 0, 90])

	  //Grill
	  for(p=[0:slot_v_spacing:n*slot_v_spacing])
	  {
	       translate([0, p, 0])
		    cube([slot_x, slot_y, 100], center=true);
	  
	       if(0)
	       {
		    translate([slot_panel_xo+2*slot_row_w+slot_off_x, p-slot_off_y, 0])
			 cube([slot_x, slot_y, 100], center=true);
	  
		    translate([slot_panel_xo+3*slot_row_w+slot_off_x, p-slot_off_y, 0])
			 cube([slot_x, slot_y, 100], center=true);
	       }
	  }
}

ty = lcd_disp_y+bez_t*2+bez_t/2;

cuboid_scale = 0.8;

//back_x = rh_p_x+lcd_aa_x+bez_x*2+bez_t;
//back_y = lcd_disp_y+bez_t*2+bez_t/2;
back_x = 207;
back_y = 139;

module tv_rear1()
{
     translate([rh_p_x/2, 0, 0])
     {
	  translate([0, 0, -rh_p_th*3])
	       rotate([0, 180, 0])
	       hdrc_shell(lcd_va_x, lcd_va_y*cuboid_scale, lcd_va_y/3, 5);
    
	  difference()
	  {
	       cube([back_x, back_y, rh_p_th*2], center=true);
        
	       double_rounded_cuboid(lcd_va_x, lcd_va_y*cuboid_scale, lcd_va_y/3, 5);
        
	       translate([0, 0, -rh_p_th])
		    cube([back_x-bez_t*2, back_y-bez_t*2, rh_p_th*2], center=true);
	  }
    
     }
}

module grills()
{
     translate([-65*slot_y, ty/2-slot_x*3/2, 0])
	  r_grill(16);

     translate([-65+55*slot_y, ty/2-slot_x*3/2, 0])
	  r_grill(16);

     translate([90, 0, 0])
     {
	  translate([-55*slot_y, ty/2-slot_x*3/2, 0])
	       r_grill(16);

	  translate([-55+55*slot_y, ty/2-slot_x*3/2, 0])
	       r_grill(16);

	  translate([-50*slot_y, ty/2-slot_x*5/2-3, 0])
	       r_grill(16);

	  translate([-135*slot_y, ty/2-slot_x*5/2-3, 0])
	       r_grill(16);

	  translate([-100*slot_y, ty/2-slot_x*5/2-3, 0])
	       r_grill(9);
     }
}

module tv_rear()
{
     difference()
     {
	  tv_rear1();
         
      //  Cutout for the panel (two panels so cables can fit through)
         // that allows the cables in.
         
      translate([0, 0, 0])
      {
          cube([30, 20, 300], center=true);
      }   
      
	  translate([22, 0, 0])
	  {
	       grills();

         rotate([0, 0, 180])
          {
              translate([90, 0, 0])
{
	  translate([-50*slot_y, ty/2-slot_x*5/2-3, 0])
	       r_grill(25);

	  translate([-110*slot_y, ty/2-slot_x*5/2-3, 0])
	       r_grill(25);
          }
      }
	  }
     }
}




if(show_lcd)
{
     lcd();
}

if(show_front)
{
     tv_front();
     translate([lcd_aa_x/2+rh_p_x/2+bez_x, 0, lcd_pcb_z+lcd_disp_z+bez_x-1.8])
	  tv_rh_panel();
}

if(show_rear)
{
     tv_rear();
}

cab1_d = 3;
cab2_d = 3.5;

module full_panel()
{
translate([0, 0, -33])
    {
        rotate([180, 0, 0])
        half_double_rounded_cuboid(50, 40, 2, 2, center=true);
    }
    
    translate([-5, 0, -36.4])
    difference()
    {
      cylinder(d=cab1_d+3, h=2);
      cylinder(d=cab1_d, h=2.1);
    }

    translate([5, 0, -36.4])
    difference()
    {
      cylinder(d=cab2_d+3, h=2, center=1);
      cylinder(d=cab2_d, h=10, center=1);
    }
    
}

if(show_panel)
{
difference()
{
  full_panel();

      translate([-40/2, -30/2, -40])
      cylinder(d=1.5, h=10);
      translate([40/2, -30/2, -40])
      cylinder(d=1.5, h=10);
      translate([-40/2, 30/2, -40])
      cylinder(d=1.5, h=10);
      translate([40/2, 30/2, -40])
      cylinder(d=1.5, h=10);
    
  translate([0, 0, -33])
  if( 1 )
  {
      show_tp = (1*show_top_panel);
      show_bp = 1-show_tp;
      chop = (-1*show_tp)+(1*show_bp);
      chop_y = 25/2 * chop;
      
      echo("chopy=",chop_y);
      
      translate([0, chop_y, 0])
      cube([60, 25, 40], center=true);
  }
}

}
