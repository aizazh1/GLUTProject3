/*********
   CTIS164 - Template Source Program
----------
STUDENT : Aizaz Hassan
SECTION : 3
HOMEWORK: 3
----------
PROBLEMS:
----------
ADDITIONAL FEATURES: Added a menu screen, a difficulty screen and a game end screen. 
					 2 different difficulties available for the game.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  20 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

//states of game
#define LOAD 0 
#define RUN 1
#define END 2
#define EASY 3
#define HARD 4
#define DIFFICULTY 5

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight; // current Window width and height

int randNum1, randNum2, //numbers in equation 
	ans, //answer of equation
	opt1, opt2, opt3, posOfAns; //options
int r1 = 1, r2 = 1, r3 = 1, r4 = 1, g1 = 1, g2 = 1, g3 = 1, g4 = 1, b2 = 0, b3 = 0, b1 = 0, b4 = 0; //colors for circle
int d1, d2, d3, d4; 
int playerScore = 0, compScore = 0; //score of player and computer
float counter = 60.0; //timer
int stateOfGame = LOAD; //initial state of game
int posOfObj; //to determine if object will be on left side of screen or right side of screen
int rOfBox1=102, gOfBox1=102, bOfBox1=0, //color of box of difficult modes
	rOfBox2 = 102, gOfBox2 = 102, bOfBox2 = 0;
int diff; //difficulty of game
//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

typedef struct { //to store values of mouse position
	int x, y;
}mouse_t;

typedef struct { //x and y coordinate
	int x, y;
}point_t;

typedef struct { //data for barrel
	point_t pos; //mid point
	float angle; //angle of barrel
	int r;       //radius of barrel
}barrel_t;

typedef struct { //data for fired object
	point_t pos; //midpoint of fire
	bool active; //if fire is active or not
	float angle; //angle of fire
}fire_t;

typedef struct {  //data for balloon
	point_t pos;  //midpoint of balloon
	bool active;  //if balloon is moving or not
	int score,    //score of balloon
		radius;   //radius of balloon
	int r, g, b;  //color of balloon
}object_t;

//initialization
mouse_t mouse;
barrel_t barrel = { {0,0}, 90, 20 };
fire_t fire = { {0,0}, false, 0 };
object_t obj = { {0,0}, false, 0, 30, 1,1,0 };

void load()
{
	glColor3f(1, 1, 1);
	vprint2(-250, 100, 0.35, "GLUT HOMEWORK 3");
	vprint2(-125, 60, 0.20, "By Aizaz Hassan");
	vprint(-150, -200, GLUT_BITMAP_TIMES_ROMAN_24, "Press spacebar to start");
} //loading screen

void difficulty()
{
	vprint2(-100, 100, 0.2, "Choose Difficulty");
	glColor3ub(rOfBox1, gOfBox1, bOfBox1);
	glRectf(-100, 30, 100, -10);
	glColor3ub(rOfBox2, gOfBox2, bOfBox2);
	glRectf(-100, -20, 100, -60);
	glColor3f(1, 1, 1);
	vprint(-100, 0, GLUT_BITMAP_TIMES_ROMAN_24, "EASY");
	vprint(-100, -50, GLUT_BITMAP_TIMES_ROMAN_24, "HARD");
} //difficulty screen

void quizArea()
{
	int k;
	glColor3f(0, 0, 0);
	glRectf(-400, 300, 400, 200);
	glColor3f(1, 1, 1);
	vprint2(-390, 230, 0.35, "%d %% %d = ?", randNum1, randNum2);
	glColor3f(r1, g1, b1);
	circle((100 * ((posOfAns + 0) % 4) + 30), 250, 30);
	glColor3f(r2, g2, b2);
	circle((100 * ((posOfAns + 1) % 4) + 30), 250, 30);
	glColor3f(r3, g3, b3);
	circle((100 * ((posOfAns + 2) % 4) + 30), 250, 30);
	glColor3f(r4, g4, b4);
	circle((100 * ((posOfAns + 3) % 4) + 30), 250, 30);
	glColor3f(0, 0, 0);
	/*for(k=50;k<=350;k+=100)
		vprint2(k - 35, 230, 0.4, "%d", ans);*/
	vprint2(100 * posOfAns + 15, 230, 0.4, "%d", ans);
	vprint2(100 * ((posOfAns + 1) % 4) + 15, 230, 0.4, "%d", opt1);
	vprint2(100 * ((posOfAns + 2) % 4) + 15, 230, 0.4, "%d", opt2);
	vprint2(100 * ((posOfAns + 3) % 4) + 15, 230, 0.4, "%d", opt3);
} //quiz area of game

