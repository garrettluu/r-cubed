/*	The AMAZING 3D grapher for the TI84+CE
 *by gluu
 *currently in a very early alpha stage
 *like, i don't even have user input
 *TODO: Mathematical expression parser
 *DONE! better way of handling perspective
 *TODO: possible cylindrical and spherical plots?
 *
 *Based on the 3D parametric grapher on desmos
 *
 *
 */


/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
 
/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* CE Keypad C Library */
#include <keypadc.h>
#include <graphx.h>


//C is a dumb language 
//why do i have to define my functions up here and then actually write them all the way down there after i write main()
//still better than writing assembly code tho
void printText(const char *text, uint8_t x, uint8_t y);
void fillScreen(uint8_t color);
double x_x(double a, double b, double c);
double x_y(double a, double b, double c);
double y_x(double a, double b, double c);
double y_y(double a, double b, double c);
double z_x(double a, double b, double c);
double z_y(double a, double b, double c);
double function(double x, double y);
double mod (double a, double b);
//main function
//this is what gets run when the program starts
void main(void) {
	//initialize all the keymaps
	//I don't think all of them get used, but it's nice to have them anyways
	//feel free to delete the unused ones if you really want to save a few bytes
	kb_key_t key1;
	kb_key_t key2;
	kb_key_t key3;
	kb_key_t key4;
	kb_key_t key5;
	kb_key_t key6;	
	kb_key_t key7;
	    
	//initialize strings
	// i don't know what the * are for
    	 const char *title1 = "3D graphing project";
   	 const char *title2 = "by gluu";
	 const char *menutitle = "Menu";
	 const char *option1 = "1: Graph ";
	 const char *option2 = "2: Triangle ";
	 const char *option3 = "3: Circle ";
	 const char *option4 = "4: Quit ";

	 //declare variables and some constants
	 //uint8_t is the best thing ever
	 //it's basically an unsigned 8bit integer that takes up less space than an int
	 //but functions the same as an int for all intents and purposes (except negatives)
	 uint8_t sel = 1; //used for menu selections
	 bool kill = false; //used to terminate program
	 bool lol = false; //used to initialize graphx
	 bool lol2 = false; //used to plot points
	 bool lol3 = false; //used to draw the perspective changer
	 uint8_t mode = 0; //used for menu selections
	 int i = 0;
	 double t; //parameter for plotting points
	 double a = -2.3; //yaw
	 double b = .6; //roll
	 double c = 0; //pitch; this never changes since i don't want people to mess with the rotation of the screen
	 double a1 = a; //dummy variables we'll use later
	 double b1 = b;
	 double c1 = c;
	 double g; //g and h are used for calculating the points to plot 
	 double _g;
	 double h;
	 double _h;
	 const double s = 15; //scale of the graph
	 const double s2 = 3.75; //how much of the graph gets shown
	 const double n = 10; //resolution of the grid of the graph is (higher = more detail)
	 double dx = 1 / (n*(1 + n)); //degree of precision when plotting; higher values mean less precise plotting. This value should be called dt to be mathematically correct, but I don't feel like changing it now
	 double d1;
	 double d2;
	 uint8_t xxNodes[1124][3];
	 uint8_t xyNodes[1124][3];
	 uint8_t yxNodes[1124][3];
	 uint8_t yyNodes[1124][3];


    /* This function cleans up the screen */
   	 prgm_CleanUp();
    
    /* Print a few strings */
   	 printText( title1, 0, 0 );
   	 printText( title2, 0, 1 );
    
    /* Wait for a key press */
   	 while( !os_GetCSC() );
    
    /* Clean up*/
   	 prgm_CleanUp();

	 //more strings

	//menu items
	 printText( menutitle, 0, 0 );
	 printText( option1, 0, 1 );
	 printText( option2, 0, 2 );
	 printText( option3, 0, 3 );
	 printText( option4, 0, 4 );
    /* Loop until 2nd is pressed */
    do {
	kb_Scan();
		/* Load all keyboard registers */
	key1 = kb_Data[kb_group_1];
	key2 = kb_Data[kb_group_2];
	key3 = kb_Data[kb_group_3];
	key4 = kb_Data[kb_group_4];
	key5 = kb_Data[kb_group_5];
	key6 = kb_Data[kb_group_6];
        key7 = kb_Data[kb_group_7];
	//a bunch of rather redundant switch statements
	//i'm too lazy to change them even though most of them could just be simple if-statements
	switch(key3) {
		case kb_1:
			sel=1;
			break;
		case kb_4:
			sel=4;
			break;
		default:
			break;
	}
	switch(key4) {
		case kb_2:
			sel=2;
			break;
		default:
			break;
	}
	switch(key5) {
		case kb_3:
			sel=3;
			break;
		default:
			break;
	}
        switch(key7) {
            	case kb_Down:           
                	sel++;
                	break;
            	case kb_Up:             
                	sel--;
                	break;
            	default:
                	break;
        }
	switch(sel) {
		case 0:
			sel=1;
		case 1:
			printText("*", 0, 1);
			printText( option1, 1, 1);
			printText( option2, 0, 2 );
		 	printText( option3, 0, 3 );
	 		printText( option4, 0, 4 );
			break;
		case 2:
			printText("*", 0, 2);
			printText( option2, 1, 2);
			printText( option1, 0, 1 );
	 		printText( option3, 0, 3 );
	 		printText( option4, 0, 4 );
			break;
		case 3:
			printText("*", 0, 3);
			printText( option3, 1, 3);
			printText( option2, 0, 2 );
	 		printText( option1, 0, 1 );
	 		printText( option4, 0, 4 );
			break;
		case 4:
			printText("*", 0, 4);
			printText( option4, 1, 4);
			printText( option1, 0, 1 );
	 		printText( option2, 0, 2 );
	 		printText( option3, 0, 3 );
			break;	
		case 5:
			sel=4;
			break;
		default:
			break;
	}
	if (key6 == kb_Enter)
	{
		switch(sel){
			case 1:
				mode = 1;
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				kill = true;
				break;
		}
	}	
    } while( kb_Data[kb_group_1] != kb_2nd && !kill && mode == 0);
    do {
	    if (!lol) 
	    {
		//the stuff here will only happen the first time this loop is run
		gfx_Begin(gfx_8bpp); //initialize advanced graphx
		gfx_SetDraw(gfx_buffer);
		gfx_FillScreen( gfx_white); //fill the screen with white
		lol = true; //make sure these aren't run again
	    }
	kb_Scan(); //scan the keyboard for inputs
	key7 = kb_Data[kb_group_7]; //load the group 7 registers
	gfx_SetColor( gfx_black );
	switch (key7) {
		//this is for controlling perspective, but the ez80 is a crappy 48mHz cpu and can't do this in real time
		//(well it can, but it has to redraw the function every freakin' time)
		case kb_Up:
		//	gfx_FillScreen( gfx_white); //every time this happens, we want to clear the screen before drawing stuff again
			gfx_SetColor( gfx_white );
			gfx_FillRectangle(320-64,0,64,64);
			b1+=.15;
			lol3 = false; //this is so that it redraws the function
			break;
		case kb_Down:
			gfx_SetColor( gfx_white );
			gfx_FillRectangle(320-64,0,64,64);
			b1-=.15;
			lol3 = false; //i honestly should name my variables more intuitively so you can actually understand what they do
			break;
		case kb_Left:
			gfx_SetColor( gfx_white );
			gfx_FillRectangle(320-64,0,64,64);
			a1+=.15;
			lol3 = false;
			break;
		case kb_Right:
			gfx_SetColor( gfx_white );
			gfx_FillRectangle(320-64,0,64,64);
			a1-=.15;
			lol3 = false;
			break;
		default:
			break;	
	}
	//The perspective box
	gfx_SetColor( gfx_black );
	gfx_VertLine_NoClip(320-64,0,64);
	gfx_HorizLine_NoClip(320-64,64,64);
	//x-axis
	gfx_Line_NoClip(288-(x_x(a1,b1,c1)*24),32+(x_y(a1,b1,c1)*24),(int)(288+(x_x(a1,b1,c1)*24)),(int)(32-(x_y(a1,b1,c1)*24)));
	gfx_PrintStringXY("x",(int)(288+(x_x(a1,b1,c1)*25)),(int)(32-(x_y(a1,b1,c1)*25)));
	//y-axis
	gfx_Line_NoClip(288-(y_x(a1,b1,c1)*24),32+(y_y(a1,b1,c1)*24),(int)(288+(y_x(a1,b1,c1)*24)),(int)(32-(y_y(a1,b1,c1)*24)));
	gfx_PrintStringXY("y",(int)(288+(y_x(a1,b1,c1)*25)),(int)(32-(y_y(a1,b1,c1)*25)));
	//z-axis
	gfx_Line_NoClip(288-(z_x(a1,b1,c1)*24),32+(z_y(a1,b1,c1)*24),(int)(288+(z_x(a1,b1,c1)*24)),(int)(32-(z_y(a1,b1,c1)*24)));
	gfx_PrintStringXY("z",(int)(288+(z_x(a1,b1,c1)*25)),(int)(32-(z_y(a1,b1,c1)*25)));
	if (!lol3){
		gfx_BlitRectangle(gfx_buffer,320-64,0,64,65);
		lol3 = true;
	}
	if (kb_Data[kb_group_6] == kb_Enter){
		a = a1;
		b = b1;
		c = c1;
		lol2 = false;
	}	

	



	if (!lol2){
		//basically, this is the for-loop that controls graphing
		//initial condition: t=0
		//go until t is greater than one
		//add dx to t each time
		gfx_FillScreen( gfx_white);
		gfx_SetColor( gfx_black ); //we want to draw in black
		//x-axis
		//apparently the _NoClip functions are faster than their clipped counterparts
		gfx_Line_NoClip(160-(x_x(a,b,c)*100),120+(x_y(a,b,c)*100),(int)(160+(x_x(a,b,c)*100)),(int)(120-(x_y(a,b,c)*100))); //we're casting doubles to ints here, but i think that is redundant since gfx_Line converts doubles automatically
		gfx_PrintStringXY("x",(int)(160+(x_x(a,b,c)*100)),(int)(120-(x_y(a,b,c)*100))); //axis labels
		//y-axis
		gfx_Line_NoClip(160-(y_x(a,b,c)*100),120+(y_y(a,b,c)*100),(int)(160+(y_x(a,b,c)*100)),(int)(120-(y_y(a,b,c)*100))); //btw (160,120) is the center of the screen, counting from the top left corner as (0,0)
		gfx_PrintStringXY("y",(int)(160+(y_x(a,b,c)*100)),(int)(120-(y_y(a,b,c)*100)));
		//z-axis
		gfx_Line_NoClip(160-(z_x(a,b,c)*100),120+(z_y(a,b,c)*100),(int)(160+(z_x(a,b,c)*100)),(int)(120-(z_y(a,b,c)*100))); //the resolution of the TI84+CE is 320*240	
		gfx_PrintStringXY("z",(int)(160+(z_x(a,b,c)*100)),(int)(120-(z_y(a,b,c)*100)));
		i=0;
		for(t = 0; t <= 1 && kb_Data[kb_group_1] != kb_2nd; t+=dx) 
		{
			kb_Scan(); //keep scanning for key presses
			gfx_SetColor(gfx_green); //we want to graph in green
			//equations for the grid/graph
			g = 2*s2*((floor(t*(n+1)) / n)-.5);
		//	_g = 2*s2*((floor((t+dx)*(n+1))/n)-.5); //unused for now
			h = 2*s2*(mod(t*(n+1),1) - .5);
		//	_h = 2*s2*(mod((t+dx)*(n+1),1)-.5); //unused for now
			xxNodes[i][0]=(uint8_t)(160+(s*((x_x(a,b,c)*g) + (y_x(a,b,c)*h) + (z_x(a,b,c)*function(g,h))))); //fill out the arrays of nodes
			xyNodes[i][0]=(uint8_t)(120-(s*((x_y(a,b,c)*g) + (y_y(a,b,c)*h) + (z_y(a,b,c)*function(g,h)))));
			yxNodes[i][0]=(uint8_t)(160+(s*((x_x(a,b,c)*h) + (y_x(a,b,c)*g) + (z_x(a,b,c)*function(h,g)))));
			yyNodes[i][0]=(uint8_t)(120-(s*((x_y(a,b,c)*h) + (y_y(a,b,c)*g) + (z_y(a,b,c)*function(h,g)))));

//			xxNodes[i][1]=(uint8_t)(160+(s*((x_x(a+.15,b,c)*g) + (y_x(a+.15,b,c)*h) + (z_x(a+.15,b,c)*function(g,h)))));
//			xyNodes[i][1]=(uint8_t)(120-(s*((x_y(a+.15,b,c)*g) + (y_y(a+.15,b,c)*h) + (z_y(a+.15,b,c)*function(g,h)))));
//			yxNodes[i][1]=(uint8_t)(160+(s*((x_x(a+.15,b,c)*h) + (y_x(a+.15,b,c)*g) + (z_x(a+.15,b,c)*function(h,g)))));
//			yyNodes[i][1]=(uint8_t)(120-(s*((x_y(a+.15,b,c)*h) + (y_y(a+.15,b,c)*g) + (z_y(a+.15,b,c)*function(h,g)))));

//			xxNodes[i][2]=(uint8_t)(160+(s*((x_x(a-.15,b,c)*g) + (y_x(a-.15,b,c)*h) + (z_x(a-.15,b,c)*function(g,h)))));
//			xyNodes[i][2]=(uint8_t)(120-(s*((x_y(a-.15,b,c)*g) + (y_y(a-.15,b,c)*h) + (z_y(a-.15,b,c)*function(g,h)))));
//			yxNodes[i][2]=(uint8_t)(160+(s*((x_x(a-.15,b,c)*h) + (y_x(a-.15,b,c)*g) + (z_x(a-.15,b,c)*function(h,g)))));
//			yyNodes[i][2]=(uint8_t)(120-(s*((x_y(a-.15,b,c)*h) + (y_y(a-.15,b,c)*g) + (z_y(a-.15,b,c)*function(h,g)))));
			if (i>=12) //for some reason the first 12 nodes don't connect to the rest, so we omit them
			{
				d1 = pow(pow(xxNodes[i][0]-xxNodes[i-1][0],2) + pow(xyNodes[i][0]-xyNodes[i-1][0],2),.5); //find distance between nodes
				d2 = pow(pow(yxNodes[i][0]-yxNodes[i-1][0],2) + pow(yyNodes[i][0]-yyNodes[i-1][0],2),.5); //this is used to determine whether or not to connect two nodes together
				if (d1 < 45) { //if the distance is less than 45 (or some arbitrary constant)
					gfx_Line(xxNodes[i-1][0],xyNodes[i-1][0], xxNodes[i][0], xyNodes[i][0]); //connect the nodes
				}
			gfx_SetColor(gfx_blue); //we want graph in blue
				if (d2 < 45) { //do the same thing
					gfx_Line(yxNodes[i-1][0],yyNodes[i-1][0], yxNodes[i][0], yyNodes[i][0]);
				}
			}
			i++; //increment i

			//really inefficient pixel plotter. slow af
			//use this at your own risk
		//	gfx_SetPixel( 160 + (s*((x_x(a,b,c)*g)+(y_x(a,b,c)*h)+(z_x(a,b,c)*function(g,h)))) ,   120-(s*((x_y(a,b,c)*g) + (y_y(a,b,c)*h) + (z_y(a,b,c)*function(g,h)))));
		//	gfx_SetColor(gfx_blue);
		//	gfx_SetPixel(160+(s*((x_x(a,b,c)*h)+(y_x(a,b,c)*g)+(z_x(a,b,c)*function(h,g)))),120-(s*((x_y(a,b,c)*h) + (y_y(a,b,c)*g) + (z_y(a,b,c)*function(h,g))))); 
			
		}
		gfx_SwapDraw();
		lol2 = true; // this is so that it only plots the function once
		lol3 = false;
    	}
	
    } while ( kb_Data[kb_group_1] != kb_2nd && mode == 1 && !kill);
    gfx_End(); //stop the gfx
    prgm_CleanUp(); //clear the screen
    //end of program
}
//function that prints simple text on screen
//similar to the Output() function of TI-BASIC
void printText(const char *text, uint8_t xpos, uint8_t ypos) {
    os_SetCursorPos(ypos, xpos); //set the place to start putting text
    os_PutStrFull(text); //output the string
}
/* Simple way to fill the screen with a given color */
void fillScreen(uint8_t color) {
    memset_fast(lcd_Ram, color, LCD_SIZE);
}
//used to calculate the position of points in the 3d world and translate them into 2d coordinates to draw onto the screen
//when using these, ALWAYS type (a,b,c) as the arguments of the function.
double x_x(double a, double b, double c){
    return (cos(c)*cos(a)) - (sin(c)*sin(a)*sin(b));
}
double x_y(double a, double b, double c) {
	return (cos(c)*sin(a)*sin(b)) + (sin(c)*cos(a));
}
double y_x(double a, double b, double c) {
	return (-1*cos(c)*sin(a)) - (sin(c)*cos(a)*sin(b));
}
double y_y(double a, double b, double c) {
	return (cos(c)*cos(a)*sin(b)) - (sin(c)*sin(a));
}
double z_x(double a, double b, double c) {
	return -1*sin(c)*cos(b);
}
double z_y(double a, double b, double c) {
	return cos(c)*cos(b);
}
//the 3d function you want to graph
//note that this is in the format f(x,y)=
//for example: for f(x,y)=cos(xy)-y^2, type in
//	return cos(x*y)-(pow(y,2));
//soon i should have user input
double function(double x, double y) {
	return x;
}
//proper modulus function 
//the % operator only works with integers
//and the fmod() function can't handle negative numbers properly
double mod(double a, double N)
{
    return a - N * floor (a / N);
}