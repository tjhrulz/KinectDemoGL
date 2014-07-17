#include "stdafx.h"

#include <string.h>
#include <math.h>


#include <Windows.h>
#include <Ole2.h>

//GL extension wrangler things
#include <GL/glew.h>

//GLUT and OpenGL things
//#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/glut.h>
#define GLEW_STATIC
//Kinect things
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>
#include <NuiSkeleton.h>

#include <math.h>
#include <iostream>
#include <string>
using namespace std;

//TODO:
	//Import start vars from file

const int BLURFREQUENCY = 0;	//Set how many partial transitions between each texture update (unfinished)
//How many texture files to load (recently untested)
const double PI = 3.14159265358979323846;	//PI for when needed (currently unused)

//Collab 4K tv 142.3 cm Width 80.5 Height
//Laptop 34.5 cm Width 19.5 Height
//Asus 3D Display 60.2 cm Width 34.2 cm Height

//In centimeters, Active display can now have seperate display demensions from passive
const float screenWidthCmActive = 60.2/2; 
const float screenHeightCmActive = 34.2/2;

const float screenWidthCmPassive = 142.3/2;
const float screenHeightCmPassive = 80.5/2;
//Var to store current used screen demensions
float screenWidthCm = 0.0; //Remember these values should be half of whatever the width or height is
float screenHeightCm = 0.0;

const int BMPHEADERSIZE = 54; //by default this is how large the header in BMPs

//Some consts for better readability of code when using arrays
const int X = 0;
const int Y = 1;
const int Z = 2;

const int MIN = 0;
const int MAX = 3;


// Camera Postition Vars
float xpos = 0.0; 
float ypos = 0.0;
float zpos = 0.0;

float xrot = 0.0;
float yrot = 0.0;
float zrot = 0.0;

float scale = 1.0;

float worldHeadLoc[3];


//Texture Importer Vars
GLuint * textureID2D;

int textureToLoad = 0;
int textureToLoad3D = 0;

bool textureBuffer3Dsupported = false;
GLuint *textureID3D;
char *displayMethod;
int *textureSliceCount;
int number3DTextures;

//Mesh Importer Vars
float* faces_Triangles;
float* faces_Quads;
float* vertex_Buffer;
float* normals;
 
int totalConnectedTriangles;	
int totalConnectedQuads;	
int totalConnectedPoints;
int totalFaces;

float meshxpos = 0;
float meshypos = 0;
float meshzpos = 0;

float meshxrot = 0;
float meshyrot = 0;
float meshzrot = 0;


//Lighting Vars
GLfloat green[] = {0.0, 1.0, 0.0, 1.0}; //Green Color
GLfloat blue[] = {0.0, 0.0, 1.0, 1.0}; //Blue Color
GLfloat orange[] = {1.0, 0.5, 0.0, 1.0}; //Orange Color
GLfloat purple[] = {0.5, 0.0, 0.5, 1.0}; //Purple Color
GLfloat black[] = {0.0, 0.0, 0.0, 1.0}; //Black Color

GLfloat red[] = {1.0, 0.0, 0.0, 1.0}; //Red Color

GLfloat white[] = {1.0, 1.0, 1.0, 1.0}; //White Color

GLfloat ambientLight[]    = {0.2, 0.2, 0.2, 1.0};
GLfloat diffuseLight[]    = {0.5, 0.5, 0.5, 1.0};
GLfloat specularLight[]    = {0.2, 0.2, 0.2, 1.0};

GLfloat lightPosition[] = {0, 0, 0, 0}; //Now set in code so that light updates to current screen (Fix to be same distance from model)


//Kinect Vars
HANDLE depthStream;
INuiSensor* sensor;
Vector4 skeletonPosition[NUI_SKELETON_POSITION_COUNT];
float kinectOffsetCm[3] = {0,0,0};


//Generic Vars

//Bools
bool infoToggle = false;
bool yzTracking = true;
bool drawCenter = false;
bool kinectToggle = true;
bool hudToggle = true;
int whichDisplayType = 0;
bool leftRightToggle = true;
bool invertEyes = false;
bool isFullscreen = false;
bool initCompleted = false;

//Window Vars
int pixelWidth;
int pixelHeight;
float pixelRatio;

//FPS Vars
int frameCount = 0;
int fps = 0;
float previousTime = 0.0;

//3D Display Vars
float eyeDistCm = 8.0/2;
bool isFirstEye = false;

//Debug Vars
float ofTesting = 1;
float ofTesting1 = 0;
float ofTesting2 = 0;
float xMoveAmount = -50;

//Scene Vars
bool drawSceneOne = false;
bool drawSceneTwo = false;
bool drawSceneThree = true;

//Pic chosen Vars
float picDistAbs = 100;
bool invertImages = false;

bool initKinect() 
{
    // Get a working kinect sensor
    int numSensors;
    if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1) 
	{
		return false; //If no sensor return false (No kinect found/ready)
	}
    if (NuiCreateSensorByIndex(0, &sensor) < 0) 
	{
		return false; //If sensor not index return false (No kinect found/ready)
	}
    // Initialize sensor
    sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
    sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,	// Depth camera
        NUI_IMAGE_RESOLUTION_80x60,									    // Image resolution (Low resolution test since it seems to work as well but need to test latency)
        0,																// Image stream flags, e.g. near mode
        0,																// Number of frames to buffer
        NULL,															// Event handle
        &depthStream);
	sensor->NuiSkeletonTrackingEnable(NULL, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT); //Assume person is sitting/Only track upper body
    return true;
}

