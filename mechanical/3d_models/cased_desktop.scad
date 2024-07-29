// Case for desktop ESC
//
//

$fn = 200;

show_pcb             = 0;
show_top             = 0;
show_kb_part         = 0;
show_kb_part_proj    = 0;
show_left_support    = 0;
show_right_support   = 1;
show_front_panel     = 0;
show_base            = 0;
show_kb_pcb_holder   = 0;
show_angle_bracket   = 0;
show_name_plate      = 0;
show_lh_ruler        = 0;
show_rh_ruler        = 0;

// Angle of back part of kb plate
kb_bk_angle= 4;
front_panel_angle = 4;

// Extra 1mm all round for wall to hold PCBs
len_x = 229+2;
len_y = 104.5+2;
kb_plate_x = len_x;
kb_plate_y = len_y;  // Extra 1mm all round

kb_fr_y = 1;
kb_fr_z = 41.19/2;

kb_bk_x = len_x;
kb_bk_y = 1;
kb_bk_z = 20.76/2;

th=1.5;
wall_z = 2.8;

kb_panel_rad = 2.5;

front_panel_x = len_x - th;
front_panel_y = 68.5-kb_bk_z;

case_top_x = len_x+2*th;
case_top_y = 184; 

case_y = 291;
case_z = 110+th;

pcb_x = 229;
pcb_y = 104.5;
pcb_th = 1.6;

module kb_pcb()
{
     cube([pcb_x, pcb_y, pcb_th], center=true);
     import("overlay1-F_SilkS.dxf");
}

// Put a lip at one end of the PCB sandwich and a plate at the other end
// that allows a retaining bracket to be mounted

bracket_y = 10;

// Make a sliced rounded tube that can be used for filleting and radiusing corners.

module part_hollow_tube(len, rs, rb, angle1, angle2)
{
     translate([0, 0, 0])
	  rotate([0, 0, 0])
	  difference()
     {
	  rotate([0, 90, 0])
	       cylinder(r=rb, h=len, center=true);
	  rotate([0, 90, 0])
	       cylinder(r=rs, h=len+0.1, center=true);

	  translate([-len/2-0.1, -(rb+0.1), -(rb+0.1)])
	       cube([len+0.2, rb+0.11, rb+0.11]);
	  translate([-len/2-0.1, 0, -(rb+0.1)])
	       cube([len+0.2, rb+0.11, rb+0.11]);
	  translate([-len/2-0.1, 0, 0])
	       cube([len+0.2, rb+0.11, rb+0.11]);
         
	  rotate([angle1, 0, 0])
	       translate([-len/2-0.1, 0, 0])
	       cube([len+0.2, rb+0.1, rb+0.1]);
     }
}


module retain_a()
{   
// Wall around the PCBs
     cube([len_x, len_y, wall_z], center=true);

// Lip to hold one side
     translate([0, -len_y/2+3/2, -wall_z/2-1])
	  cube([len_x, 4, 2], center=true);
    
//Top bracket mount  
     translate([0, len_y/2+bracket_y/2, 0])
	  cube([len_x, bracket_y, wall_z], center=true);
}

module key_plate_1()
{
     translate([-len_x/2+1, -kb_plate_y/2+1, 0])
	  linear_extrude(height = th, center = true, convexity = 10,twist=0)
	  import("../kb_holes.dxf");


//cylinder(d=th, h=kb_bk_x, center=true);

//translate([0, kb_plate_y/2, 0])
//rotate([0, 90, 0])
//cylinder(d=th, h=kb_bk_x, center=true);

//translate([1, kb_plate_y/2-th*2/3, th])
//rotate([0, 90, 0])
//cylinder(d=th, h=kb_bk_x, center=true);
   
    
// Put a wall round the edge to hold the two PCBs
     translate([0, 0, -th/2-wall_z/2])
	  difference()
     {
	  retain_a();

// Take centre of cube away to leave a wall around the PCBs
	  cube([len_x-2, len_y-2, wall_z+0.1], center=true);
     
	  for(x=[-len_x/2+20:20:len_x/2-20])
	       translate([x, len_y/2+bracket_y/2, 0])   
		    cylinder(d=2.5, h=6, center=true);
     }

   
}

