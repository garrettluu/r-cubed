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
 *Please bear with parts of code that are redundant or inefficient.
	I don't care enough to fix them for now.
 *
 * I think I fixed all indentation and formatting
 *	
 */


//headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

//std headers
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* CE Keypad C Library */
#include <keypadc.h>
#include <graphx.h>
#include <fileioc.h>


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

double mod(double a, double b);

char *tokenize(char *input);

//global vars
char text[15];
float r;
float aa;
float bb;
char c[40];
char *d;
char *e, *w = " ";
char *tokens;
char *floatToStr(float arg);
typedef struct node
{
	double val;
	struct Node* next;
} node;

double val[24];
int size;

typedef struct opStack
{
    char op[96];
    int size;
} opStack;
opStack OP;

void initRPN(void);

void pushRPN(double val);

double popRPN(void);

double parseRPN(char *s, double X, double Y);

void shunt(void);

//main function
//this is what gets run when the program starts
void main(void)
{
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
    const char *graph = "Graphing...";
    const char *menuTitle = "Main Menu";

    //declare variables and some constants
    //uint8_t is the best thing ever
    //it's basically an unsigned 8bit integer that takes up less space than an int
    //but functions the same as an int for all intents and purposes (except negatives)
    int8_t sel = 0; //used for menu selections
    bool kill = false; //used to terminate program
    bool firstLoopIsComplete = false; //used to initialize graphx
    bool graphingIsComplete = false; //used to plot points
    bool lol3 = false; //used to draw the perspective changer
    bool main = true;
    bool keyPressed = false;
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
    double _g; //_g and _h are unused for now, keeping it as legacy in case i might need them someday
    double h;
    double _h;
    const double s = 15; //scale of the graph
    const double s2 = 3.75; //how much of the graph gets shown
    const double n = 10; //resolution of the grid of the graph is (higher = more detail)
    double dx = 1 / (n * (1 +
                          n)); //degree of precision when plotting; higher values mean less precise plotting. This value should be called dt to be mathematically correct, but I don't feel like changing it now
    double d1;
    double d2;

    double aaa;
    char equtest[40] = " x x * y y * - 4 /";
    char equ[40] = "x 2 ^ y 2 ^ - .75 *";
    char *test = "shrek is love";
    uint8_t xxNodes[256][3]; //create the node arrays
    uint8_t xyNodes[256][3]; //why are they multidimensional? this is like 2am code so who knows
    uint8_t yxNodes[256][3];
    uint8_t yyNodes[256][3];


    //------program actually starts here------\\

    prgm_CleanUp(); //clear the screen

    gfx_Begin(gfx_8bpp); //initialize advanced graphx
    gfx_SetDraw(gfx_buffer);

    gfx_SetTextFGColor(gfx_black);

    floatToStr(5);

    //testing purposes
//    for (t = 0; t < 1 && kb_Data[kb_group_1] != kb_2nd; t += .1)
//    for(tokens = strtok(equtest, " "); tokens != NULL; tokens = strtok(NULL," "))
//    {
//    	g = 2*strtod(tokens,&e);
//    	h = 8;
//    	floatToStr(g);
//    	if (e > tokens && *tokens != '+') gfx_PrintStringXY(text, 12, 21 + (9 * i));
////    	floatToStr(parseRPN(equ,g,h));
//    	gfx_PrintStringXY(tokens, 150, 21 + (9 * i));
//    	i++;
//    }



    gfx_PrintStringXY("R3 - 3D grapher for the TI84PCE ", 12, 12); //print title text
//    gfx_PrintStringXY(text, 12, 21);

//    gfx_PrintStringXY(text, 12, 30);

    gfx_SwapDraw(); //update the screen


    while (!os_GetCSC()); //wait for input
    gfx_FillScreen(gfx_white);
    gfx_SwapDraw();

    prgm_CleanUp(); //clear the screen
//
//    do
//    {
//    	boot_WaitShort();
//    	kb_Scan();
//    	if (sel > 1) sel = 1;
//    	else if (sel < 0) sel = 0;
//    	if (!firstLoopIsComplete)
//    	{
//    		gfx_SetDraw(gfx_buffer);
//    		gfx_FillScreen(gfx_white);
//    		gfx_PrintStringXY(menuTitle, 160-gfx_GetStringWidth(menuTitle),12);
//    		gfx_PrintStringXY("1. Enter equation ", 12, 21);
//    		gfx_PrintStringXY("2. Graph", 12, 30);
//    		gfx_PrintStringXY("*",4,21+(sel*9));
//    		gfx_SwapDraw();
//    		firstLoopIsComplete = true;
//    	}
//
//    	key7 = kb_Data[kb_group_7]; //load the group 7 registers
//    	switch(key7)
//    	{
//    		case kb_Up:
//    			--sel;
//    			if (sel > 1) sel = 1;
//    			    	else if (sel < 0) sel = 0;
//    			firstLoopIsComplete = false;
//    			keyPressed = true;
//    			key7 = kb_Left;
//    			break;
//    		case kb_Down:
//    			++sel;
//    			if (sel > 1) sel = 1;
//    			    	else if (sel < 0) sel = 0;
//    			gfx_SwapDraw();
//    			firstLoopIsComplete = false;
//    			key7 = kb_Left;
//				break;
//    		default:
//    			break;
//    	}
//    	if (key6 == kb_Enter && main)
//    	{
//    		main = false;
//    		if (sel == 0)
//    		{
//    			kill = true;
//    		}
//    		else if (sel == 1)
//    		{
//
//    		}
//    	}
//    	if (sel > 1) sel = 1;
//    	else if (sel < 0) sel = 0;
//
//    }
//    while(main && kb_Data[kb_group_1] != kb_2nd);
    do
    {
        //THIS IS WHERE THE FUN BEGINS
        if (!firstLoopIsComplete)
        {
            i = 0;
            //the stuff here will only happen the first time this loop is run
            gfx_FillScreen(gfx_white); //fill the screen with white
            gfx_PrintStringXY(graph, 320 - gfx_GetStringWidth(graph), 230);
            gfx_BlitRectangle(gfx_buffer, 320 - gfx_GetStringWidth(graph), 230, gfx_GetStringWidth(graph), 10);
            firstLoopIsComplete = true; //make sure these aren't run again
        }
        kb_Scan(); //scan the keyboard for inputs
        key7 = kb_Data[kb_group_7]; //load the group 7 registers
        gfx_SetColor(gfx_black);
        switch (key7)
        {
            //this is for controlling perspective, but the ez80 is a crappy 48mHz cpu and can't do this in real time
            //(well it can, but it has to redraw the function every freakin' time)
            case kb_Up:
                //	gfx_FillScreen( gfx_white); //every time this happens, we want to clear the screen before drawing stuff again
                gfx_SetColor(gfx_white);
                gfx_FillRectangle(320 - 64, 0, 64, 64);
                b1 += .15;
                lol3 = false; //this is so that it redraws the function
                break;
            case kb_Down:
                gfx_SetColor(gfx_white);
                gfx_FillRectangle(320 - 64, 0, 64, 64);
                b1 -= .15;
                lol3 = false; //i honestly should name my variables more intuitively so you can actually understand what they do
                break;
            case kb_Left:
                gfx_SetColor(gfx_white);
                gfx_FillRectangle(320 - 64, 0, 64, 64);
                a1 += .15;
                lol3 = false;
                break;
            case kb_Right:
                gfx_SetColor(gfx_white);
                gfx_FillRectangle(320 - 64, 0, 64, 64);
                a1 -= .15;
                lol3 = false;
                break;
            default:
                break;
        }
        //The perspective box
        //This is my solution to controlling perspective, since we can't rotate in real time
        //rotate the preview axes to desired position and press enter, and the function will redraw itself
        gfx_SetColor(gfx_black);
        gfx_VertLine_NoClip(320 - 64, 0, 64); //borders for box
        gfx_HorizLine_NoClip(320 - 64, 64, 64);
        //x-axis
        gfx_Line_NoClip(288 - (x_x(a1, b1, c1) * 24), 32 + (x_y(a1, b1, c1) * 24), (int) (288 + (x_x(a1, b1, c1) * 24)),
                        (int) (32 - (x_y(a1, b1, c1) * 24)));
        gfx_PrintStringXY("x", (int) (288 + (x_x(a1, b1, c1) * 25)), (int) (32 - (x_y(a1, b1, c1) * 25))); //axis labels
        //y-axis
        gfx_Line_NoClip(288 - (y_x(a1, b1, c1) * 24), 32 + (y_y(a1, b1, c1) * 24), (int) (288 + (y_x(a1, b1, c1) * 24)),
                        (int) (32 - (y_y(a1, b1, c1) * 24)));
        gfx_PrintStringXY("y", (int) (288 + (y_x(a1, b1, c1) * 25)), (int) (32 - (y_y(a1, b1, c1) * 25)));
        //z-axis
        gfx_Line_NoClip(288 - (z_x(a1, b1, c1) * 24), 32 + (z_y(a1, b1, c1) * 24), (int) (288 + (z_x(a1, b1, c1) * 24)),
                        (int) (32 - (z_y(a1, b1, c1) * 24)));
        gfx_PrintStringXY("z", (int) (288 + (z_x(a1, b1, c1) * 25)), (int) (32 - (z_y(a1, b1, c1) * 25)));
        if (!lol3)
        {
            gfx_BlitRectangle(gfx_buffer, 320 - 64, 0, 64,
                              65); //blit the corner; take from the buffer and draw it on screen
            lol3 = true; //we don't want this to keep running
        }
        if (kb_Data[kb_group_6] == kb_Enter)
        {
            gfx_PrintStringXY(graph, 320 - gfx_GetStringWidth(graph), 230);
            gfx_BlitRectangle(gfx_buffer, 320 - gfx_GetStringWidth(graph), 230, gfx_GetStringWidth(graph), 10);
            a = a1;
            b = b1;
            c = c1;
            graphingIsComplete = false;
        }


        if (!graphingIsComplete)
        {
            //basically, this is the for-loop that controls graphing
            //initial condition: t=0
            //go until t is greater than one
            //add dx to t each time
            gfx_FillScreen(gfx_white);
            gfx_SetColor(gfx_black); //we want to draw in black
            //x-axis
            //apparently the _NoClip functions are faster than their clipped counterparts
            gfx_Line_NoClip(160 - (x_x(a, b, c) * 100), 120 + (x_y(a, b, c) * 100), (int) (160 + (x_x(a, b, c) * 100)),
                            (int) (120 - (x_y(a, b, c) *
                                          100))); //we're casting doubles to ints here, but i think that is redundant since gfx_Line converts doubles automatically
            gfx_PrintStringXY("x", (int) (160 + (x_x(a, b, c) * 100)),
                              (int) (120 - (x_y(a, b, c) * 100))); //axis labels, according to the right-hand rule
            //y-axis
            gfx_Line_NoClip(160 - (y_x(a, b, c) * 100), 120 + (y_y(a, b, c) * 100), (int) (160 + (y_x(a, b, c) * 100)),
                            (int) (120 - (y_y(a, b, c) *
                                          100))); //btw (160,120) is the center of the screen, counting from the top left corner as (0,0)
            gfx_PrintStringXY("y", (int) (160 + (y_x(a, b, c) * 100)), (int) (120 - (y_y(a, b, c) * 100)));
            //z-axis
            gfx_Line_NoClip(160 - (z_x(a, b, c) * 100), 120 + (z_y(a, b, c) * 100), (int) (160 + (z_x(a, b, c) * 100)),
                            (int) (120 - (z_y(a, b, c) * 100))); //the resolution of the TI84+CE is 320*240
            gfx_PrintStringXY("z", (int) (160 + (z_x(a, b, c) * 100)), (int) (120 - (z_y(a, b, c) * 100)));
            i = 0;
            for (t = 0; t < 1 && kb_Data[kb_group_1] != kb_2nd; t += .00909)
            {
                kb_Scan(); //keep scanning for key presses
                gfx_SetColor(gfx_green); //we want to graph in green
                //equations for the grid/graph
                g = 2 * s2 * ((floor(t * (n + 1)) / n) - .5);
                h = 2 * s2 * (mod(t * (n + 1), 1) - .5);
                //	_g = 2*s2*((floor((t+dx)*(n+1))/n)-.5); //unused for now
                //	_h = 2*s2*(mod((t+dx)*(n+1),1)-.5); //unused for now



                xxNodes[i][0] = (uint8_t)(160 + (s * ((x_x(a, b, c) * g) + (y_x(a, b, c) * h) +
                                                      (z_x(a, b, c) * parseRPN(equ,g, h) * .25)))); //fill out the arrays of nodes
                xyNodes[i][0] = (uint8_t)(
                        120 - (s * ((x_y(a, b, c) * g) + (y_y(a, b, c) * h) + (z_y(a, b, c) * parseRPN(equ,g, h) * .25))));
                yxNodes[i][0] = (uint8_t)(
                        160 + (s * ((x_x(a, b, c) * h) + (y_x(a, b, c) * g) + (z_x(a, b, c) * parseRPN(equ,h, g) * .25))));
                yyNodes[i][0] = (uint8_t)(
                        120 - (s * ((x_y(a, b, c) * h) + (y_y(a, b, c) * g) + (z_y(a, b, c) * parseRPN(equ,h, g) * .25))));



                //testing purposes
//                xxNodes[i][0] = (uint8_t)(160 + (s * ((x_x(a, b, c) * g) + (y_x(a, b, c) * h) +
//                                                      (z_x(a, b, c) * function(g, h))))); //fill out the arrays of nodes
//                xyNodes[i][0] = (uint8_t)(
//                        120 - (s * ((x_y(a, b, c) * g) + (y_y(a, b, c) * h) + (z_y(a, b, c) * function( g, h)))));
//                yxNodes[i][0] = (uint8_t)(
//                        160 + (s * ((x_x(a, b, c) * h) + (y_x(a, b, c) * g) + (z_x(a, b, c) *function( h, g)))));
//                yyNodes[i][0] = (uint8_t)(
//                        120 - (s * ((x_y(a, b, c) * h) + (y_y(a, b, c) * g) + (z_y(a, b, c) * function( h, g)))));


                if (i >= 12) //for some reason the first 12 nodes don't connect to the rest, so we omit them
                {
                    if ((i - 1) % (int) (n) != 0) // we don't want to connect the nodes that are on opposite sides
                    {
                        gfx_Line(xxNodes[i - 1][0], xyNodes[i - 1][0], xxNodes[i][0],
                                 xyNodes[i][0]); //connect the nodes
                    }
                    gfx_SetColor(gfx_blue); //we want graph in blue
                    if ((i - 1) % (int) (n) != 0)
                    { //do the same thing
                        gfx_Line(yxNodes[i - 1][0], yyNodes[i - 1][0], yxNodes[i][0], yyNodes[i][0]);
                    }
                }
                i++; //increment i
                g = 2 * s2 * ((floor(t * (n + 1)) / n) - .5);
                h = 2 * s2 * (mod(t * (n + 1), 1) - .5);
                //really inefficient pixel plotter. slow af
                //use this at your own risk
                //	gfx_SetPixel( 160 + (s*((x_x(a,b,c)*g)+(y_x(a,b,c)*h)+(z_x(a,b,c)*function(g,h)))) ,   120-(s*((x_y(a,b,c)*g) + (y_y(a,b,c)*h) + (z_y(a,b,c)*function(g,h)))));
                //	gfx_SetColor(gfx_blue);
                //	gfx_SetPixel(160+(s*((x_x(a,b,c)*h)+(y_x(a,b,c)*g)+(z_x(a,b,c)*function(h,g)))),120-(s*((x_y(a,b,c)*h) + (y_y(a,b,c)*g) + (z_y(a,b,c)*function(h,g)))));

            }
            gfx_SwapDraw();
            graphingIsComplete = true; // this is so that it only plots the function once
            lol3 = false;
        }

    } while (!kill && kb_Data[kb_group_1] != kb_2nd);
    gfx_End(); //stop the gfx
    prgm_CleanUp(); //clear the screen
    //end of program
}

