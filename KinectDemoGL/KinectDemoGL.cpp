/*
Ronald O'Dell and Trevor Hamilton CPSC 456
*/



#include "stdafx.h"

#include <string.h>
#include <math.h>


#include <Windows.h>
#include <Ole2.h>

//GLUT and OpenGL things
#include <GL/glut.h>

//Kinect things
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>
#include <NuiSkeleton.h>

#include <math.h>
#include <iostream>
using namespace std;




GLUquadric* qobj;
// Camera Postitions, and tons upon tons of vertices. 
float xposOriginal = -25.0; 
float yposOriginal = -15.0;
float zposOriginal = 0.0;

float xpos = xposOriginal; 
float ypos = yposOriginal;
float zpos = zposOriginal;

float xrotOriginal = 0.0;
float yrotOriginal = 0.0;
float zrotOriginal = 0.0; 

float xrot = xrotOriginal;
float yrot = yrotOriginal;
float zrot = zrotOriginal; 

float scale = 1.0;
float rotScale = 1.0; //May need different scales for each direction
float posScale = 1.0;

bool infoToggle = false;
double PI = 3.14159265358979323846;

GLfloat lightPosition[]    = {0.5, 0, -3.5, 0.5};

GLfloat green[] = {0.0, 1.0, 0.0, 1.0}; //Green Color
GLfloat blue[] = {0.0, 0.0, 1.0, 1.0}; //Blue Color
GLfloat orange[] = {1.0, 0.5, 0.0, 1.0}; //Orange Color
GLfloat purple[] = {0.5, 0.0, 0.5, 1.0}; //Purple Color
GLfloat black[] = {0.0, 0.0, 0.0, 1.0}; //Black Color

GLfloat white[] = {1.0, 1.0, 1.0, 1.0}; //White Color



//Kinect Vars
HANDLE depthStream;
HANDLE rgbStream;
INuiSensor* sensor;
Vector4 skeletonPosition[NUI_SKELETON_POSITION_COUNT];

bool initKinect() {
    // Get a working kinect sensor
    int numSensors;
    if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1) return false;
    if (NuiCreateSensorByIndex(0, &sensor) < 0) return false;

    // Initialize sensor
    sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);
    sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, // Depth camera or rgb camera?
        NUI_IMAGE_RESOLUTION_640x480,                // Image resolution
        0,        // Image stream flags, e.g. near mode
        2,        // Number of frames to buffer
        NULL,     // Event handle
        &depthStream);
	sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, // Depth camera or rgb camera?
        NUI_IMAGE_RESOLUTION_640x480,                // Image resolution
        0,      // Image stream flags, e.g. near mode
        2,      // Number of frames to buffer
        NULL,   // Event handle
		&rgbStream);
	sensor->NuiSkeletonTrackingEnable(NULL, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT); // NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT for only upper body
    return sensor;
}

void getSkeletalData() {
	NUI_SKELETON_FRAME skeletonFrame = {0};
    if (sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame) >= 0) {
		sensor->NuiTransformSmooth(&skeletonFrame, NULL);
		// Loop over all sensed skeletons
		for (int z = 0; z < NUI_SKELETON_COUNT; ++z) {
			const NUI_SKELETON_DATA& skeleton = skeletonFrame.SkeletonData[z];
			// Check the state of the skeleton
			if (skeleton.eTrackingState == NUI_SKELETON_TRACKED) {			
				// Copy the joint positions into our array
				for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i) {
					skeletonPosition[i] = skeleton.SkeletonPositions[i];
					if (skeleton.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_NOT_TRACKED) {
						skeletonPosition[i].w = 0;
					}
				}
				return; // Only take the data for one skeleton
			}
		}
	}
}


void spheres()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	// Set lighting intensity
	GLfloat ambientLight[]    = {0.2, 0.2, 0.2, 1.0};
	GLfloat diffuseLight[]    = {0.8, 0.8, 0.8, 1.0};
	GLfloat specularLight[]    = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    
    // Set the light position
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	//Set light color
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, green);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
	glPushMatrix();
		glColor3f(0.0, 1.0, 0.0); //wanted center of projection green sphere
		glTranslatef (0.0, 0.0, 0.0);
		glutSolidSphere(1, 100, 100);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blue);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
	glPushMatrix();
		glColor3f(0.0, 0.0, 1.0); //blue
		glTranslatef (4.0, 0.0, 0.0);
		glutSolidSphere(1, 100, 100);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, orange);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, orange);
	glPushMatrix();
		glColor3f(1.0, 0.5, 0.0); //orange
		glTranslatef (-4.0, 0.0, 0.0);
		glutSolidSphere(1, 100, 100);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, purple);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, purple);
	glPushMatrix();
		glColor3f(0.5, 0.0, 0.5); //purple
		glTranslatef (0.0, 0.0, 5.0);
		glutSolidSphere(1, 100, 100);
	glPopMatrix();
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, black);
	glPushMatrix();
		glColor3f(0, 0.0, 0); //black
		glTranslatef (0.0, 0.0, -5.0); 
		glutSolidSphere(1, 100, 100);
	glPopMatrix();   

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}