// Bar that holds keyboard pcbs in the frame

module holder_a()
{   
     translate([0, len_y/2+bracket_y/2, 0])
	  cube([len_x-170, bracket_y*2.0, wall_z], center=true);
}

module kb_pcb_holder()
{
     translate([0, -bracket_y/2, -th/2-wall_z/2])
	  difference()
     {
	  holder_a();

	  cube([len_x-2, len_y-4, wall_z+0.1], center=true);
     
	  for(x=[-len_x/2+20:20:len_x/2-20])
	       translate([x, len_y/2+bracket_y/2+4, 0])   
		    cylinder(d=2.5, h=6, center=true);
     }
   
}


module k_bevel()
{
     kh = [
	  [0, 0, 0], 
	  [29.986, -.4977, 0], 
	  [29.986+15, -.4977, 0], 
	  [29.986+2*15, -.4977, 0], 
	  [29.986+3*15, -.4977, 0], 
	  [29.986+5*15, -.4977, 0], 
	  [29.986+8*15, -.4977, 0],
	  [0, 29.4979, 0], 
	  [29.986, 29.4979, 0], 
	  [29.986+1*15, 29.4979, 0], 
	  [29.986+2*15, 29.4979, 0], 
	  [29.986+5*15, 29.4979, 0], 
	  [29.986+6*15, 29.4979, 0], 
	  [29.986+8*15, 29.4979, 0],
	  [-7.513+0*15, 14.5036, 0], 
	  [-7.513+1*15, 14.5036, 0], 
	  [-7.513+3*15, 14.5036, 0], 
	  [-7.513+4*15, 14.5036, 0], 
	  [-7.513+5*15, 14.5036, 0], 
	  [-7.513+8*15, 14.5036, 0], 
	  [-7.513+10*15-2, 14.5036, 0], 
	  [-7.513+0*15, 14.5036+29.9801, 0], 
	  [-7.513+1*15, 14.5036+29.9801, 0], 
	  [-7.513+3*15, 14.5036+29.9801, 0], 
	  [-7.513+4*15, 14.5036+29.9801, 0], 
	  [-7.513+5*15, 14.5036+29.9801, 0], 
	  [-7.513+7*15, 14.5036+29.9801, 0], 
	  [-7.513+8*15, 14.5036+29.9801, 0], 
	  ];
    
     translate([-kb_plate_x/2+37.5206+1, -kb_plate_y/2+37.9969+1, -th/2+th/2])    
	  for( a = kh) for( b = a) {
		    translate(a) 
			 cylinder(r1=6.252, r2=6.252+1, h=th+0.1, center=true);
	       }
}


module key_plate()
{
     difference()
     {
	  key_plate_1();
	  k_bevel();
       
     }
}

fr_block_x = 10;
fr_block_y = 10;
fr_block_z = 5;

//translate([0, 0, kb_fr_z/2])
//rotate([0, 90, 0])
//%cylinder(d=th, h=kb_bk_x, center=true);
module fr()
{
     translate([0, 0, kb_fr_z/2])
	  
	  rotate([12, 0, 0])
	  
	  translate([0, kb_plate_y/2, 0])
	  key_plate();

     sh2 = 2;
     translate([0, kb_plate_y*cos(12), kb_fr_z/2+kb_plate_y*sin(12)])
	  rotate([-kb_bk_angle, 0, 0])    
	  translate([0, 0, kb_bk_z/2-sh2/2])    
	  cube([len_x, th, kb_bk_z+sh2], center=true);
}

module kb_part()
{
 
     sh = 2.5;
    
// front vertical surface   
     translate([0, 0, sh/2])
	  cube([len_x, th, kb_fr_z+sh], center=true);

// Add some tabs for fixing the front of the part to the base
     for(off=[-100:50:100])
     {
	  translate([off, fr_block_y/2, -kb_fr_z/2+fr_block_z/2+th])
	       difference()
	  {
	       cube([fr_block_x, fr_block_y, fr_block_z], center=true);
	       cylinder(d=2.5, h=fr_block_z*1.2, center=true);
	  }
     }
    
