/*  _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
 * / \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \  
 * \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ 
 *
 * Hub to mount a 12mm hex RC car wheel to a Micro Metal Gear Motor
 * you will need to pause the print to place the M4 nut inside
 * the height will be shown in the console
 * 
 * 	          visit
 * 		www.ideahex.com
 * 
 * Written by Damian Kleiss 
 *  _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
 * / \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \  
 * \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \*/
 
 
 // HUB MOTOR END //
 shaftMountOuterWidth = 13.5;
 shaftDia = 3.6;        // Diameter of the motor shaft
 shaftLength = 8.1;     // Length of motor shaft
 shaftFlat = 2.9;       // This is the flat part of the motor shaft
 shaftNutWidth = 5.9;   // distance across flats of shaft nuts
 shaftnutHeight = 2.8;  // height of shaft nut
 shaftBoltDia   = 3.5;  // Required hole for grub screw
 
 // HUBWHEEL END //
 hexWidth = 12.1;       // width between flats of the Wheel Hex
 hexLength = 6.2;       // Length of the hex that goes into the wheel
 wheelNutWidth = 7.7;   // Width between flats of the nut that the wheel is held on with
 wheelNutHeight = 3.9;  // Height of the nut that holds the wheel on
 wheelBoltDia = 3.5;    // Diameter of the hole 
 
 // ORING  //
 oringInnerDia   = 50;
 oringThickness  = 5;
 
 // WHEEL //
 wheelDia = oringInnerDia+oringThickness;
 wheelThickness = oringThickness;
  
 // GENERIC //
 minWallThickness = 0.9;    // This is only used between the shaft nut and shaft
 smidge = 0.01; // small value used to get manifold geometry
 
 $fn = 100;  // number of facets that make up round things
 
 
 // ASSMBLY //

translate([0,0,wheelThickness/2])
{
  Wheel(hubShape = "Round", shaft = "D");
  Oring(id = oringInnerDia, thickness = oringThickness);
}

//color("blue")
// rotate([180,0,0])
//HubHoles(shape = "Hex");
//color("red")
//
//Hub(shape = "Round");
 
 module Wheel(hubShape = "Round", shaft = "D")
 {
    color("red")
    difference()
    {
        union()
        {
             translate([0,0,shaftLength/2+1])         
             Hub(shape = hubShape);
             Rim();
        }
        translate([0,0,shaftLength/2+1])
        rotate([180,0,0])
        HubHoles(shape = shaft);
    }
}


 
 
 module Rim()
 {
     difference()
     {
        cylinder(d=wheelDia,h=wheelThickness,center=true);
        Oring(id = oringInnerDia, thickness = oringThickness);
     }
 }


module Oring(id = 50, thickness = 5)
{
    color("DimGray")
    rotate_extrude(convexity = 10)
    {
        translate([(id+thickness)/2,0,0])
        circle(r=thickness/2);
    }
}

module Hub(shape = "Hex")
{
        rotate([0,0,30])
    if(shape == "Hex")
    {
        cylinder(r =  shaftMountOuterWidth / 2 / cos(180/6), h = shaftLength, $fn=6, center = true); // Motor End
    }
    else if(shape == "Round")
    {
        cylinder(r =  shaftMountOuterWidth / 2 / cos(180/6), h = shaftLength, center = true); // Motor End
    }
        
}

module HubHoles(shape = "D")
{
          // Shaft Mount Holes        
        color("red")
    
    if (shape == "D")
    {
        union()
        {                
            rotate([0,90,0])
            {
                translate([0,0,shaftDia-shaftFlat+minWallThickness])
                hull()      // Hull two nuts so we can slip a nut in after the hub has been printed
                {
                    cylinder(r = shaftNutWidth / 2 / cos(180/6), h = shaftnutHeight, $fn=6); // ShaftNut Hole
                    translate([shaftLength/2,0,0])
                    cylinder(r = shaftNutWidth / 2 / cos(180/6), h = shaftnutHeight, $fn=6); // ShaftNut Hole
                }
                translate([0,0,hexWidth/2])
                cylinder(r = shaftBoltDia /2, h=hexWidth,center = true,$fn=100);            // Shaft Bolt hole
            }
            
            
            // Shaft Hole
            difference()
            {
                cylinder(r=shaftDia/2,h=shaftLength+smidge,center = true, $fn = 100);
                translate([(shaftDia-(shaftDia-shaftFlat)),0,0])
                cube([shaftDia,shaftDia,shaftLength+(smidge*2)],center=true);       // Shaft Flat
            }
        }  
    }
    else if (shape == "Hex")
    {    
        cylinder(r = shaftDia / 2 / cos(180/6), h = shaftLength+smidge,center = true, $fn=6); // ShaftNut Hole
        translate([0,0,shaftLength/2 + wheelThickness/2])
        cylinder(r = wheelBoltDia, h = wheelThickness,center = true); // ShaftNut Hole
    }
    
}


 
 
// echo("Pause at Z height of ",shaftLength+wheelNutHeight, " mm");
 

/*
 difference() // Uncomment for cross section view
 {
     union()
     {      
         
         // WHEEL END //
         translate([0,0,shaftLength])
         translate([0,0,hexLength/2])
         rotate([0,0,30])       // Rotate so it lines up with shaft end
         difference()
         {             
             cylinder(r =  hexWidth / 2 / cos(180/6), h = hexLength, $fn=6, center = true); // Wheel Hex End
             color("red")
             union()
             {
                 translate([0,0,-(hexLength+smidge)/2])  // Move nut hole to end of Hex
                 cylinder(r =  wheelNutWidth / 2 / cos(180/6), h = wheelNutHeight, $fn=6 ); // Wheel nut hole
                 
                 cylinder(r=wheelBoltDia/2,h=hexLength+smidge,center=true,$fn=100); // wheel bolt hole
             }
         }
         
         // MOTOR END //
         translate([0,0,shaftLength/2])
         difference()
         {
            rotate([0,0,30])
            cylinder(r =  shaftMountOuterWidth / 2 / cos(180/6), h = shaftLength, $fn=6, center = true); // Motor End
               
            // Shaft Mount Holes        
            color("red")
            union()
            {                
                rotate([0,90,0])
                {
                    translate([0,0,shaftDia-shaftFlat+minWallThickness])
                    hull()      // Hull two nuts so we can slip a nut in after the hub has been printed
                    {
                        cylinder(r = shaftNutWidth / 2 / cos(180/6), h = shaftnutHeight, $fn=6); // ShaftNut Hole
                        translate([shaftLength/2,0,0])
                        cylinder(r = shaftNutWidth / 2 / cos(180/6), h = shaftnutHeight, $fn=6); // ShaftNut Hole
                    }
                    translate([0,0,hexWidth/2])
                    cylinder(r = shaftBoltDia /2, h=hexWidth,center = true,$fn=100);            // Shaft Bolt hole
                }
                
                
                // Shaft Hole
                difference()
                {
                    cylinder(r=shaftDia/2,h=shaftLength+smidge,center = true, $fn = 100);
                    translate([(shaftDia-(shaftDia-shaftFlat)),0,0])
                    cube([shaftDia,shaftDia,shaftLength+(smidge*2)],center=true);       // Shaft Flat
                }
            }
        }
    }
    translate([-shaftMountOuterWidth,0,-smidge/2])
    cube([shaftMountOuterWidth*2,shaftMountOuterWidth*2,shaftLength+hexLength+smidge],center=false);
}
*/
 