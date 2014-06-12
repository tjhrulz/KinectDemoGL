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
#include <string>
using namespace std;


const int TEXTURECOUNT = 500;
const double PI = 3.14159265358979323846;
const float screenWidthCm = 142.3;
const float screenHeightCm = 80.5;
const float kinectOffsetCm[3] = {0,-46,0};

const int X = 0;
const int Y = 1;
const int Z = 2;
//Collab 4K tv 142.3 cm Width 80.5 Height
//Collab Resolution 3840 x 2160

//Laptop 34.5 cm Width 19.5 Height
//Laptop Resolution 1920 x 1080
//In centimeters


// Camera Postition Vars
float xpos = 0.0; 
float ypos = 0.0;
float zpos = 0.0;

float xrot = 0.0;
float yrot = 0.0;
float zrot = 0.0;

float worldHeadLoc[3];

//Textures and Normals Vars
unsigned int textureWidth[TEXTURECOUNT];
unsigned int textureHeight[TEXTURECOUNT];
char * textureData[TEXTURECOUNT];
GLuint textureID[TEXTURECOUNT];
int textureToLoad = 0;

//Lighting Vars
GLfloat lightPosition[]    = {screenWidthCm, screenHeightCm, 50, 0.0};

GLfloat green[] = {0.0, 1.0, 0.0, 1.0}; //Green Color
GLfloat blue[] = {0.0, 0.0, 1.0, 1.0}; //Blue Color
GLfloat orange[] = {1.0, 0.5, 0.0, 1.0}; //Orange Color
GLfloat purple[] = {0.5, 0.0, 0.5, 1.0}; //Purple Color
GLfloat black[] = {0.0, 0.0, 0.0, 1.0}; //Black Color

GLfloat red[] = {1.0, 0.0, 0.0, 1.0}; //Red Color

GLfloat white[] = {1.0, 1.0, 1.0, 1.0}; //White Color

GLfloat ambientLight[]    = {0.2, 0.2, 0.2, 1.0};
GLfloat diffuseLight[]    = {0.8, 0.8, 0.8, 1.0};
GLfloat specularLight[]    = {1.0, 1.0, 1.0, 1.0};


//Kinect Vars
HANDLE depthStream;
INuiSensor* sensor;
Vector4 skeletonPosition[NUI_SKELETON_POSITION_COUNT];
//Generic Vars
bool infoToggle = false;


//Collab 4K tv 142.3 cm Width 80.5 Height
//Collab Resolution 3840 x 2160
//Laptop 34.5 cm Width 19.5 Height
//Laptop Resolution 1920 x 1080
//In centimeters
int pixelWidth;
int pixelHeight;
float pixelRatio;

//Clean up kinect code some uneeded stuff in here
bool initKinect() 
{
    // Get a working kinect sensor
    int numSensors;
    if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1) return false;
    if (NuiCreateSensorByIndex(0, &sensor) < 0) return false;

    // Initialize sensor
    sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
    sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, // Depth camera
        NUI_IMAGE_RESOLUTION_80x60,                // Image resolution
        0,        // Image stream flags, e.g. near mode
        0,        // Number of frames to buffer
        NULL,     // Event handle
        &depthStream);
	sensor->NuiSkeletonTrackingEnable(NULL, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT); // NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT for only upper body
    return true;
}

void getSkeletalData() 
{	
	if(sensor)
	{
		NUI_SKELETON_FRAME skeletonFrame = {0};
		if (sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame) >= 0) 
		{	
			sensor->NuiTransformSmooth(&skeletonFrame, NULL);
			// Loop over all sensed skeletons
			for (int z = 0; z < NUI_SKELETON_COUNT; ++z) 
			{
				const NUI_SKELETON_DATA& skeleton = skeletonFrame.SkeletonData[z];
				// Check the state of the skeleton
				if (skeleton.eTrackingState == NUI_SKELETON_TRACKED) 
				{			
				
					skeletonPosition[NUI_SKELETON_POSITION_HEAD] = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HEAD];
					if (skeleton.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HEAD] == NUI_SKELETON_POSITION_NOT_TRACKED) 
					{
						skeletonPosition[NUI_SKELETON_POSITION_HEAD].w = 0;
					}
					glutPostRedisplay();
					return; // Only take the data for one skeleton
				}
			}
		}
	}
}