     fr();
     
}

module top_side(off_y)
{

     translate([off_y, 0, -kb_fr_z/2])
	  rotate([0, -90, 0])
	  rotate([0, 0, 0])
	  linear_extrude(1) 
     {
	  polygon(
	       points = 
	       [
		    [0, -th], 
		    [20.5+th, -th], 
		    [42.5+th, 101], 
		    [115.08+th, 106.67], 
		    [104.5+th, 291], 
		    [0, 291], 
		    ]

		    
	       );
     }
 
}

module top_top()
{
     translate([0, (case_y-case_top_y)+case_top_y/2, case_z-kb_fr_z/2])
	  rotate([-3, 0, 0])
	  cube([case_top_x, case_top_y, th], center=true);
}

module top()
{
     top_top();
     top_side(-len_x/2-th/2);
     top_side( len_x/2+th/2);
}

module f_panel()
{
     linear_extrude(height = 1.5, convexity = 20, twist=0)
    
	  //import("../test.dxf");
	  import("../front_panel2.dxf");
     //import("../kb_holes.dxf");
    
}

module front_panel()
{
     translate([-115.5, kb_plate_y*cos(12)+0.7, kb_bk_z*2+kb_fr_z/2+kb_plate_y*sin(12)-kb_bk_z])
	  translate([0, 0, -kb_fr_z/2])
	  rotate([-front_panel_angle+90, 0, 0])
	  translate([0, front_panel_y/2-18.9, 0])
     {
	  f_panel();
	  translate([10, 10, 0])
	       cylinder(d=3, h=40, center=true);
	  translate([231-10, 10, 0])
	       cylinder(d=3, h=40, center=true);
	  translate([10, 64-10, 0])
	       cylinder(d=3, h=40, center=true);
	  translate([231-10, 64-10, 0])
	       cylinder(d=3, h=40, center=true);
           
	  //cube([front_panel_x, front_panel_y, th], center=true);
     }
}

module base()
{
     translate([0, 288/2, -9.5])
	  cube([231, 288, 1.5], center=true);
}

module base_support(dir)
{
     translate([0, 2, 55.6])
	  rotate([-4, 0, 0])
	  cube([10, 10, 65], center=true);
     
     cube([10, 10, 50], center=true);
    
     translate([dir*10, 0, -22.5])
	  cube([20, 10, 5], center=true); 

     translate([0, 10, -22.5])
	  cube([10, 20, 5], center=true); 
    
}

// 3D printed name plate. needs to be painted to match the original colour scheme.
// Embossed writing should helpo the painting.
// name plate is an engraved outline with a vinyl sticker
// stuck on top. So no separate part.

module name_plate_core()
{
if ( 0 )
{
     rotate([-4, 0, 0])
	  cube([38, 1.5, 8.5], center=true);
	  
     translate([-38/4, 0, 0])
	  rotate([90-4, 0, 0])
	  translate([0, 0, -30])
	  cylinder(d=3, h=30);
	  
     translate([+38/4, 0, 0])
	  rotate([90-4, 0, 0])
	  translate([0, 0, -30])
	  cylinder(d=3, h=30);
} 
}

module name_plate_writing()
{
      translate([-15, -0.8, -4])
      rotate([90-4, 0, 0])
	  linear_extrude(height = 0.7, center = true, convexity = 10,twist=0)
	  import("../name_plate_writing.dxf");
}

module name_plate()
{
     translate([-76.5, 108.1, 89.3])
     {
	  difference()
	  {
	       name_plate_core();
	       name_plate_writing();
	  }
     }
}

module left_support()
{
     difference()
     {   
	  base_support(1);
	  translate([0, 15, -40])
	       cylinder(d=3.5, h=40);
	  translate([15, 0, -40])
	       cylinder(d=3.5, h=40);
     }
     //translate([28, 2, 73])
	  //rotate([-4, 0, 0])
	  //cube([38+9, 5, 17], center=true); 

     if (show_lh_ruler)
       {
       translate([20, 0, 32.5])
       cube([10,10,115], center=true);
       }
}