void getSkeletalData() 
{	
 	if(sensor) //Prevent crashes
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
					//glutPostRedisplay(); //Comment out to get updates as fast as possible instead of synced with the kinect (Also set display as idle function)
					return; // Only take the data for first skeleton
				}
			}
		}
	}
}

void initTexturesBmp(string baseFileName, int textureCount, char loadParams)
{
	if(textureBuffer3Dsupported)
	{
		char * textureData3D;
		int textureWidth3D;
		int textureHeight3D;
		cout << "3D textures supported :)" << endl; 

		char header[BMPHEADERSIZE]; //How large the header should be in a bmp

		long long int imageSize3D = textureCount;
		textureSliceCount[textureToLoad3D] = textureCount;
		displayMethod[textureToLoad3D] = loadParams;


		while(textureToLoad < textureCount)
		{
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

			int dataPos    = *(int*)&(header[0x0A]); //Dont assume headers all same size
			int imageSize2D  = *(int*)&(header[0x22]);
		
			if(textureToLoad == 0)
			{
				imageSize3D *= imageSize2D;
				textureWidth3D = *(int*)&(header[0x12]);
				textureHeight3D = *(int*)&(header[0x16]);

				textureData3D = new char [imageSize3D]; //Where the data for each image will be stored allocate for current image size (Non-Jagged Array assumes images all same size)
			}
 
			fseek(file, dataPos-BMPHEADERSIZE, SEEK_CUR);
			char *textureDataTemp = new char [imageSize2D];
			fread(textureDataTemp,1,imageSize2D,file); //Load in the data from the image to appropriate array location

	//		fread((textureData3D + imageSize2D * textureToLoad),1,imageSize2D,file); //Load in the data from the image to appropriate array location

			//memcpy(textureData3D + sizeof(char *) * textureToLoad, textureDataTemp, sizeof(textureDataTemp)); 
			for(int i = 0; i < imageSize2D; i++)
			{
				textureData3D[i+((long long int)imageSize2D*textureToLoad)] = textureDataTemp[i];
			}
			
			fclose(file); //Clean up unneeded resources
			textureToLoad++;
			cout << "Texture Number: " << textureToLoad << endl;

		}
		

		glBindTexture(GL_TEXTURE_3D, textureID3D[textureToLoad3D]);
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage3D(GL_TEXTURE_3D, 0,GL_RGB, textureWidth3D, textureHeight3D,textureCount,0, GL_BGR_EXT,GL_UNSIGNED_BYTE,textureData3D);
		
		textureToLoad = 0;
		textureToLoad3D++;
	}
	else
	{
		//This code needs reworked it is way to hacky and is done wrong, use the 3D buffers since it uses way less space since everything is managed correctly
		cout << "3D textures not supported :(" << endl; 
		cout << "This code probably doesnt work anymore :(";
		//Load in all the textures
		textureID2D = new GLuint[textureCount];
		while(textureToLoad < textureCount)
		{
			int * textureWidth2D = new int [textureCount];
			int * textureHeight2D = new int [textureCount];
			char ** textureData2D = new char*[textureCount];
			
			char header[BMPHEADERSIZE]; //How large the header should be in a bmp
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
			textureWidth2D[textureToLoad]      = *(int*)&(header[0x12]);
			textureHeight2D[textureToLoad]     = *(int*)&(header[0x16]);

			textureData2D[textureToLoad] = new char [imageSize]; //Where the data for each image will be stored allocate for current image size (Jagged Array)
 
			fseek(file, dataPos-BMPHEADERSIZE, SEEK_CUR);
			fread(textureData2D[textureToLoad],1,imageSize,file); //Load in the data from the image to appropriate array location
			cout << sizeof(textureData2D[textureToLoad]);
			fclose(file); //Clean up unneeded resources
			
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
			glGenTextures(1, &textureID2D[textureToLoad]);
			
			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, textureWidth2D[textureToLoad], textureHeight2D[textureToLoad], 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, textureData2D[textureToLoad]);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
			textureToLoad++;
			cout << "Texture Number: " << textureToLoad << endl;
		}
		textureToLoad = 0;
	}
}

float* calculateNormal( float *coord1, float *coord2, float *coord3 )
{
   //calculate Vector1 and Vector2
   float va[3], vb[3], vr[3], val;
   va[0] = coord1[0] - coord2[0];
   va[1] = coord1[1] - coord2[1];
   va[2] = coord1[2] - coord2[2];
 
   vb[0] = coord1[0] - coord3[0];
   vb[1] = coord1[1] - coord3[1];
   vb[2] = coord1[2] - coord3[2];
 
   //cross product
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   //normalization factor
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	float norm[3];
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
 
 
	return norm;
}