void initTexturesBmp(string baseFileName)
{
	//Load in all the textures
	while(textureToLoad < TEXTURECOUNT)
	{
		char header[54]; //How large the header should be in a bmp
		int dataPos; //Where the image starts     
		int imageSize; //How many pixels are there


		string fileName = baseFileName + " (" + std::to_string(static_cast<long long>(textureToLoad+1)) + ").bmp";	//Take the base image name and add on the counting system which is image (x+1).bmp (Easy to rename on widows like that
		//Right now this is hard coded until I can figure out a platform agnostic way to programatically get the name of every image in a folder		

 		FILE * file = fopen(fileName.c_str(),"rb"); //Convert string to char and open it Using fopen since openGL expects char* not strings
		if (!file)
		{
			//File could not be found
			cout << "Image could not be found/opened" << endl;; 
			system("pause");
			exit(0);
		}
		if (fread(header, 1, 54, file)!=54 ||  header[0]!='B' || header[1]!='M' )
		{
			cout << "BMP file is not formated correctly" << endl;
			system("pause");
			exit(0);
		}
		dataPos    = *(int*)&(header[0x0A]);
		imageSize  = *(int*)&(header[0x22]);
		textureWidth[textureToLoad]      = *(int*)&(header[0x12]);
		textureHeight[textureToLoad]     = *(int*)&(header[0x16]);

		textureData[textureToLoad] = new char [imageSize]; //Where the data for each image will be stored allocate for current image size (Jagged Array)
 
		fread(textureData[textureToLoad],1,imageSize,file); //Load in the data from the image to appropriate array location
 
		fclose(file); //Clean up unneeded resources

		glGenTextures(1, &textureID[textureToLoad]);
 
		textureToLoad++;
		cout << "Texture Number: " << textureToLoad << endl;
	}
	textureToLoad = 0;
}



void spheres()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	// Set lighting intensity


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
		glutSolidSphere(1*screenHeightCm/8, 100, 100);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blue);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
	glPushMatrix();
		glColor3f(0.0, 0.0, 1.0); //blue
		glTranslatef (0.0, 0.0, screenHeightCm);
		glutSolidSphere(1*screenHeightCm/8, 100, 100);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, orange);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, orange);
	glPushMatrix();
		glColor3f(1.0, 0.5, 0.0); //orange
		glTranslatef (.0, 0.0, -screenHeightCm);
		glutSolidSphere(1*screenHeightCm/8, 100, 100);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, purple);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, purple);
	glPushMatrix();
		glColor3f(0.5, 0.0, 0.5); //purple
		glTranslatef (screenHeightCm, 0.0, 0.0);
		glutSolidSphere(1*screenHeightCm/8, 100, 100);
	glPopMatrix();
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, black);
	glPushMatrix();
		glColor3f(0.0, 0.0, 0.0); //black
		glTranslatef (-screenHeightCm, 0.0, 0.0); 
		glutSolidSphere(1*screenHeightCm/8, 100, 100);
	glPopMatrix();   

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	glPushMatrix();
		glColor3f(1.0, 0.0, 0.0); //red
		glTranslatef (0.0, 0.0, 2* screenHeightCm); 
		glutSolidSphere(1*screenHeightCm/8, 100, 100);
	glPopMatrix();   

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

}