//function that prints simple text on screen
//similar to the Output() function of TI-BASIC
void printText(const char *text, uint8_t xpos, uint8_t ypos)
{
    os_SetCursorPos(ypos, xpos); //set the place to start putting text
    os_PutStrFull(text); //output the string
}

/* Simple way to fill the screen with a given color */
void fillScreen(uint8_t color)
{
    memset_fast(lcd_Ram, color, LCD_SIZE);
}

//used to calculate the position of points in the 3d world and translate them into 2d coordinates to draw onto the screen
//when using these, ALWAYS type (a,b,c) as the arguments of the function.
double x_x(double a, double b, double c)
{
    return (cos(c) * cos(a)) - (sin(c) * sin(a) * sin(b));
}

double x_y(double a, double b, double c)
{
    return (cos(c) * sin(a) * sin(b)) + (sin(c) * cos(a));
}

double y_x(double a, double b, double c)
{
    return (-1 * cos(c) * sin(a)) - (sin(c) * cos(a) * sin(b));
}

double y_y(double a, double b, double c)
{
    return (cos(c) * cos(a) * sin(b)) - (sin(c) * sin(a));
}

double z_x(double a, double b, double c)
{
    return -1 * sin(c) * cos(b);
}

double z_y(double a, double b, double c)
{
    return cos(c) * cos(b);
}

