///-----------------------------------------------------------------------------
/// 
/// \file	main.cpp
/// \date	15/3/2005
/// \author	Rob Bateman
/// \brief	This demonstrates how to use a spring to make the camera smoothly 
///			follow a given point. This is really useful for cameras within games.
///			It works by using a target position and an actual position for the
///			camera. A spring is then used to keep the cameras actual position
///			close to where the target position. Because a spring is used to connect
///			the two, there is a little bit of lag before the camera catches up 
///			with the target. The result is fairly nice ;) 
/// \note	DampenedSpring() is the important function here!
///
//-------------------------------------------------------------------------------

#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include "FrameTimer.h"

/// the y rotation of the camera in degrees
float angle=0;
float angley=0;

///location of click
int lastx;
int lasty;

///button boolean
float Buttons[1]={0};

/// the angle resolved into a direction vector
float direction[3]={0,0,-1};

/// the inverted position of the camera.
float position[3]={0,-2,0};

/// the target position of the camera (not inverted). Just simply change
/// this target position and the camera will follow.... 
float target[3]={0,2,0};

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



//-------------------------------------------------------------------------------	OnInit()
//
void OnInit() 
{
	InitFrameTimer();
}

//-------------------------------------------------------------------------------	OnDisplay()
//
void OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLoadIdentity();

	// set camera position
	glRotatef(angle,0,1,0);
	glRotatef(angley,1,0,0);
	

	glTranslatef(position[0],position[1],position[2]);

	// draw grid
	glBegin(GL_LINES);
	for(int i=-10;i<=10;++i) {
		glVertex3f(i,0,-10);
		glVertex3f(i,0,10);

		glVertex3f(10,0,i);
		glVertex3f(-10,0,i);
	}
		glVertex3f(5,5,5);
		glVertex3f(8,8,8);
	glEnd();

	glutSwapBuffers();
}

//-------------------------------------------------------------------------------	OnReshape()
//
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

/// used to keep track of the cursor key states
bool g_keys[128]={false};


//-------------------------------------------------------------------------------	OnIdle()
//
void OnIdle() {

	// update the frame timer
	SortFrameTimer();

	// will hold the new position for the camera
	float NewPosition[3];

	// get the frame time
	float dt = FrameTime();

	// if turning left
	if (g_keys['a']) {
		angle -= dt * 90.0f;
	}

	// if turning right
	if (g_keys['d']) {
		angle += dt * 90.0f;
	}

	// calculate the direction vector from the angle
	direction[0] = sin(angle*3.14159f/180.0f);
	direction[2] = -cos(angle*3.14159f/180.0f);
	direction[1] = 0.0f;

	// move the target forward relative to the direction vector
	if (g_keys['w']) {
		target[0] += direction[0]*dt*5.0f;
		target[1] += direction[1]*dt*5.0f;
		target[2] += direction[2]*dt*5.0f;
	}
	// move the target backwards relative to the direction vector
	if (g_keys['s']) {
		target[0] -= direction[0]*dt*5.0f;
		target[1] -= direction[1]*dt*5.0f;
		target[2] -= direction[2]*dt*5.0f;
	}

	// clauclate a new position for the camera
	DampenedSpring(position,target,NewPosition,dt); 

	// update the position
	position[0] = NewPosition[0];
	position[1] = NewPosition[1];
	position[2] = NewPosition[2];

	// redraw the scene
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------	OnSpecial()
//
void OnKey(unsigned char key,int,int) {
	g_keys[key]=true;
}

void onMouse(int b, int s, int x, int y){
	lastx=x;
	lasty=y;

	Buttons[0] = true;
}

void onMotion(int x, int y){
	std::cout << y << " " << x << std::endl;

	int diffx=x-lastx;
	int diffy=y-lasty;
	lastx=x;
	lasty=y;


if (Buttons[0]){
	angle = 0;//-= (float) 0.5f * diffx;
	angley -= (float) 0.5f * diffy;
}

}
//-------------------------------------------------------------------------------	OnSpecialUp()
//
void OnKeyUp(unsigned char key,int,int) {
	g_keys[key]=false;
}

//-------------------------------------------------------------------------------	main()
///
int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(640,480);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Camera");
	
	// set glut callbacks
	glutDisplayFunc(OnDisplay);
	glutReshapeFunc(OnReshape);

	glutKeyboardFunc(OnKey);
	glutKeyboardUpFunc(OnKeyUp);


	glutMotionFunc(onMotion);
	glutMouseFunc(onMouse);


	glutIdleFunc(OnIdle);

	OnInit();

	glutMainLoop();
}

