rh_pl_x = 29;
rh_pl_z = 32;
sw_pl_hole_x = 24;
sw_pl_hole_z = 27;

module right_support()
{
     difference()
     {   
	  base_support(-1);
	  translate([0, 15, -40])
	       cylinder(d=3.5, h=40);
	  translate([-15, 0, -40])
	       cylinder(d=3.5, h=40);
         
         
      translate([-17.5, 4, 72])
         rotate([-3, 0, 0])
         {
        translate([sw_pl_hole_x/2, 10, sw_pl_hole_z/2])
       rotate([90, 0, 0])
       cylinder(r=3/2,h=20);

      translate([sw_pl_hole_x/2, 10, -sw_pl_hole_z/2])
       rotate([90, 0, 0])
       cylinder(r=3/2,h=20);
         }
     
         }
     
     translate([-17.5, 4, 72])
	  rotate([-3, 0, 0])
      difference()
     {
	  cube([rh_pl_x, 4, rh_pl_z], center=true); 
         
       translate([sw_pl_hole_x/2, 10, sw_pl_hole_z/2])
       rotate([90, 0, 0])
       cylinder(r=3/2,h=20);

       translate([-sw_pl_hole_x/2, 10, sw_pl_hole_z/2])
       rotate([90, 0, 0])
       cylinder(r=3/2,h=20);

       translate([sw_pl_hole_x/2, 10, -sw_pl_hole_z/2])
       rotate([90, 0, 0])
       cylinder(r=3/2,h=20);

       translate([-sw_pl_hole_x/2, 10, -sw_pl_hole_z/2])
       rotate([90, 0, 0])
       cylinder(r=3/2,h=20);

       translate([0, 10, 0])
       rotate([90, 0, 0])
       cylinder(d=12,h=20);

     }
     
     if (show_rh_ruler)
       {
       translate([20, 0, 32.5])
       cube([10,10,115], center=true);
       }

}

//==============================================================================

//f_panel();

//key_plate();

if( show_base )
{
     base();
}

if( show_pcb )
{
     translate([0, len_y/2, kb_fr_z-th])
	  rotate([front_panel_angle, 0])
	  kb_pcb();
}

module kb_part_full()
{
     translate([0, len_y-kb_panel_rad*2-0.4, sin(12)*len_y+kb_bk_z+th/2+kb_panel_rad-0.65])
	  rotate([180, 0, 0])
	  part_hollow_tube(len_x, kb_panel_rad, kb_panel_rad+th, kb_bk_angle, 0);
     
     difference()
     {
	  kb_part();
	  translate([0, kb_panel_rad-th/2, kb_panel_rad+th+4.86])
	       rotate([0, 0, 0])
	       part_hollow_tube(case_top_x, kb_panel_rad, kb_panel_rad*2, 12, 0);
     }
}
 
if(show_kb_part)
{
     kb_part_full();
}

if(show_kb_part_proj)
{
     projection()
     {
	  kb_part_full();
     }
}

if(show_top)
{
     top();
}

if(show_front_panel)
{
     front_panel();
}

if( show_left_support )
{
 
     difference()
     {
	  translate([-231/2+10-0, 115, 65/4+0.5])
	       left_support();
	  kb_part_full();
	  front_panel();
	  name_plate();
     }
}

if( show_right_support )
{
     difference()
     {
	  translate([231/2+10-20, 115, 65/4-0.5])
	       right_support();
	  kb_part_full();
	  front_panel();
     }
}

if( show_kb_pcb_holder )
{
     translate([0, len_y/2, kb_bk_z+15])
	  kb_pcb_holder();
}

// Bracket that bolts base to top cover
if( show_angle_bracket )
{
     difference()
     {
	  cube([14, 14, 14], center= true);
	  translate([0, 5, 5])
	       cube([14.1, 14, 14], center= true);
        
        
	  translate([0, 14/2-9/2, -12])
	       cylinder(d=2.5, h=20);

	  translate([0, 0, 14/2-9/2])
	       rotate([90, 0, 0])
	       cylinder(d=2.5, h=20);

     }
}

if( show_name_plate )
{
     name_plate();
    
}
