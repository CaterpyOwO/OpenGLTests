#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include "FrameTimer.h"

///OBJ loader globals
GLuint elephant;
float elephantrot;

///camera rotation in degrees
float angle=0;
float angley=0;

///last location of click
int lastx;
int lasty;

///button boolean
float button = false;

///
float id;

/// the angle resolved into a direction vector
float direction[3]={0,0,-1};

/// the inverted position of the camera.
float position[3]={0,-2,0};

/// the target position of the camera (not inverted)
float target[3]={0,2,-3};

/// the strength of the spring. Higher values make the camera more rigid.
float spring_strength=0.5;


//-------------------------------------------------------------------------------	DampenedSpring()
//
void DampenedSpring(float *CurrentPosition,
				    float *TargetPosition,
				    float *NewPosition,
				    float &dt) 
{
	// copy current position into new position just incase we do nothing
	memcpy(NewPosition,CurrentPosition,sizeof(float)*3);

	// calculate the displacement between the target and the current position
	float Displacement[3];
	memcpy(Displacement,NewPosition,sizeof(float)*3);
	Displacement[0] += TargetPosition[0];
	Displacement[1] += TargetPosition[1];
	Displacement[2] += TargetPosition[2];

    // whats the distance between them?
	float DisplacementLength=sqrt(Displacement[0]*Displacement[0] +
								  Displacement[1]*Displacement[1] +
								  Displacement[2]*Displacement[2] );

	float DampConstant=0.00065; // Something v.small to offset 1/ displacement length

	// Stops small position fluctuations (integration errors probably - since only using euler)
	if (DisplacementLength<0.001)
		return;

	// the strength of the spring increases the further away the camera is from
	// the target.
	float SpringMagitude = 10 * DisplacementLength + 
		DampConstant * (1 / DisplacementLength);

	// Normalise the displacement and scale by the spring magnitude 
	// and the amount of time passed
	float scalar = (1.0f/DisplacementLength) * SpringMagitude * dt;
	Displacement[0] *= scalar;
	Displacement[1] *= scalar;
	Displacement[2] *= scalar;

	// move the camera a bit towards the target
	NewPosition[0] -= Displacement[0];
	NewPosition[1] -= Displacement[1];
	NewPosition[2] -= Displacement[2];
}

void loadObj(const char* fname)
{
FILE *fp;
int read;
GLfloat x, y, z;
char ch;
elephant=glGenLists(1);
fp=fopen(fname,"r");
if (!fp) 
    {
        printf("can't open file %s\n", fname);
	  exit(1);
    }
glPointSize(2.0);
glNewList(elephant, GL_COMPILE);
{
glPushMatrix();
glBegin(GL_POINTS);
while(!(feof(fp)))
 {
  read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
  if(read==4&&ch=='v')
  {
   glVertex3f(x,y+2,z);
  }
 }
glEnd();
}
glPopMatrix();
glEndList();
fclose(fp);
}

void OnInit() 
{
	glutSetCursor(GLUT_CURSOR_NONE);
	InitFrameTimer();
	loadObj("data/teapot.obj");
}


void OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLoadIdentity();

	// set camera angle
	glRotatef(-angley,direction[2]*2.0f,0,0);
	glRotatef(angle,0,1,0);


	// set camera position
	glTranslatef(position[0],position[1],position[2]);

	glBegin(GL_LINES);
	// draw grid
	for(int i=-10;i<=10;++i) {
		glVertex3f(i,0,-10);
		glVertex3f(i,0,10);

		glVertex3f(10,0,i);
		glVertex3f(-10,0,i);
	}
	glEnd();

	//render & rotate teapot
	glPushMatrix();
		glRotatef(elephantrot,0,1,0);
		glCallList(elephant);
		elephantrot=elephantrot+0.1;
		if(elephantrot>360)elephantrot=elephantrot-360;
	glPopMatrix();

	//optional lol text
/*
	glTranslatef(-15,20,0);	
	glScalef(0.1,0.1,0.1);

	glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, 'l');

	glTranslatef(0,0,0);
	glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, 'o');

	glTranslatef(10,0,0);
	glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, 'l');
*/
	glutSwapBuffers();
}


void OnReshape(int w, int h)
{
	// prevent divide by 0 error when minimised
	if(w==0) 
		h = 1;

	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,(float)w/h,0.1,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/// cursor key states
bool g_keys[128]={false};


void OnIdle() {

	// update frame timer
	SortFrameTimer();

	// hold the new position for the camera
	float NewPosition[3];

	// get the frame time
	float dt = FrameTime();

	// calculate the direction vector from the angle
	direction[0] = sin(angle*3.14159f/180.0f);
	direction[2] = -cos(angle*3.14159f/180.0f);
	direction[1] = 0.0f;

	//move left
	if (g_keys['a']) {
		target[0] -= sin((angle+90)*3.14159f/180.0f)*dt*5.0f;
		target[1] -= direction[1]*dt*5.0f;
		target[2] -= -cos((angle+90)*3.14159f/180.0f)*dt*5.0f;
	}

	// move right
	if (g_keys['d']) {
		target[0] -= sin((angle-90)*3.14159f/180.0f)*dt*5.0f;
		target[1] -= direction[1]*dt*5.0f;
		target[2] -= -cos((angle-90)*3.14159f/180.0f)*dt*5.0f;
	}


	// move forward
	if (g_keys['w']) {
		target[0] += direction[0]*dt*5.0f;
		target[1] += direction[1]*dt*5.0f;
		target[2] += direction[2]*dt*5.0f;
	}
	// move backwards
	if (g_keys['s']) {
		target[0] -= direction[0]*dt*5.0f;
		target[1] -= direction[1]*dt*5.0f;
		target[2] -= direction[2]*dt*5.0f;
	}

	// calculate new position for camera
	DampenedSpring(position,target,NewPosition,dt); 

	// update the position
	position[0] = NewPosition[0];
	position[1] = NewPosition[1];
	position[2] = NewPosition[2];

	// redraw the scene
	glutPostRedisplay();
}

// key input
void OnKey(unsigned char key,int,int) {
	g_keys[key]=true;
}

// mouse input
void onMouse(int b, int s, int x, int y){
	lastx=x;
	lasty=y;
	button = true;
}

// mouse motion input
void onMotion(int x, int y){
	int diffx=x-lastx;
	int diffy=y-lasty;

	lastx=x;
	lasty=y;


if (button){
	angle -= (float) 0.3f * diffx;
	angley -= (float) 0.3f * diffy;
}

}

// update key press
void OnKeyUp(unsigned char key,int,int) {
	g_keys[key]=false;
	if (key == ''){
		glutDestroyWindow(id);
	}
}


//main function
int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(640,480);
	glutInitWindowPosition(100,100);
	id = glutCreateWindow("Camera v1.3 + .obj loading");
	
	// set glut callbacks
	glutDisplayFunc(OnDisplay);
	glutReshapeFunc(OnReshape);

	// set key callbacks
	glutKeyboardFunc(OnKey);
	glutKeyboardUpFunc(OnKeyUp);

	// set mouse callbacks
	glutPassiveMotionFunc(onMotion);
	glutMouseFunc(onMouse);


	// set idle function
	glutIdleFunc(OnIdle);

	// initialize camera
	OnInit();
	glutMainLoop();
}

























