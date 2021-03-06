/*    The (not) AMAZING 3D grapher for the TI84+CE
 *by gluu
 *currently in a very early alpha stage
 *DONE! Mathematical expression parser
 *DONE! better way of handling perspective
 *DONE(sort of)! fix bug regarding wrapping of screen when graphing
 *TODO: shunting yard algorithm
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

//c libraries
#include <keypadc.h>
#include <graphx.h>
#include <fileioc.h>

#define STEP .15

#define PERSPECTIVE_BOX_SCALE 25
#define PERSPECTIVE_BOX_WIDTH 64
#define PERSPECTIVE_BOX_HEIGHT 64
#define HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH 32

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define HALF_SCREEN_WIDTH 160
#define HALF_SCREEN_HEIGHT 120

#define TEXT_START_X 12
#define TEXT_START_Y 12
#define TEXT_HEIGHT 9
#define TEXT_SPACING 10

#define CURSOR_START_X 4

#define GRAPH_SCALE 100

#define MODE_INPUT = 1
#define MODE_GRAPH = 2

//initialize strings
const char *graph = "Graphing...";
const char *title = "R3 - 3D grapher for the TI84PCE";
const char *menuTitle = "Main Menu";
const char *functionInput = "f(x, y) = ";
const char *xAxisLabel = "x";
const char *yAxisLabel = "y";
const char *zAxisLabel = "z";

double x_x(double a, double b, double c);
double x_y(double a, double b, double c);
double y_x(double a, double b, double c);
double y_y(double a, double b, double c);
double z_x(double a, double b, double c);
double z_y(double a, double b, double c);

double mod(double a, double b);
char* backspace(char* str);
char* ftoa(float arg);

void pushRPN(double val);
double popRPN(void);
double parseRPN(char *s, double X, double Y);

void shunt(char *input);

static double yaw_orig; //dummy variables we'll use later
static double roll_orig;
static double pitch_orig;
static void drawPerspectiveBox(void)
{
    int xx_25 = x_x(yaw_orig, roll_orig, pitch_orig) * PERSPECTIVE_BOX_SCALE;
    int xy_25 = x_y(yaw_orig, roll_orig, pitch_orig) * PERSPECTIVE_BOX_SCALE;
    int yx_25 = y_x(yaw_orig, roll_orig, pitch_orig) * PERSPECTIVE_BOX_SCALE;
    int yy_25 = y_y(yaw_orig, roll_orig, pitch_orig) * PERSPECTIVE_BOX_SCALE;
    int zx_25 = z_x(yaw_orig, roll_orig, pitch_orig) * PERSPECTIVE_BOX_SCALE;
    int zy_25 = z_y(yaw_orig, roll_orig, pitch_orig) * PERSPECTIVE_BOX_SCALE;

    //The perspective box
    //This is my solution to controlling perspective, since we can't rotate in real time
    //rotate the preview axes to desired position and press enter, and the function will redraw itself

    gfx_SetColor(gfx_white);
    gfx_FillRectangle(SCREEN_WIDTH - PERSPECTIVE_BOX_WIDTH, 0, PERSPECTIVE_BOX_WIDTH, PERSPECTIVE_BOX_HEIGHT);

    gfx_SetColor(gfx_black);
    gfx_VertLine_NoClip(SCREEN_WIDTH - PERSPECTIVE_BOX_WIDTH, 0, PERSPECTIVE_BOX_HEIGHT); //borders for box
    gfx_HorizLine_NoClip(SCREEN_WIDTH - PERSPECTIVE_BOX_WIDTH, PERSPECTIVE_BOX_HEIGHT, PERSPECTIVE_BOX_WIDTH);

    //x-axis
    gfx_Line_NoClip(SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - xx_25,
            HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + xy_25, SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + xx_25,
            HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - xy_25);

    //x-axis label
    gfx_PrintStringXY(xAxisLabel, SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + xx_25,
            HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - xy_25);

    //y-axis
    gfx_Line_NoClip(SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - yx_25, HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + yy_25,
            SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + yx_25, HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - yy_25);

    //y-axis label
    gfx_PrintStringXY(yAxisLabel, SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + yx_25,
            HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - yy_25);

    //z-axis
    gfx_Line_NoClip(SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - zx_25, HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + zy_25,
            SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + zx_25, HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - zy_25);

    //z-axis label
    gfx_PrintStringXY(zAxisLabel, SCREEN_WIDTH - HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH + zx_25,
            HALF_PERSPECTIVE_BOX_HEIGHT_WIDTH - zy_25);

    gfx_BlitRectangle(gfx_buffer, SCREEN_WIDTH - PERSPECTIVE_BOX_WIDTH, 0, PERSPECTIVE_BOX_WIDTH, PERSPECTIVE_BOX_HEIGHT + 1);
}

char *infix = "5 + 3";
char equ[40];
char postfix[40];

static void addToEqu(const char *str)
{
    strcat(equ, str);
}

//main function
//this is what gets run when the program starts
void main(void)
{
    sk_key_t key;

    //declare variables and some constants
    //uint8_t is the best thing ever
    //it's basically an unsigned 8bit integer that takes up less space than an int
    //but functions the same as an int for all intents and purposes (except negatives)
    uint8_t sel; //used for menu selections
    bool graphingIsComplete = false; //used to plot points
    bool main = true;
    bool second = false;
    bool alpha = false;
    uint8_t mode = 0; //used for menu selections
    double yaw = -2.3; //yaw
    double roll = .6; //roll
    double pitch = 0; //pitch; this never changes since i don't want people to mess with the rotation of the screen
    const double s = 15; //scale of the graph
    const double s2 = 3.75; //how much of the graph gets shown
    const uint8_t n = 10; //resolution of the grid of the graph is (higher = more detail)

    uint8_t *xxNodes = malloc(128); //create the node arrays
    uint8_t *xyNodes = malloc(128);
    uint8_t *yxNodes = malloc(128);
    uint8_t *yyNodes = malloc(128);

    //dummy variables we'll use later
    yaw_orig = yaw;
    roll_orig = roll;
    pitch_orig = pitch;

    //------program actually starts here------\\

    prgm_CleanUp(); //clear the screen

    gfx_Begin(); //initialize advanced graphx
    gfx_SetDrawBuffer();

    gfx_SetTextScale(1,1);
    gfx_PrintStringXY(title, TEXT_START_X, TEXT_START_Y); //print title text
    gfx_SetTextScale(1,1);
    gfx_PrintStringXY(functionInput, TEXT_START_X, TEXT_START_Y + TEXT_HEIGHT);
//    shunt(infix);
    gfx_PrintStringXY(equ, gfx_GetStringWidth(functionInput), TEXT_START_Y + TEXT_HEIGHT);
    gfx_SwapDraw();

    while (!os_GetCSC()); //wait for input

    sel = 0;
    key = 1;

MAIN:

    do
    {
        if (key) {
            gfx_FillScreen(gfx_white);
            gfx_PrintStringXY(menuTitle, HALF_SCREEN_WIDTH - gfx_GetStringWidth(menuTitle), TEXT_START_Y);
            gfx_PrintStringXY("Enter equation ", TEXT_START_X, TEXT_START_Y + TEXT_HEIGHT);
            gfx_PrintStringXY("Graph", TEXT_START_X, TEXT_START_Y + TEXT_HEIGHT + TEXT_HEIGHT);
            gfx_PrintStringXY("*", CURSOR_START_X, TEXT_START_X + TEXT_HEIGHT + (sel * TEXT_HEIGHT));
            gfx_SwapDraw();
        }

        key = os_GetCSC();

        switch (key)
        {
            case sk_Up:
                if (sel > 0)
                {
                    --sel;
                }
                break;
            case sk_Down:
                if (sel < 1)
                {
                    ++sel;
                }
                break;
            case sk_Enter:
                if (main)
                {
                    main = false;
                    mode = sel + 1;
                }
                break;
            default:
                break;
        }
    } while(main && key != sk_2nd);
    
    if (mode == 1)
    {
        //quite possibly the most annoying thing to code
        do
        {
            key = os_GetCSC();
            
            gfx_FillScreen(gfx_white);
            gfx_PrintStringXY(functionInput, TEXT_START_X, TEXT_START_Y + TEXT_HEIGHT);
            gfx_PrintStringXY(equ, gfx_GetStringWidth(functionInput), TEXT_START_Y + TEXT_HEIGHT);
            gfx_PrintStringXY("|", gfx_GetStringWidth(functionInput) + gfx_GetStringWidth(equ),
                    TEXT_START_Y + TEXT_HEIGHT);
            gfx_SwapDraw();
            
            //Welcome to Switch-Statement City! Population: 0, cuz no one wants to live in this atrocious town
            //Hope you enjoy your stay!
            switch(key)
            {
            	case sk_2nd:
            		second = !second;
            		alpha = false;
            		break;
            	case sk_Alpha:
            		alpha = !alpha;
            		second = false;
            		break;
                case sk_Del:
                    backspace(equ);
                    break;
                case sk_Ln:
                    addToEqu(" ln ");
                    break;
                case sk_Log:
                    addToEqu(" log ");
                    break;
                case sk_0:
                    addToEqu("0");
                    break;
                case sk_1:
                    addToEqu(alpha ? "1" : " y ");
                    break;
                case sk_4:
                    addToEqu("4");
                    break;
                case sk_7:
                    addToEqu("7");
                    break;
                case sk_Comma:
                    addToEqu(" ");
                    break;
                case sk_Sin:
                    addToEqu(" sin ");
                    break;
                case sk_Apps:
                    break;
                case sk_GraphVar:
                    addToEqu(" x ");
                    break;
                case sk_DecPnt:
                    addToEqu(".");
                    break;
                case sk_2:
                    addToEqu("2");
                    break;
                case sk_5:
                    addToEqu("5");
                    break;
                case sk_8:
                    addToEqu("8");
                    break;
                case sk_Cos:
                    addToEqu(" cos ");
                    break;
                case sk_Stat:
                    addToEqu(" y ");
                    break;
                case sk_Chs:
                    addToEqu("`");
                    break;
                case sk_3:
                    addToEqu("3");
                    break;
                case sk_6:
                    addToEqu("6");
                    break;
                case sk_9:
                    addToEqu("9");
                    break;
                case sk_Tan:
                    addToEqu(" tan ");
                    break;
                case sk_Enter:
                    main = true;
                    mode = 0;
                    goto MAIN;
                case sk_Sub:
                    addToEqu(" - ");
                    break;
                case sk_Add:
                    addToEqu(" + ");
                    break;
                case sk_Mul:
                    addToEqu(" * ");
                    break;
                case sk_Div:
                    addToEqu(" / ");
                    break;
                case sk_Power:
                    addToEqu(" ^ ");
                    break;
                case sk_Clear:
                    addToEqu("");
                    break;
                default:
                    break;
            }
        } while(mode == 1);
    }
    
    if (mode == 2)
    {
        //the stuff here will only happen the first time this loop is run
        gfx_FillScreen(gfx_white); //fill the screen with white
        gfx_PrintStringXY(graph, SCREEN_WIDTH - gfx_GetStringWidth(graph), SCREEN_HEIGHT - TEXT_SPACING);
        gfx_BlitRectangle(gfx_buffer, SCREEN_WIDTH - gfx_GetStringWidth(graph), SCREEN_HEIGHT - TEXT_SPACING,
                gfx_GetStringWidth(graph), TEXT_SPACING);

        do
        {
            key = os_GetCSC();

            switch (key)
            {
                //this is for controlling perspective, but the ez80 is a crappy 48mHz cpu and can't do this in real time
                //(well it can, but it has to redraw the function every freakin' time)
                case sk_Up:
                    roll_orig += STEP;
                    drawPerspectiveBox();
                    break;
                case sk_Down:
                    roll_orig -= STEP;
                    drawPerspectiveBox();
                    break;
                case sk_Left:
                    yaw_orig += STEP;
                    drawPerspectiveBox();
                    break;
                case sk_Right:
                    yaw_orig -= STEP;
                    drawPerspectiveBox();
                    break;
                default:
                    break;
            }

            if (key == sk_Enter)
            {
                uint8_t strWidth = gfx_GetStringWidth(graph);
                gfx_PrintStringXY(graph, SCREEN_WIDTH - strWidth, SCREEN_HEIGHT - );
                gfx_BlitRectangle(gfx_buffer, SCREEN_WIDTH - strWidth, SCREEN_HEIGHT - TEXT_SPACING, strWidth, TEXT_SPACING);
                yaw = yaw_orig;
                roll = roll_orig;
                pitch = pitch_orig;
                graphingIsComplete = false;
            }

            if (!graphingIsComplete)
            {
                uint8_t i;
                float t;

                float xx_a_b_c = x_x(yaw, roll, pitch);
                float xy_a_b_c = x_y(yaw, roll, pitch);
                float yx_a_b_c = y_x(yaw, roll, pitch);
                float yy_a_b_c = y_y(yaw, roll, pitch);
                float zx_a_b_c = z_x(yaw, roll, pitch);
                float zy_a_b_c = z_y(yaw, roll, pitch);

                int xx_a_b_c_100 = xx_a_b_c * GRAPH_SCALE;
                int xy_a_b_c_100 = xy_a_b_c * GRAPH_SCALE;
                int yx_a_b_c_100 = yx_a_b_c * GRAPH_SCALE;
                int yy_a_b_c_100 = yy_a_b_c * GRAPH_SCALE;
                int zx_a_b_c_100 = zx_a_b_c * GRAPH_SCALE;
                int zy_a_b_c_100 = zy_a_b_c * GRAPH_SCALE;
                
                //basically, this is the for-loop that controls graphing
                //initial condition: t=0
                //go until t is greater than one
                //add dx to t each time
                gfx_FillScreen(gfx_white);
                gfx_SetColor(gfx_black);
                
                //x-axis
                gfx_Line_NoClip(HALF_SCREEN_WIDTH - xx_a_b_c_100, HALF_SCREEN_HEIGHT + xy_a_b_c_100,
                        HALF_SCREEN_WIDTH + xx_a_b_c_100, HALF_SCREEN_HEIGHT - xy_a_b_c_100);
                gfx_PrintStringXY("x", HALF_SCREEN_WIDTH + xx_a_b_c_100, HALF_SCREEN_HEIGHT - xy_a_b_c_100);
                
                //y-axis
                gfx_Line_NoClip(HALF_SCREEN_WIDTH - yx_a_b_c_100, HALF_SCREEN_HEIGHT + yy_a_b_c_100,
                        HALF_SCREEN_WIDTH + yx_a_b_c_100, HALF_SCREEN_HEIGHT - yy_a_b_c_100);
                gfx_PrintStringXY("y", HALF_SCREEN_WIDTH + yx_a_b_c_100, HALF_SCREEN_HEIGHT - yy_a_b_c_100);
                
                //z-axis
                gfx_Line_NoClip(HALF_SCREEN_WIDTH - zx_a_b_c_100, HALF_SCREEN_HEIGHT + zy_a_b_c_100,
                        HALF_SCREEN_WIDTH + zx_a_b_c_100, HALF_SCREEN_HEIGHT - zy_a_b_c_100);
                gfx_PrintStringXY("z", HALF_SCREEN_WIDTH + zx_a_b_c_100, HALF_SCREEN_HEIGHT - zy_a_b_c_100);
                
                for (i = 0, t = 0; t < 1 && os_GetCSC() != sk_2nd; t += .00909, i++)
                {
                    uint8_t xxNode, xyNode, yxNode, yyNode;
                    uint8_t xxNode_p, xyNode_p, yxNode_p, yyNode_p;

                    //equations for the grid/graph
                    double g = 2 * s2 * ((floor(t * (n + 1)) / n) - .5);
                    double h = 2 * s2 * (mod(t * (n + 1), 1) - .5);
                    double pRPN_h_g = parseRPN(equ, h, g);
                    double pRPN_g_h = parseRPN(equ, g, h);

                    //fill out the arrays of nodes
                    xxNode = (HALF_SCREEN_WIDTH + (s * ((xx_a_b_c * g) + (yx_a_b_c * h) + (zx_a_b_c * pRPN_g_h))));
                    xyNode = (HALF_SCREEN_HEIGHT - (s * ((xy_a_b_c * g) + (yy_a_b_c * h) + (zy_a_b_c * pRPN_g_h))));
                    yxNode = (HALF_SCREEN_WIDTH + (s * ((xx_a_b_c * h) + (yx_a_b_c * g) + (zx_a_b_c * pRPN_h_g))));
                    yyNode = (HALF_SCREEN_HEIGHT - (s * ((xy_a_b_c * h) + (yy_a_b_c * g) + (zy_a_b_c * pRPN_h_g))));

                    if (xxNode > SCREEN_WIDTH)
                    {
                        xxNode = SCREEN_WIDTH;
                    }
                    else if (xxNode < 0)
                    {
                        xxNode = 0;
                    }
                    
                    if (xyNode > SCREEN_HEIGHT)
                    {
                        xyNode = SCREEN_HEIGHT - 1;
                    }
                    else if (xyNode < 0)
                    {
                        xyNode = 0;
                    }
                    
                    if (yxNode > SCREEN_WIDTH)
                    {
                        yxNode = SCREEN_WIDTH;
                    }
                    else if (yxNode < 0)
                    {
                        yxNode = 0;
                    }
                    
                    if (yyNode > SCREEN_HEIGHT)
                    {
                        yyNode = SCREEN_HEIGHT - 1;
                    }
                    else if (yyNode < 0)
                    {
                        yyNode = 0;
                    }

                    xxNodes[i] = xxNode;
                    xyNodes[i] = xyNode;
                    yxNodes[i] = yxNode;
                    yyNodes[i] = yyNode;
                    
                    if (i >= 12) //for some reason the first 12 nodes don't connect to the rest, so we omit them
                    {
                        if ((i - 1) % (int)n) // we don't want to connect the nodes that are on opposite sides
                        {
                            gfx_SetColor(gfx_green);
                            gfx_Line_NoClip(xxNodes[i - 1], xyNodes[i - 1], xxNode, xyNode);
                            
                            gfx_SetColor(gfx_blue);
                            gfx_Line_NoClip(yxNodes[i - 1], yyNodes[i - 1], yxNode, yyNode);
                        }
                    }
                }
                drawPerspectiveBox();
                gfx_SwapDraw();
                graphingIsComplete = true; // this is so that it only plots the function once
            }

        } while (key != sk_2nd);
    }

    gfx_End(); //stop the gfx
    prgm_CleanUp(); //clear the screen
    //end of program
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

//proper modulus function
//the % operator only works with integers
//and the fmod() function can't handle negative numbers properly
double mod(double a, double N)
{
    return a - N * floor(a / N);
}

//global vars
char text[15];
float r;
float aa;
float bb;
char c[40];
char *d;
char *e, *w = " ";
char *tokens;
char *equCursor;
char *equPointer;

char* result;
char* ftoa(float arg)
{
    const real_t arg2 = os_FloatToReal(arg);
    os_RealToStr(result, &arg2, 0, 0, 0);
    return result;
}

double val[24];
int size;

typedef struct opStack
{
    char op[96];
    int size;
} opStack;
opStack OP;

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

double parseRPN(char *s, double X, double Y) //THE REVERSE POLISH NOTATION PARSER IS HERE
{
    char tok;
    double a, b; //declare my vars
    size = 0; //set stack size to 0;
    strcpy(c, s); //copy the string s to c, so we can mess with c all we want without damaging s
    for(tokens = strtok(c, w); tokens; tokens = strtok(NULL, w)) //chop c into tokens, separated by spaces as delims
    {
        //for each token we will:
        a = strtod(tokens,&e); //convert it to a number and store it in a
        tok = *tokens;
        
        if (e > tokens && tok != '+' && tok != '-' && tok != '/' && tok != '*' && tok != '^' && tok != 'x' && tok != 'y' ) //stupid
        {
            pushRPN(a); //push a to the stack if the token was an actual number
        }
        else if (tok == 'x')
        {
            pushRPN(X); //push the value of argument X to stack
        }
        else if (tok == 'y')
        {
            pushRPN(Y); //push argument Y to stack
        }
#define binop(x)  b = popRPN(), a = popRPN(), pushRPN(x) //the binary operators: takes top 2 numbers on stack, evaluates them, then pushes the result back to stack
        else if (tok == '+')
        {
            binop(a + b);
        }
        else if (tok == '-')
        {
            binop(a - b);
        }
        else if (tok == '*')
        {
            binop(a * b);
        }
        else if (tok == '/')
        {
            binop(a / b);
        }
        else if (tok == '^')
        {
            binop(pow(a, b));
        }
#undef binop
#define monop(x) b = popRPN(), pushRPN(x); //mono operators: takes top number on stack, evaluates it, then pushes the result back to the stack
        else if (strcmp(tokens,"sin") == 0)
        {
            monop(sin(b));
        }
        else if (strcmp(tokens,"cos") == 0)
        {
            monop(cos(b));
        }
        else if (strcmp(tokens,"tan") == 0)
        {
            monop(tan(b));
        }
        else if (strcmp(tokens,"ln") == 0)
        {
            monop(tan(b));
        }
        else if (strcmp(tokens,"log") == 0)
        {
            monop(tan(b));
        }
#undef monop
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
void shunt(char *input)
{
	char tok;
	double a;
	int i = 0;
    strcpy(c,input);


//	for (tokens = strtok(c, w); tokens; tokens = strtok(NULL, w))
//	{
//		a = strtod(tokens,&e); //convert token to double
//		tok = *tokens;
//		if (e > tokens && tok != '+' && tok != '-' && tok != '/' && tok != '*' && tok != '^' && tok != 'x' && tok != 'y' ) //stupid
//		{
//            *postfix = &tokens;
//		}
//		else if (tok == 'x' )
//		{
//
//		}
//        else if (tok == 'y')
//        {
//
//        }
//        else
//        {
//
//        }
//
//	}
}
char* backspace(char* str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        i++;

    }
    str[i-1] = '\0';
    return str;
}
void removeSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}