void initMesh(char* filename, float meshScaleX, float meshScaleY, float meshScaleZ, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot)
{
	totalConnectedTriangles = 0; 
	totalConnectedQuads = 0;
	totalConnectedPoints = 0;
	
	meshxpos = xPos;
	meshypos = yPos;
	meshzpos = zPos;
	
	meshxrot = xRot;
	meshyrot = yRot;
	meshzrot = zRot;

 

	FILE* file = fopen(filename,"r");
 
	if (!file)
	{
			//File could not be found
			cout << "Mesh could not be found/opened" << endl;
			system("pause");
			exit(0);
	}
	fseek(file,0,SEEK_END);
	long fileSize = ftell(file);
 

	vertex_Buffer = (float*) malloc (ftell(file));

	fseek(file,0,SEEK_SET); 
 
	faces_Triangles = (float*) malloc(fileSize*sizeof(float));
	normals  = (float*) malloc(fileSize*sizeof(float));

	float boundingBox[6] = {0,0,0,0,0,0};
	float meshSize[3];

	int i = 0;   
	int temp = 0;
	int quads_index = 0;
	int triangle_index = 0;
	int normal_index = 0;
	char buffer[1000];
 
 
	fgets(buffer,300,file);			// ply
 
 
	// READ HEADER
	// -----------------
 
	// Find number of vertexes
	while (  strncmp( "element vertex", buffer,strlen("element vertex")) != 0  )
	{
		fgets(buffer,300,file);			// format
	}
	strcpy(buffer, buffer+strlen("element vertex"));
	sscanf(buffer,"%i", &totalConnectedPoints);
 
 
	// Find number of vertexes
	fseek(file,0,SEEK_SET);
	while (  strncmp( "element face", buffer,strlen("element face")) != 0  )
	{
		fgets(buffer,300,file);			// format
	}
	strcpy(buffer, buffer+strlen("element face"));
	sscanf(buffer,"%i", &totalFaces);
 
 
	// go to end_header
	while (  strncmp( "end_header", buffer,strlen("end_header")) != 0  )
	{
		fgets(buffer,300,file);			// format
	}
 
	//----------------------
 
 
	// read verteces
	i =0;
	for (int iterator = 0; iterator < totalConnectedPoints; iterator++)
	{
		fgets(buffer,300,file);
 
		sscanf(buffer,"%f %f %f", &vertex_Buffer[i+X], &vertex_Buffer[i+Y], &vertex_Buffer[i+Z]);
			
		if(vertex_Buffer[i+X] < boundingBox[X+MIN])
			{
				boundingBox[X+MIN] = vertex_Buffer[i+X];
			}
			if(vertex_Buffer[i+X] > boundingBox[X+MAX])
			{
				boundingBox[X+MAX] = vertex_Buffer[i+X];
			}
			if(vertex_Buffer[i+Y] < boundingBox[Y+MIN])
			{
				boundingBox[Y+MIN] = vertex_Buffer[i+Y];
			}
			if(vertex_Buffer[i+Y] > boundingBox[Y+MAX])
			{
				boundingBox[Y+MAX] = vertex_Buffer[i+Y];
			}
			if(vertex_Buffer[i+Z] < boundingBox[Z+MIN])
			{
				boundingBox[Z+MIN] = vertex_Buffer[i+Z];
			}
			if(vertex_Buffer[i+Z] > boundingBox[Z+MAX])
			{
				boundingBox[Z+MAX] = vertex_Buffer[i+Z];
			}


		i += 3;
	}
 		
	meshSize[X] = (abs(boundingBox[X+MIN]) + abs(boundingBox[X+MAX]));//*(screenWidthCm/screenHeightCm);
	meshSize[Y] = (abs(boundingBox[Y+MIN]) + abs(boundingBox[Y+MAX]));//*(screenWidthCm/screenHeightCm);
	meshSize[Z] = (abs(boundingBox[Z+MIN]) + abs(boundingBox[Z+MAX]));//*(screenWidthCm/screenHeightCm);


	// read faces
	i =0;
	for (int iterator = 0; iterator < totalFaces; iterator++)
	{
		fgets(buffer,300,file);
 
		if (buffer[0] == '3')
		{
 
			int vertex1 = 0, vertex2 = 0, vertex3 = 0;
			buffer[0] = ' ';
			sscanf(buffer,"%i%i%i", &vertex1,&vertex2,&vertex3 );

			//+((-meshSize[X]/2)-abs(boundingBox[X+MIN]))) possible pos change to get centered with point 0,0,0

			faces_Triangles[triangle_index]   = (vertex_Buffer[3*vertex1+X])*(meshScaleX/meshSize[X]);
			faces_Triangles[triangle_index+1] = (vertex_Buffer[3*vertex1+Y])*(meshScaleY/meshSize[Y]);
			faces_Triangles[triangle_index+2] = (vertex_Buffer[3*vertex1+Z])*(meshScaleZ/meshSize[Z]);
			faces_Triangles[triangle_index+3] = (vertex_Buffer[3*vertex2+X])*(meshScaleX/meshSize[X]);
			faces_Triangles[triangle_index+4] = (vertex_Buffer[3*vertex2+Y])*(meshScaleY/meshSize[Y]);
			faces_Triangles[triangle_index+5] = (vertex_Buffer[3*vertex2+Z])*(meshScaleZ/meshSize[Z]);
			faces_Triangles[triangle_index+6] = (vertex_Buffer[3*vertex3+X])*(meshScaleX/meshSize[X]);
			faces_Triangles[triangle_index+7] = (vertex_Buffer[3*vertex3+Y])*(meshScaleY/meshSize[Y]);
			faces_Triangles[triangle_index+8] = (vertex_Buffer[3*vertex3+Z])*(meshScaleZ/meshSize[Z]);
 
			float coord1[3] = {faces_Triangles[triangle_index],	faces_Triangles[triangle_index+1], faces_Triangles[triangle_index+2]};
			float coord2[3] = {faces_Triangles[triangle_index+3],	faces_Triangles[triangle_index+4], faces_Triangles[triangle_index+5]};
			float coord3[3] = {faces_Triangles[triangle_index+6],	faces_Triangles[triangle_index+7], faces_Triangles[triangle_index+8]};
			float *norm = calculateNormal(coord1, coord2, coord3);
 
			normals[normal_index] = norm[0];
			normals[normal_index+1] = norm[1];
			normals[normal_index+2] = norm[2];
			normals[normal_index+3] = norm[0];
			normals[normal_index+4] = norm[1];
			normals[normal_index+5] = norm[2];
			normals[normal_index+6] = norm[0];
			normals[normal_index+7] = norm[1];
			normals[normal_index+8] = norm[2];
 
			normal_index += 9;
 
			triangle_index += 9;
			totalConnectedTriangles += 3;
		}
 
 
		i += 3;
	}
	fclose(file);
}

 
void drawMesh()
{
	glPushMatrix();	

	//Move and rotate object as requested
	glRotatef(meshxrot, 1, 0, 0);
	glRotatef(meshyrot, 0, 1, 0);
	glRotatef(meshzrot, 0, 0, 1);
	glTranslatef(meshxpos, meshypos, meshzpos);	
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	if(isFirstEye) //Allow for different color for each eye for testing
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	}
	else
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	}

	//Draw mesh
	glEnableClientState(GL_VERTEX_ARRAY);	
 	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3,GL_FLOAT,	0,faces_Triangles);	
	glNormalPointer(GL_FLOAT, 0, normals);
	glDrawArrays(GL_TRIANGLES, 0, totalConnectedTriangles);	
	glDisableClientState(GL_VERTEX_ARRAY);    
	glDisableClientState(GL_NORMAL_ARRAY);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glPopMatrix();
}