void scene()
{
	glEnable(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, textureWidth[textureToLoad], textureHeight[textureToLoad], 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, textureData[textureToLoad]);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBegin(GL_QUADS); //Bottom
		glColor3f(1.0,1.0,1.0);
		glNormal3d(0, 1, 0); 
		glTexCoord2f(5.0,5.0);		glVertex3f( screenWidthCm,-screenHeightCm, 2*screenHeightCm);  
		glTexCoord2f(-5.0,5.0);		glVertex3f( screenWidthCm,-screenHeightCm,-screenHeightCm);
		glTexCoord2f(-5.0,-5.0);	glVertex3f(-screenWidthCm,-screenHeightCm,-screenHeightCm);
		glTexCoord2f(5.0,-5.0);		glVertex3f(-screenWidthCm,-screenHeightCm, 2*screenHeightCm);
    glEnd();
	glBegin(GL_QUADS);//Left
		glColor3f(1.0,1.0,1.0);
		glNormal3d(1, 0, 0);
		glTexCoord2f(-5.0,-5.0);	glVertex3f(-screenWidthCm,-screenHeightCm,-screenHeightCm);
		glTexCoord2f(-5.0,5.0);		glVertex3f(-screenWidthCm, screenHeightCm,-screenHeightCm); 
		glTexCoord2f(5.0,5.0);		glVertex3f(-screenWidthCm, screenHeightCm, 2*screenHeightCm);  
		glTexCoord2f(5.0,-5.0);		glVertex3f(-screenWidthCm,-screenHeightCm, 2*screenHeightCm);
    glEnd();
	glBegin(GL_QUADS);//Right
		glColor3f(1.0,1.0,1.0);
		glNormal3d(-1, 0, 0); 
		glTexCoord2f(5.0,-5.0);		glVertex3f(screenWidthCm,-screenHeightCm, 2*screenHeightCm);
		glTexCoord2f(5.0,5.0);		glVertex3f(screenWidthCm, screenHeightCm, 2*screenHeightCm);
		glTexCoord2f(-5.0,5.0);		glVertex3f(screenWidthCm, screenHeightCm,-screenHeightCm);
		glTexCoord2f(-5.0,-5.0);	glVertex3f(screenWidthCm,-screenHeightCm,-screenHeightCm);
    glEnd();
	glBegin(GL_QUADS);//Top
		glColor3f(1.0,1.0,1.0);
		glNormal3d(0, -1, 0);
		glTexCoord2f(-5.0,5.0);		glVertex3f( screenWidthCm,screenHeightCm,-screenHeightCm); 
		glTexCoord2f(5.0,5.0);		glVertex3f( screenWidthCm,screenHeightCm, 2*screenHeightCm);  
		glTexCoord2f(5.0,-5.0);		glVertex3f(-screenWidthCm,screenHeightCm, 2*screenHeightCm);
		glTexCoord2f(-5.0,-5.0);	glVertex3f(-screenWidthCm,screenHeightCm,-screenHeightCm);
    glEnd();
	glBegin(GL_QUADS); //Back
		glColor3f(1.0,1.0,1.0);
		glNormal3d(0, 0, 1);
		glTexCoord2f(1,-0);		glVertex3f( screenWidthCm,-screenHeightCm,-screenHeightCm); 
		glTexCoord2f(1,1);		glVertex3f( screenWidthCm, screenHeightCm,-screenHeightCm);  
		glTexCoord2f(-0,1);		glVertex3f(-screenWidthCm, screenHeightCm,-screenHeightCm);
		glTexCoord2f(-0,-0);	glVertex3f(-screenWidthCm,-screenHeightCm,-screenHeightCm);
    glEnd();
	glDisable(GL_TEXTURE_2D);

	textureToLoad++;
	if(textureToLoad == TEXTURECOUNT)
	{
		textureToLoad = 0;
	}
}