void keyboard (unsigned char key, int x, int y) {
 
	switch (key) {
		case  27:  
			exit (0);
		case 'w': 
			yposOriginal += 1;
			glutPostRedisplay();  
			break;
		case 's': 
			yposOriginal -= 1;
			glutPostRedisplay();  
			break;
		case 'a': 
			zposOriginal += 1;
			glutPostRedisplay();  
			break;
		case 'd': 
			zposOriginal -= 1;
			glutPostRedisplay();  
			break;
		case 'q': 
			xposOriginal += 1;
			glutPostRedisplay();  
			break;
		case 'e': 
			xposOriginal -= 1;
			glutPostRedisplay();  
			break;
		case 'p':
			cout << "xposOriginal = " << xposOriginal << endl; 
			cout << "yposOriginal = " << yposOriginal << endl;
			cout << "zposOriginal = " << zposOriginal << endl;

			cout << "xpos = " << xpos << endl; 
			cout << "ypos = " << xpos << endl;
			cout << "zpos = " << xpos << endl;

			cout << "xrot = " << xrot << endl;
			cout << "yrot = " << yrot << endl;
			cout << "zrot = " << zrot << endl; 

			cout << "scale = " << scale << endl;
			cout << "rotScale = " << rotScale << endl;
			cout << "posScale = " << posScale << endl;
			break;
		case 'i':
			infoToggle = !infoToggle;
			break;
		case '8': 
			posScale += .5;
			glutPostRedisplay();  
			break;
		case '5': 
			posScale -= .5;
			glutPostRedisplay();  
			break;

		case 'W': 
			zrotOriginal += 1;
			glutPostRedisplay();  
			break;
		case 'S': 
			zrotOriginal -= 1;
			glutPostRedisplay();  
			break;
		case 'A': 
			yrotOriginal -= 1;
			glutPostRedisplay();  
			break;
		case 'D': 
			yrotOriginal += 1;
			glutPostRedisplay();  
			break;
		case 'Q': 
			scale -= 1;
			glutPostRedisplay();  
			break;
		case 'E': 
			scale += 1;
			glutPostRedisplay();  
			break;
		case ' ':
			glutFullScreen();
			break;

		default :  printf ("   key = %c -> %d\n", key, key);
   }
}

void updatePosition(int value)
{
	glutPostRedisplay();  
	glutTimerFunc(16, updatePosition, 1);    
}


void display()
{ 
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10.0, 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    

	if(sensor)
	{
		getSkeletalData();//grab skeleton data
		
		//change based on pos
		//zpos =  skeletonPosition[NUI_SKELETON_POSITION_HEAD].x * posScale + zposOriginal;
		//ypos = -skeletonPosition[NUI_SKELETON_POSITION_HEAD].y * posScale + yposOriginal; //may not need later
		//xpos = -skeletonPosition[NUI_SKELETON_POSITION_HEAD].z * posScale + xposOriginal; //may not need later

		//Change based on rotation
		yrot = ((atan(skeletonPosition[NUI_SKELETON_POSITION_HEAD].x/(-skeletonPosition[NUI_SKELETON_POSITION_HEAD].z)) * 180) / PI) * rotScale + yrotOriginal;
		zrot = -((atan(-skeletonPosition[NUI_SKELETON_POSITION_HEAD].y/(-skeletonPosition[NUI_SKELETON_POSITION_HEAD].z)) * 180) / PI) * rotScale + zrotOriginal; //may not need later
		//xrot = -((atan(-skeletonPosition[NUI_SKELETON_POSITION_HEAD].x/(-skeletonPosition[NUI_SKELETON_POSITION_HEAD].y)) * 180) / PI) * rotScale + zrotOriginal; //Not applicable

		//change using gluLookAt
		//glRotatef(90, 0, 1, 0);
		//gluLookAt(skeletonPosition[NUI_SKELETON_POSITION_HEAD].z, -skeletonPosition[NUI_SKELETON_POSITION_HEAD].y, -skeletonPosition[NUI_SKELETON_POSITION_HEAD].x, 0.0, 0, 0, 0, 1, 0);

		if(infoToggle)
		{
			cout << skeletonPosition[NUI_SKELETON_POSITION_HEAD].x << "      " << -skeletonPosition[NUI_SKELETON_POSITION_HEAD].y << "      " << -skeletonPosition[NUI_SKELETON_POSITION_HEAD].z << endl;
	
		}
	}
	
	glTranslatef (xpos, ypos, zpos);  // Translations.
	
	glRotatef (zrot, 0,0,1);        // Rotations.
	glRotatef (yrot, 0,1,0);
	glRotatef (xrot, 1,0,0);


    glScalef (scale, scale, scale);
    spheres();
	//light position needs to rotate
	glutSwapBuffers();
    
}

// reshapes window, gives the actual size of the xzy coordinate units
void reshape(int w, int h)
{
	glViewport(0,0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-20.0, 20.0, -20.0, 20.0, -20.0, 230.0); //Old parralel projection
	//gluPerspective(10, w/h, 10, 100);
	glFrustum(-5.0*w/h, 5.0*w/h, -5.0, 5.0, 30.0, 100.0);
}


//get it all up and ready at start
void init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);      
	qobj = gluNewQuadric();
	gluQuadricNormals(qobj, GLU_SMOOTH);
}

//get window size, position, and start the functions to draw it all 
int main(int argc, char** argv)
{
	initKinect();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	
	glutInitWindowSize(800, 450);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("WarGames");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	init();
	updatePosition(1);
	glutMainLoop();   
}