void spheres()
{
	glPushMatrix();
	glTranslatef(0,0,-screenHeightCm); //Draw blue centered instead of green

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, green);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
	glPushMatrix();
		glColor3f(0.0, 1.0, 0.0); //wanted center of projection green sphere
		glTranslatef (0.0, 0.0, 0.0);
		glutSolidSphere(1*screenHeightCm/8, 25, 25);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blue);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
	glPushMatrix();
		glColor3f(0.0, 0.0, 1.0); //blue
		glTranslatef (0.0, 0.0, screenHeightCm);
		glutSolidSphere(1*screenHeightCm/8, 25, 25);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, orange);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, orange);
	glPushMatrix();
		glColor3f(1.0, 0.5, 0.0); //orange
		glTranslatef (.0, 0.0, -screenHeightCm);
		glutSolidSphere(1*screenHeightCm/8, 25, 25);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, purple);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, purple);
	glPushMatrix();
		glColor3f(0.5, 0.0, 0.5); //purple
		glTranslatef (screenHeightCm, 0.0, 0.0);
		glutSolidSphere(1*screenHeightCm/8, 25, 25);
	glPopMatrix();
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, black);
	glPushMatrix();
		glColor3f(0.0, 0.0, 0.0); //black
		glTranslatef (-screenHeightCm, 0.0, 0.0); 
		glutSolidSphere(1*screenHeightCm/8, 25, 25);
	glPopMatrix();   

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	glPushMatrix();
		glColor3f(1.0, 0.0, 0.0); //red
		glTranslatef (0.0, 0.0, 2* screenHeightCm); 
		glutSolidSphere(1*screenHeightCm/8, 25, 25);
	glPopMatrix();   
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glPopMatrix(); 
}