void scoreBoard()
{
	glColor3f(0, 0, 0);
	glRectf(-400, -300, 400, -200);
	glColor3f(1, 1, 1);
	vprint(-350, -250, GLUT_BITMAP_TIMES_ROMAN_24, "Time: %.2f", counter);
	vprint(-50, -250, GLUT_BITMAP_TIMES_ROMAN_24, "Computer: %d", compScore);
	vprint(250, -250, GLUT_BITMAP_TIMES_ROMAN_24, "Player: %d", playerScore);
}  //scoreboard area of game

void bg()
{
	glColor3ub(128, 149, 255);
	glRectf(-400, 200, 400, -200);
}  //background of game

void drawBarrel(barrel_t barrel)
{
	glColor3f(1, 1, 1);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(barrel.pos.x, barrel.pos.y);
	glVertex2f(barrel.pos.x + 50 * cos(barrel.angle*D2R), barrel.pos.y + 50 * sin(barrel.angle*D2R));
	glEnd();
	glLineWidth(1);
	glColor3ub(128, 42, 0);
	circle(barrel.pos.x, barrel.pos.y, barrel.r);
	glColor3f(0, 0.8, 0);
	circle(barrel.pos.x, barrel.pos.y, barrel.r - 5);
	glColor3f(1, 1, 1);
	vprint(barrel.pos.x - 9, barrel.pos.y - 5, GLUT_BITMAP_8_BY_13, "%.0f", barrel.angle);
}  //to draw barrel

void drawFire(fire_t fire)
{
	if (fire.active)
	{
		glColor3f(0, 0, 0);
		circle(fire.pos.x, fire.pos.y, 3);
	}
}  //to draw fire

void drawObject(object_t obj)
{
	glColor3ub(obj.r, obj.g, obj.b);
	circle(obj.pos.x, obj.pos.y, obj.radius);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(obj.pos.x, obj.pos.y);
	glVertex2f(obj.pos.x, obj.pos.y - 50);
	glEnd();
	glLineWidth(1);
	glColor3f(1, 1, 1);
	vprint(obj.pos.x - 5, obj.pos.y, GLUT_BITMAP_8_BY_13, "%d", obj.score);
}  //to draw balloon