//the 3d function you want to graph
//note that this is in the format f(x,y)=
//for example: for f(x,y)=cos(xy)-y^2, type in
//	return cos(x*y)-(pow(y,2));
//soon i should have user input
double function(double x, double y)
{
    return x-y;
}

//proper modulus function
//the % operator only works with integers
//and the fmod() function can't handle negative numbers properly
double mod(double a, double N)
{
    return a - N * floor(a / N);
}

char *tokenize(char *input)
{
    int i = 0;
    char *output;
    output = strtok(input, ",");
    return output;
}

char *floatToStr(float arg)
{
    const real_t arg2 = os_FloatToReal(arg);
    os_RealToStr(text, &arg2, -1, 1, -1);
    return text;
}
float strToFloat(char *arg, char *end)
{
	const real_t arg2 = os_StrToReal(arg,&end);
	return os_RealToFloat(&arg2);
}
//sets the size of the stack to 0
void initRPN()
{
    size = 0;
}
//pushes the argument to the stack
void pushRPN(double v)
{
    val[size++] = v;
}
//returns the value of the element on top of the stack and removes it
double popRPN(void)
{
    return val[--size];
}
//THIS FINALLY WORKS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
double parseRPN(char *s, double X, double Y) //THE REVERSE POLISH NOTATION PARSER IS HERE
{
    double a, b; //declare my vars
    initRPN(); //set stack size to 0;
    strcpy(c,s); //copy the string s to c, so we can mess with c all we want without damaging s
    for(tokens = strtok(c,w); tokens != NULL; tokens = strtok(NULL,w)) //chop c into tokens, separated by spaces as delims
    {
    	//for each token we will:
    	a = strtod(tokens,&e); //convert it to a number and store it in a
        if (e > tokens && *tokens != '+' && *tokens != '-' && *tokens != '/' && *tokens != '*' && *tokens != '^' && *tokens != 'x' && *tokens != 'y' ) //stupid
        {
        	pushRPN(a); //push a to the stack if the token was an actual number
        }
        else if (*tokens == 'x')
        {
        	pushRPN(X); //push the value of argument X to stack
        }
        else if (*tokens == 'y')
        {
        	pushRPN(Y); //push argument Y to stack
        }
	#define binop(x)  b = popRPN(), a = popRPN(), pushRPN(x) //pop, pop, push
        else if (*tokens == '+')
        {
        	binop(a + b);
        }
        else if (*tokens == '-')
        {
        	binop(a - b);
        }
        else if (*tokens == '*')
        {
        	binop(a * b);
        }
        else if (*tokens == '/')
        {
        	binop(a / b);
        }
        else if (*tokens == '^')
        {
        	binop(pow(a, b));
        }
#undef binop

    }
    return popRPN();
}

void pushOP(char op)
{
    OP.op[OP.size++] = op;
}
double popOP(void)
{
    return OP.op[--OP.size];
}
void shunt(void)
{

}