void texBox()
{
	glPushMatrix();
	glTranslatef(0,0,2*screenHeightCm); //Set to end at screen

	//Temporary fix to the eyes being in the wrong order for images
	if(isFirstEye) //Assumes point given by the kinect is perfectly in between two eyes
	{
		worldHeadLoc[X] -= 2*eyeDistCm;
	}
	else
	{
		worldHeadLoc[X] += 2*eyeDistCm;
	}


	float picInterval = picDistAbs / textureSliceCount[textureToLoad3D];

	if(textureBuffer3Dsupported)
	{
		glEnable(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, textureID3D[textureToLoad3D]);

		float textureSquareLoc = picDistAbs/2;
		if(!invertImages)
		{
			if(worldHeadLoc[X] >= picDistAbs/2)
			{
				textureSquareLoc = picDistAbs / picInterval -1;
			}
			else if (worldHeadLoc[X] <= -picDistAbs/2)
			{
				textureSquareLoc = 0.1;
			}
			else
			{
				textureSquareLoc = ((picDistAbs/2) + worldHeadLoc[X]) / picInterval - 1;
			}
		}
		else
		{
			if(worldHeadLoc[X] >= picDistAbs/2)
			{
				textureSquareLoc = 0.1;
			}
			else if (worldHeadLoc[X] <= -picDistAbs/2)
			{
				textureSquareLoc = picDistAbs / picInterval -1;
			}
			else
			{
				textureSquareLoc = (picDistAbs / picInterval -1) -(((picDistAbs/2) + worldHeadLoc[X]) / picInterval - 1);
			}
		}
		
		//glTexImage3D(GL_TEXTURE_3D, 0,GL_INTENSITY, textureWidth3D, textureHeight3D,TEXTURECOUNT,0, GL_BGR_EXT,GL_UNSIGNED_BYTE,textureData3D);
		
		glBegin(GL_QUADS); //Back
			glColor3f(1.0,1.0,1.0);
			glNormal3d(0, 0, 1);
			if(displayMethod[textureToLoad3D] == 't')
			{
				glTexCoord3f(1,-0,((int)textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1));		glVertex3f( screenWidthCm,-screenHeightCm,-2*screenHeightCm+.01); 
				glTexCoord3f(1,1,((int)textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1));		glVertex3f( screenWidthCm, screenHeightCm,-2*screenHeightCm+.01);  
				glTexCoord3f(0,1,((int)textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1));		glVertex3f(-screenWidthCm, screenHeightCm,-2*screenHeightCm+.01);
				glTexCoord3f(0,-0,((int)textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1));		glVertex3f(-screenWidthCm,-screenHeightCm,-2*screenHeightCm+.01);
			}
			else
			{
				glTexCoord3f(1,-0,textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1);		glVertex3f( screenWidthCm,-screenHeightCm,-2*screenHeightCm+.01); 
				glTexCoord3f(1,1,textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1);		glVertex3f( screenWidthCm, screenHeightCm,-2*screenHeightCm+.01);  
				glTexCoord3f(0,1,textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1);		glVertex3f(-screenWidthCm, screenHeightCm,-2*screenHeightCm+.01);
				glTexCoord3f(0,-0,textureSquareLoc/(float)textureSliceCount[textureToLoad3D]+ofTesting1);		glVertex3f(-screenWidthCm,-screenHeightCm,-2*screenHeightCm+.01);
			}

		glEnd();
		glDisable(GL_TEXTURE_3D);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, textureID2D[textureToLoad]);
		if(!invertImages)
		{
			if(worldHeadLoc[X] >= picDistAbs/2)
			{
				textureToLoad = picDistAbs / picInterval -1;
			}
			else if (worldHeadLoc[X] <= -picDistAbs/2)
			{
				textureToLoad = 0;
			}
			else
			{
				textureToLoad = ((picDistAbs/2) + worldHeadLoc[X]) / picInterval - 1;
			}
		}
		else
		{
			if(worldHeadLoc[X] >= picDistAbs/2)
			{
				textureToLoad = 0;
			}
			else if (worldHeadLoc[X] <= -picDistAbs/2)
			{
				textureToLoad = picDistAbs / picInterval -1;
			}
			else
			{
				textureToLoad = (picDistAbs / picInterval -1) -(((picDistAbs/2) + worldHeadLoc[X]) / picInterval - 1);
			}
		}

		/*
		glBegin(GL_QUADS); //Bottom
			glColor3f(1.0,1.0,1.0);
			glNormal3d(0, 1, 0); 
			glTexCoord2f(5.0,5.0);		glVertex3f( screenWidthCm,-screenHeightCm, screenHeightCm);  
			glTexCoord2f(-5.0,5.0);		glVertex3f( screenWidthCm,-screenHeightCm,-2*screenHeightCm);
			glTexCoord2f(-5.0,-5.0);	glVertex3f(-screenWidthCm,-screenHeightCm,-2*screenHeightCm);
			glTexCoord2f(5.0,-5.0);		glVertex3f(-screenWidthCm,-screenHeightCm, screenHeightCm);
		glEnd();
		glBegin(GL_QUADS);//Left
			glColor3f(1.0,1.0,1.0);
			glNormal3d(1, 0, 0);
			glTexCoord2f(-5.0,-5.0);	glVertex3f(-screenWidthCm,-screenHeightCm,-2*screenHeightCm);
			glTexCoord2f(-5.0,5.0);		glVertex3f(-screenWidthCm, screenHeightCm,-2*screenHeightCm); 
			glTexCoord2f(5.0,5.0);		glVertex3f(-screenWidthCm, screenHeightCm, screenHeightCm);  
			glTexCoord2f(5.0,-5.0);		glVertex3f(-screenWidthCm,-screenHeightCm, screenHeightCm);
		glEnd();
		glBegin(GL_QUADS);//Right
			glColor3f(1.0,1.0,1.0);
			glNormal3d(-1, 0, 0); 
			glTexCoord2f(5.0,-5.0);		glVertex3f(screenWidthCm,-screenHeightCm, screenHeightCm);
			glTexCoord2f(5.0,5.0);		glVertex3f(screenWidthCm, screenHeightCm, screenHeightCm);
			glTexCoord2f(-5.0,5.0);		glVertex3f(screenWidthCm, screenHeightCm,-2*screenHeightCm);
			glTexCoord2f(-5.0,-5.0);	glVertex3f(screenWidthCm,-screenHeightCm,-2*screenHeightCm);
		glEnd();
		glBegin(GL_QUADS);//Top
			glColor3f(1.0,1.0,1.0);
			glNormal3d(0, -1, 0);
			glTexCoord2f(-5.0,5.0);		glVertex3f( screenWidthCm,screenHeightCm,-2*screenHeightCm); 
			glTexCoord2f(5.0,5.0);		glVertex3f( screenWidthCm,screenHeightCm, screenHeightCm);  
			glTexCoord2f(5.0,-5.0);		glVertex3f(-screenWidthCm,screenHeightCm, screenHeightCm);
			glTexCoord2f(-5.0,-5.0);	glVertex3f(-screenWidthCm,screenHeightCm,-2*screenHeightCm);
		glEnd();*/
		glBegin(GL_QUADS); //Back
			glColor3f(1.0,1.0,1.0);
			glNormal3d(0, 0, 1);
			glTexCoord2f(1,-0);		glVertex3f( screenWidthCm,-screenHeightCm,-2*screenHeightCm+.01); 
			glTexCoord2f(1,1);		glVertex3f( screenWidthCm, screenHeightCm,-2*screenHeightCm+.01);  
			glTexCoord2f(-0,1);		glVertex3f(-screenWidthCm, screenHeightCm,-2*screenHeightCm+.01);
			glTexCoord2f(-0,-0);	glVertex3f(-screenWidthCm,-screenHeightCm,-2*screenHeightCm+.01);
		glEnd();

		}	

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

	glPopMatrix(); 
	//Temporary until I reverse the image loader
	if(isFirstEye) //Assumes point given by the kinect is perfectly in between two eyes
	{
		worldHeadLoc[X] += 2*eyeDistCm;
	}
	else
	{
		worldHeadLoc[X] -= 2*eyeDistCm;
	}
}