void end()
{
	vprint(-100, 50, GLUT_BITMAP_TIMES_ROMAN_24, "PLAYER SCORE: %d", playerScore);
	vprint(-120, 0, GLUT_BITMAP_TIMES_ROMAN_24, "COMPUTER SCORE: %d", compScore);
	vprint(-150, -200, GLUT_BITMAP_8_BY_13, "Press F1 to choose difficulty or ESC to end game");
}  //to display game end screen
//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0.4, 0.4, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	switch (stateOfGame)
	{
	case LOAD:load(); break;
	case DIFFICULTY:difficulty(); break;
	case RUN:quizArea();
		scoreBoard();
		bg();
		drawBarrel(barrel);
		drawFire(fire);
		drawObject(obj);
		break;
	case END:end(); break;
	}
	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' '&&stateOfGame==LOAD) //to change state of game
		stateOfGame = DIFFICULTY;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	if (key == GLUT_KEY_F1 && stateOfGame==END) //to restart game
	{
		stateOfGame = DIFFICULTY;
		counter = 60.0;
		compScore = 0;
		playerScore = 0;
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && stateOfGame==RUN)  //to check if right circle is clicked and assign random values 
	{
		if (d1 <= 30 || d2 <= 30 || d3 <= 30 || d4 <= 30)
		{
			randNum1 = rand() % 9 + 1;
			randNum2 = rand() % 9 + 1;
			ans = randNum1 % randNum2;
			opt1 = rand() % 10;
			opt2 = rand() % 10;
			opt3 = rand() % 10;
			while (opt1 == ans || opt1 == opt2 || opt1 == opt3)
				opt1 = rand() % 10;
			while (opt2 == ans || opt2 == opt1 || opt2 == opt3)
				opt2 = rand() % 10;
			while (opt3 == ans || opt3 == opt2 || opt3 == opt1)
				opt3 = rand() % 10;
			posOfAns = rand() % 4;
			if (d1 <= 30)
			{
				r1 = 0;
				g1 = 1;
				b1 = 0;
				obj.active = true;
			}
			else if (d2 <= 30)
			{
				compScore += obj.score;
				r2 = 1;
				g2 = 0;
				b2 = 0;
				posOfObj = rand() % 2;
				switch (posOfObj)
				{
				case 0:obj.pos.x = rand() % 61 - 360; break;
				case 1:obj.pos.x = rand() % 331 + 50; break;
				}
				obj.pos.y = rand() % 51 - 151;
				obj.score = rand() % 5 + 1;
				fire.active = false;
				barrel.angle = 90;
			}
			else if (d3 <= 30)
			{
				compScore += obj.score;
				r3 = 1;
				g3 = 0;
				b3 = 0;
				posOfObj = rand() % 2;
				switch (posOfObj)
				{
				case 0:obj.pos.x = rand() % 61 - 360; break;
				case 1:obj.pos.x = rand() % 331 + 50; break;
				}
				obj.pos.y = rand() % 51 - 151;
				obj.score = rand() % 5 + 1;
				fire.active = false;
				barrel.angle = 90;
			}
			else if (d4 <= 30)
			{
				compScore += obj.score;
				r4 = 1;
				g4 = 0;
				b4 = 0;
				posOfObj = rand() % 2;
				switch (posOfObj)
				{
				case 0:obj.pos.x = rand() % 61 - 360; break;
				case 1:obj.pos.x = rand() % 331 + 50; break;
				}
				obj.pos.y = rand() % 51 - 151;
				obj.score = rand() % 5 + 1;
				fire.active = false;
				barrel.angle = 90;
			}
		}
	}

	mouse.x = x - winWidth / 2;
	mouse.y = winHeight / 2 - y;
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && stateOfGame == DIFFICULTY)//to select difficulty of game
	{
		if (mouse.x > -100 && mouse.x < 100 && mouse.y<30 && mouse.y>-10)
		{
			stateOfGame = RUN;
			diff = EASY;
		}
		else if (mouse.x > -100 && mouse.x < 100 && mouse.y<-20 && mouse.y>-60)
		{
			diff = HARD;
			stateOfGame = RUN;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void turnBarrel(barrel_t *barrel, int inc)
{
	barrel->angle += inc;
	if (barrel->angle > 360)
		barrel->angle -= 360;
	if (barrel->angle < 0)
		barrel->angle += 360;
} //rotates barrel at known speed

bool checkCollision(fire_t fire, object_t obj) {
	float dx = fire.pos.x - obj.pos.x;
	float dy = fire.pos.y - obj.pos.y;
	float d = sqrtf(dx*dx + dy * dy);
	if (d <= obj.radius)
		return true;
	else
		return false;
}  //checks if fire collides with balloon

bool barrelAlignment(barrel_t bar, object_t obj) {
	if ((tan(barrel.angle*D2R) >= (double)(obj.pos.y - 10) / obj.pos.x && tan(barrel.angle*D2R) <= (double)(obj.pos.y + 10) / obj.pos.x) || (tan((barrel.angle - 180)*D2R) >= (double)(obj.pos.y - 10) / obj.pos.x && (tan(barrel.angle - 180)*D2R) <= (double)(obj.pos.y + 10) / obj.pos.x))
		return true;
	else
		return false;
}  //checks if barrel is aligned with balloon in order to fire

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	mouse.x = x - winWidth / 2;
	mouse.y = winHeight / 2 - y;

	if (stateOfGame == RUN) { //to change colors of circles when mouse is hovered over them
		float dx1, dx2, dx3, dx4, dy1, dy2, dy3, dy4;

		/*100 * ((posOfAns + 0) % 4) + 30*/

		dx1 = mouse.x - (100 * ((posOfAns + 0) % 4) + 30);
		dy1 = mouse.y - 250.;
		d1 = sqrtf(dx1*dx1 + dy1 * dy1);
		if (d1 <= 30)
		{
			r1 = 0;
			g1 = 0;
			b1 = 1;
		}
		else
		{
			r1 = 1;
			g1 = 1;
			b1 = 0;
		}

		dx2 = mouse.x - (100 * ((posOfAns + 1) % 4) + 30);
		dy2 = mouse.y - 250.;
		d2 = sqrtf(dx2*dx2 + dy2 * dy2);
		if (d2 <= 30)
		{
			r2 = 0;
			g2 = 0;
			b2 = 1;
		}
		else
		{
			r2 = 1;
			g2 = 1;
			b2 = 0;
		}

		dx3 = mouse.x - (100 * ((posOfAns + 2) % 4) + 30);
		dy3 = mouse.y - 250.;
		d3 = sqrtf(dx3*dx3 + dy3 * dy3);
		if (d3 <= 30)
		{
			r3 = 0;
			g3 = 0;
			b3 = 1;
		}
		else
		{
			r3 = 1;
			g3 = 1;
			b3 = 0;
		}

		dx4 = mouse.x - (100 * ((posOfAns + 3) % 4) + 30);
		dy4 = mouse.y - 250.;
		d4 = sqrtf(dx4*dx4 + dy4 * dy4);
		if (d4 <= 30)
		{
			r4 = 0;
			g4 = 0;
			b4 = 1;
		}
		else
		{
			r4 = 1;
			g4 = 1;
			b4 = 0;
		}
	}

	if (stateOfGame == DIFFICULTY) //to change colors of boxes that cover the difficulty of game
	{
		if (mouse.x > -100 && mouse.x<100 && mouse.y<30 && mouse.y>-10)
		{
			rOfBox1 = 255;
			gOfBox1 = 106;
			bOfBox1 = 77;
		}
		else {
			rOfBox1 = 102;
			gOfBox1 = 102;
			bOfBox1 = 0;
		}
		if (mouse.x > -100 && mouse.x < 100 && mouse.y<-20 && mouse.y>-60)
		{
			rOfBox2 = 255;
			gOfBox2 = 106;
			bOfBox2 = 77;
		}
		else {
			rOfBox2 = 102;
			gOfBox2 = 102;
			bOfBox2 = 0;
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	if (stateOfGame == RUN)
	{
		counter -= 0.02; //changes to timer value
		if (counter <= 0)
			counter = 0;
		if (counter == 0) //ends game
		{
			stateOfGame = END;
			barrel.angle = 90;
			fire.angle = barrel.angle;
			fire.pos = barrel.pos;
			fire.active = false;
		}
	}

	if (stateOfGame == RUN) { //to adjust values of barrel, fire and balloon
		if (barrelAlignment(barrel, obj) && fire.active == false) {
			fire.pos = barrel.pos;
			fire.angle = barrel.angle;
			fire.active = true;
		}
		if (fire.active) {  //changes value of fire when fire is active
			fire.pos.x += 10 * cos(fire.angle*D2R);
			fire.pos.y += 10 * sin(fire.angle*D2R);
			if (fire.pos.x > 400 || fire.pos.x < -400 || fire.pos.y>200 || fire.pos.y < -200) {
				fire.active = false;
				fire.pos = barrel.pos;
				barrel.angle = 90;
			}
		}

		//different speeds at which barrel moves according to difficulty
		if (diff == EASY) {
			switch (posOfObj)
			{
			case 0:turnBarrel(&barrel, 1); break;
			case 1:turnBarrel(&barrel, -1); break;
			}
		}

		if (diff == HARD) {
			switch (posOfObj)
			{
			case 0:turnBarrel(&barrel, 2); break;
			case 1:turnBarrel(&barrel, -2); break;
			}
		}

		if (checkCollision(fire, obj)) //to make changes when fire collides with object
		{
			compScore += obj.score;
			posOfObj = rand() % 2;
			switch (posOfObj)
			{
			case 0:obj.pos.x = rand() % 61 - 360; break;
			case 1:obj.pos.x = rand() % 331 + 50; break;
			}
			obj.pos.y = rand() % 51 - 151;
			obj.score = rand() % 5 + 1;
			obj.r = rand() % 256;
			obj.g = rand() % 256;
			obj.b = rand() % 256;
			fire.active = false;
			barrel.angle = 90;

			randNum1 = rand() % 9 + 1;
			randNum2 = rand() % 9 + 1;
			ans = randNum1 % randNum2;
			opt1 = rand() % 10;
			opt2 = rand() % 10;
			opt3 = rand() % 10;
			while (opt1 == ans || opt1 == opt2 || opt1 == opt3)
				opt1 = rand() % 10;
			while (opt2 == ans || opt2 == opt1 || opt2 == opt3)
				opt2 = rand() % 10;
			while (opt3 == ans || opt3 == opt2 || opt3 == opt1)
				opt3 = rand() % 10;
			posOfAns = rand() % 4;
		}

		if (obj.active) //to make changes to balloon when correct answer is given
		{
			(obj.pos.y) += 10;
			if (obj.pos.y + obj.radius > 200)
			{
				playerScore += obj.score;
				obj.pos.y = rand() % 51 - 151;
				obj.score = rand() % 5 + 1;
				obj.r = rand() % 256;
				obj.g = rand() % 256;
				obj.b = rand() % 256;
				posOfObj = rand() % 2;
				switch (posOfObj)
				{
				case 0:obj.pos.x = rand() % 61 - 360; break;
				case 1:obj.pos.x = rand() % 331 + 50; break;
				}
				obj.active = false;
			}
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("GLUT HW3 by Aizaz Hassan");

	//assigns random values to all variables
	srand(time(0));
	randNum1 = rand() % 9 + 1;
	randNum2 = rand() % 9 + 1;
	ans = randNum1 % randNum2;
	opt1 = rand() % 10;
	opt2 = rand() % 10;
	opt3 = rand() % 10;
	while (opt1 == ans || opt1 == opt2 || opt1 == opt3)
		opt1 = rand() % 10;
	while (opt2 == ans || opt2 == opt1 || opt2 == opt3)
		opt2 = rand() % 10;
	while (opt3 == ans || opt3 == opt2 || opt3 == opt1)
		opt3 = rand() % 10;
	posOfAns = rand() % 4;

	posOfObj = rand() % 2;
	switch (posOfObj)
	{
	case 0:obj.pos.x = rand() % 61 - 360; break;
	case 1:obj.pos.x = rand() % 331 + 50; break;
	}
	obj.pos.y = rand() % 51 - 151;
	obj.score = rand() % 5 + 1;
	obj.r = rand() % 256;
	obj.g = rand() % 256;
	obj.b = rand() % 256;

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}