void keyboard (unsigned char key, int x, int y) {
 
	switch (key) {
		case  27:  
			exit (0);
		case 'w': 
			zpos += 10;
			glutPostRedisplay();  
			break;
		case 's': 
			zpos -= 10;
			glutPostRedisplay();  
			break;
		case 'a': 
			xpos += 1;
			glutPostRedisplay();  
			break;
		case 'd': 
			xpos -= 1;
			glutPostRedisplay();  
			break;
		case 'q': 
			ypos += 1;
			glutPostRedisplay();  
			break;
		case 'e': 
			ypos -= 1;
			glutPostRedisplay();  
			break;
		case 'p':
			cout << "xpos = " << xpos << endl; 
			cout << "ypos = " << ypos << endl;
			cout << "zpos = " << zpos << endl;

			cout << "xrot = " << xrot << endl;
			cout << "yrot = " << yrot << endl;
			cout << "zrot = " << zrot << endl;

			cout << "worldHeadLoc[x] = " << worldHeadLoc[X] << endl;
			cout << "worldHeadLoc[y] = " << worldHeadLoc[Y] << endl;
			cout << "worldHeadLoc[z] = " << worldHeadLoc[Z] << endl;

			break;
		case 'i':
			infoToggle = !infoToggle;
			break;
		case 'W': 
			xrot += 1;
			glutPostRedisplay();  
			break;
		case 'S': 
			xrot -= 1;
			glutPostRedisplay();  
			break;
		case 'A': 
			yrot -= 1;
			glutPostRedisplay();  
			break;
		case 'D': 
			yrot += 1;
			glutPostRedisplay();  
			break;
		case 'Q': 
			zrot -= 1;
			glutPostRedisplay();  
			break;
		case 'E': 
			zrot += 1;
			glutPostRedisplay();  
			break;
		case '7': 
			worldHeadLoc[Z] += 10;
			glutPostRedisplay(); 
			break;
		case '9': 
			worldHeadLoc[Z] -= 10;
			glutPostRedisplay();
			break;
		case '8': 
			worldHeadLoc[X] += 10;
			glutPostRedisplay();
			break;
		case '5': 
			worldHeadLoc[X] -= 10;
			glutPostRedisplay();
			break;
		case '4': 
			worldHeadLoc[Y] -= 10;
			glutPostRedisplay();
			break;
		case '6': 
			worldHeadLoc[Y] += 10;
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
    


	//grab head positions and convert to cm, offset values based on where the kinect is relative to screen
	worldHeadLoc[X] = skeletonPosition[NUI_SKELETON_POSITION_HEAD].x * 100 + kinectOffsetCm[X];
	worldHeadLoc[Y] = skeletonPosition[NUI_SKELETON_POSITION_HEAD].y * 100 + kinectOffsetCm[Y];
	worldHeadLoc[Z] = skeletonPosition[NUI_SKELETON_POSITION_HEAD].z * 100 + kinectOffsetCm[Z]; 

	if(infoToggle)
	{
		cout << worldHeadLoc[X] << "      " << worldHeadLoc[Y] << "      " << worldHeadLoc[Z] << endl;
	
	}

	
	// Translations
	//glTranslatef (xpos, ypos, zpos);  

	// Rotations
	glRotatef (zrot, 0,0,1);        
	glRotatef (yrot, 0,1,0);
	glRotatef (xrot, 1,0,0);


	//Draw Scene
    spheres();
	scene();	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Functional
	glFrustum((-1 * pixelRatio - worldHeadLoc[X]/1000), (1 * pixelRatio - worldHeadLoc[X]/1000), (-1.0 - worldHeadLoc[Y]/1000), (1.0 - worldHeadLoc[Y]/1000),  1 + worldHeadLoc[Z]/1000, 200000.0);
	gluLookAt(xpos + worldHeadLoc[X]*1, ypos + worldHeadLoc[Y]*1, zpos + worldHeadLoc[Z]*1, 0, 0, 0, 0, 1, 0);
	
	//Beta Improvement
	//glFrustum((worldHeadLoc[X]/(screenWidthCm/2)), (worldHeadLoc[X]) / 100, (-screenHeightCm - worldHeadLoc[Y]) / 100, (screenHeightCm - worldHeadLoc[Y]) / 100, 5.0 + worldHeadLoc[Z]/100, 200000.0); 
	//gluLookAt(xpos + worldHeadLoc[X]/1, ypos + worldHeadLoc[Y]/1, zpos + worldHeadLoc[Z]/1, 0, 0, 0, 0, 1, 0);

	//Half done
	/*
	float nearPlane = worldHeadLoc[Z]/200;
	float farPlane = 100000.0;
	float fov = tan( 30 * PI / 360);
	worldHeadLoc[X] = -worldHeadLoc[X]/(screenWidthCm/2) * .5;
	worldHeadLoc[Y] = -worldHeadLoc[Y]/(screenHeightCm/2) *.25;


    glFrustum(nearPlane * (-fov * pixelRatio + worldHeadLoc[X]),  nearPlane * (fov * pixelRatio + worldHeadLoc[X]), nearPlane * (-fov + worldHeadLoc[Y]), nearPlane * (fov + worldHeadLoc[Y]),  nearPlane, farPlane);
     

    float dis = -screenHeightCm;
    gluLookAt( worldHeadLoc[X] * dis, worldHeadLoc[Y] * dis, 0, worldHeadLoc[X] * dis, worldHeadLoc[Y] * dis, -1, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glTranslatef( 0.0, 0.0, dis);
	*/
	glutSwapBuffers();
    
}

// reshapes window, gives the actual size of the xzy coordinate units
void reshape(int w, int h)
{
	pixelWidth = w;
	pixelHeight = h;
	pixelRatio = (float)w/h;
	glViewport(0,0, w, h);
}


//get it all up and ready at start
void init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
}

//get window size, position, and start the functions to draw it all 
int main(int argc, char** argv)
{
	cout << "Loading Kinect \n";
	initKinect();
	if(!sensor)
	{
		cout << "Kinect not found, proceeding to just render scene\n";
	}
	cout << "Loading Textures and Scene \n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(800, 450);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Kinect Headtracking OpenGL");
	glutDisplayFunc(display);
	glutIdleFunc(getSkeletalData);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	
	init();

	initTexturesBmp("textures\\bay\\bayScene");
	//initTexturesBmp("checkerboard");
	
	//updatePosition(1);
	glutMainLoop();   
}