void calculateFPS()
{
    //Increase times frame has been updated
    frameCount++;
 
    //Get the number of milliseconds since last called
    float currentTime = glutGet(GLUT_ELAPSED_TIME);
 
    //Calculate time passed
    int timeInterval = currentTime - previousTime;
 
	//If a second has passed
    if(timeInterval > 1000)
    {
        //calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);
 
        //Reset for next fps update
        previousTime = currentTime;
        frameCount = 0;
    }
}

void setScreenSize()
{
	//New code for dynamic screen size (call whenever screen type and size needs to be changed)
	
	if(initCompleted) //If not first time pass then increment and set
	{
		if(whichDisplayType < 2)
		{
			whichDisplayType++;
			screenWidthCm = screenWidthCmPassive;
			screenHeightCm = screenHeightCmPassive;
		}
		else
		{
			whichDisplayType = 0;
			screenWidthCm = screenWidthCmActive;
			screenHeightCm = screenHeightCmActive;
		}
	}
	else //If first time pass then just check var and set
	{
		if(whichDisplayType == 0)
		{
			screenWidthCm = screenWidthCmActive;
			screenHeightCm = screenHeightCmActive;
		}
		else
		{
			screenWidthCm = screenWidthCmPassive;
			screenHeightCm = screenHeightCmPassive;
		}
	}

	//Any other variables set at startup but are based off screen size go here

	lightPosition[0] = 10*screenHeightCm;
	lightPosition[1] = 10*screenHeightCm;
	lightPosition[2] = 10*screenHeightCm;
}

void keyboard (unsigned char key, int x, int y) 
{
 
	switch (key) {
		case  27:  
			exit (0);
		//Move object in xyz
		case 'w': 
			zpos += 1;
			glutPostRedisplay();  
			break;
		case 's': 
			zpos -= 1;
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
		//Print out essential vars (needs updated)
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
		//Invert eyes
		case 'i':
			eyeDistCm = -eyeDistCm;
			invertEyes = !invertEyes;
			break;
		case 'o':
			invertImages = !invertImages;
			break;
		//Force redisplay for testing
		case 'r':
			glutPostRedisplay();
			break;
		//Toggle full tracking or just X tracking
		case 't':
			yzTracking = !yzTracking;
			break;
		//Draw an object centered at 0,0,0 and spans across Y
		case 'c':
			drawCenter = !drawCenter;
			break;
		//Stop updating data from kinect
		case 'k':
			kinectToggle = !kinectToggle;
			break;
		//Show HUD (FPS, L, R)
		case 'f':
			hudToggle = !hudToggle;
			break;
		//Rotate object
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
		//Debug buttons
		case '7': 
			ofTesting += .1;
			glutPostRedisplay(); 
			break;
		case '4': 
			ofTesting -= .1;
			glutPostRedisplay();
			break;
		case '8':
			ofTesting1 += .05;
			glutPostRedisplay();
			break;
		case '5': 
			ofTesting1 -= .05;
			glutPostRedisplay();
			break;
		case '9': 
			eyeDistCm += .1;
			glutPostRedisplay();
			break;
		case '6': 
			eyeDistCm -= .1;
			glutPostRedisplay();
			break;
		//Rezero kinect offset position
		case '0':
			kinectOffsetCm[X] = -(skeletonPosition[NUI_SKELETON_POSITION_HEAD].x) * 100;
			kinectOffsetCm[Y] = -(skeletonPosition[NUI_SKELETON_POSITION_HEAD].y) * 100;
			//kinectOffsetCm[Z] = -worldHeadLoc[Z];
			break;
		//Toggle which scene is drawn
		case'1':
			drawSceneOne = !drawSceneOne;
			break;
		case'2':
			drawSceneTwo = !drawSceneTwo;
			break;
		case'3':
			drawSceneThree = !drawSceneThree;
			break;
		//Scale
		case '+':
			scale *=2;
			break;
		case '-':
			scale /=2;
			break;
		//Toggle fullscreen
		case ' ':
			if(isFullscreen == true)
			{
				glutReshapeWindow(800, 450);
				glutPositionWindow(50,50);
				pixelRatio = (float)800/450;
				isFullscreen = false;
			}
			else
			{
				glutFullScreen();
				isFullscreen = true;
			}
			break;
		case 8:
			if(textureToLoad3D >= number3DTextures - 1)
			{
				textureToLoad3D = 0;
			}
			else
			{
				cout << sizeof(textureID3D);
				textureToLoad3D ++;
			}
			break;
		//Update screen type and size
		case 127:
			setScreenSize();
			break;
		case 39:
			if (xMoveAmount <= 50)
			{
				xMoveAmount += .25;
			}
			else
			{
				xMoveAmount = -50;
			}
			glutPostRedisplay();
			break;

		default :  printf ("   key = %c -> %d\n", key, key);
   }
}

void display()
{
	float nearPlane = .1;
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   
	glEnable(GL_NORMALIZE); //Fixes scale and some other issues
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Clear for when not using back left/right buffers
	for(int i = 0; i < 2; i++)
	{
		if(sensor)
		{
			if(kinectToggle)
			{
				getSkeletalData();
			}
			//grab head positions and convert to cm, offset values based on where the kinect is relative to screen
			worldHeadLoc[X] = (skeletonPosition[NUI_SKELETON_POSITION_HEAD].x) * 100 + kinectOffsetCm[X];
			worldHeadLoc[Y] = (skeletonPosition[NUI_SKELETON_POSITION_HEAD].y) * 100 + kinectOffsetCm[Y];
			worldHeadLoc[Z] = (skeletonPosition[NUI_SKELETON_POSITION_HEAD].z) * 100 + kinectOffsetCm[Z]; 
		}
		else
		{
			worldHeadLoc[X] = xMoveAmount;
			worldHeadLoc[Y] = 0;
			worldHeadLoc[Z] = 150; 
		}

		/*if(infoToggle) //spit out info every update (uneeded
		{
			cout << worldHeadLoc[X] << "      " << worldHeadLoc[Y] << "      " << worldHeadLoc[Z] << endl;
	
		}*/
	
		if(whichDisplayType == 0)
		{
			glViewport(0,0, pixelWidth, pixelHeight);
			if(isFirstEye) //Draw Which Eye
			{
				glDrawBuffer(GL_BACK_LEFT);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			}
			else //Draw Other Eye
			{
				glDrawBuffer(GL_BACK_RIGHT);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			}
		}
		else if (whichDisplayType == 1)
		{
			glDrawBuffer(GL_BACK);
			if(isFirstEye) //Draw Which Eye
			{
				glViewport(0,0, pixelWidth/2, pixelHeight);
			}
			else
			{
				glViewport(pixelWidth/2,0, pixelWidth/2, pixelHeight);
		
			}
		}
		else if (whichDisplayType == 2)
		{
			glDrawBuffer(GL_BACK);
			if(isFirstEye) //Draw Which Eye
			{
				glViewport(0,0, pixelWidth, pixelHeight/2);
			}
			else
			{
				glViewport(0,pixelHeight/2, pixelWidth, pixelHeight/2);
		
			}			
		}

		if(isFirstEye) //Assumes point given by the kinect is perfectly in between two eyes
		{
			worldHeadLoc[X] += eyeDistCm;
		}
		else
		{
			worldHeadLoc[X] -= eyeDistCm;
		}
		if(!yzTracking)
		{
			//Setting these two values gets you 1D tracking, just set to near expected or average values (Likely will make later set to zeroed headLoc[Y] and headLoc[Z]
			worldHeadLoc[Y] = 0;
			worldHeadLoc[Z] = 150;
		}

		glPushMatrix();
		//Draw Scenes

		// Translations
		glTranslatef(xpos, ypos, zpos);
	
		// Rotations
		glRotatef(zrot, 0,0,1);        
		glRotatef(yrot, 0,1,0);
		glRotatef(xrot, 1,0,0);

		glScalef(scale, scale, scale);
		if(drawSceneOne) //This scene does not move when move buttons pressed
		{
			texBox();	
		}
		if(drawSceneTwo)
		{
			spheres();
		}
		if(drawSceneThree)
		{
			drawMesh();
		}

		glPopMatrix();

		
		
		string framerate =  std::to_string(static_cast<long long>(fps));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//The meat of the whole thing
		glFrustum(nearPlane*(-screenWidthCm - worldHeadLoc[X])/worldHeadLoc[Z], nearPlane*(screenWidthCm - worldHeadLoc[X])/worldHeadLoc[Z], nearPlane*(-screenHeightCm - worldHeadLoc[Y])/worldHeadLoc[Z], nearPlane*(screenHeightCm - worldHeadLoc[Y])/worldHeadLoc[Z], nearPlane, 200000.0);
		gluLookAt(worldHeadLoc[X], worldHeadLoc[Y], worldHeadLoc[Z], worldHeadLoc[X], worldHeadLoc[Y], 0, 0, 1, 0);
		// /meat 

		//Crappy version of meat (What the images currently use since simpler to render)
		//glFrustum(-1.0*pixelWidth/pixelHeight, 1.0*pixelWidth/pixelHeight, -1.0, 1.0, 5.0, 2000.0);
 		//xrot = -((asin(worldHeadLoc[X]/(worldHeadLoc[Z])) * 180) / PI);
		//glRotatef (xrot, 0.0,0.0,1.0);
		//gluLookAt(0,0,100,0,0,0,0,1,0);

		//Attempt to get somewhere in between without using transformations (Specifically frustum based transformations)
		//glFrustum(-1.0*pixelWidth/pixelHeight, 1.0*pixelWidth/pixelHeight, -1.0, 1.0, 5.0, 2000.0);
		//glTranslatef(worldHeadLoc[X], worldHeadLoc[Y], -worldHeadLoc[Z]);
		//gluLookAt(-worldHeadLoc[X], 0, 100, 0, 0, 0, 0, 1, 0);

		if(drawCenter)
		{
			//Test cylinder to show center of "rotation" during headtracking
			glPushMatrix();
				glRotatef(-90,1,0,0);
				glTranslatef(0, 0, -50);
				glColor3f(1.0,1.0,1.0);
				GLUquadricObj *quadObj = gluNewQuadric();
				gluCylinder(quadObj, 1, 1, 4000, 100, 100);
			glPopMatrix();
		}
		if(hudToggle)
		{
			glColor3f(1.0, 0.0, 0.0);	
			glRasterPos3f(screenWidthCm - (screenWidthCm/8),screenHeightCm-(screenHeightCm/8),0);
			for(int i = 0; i < framerate.length(); i++)
			{

				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, framerate[i]);
			}
			if(leftRightToggle)
			{
				if(isFirstEye)
				{
					if(!invertEyes)
					{
						glColor3f(0.0, 0.0, 1.0);	
						glRasterPos3f(-screenWidthCm + (screenWidthCm/8),-screenHeightCm+(screenHeightCm/8),0);
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'l');			
					}
					else
					{	
						glRasterPos3f(screenWidthCm - (screenWidthCm/8),-screenHeightCm+(screenHeightCm/8),0);
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'r');
					}

				}
				else
				{
					if(!invertEyes)
					{
						glColor3f(1.0, 0.0, 0.0);	
						glRasterPos3f(screenWidthCm - (screenWidthCm/8),-screenHeightCm+(screenHeightCm/8),0);
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'r');			
					}
					else
					{
						glColor3f(0.0, 0.0, 1.0);	
						glRasterPos3f(-screenWidthCm + (screenWidthCm/8),-screenHeightCm+(screenHeightCm/8),0);
						glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'l');

					}
				}
			}
		}
		isFirstEye = !isFirstEye;
	}
	glutSwapBuffers();
    calculateFPS();
}

// reshapes window, gives the actual size of the xzy coordinate units
void reshape(int w, int h)
{
	pixelWidth = w;
	pixelHeight = h;
	pixelRatio = (float)w/h;
}

bool test = true;
//get it all up and ready at start
void init(int numTextures)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0); //Background is better black than white to reduce ghosting

	glewInit();

	if(glewIsSupported("GL_VERSION_1_4"))
	{
		textureBuffer3Dsupported = true;
	}

	textureID3D = new GLuint[numTextures];
	textureSliceCount = new int[numTextures];
	displayMethod = new char[numTextures];

	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(numTextures, textureID3D);

	cout << "Loading Textures" << endl;	
	initTexturesBmp("textures\\v2\\v2", 79, 'n');
	initTexturesBmp("textures\\v2\\v2", 79, 't');
	initTexturesBmp("textures\\bunny\\bunny", 119, 'n');
	initTexturesBmp("textures\\bunny\\bunny", 119, 't');
	initTexturesBmp("textures\\bunny_reduced\\bunny", 60, 'n');
	initTexturesBmp("textures\\bunny_reduced\\bunny", 60, 't');
	number3DTextures = numTextures;

	//initTexturesBmp("textures\\v2_xpos\\v2_xpos", 340);
	//initMesh("meshes\\xyzrgb_statuette_simplify.ply", 9, 15.5, 8, 5, -5.35, 17, 0, 0, 0);

	cout << "Loading Mesh" << endl;
	initMesh("meshes\\xyzrgb_statuette_simplify.ply", 30, 60, 30, 0, 30, 0, 90, 0, 0);

	textureToLoad3D = 0;
}


//get window size, position, and start the functions to draw it all 
int main(int argc, char** argv)
{
	setScreenSize();
	

	cout << "Loading Kinect \n";
	initKinect();
	if(!sensor)
	{
		cout << "Kinect not found, proceeding to just render scene\n";
		eyeDistCm = 0;
		hudToggle = false;
	}
	glutInit(&argc, argv);

	try
	{
		glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE|GLUT_STEREO);
		glutInitWindowSize(800, 450);
		pixelRatio = (float)800/450;
		glutInitWindowPosition(50, 50);
		glutCreateWindow("Kinect Headtracking OpenGL");
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);

		//To limit to when kinect is ready set to getSkeletalData and uncomment out glutPostRedisplay in get getSkeletalData 
			//if you wanna be clean comment out getSkeletalData from display as well
		//otherwise comment glutPost and set this to display. 
		//At some point I will also try to get glutTimerFunc working again
		glutIdleFunc(display);
				
		
		init(6);

	}
	catch(exception e)
	{
		cout << e.what();
		system("pause");
		exit(0);
	}
	
	GLboolean stereo = 0;

	glGetBooleanv(GL_STEREO, &stereo);

	if (stereo)
	{
		cout << "Stero supported :)" << endl;
		glEnable(GLUT_STEREO);
	}
	else
	{
		cout << "Stereo not supported :(" << endl;
	}
	cout << "current renderer being used is: " << glGetString(GL_RENDERER);
	initCompleted = true;
	glutMainLoop();   
}