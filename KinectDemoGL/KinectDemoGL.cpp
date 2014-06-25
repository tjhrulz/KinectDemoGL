#include "stdafx.h"

#include <string.h>
#include <math.h>


#include <Windows.h>
#include <Ole2.h>

//GLUT and OpenGL things
#include <gl/GL.h>
#include <gl/GLU.h>
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

const int BLURFREQUENCY = 0;
const int TEXTURECOUNT = 1;
const double PI = 3.14159265358979323846;
//Collab 4K tv 142.3 cm Width 80.5 Height
//Collab Resolution 3840 x 2160

//Laptop 34.5 cm Width 19.5 Height
//Laptop Resolution 1920 x 1080
//In centimeters
const float screenWidthCm = 60.2/2;
const float screenHeightCm = 34.2/2;
const float kinectOffsetCm[3] = {0,13,0};
const int BMPHEADERSIZE = 54;

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
int textureWidth[TEXTURECOUNT];
int textureHeight[TEXTURECOUNT];
char * textureData[TEXTURECOUNT];
GLuint textureID[TEXTURECOUNT];
int textureToLoad = 0;
int blendsTillTexture = 0;


//Mesh Importer Vars
float* faces_Triangles;
float* faces_Quads;
float* vertex_Buffer;
float* normals;
 
int totalConnectedTriangles;	
int totalConnectedQuads;	
int totalConnectedPoints;
int totalFaces;


//Lighting Vars
GLfloat lightPosition[]    = {0, screenHeightCm, 0, 0.0};

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
bool yzTracking = false;
bool drawCenter = false;
bool kinectToggle = true;
bool fpsToggle = true;
int whichDisplayType = 0;
bool leftRightToggle = true;
bool invertEyes = false;
//Window Vars
int pixelWidth;
int pixelHeight;
float pixelRatio;
//FPS Vars
int frameCount = 0;
int fps = 0;
float currentTime = 0.0;
float previousTime = 0.0;
//3D Display Vars
float eyeDistCm = 1.5;
bool isRightEye = false;
//Debug Vars
float ofTesting = 0;
float ofTesting1 = 1;
float ofTesting2 = 0;

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
					//glutPostRedisplay();
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
		textureWidth[textureToLoad]      = *(int*)&(header[0x12]);
		textureHeight[textureToLoad]     = *(int*)&(header[0x16]);

		textureData[textureToLoad] = new char [imageSize]; //Where the data for each image will be stored allocate for current image size (Jagged Array)
 
		fseek(file, dataPos-BMPHEADERSIZE, SEEK_CUR);
		fread(textureData[textureToLoad],1,imageSize,file); //Load in the data from the image to appropriate array location
 
		fclose(file); //Clean up unneeded resources

		glGenTextures(1, &textureID[textureToLoad]);

 
		textureToLoad++;
		cout << "Texture Number: " << textureToLoad << endl;
	}
	textureToLoad = 0;
}

float* calculateNormal( float *coord1, float *coord2, float *coord3 )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = coord1[0] - coord2[0];
   va[1] = coord1[1] - coord2[1];
   va[2] = coord1[2] - coord2[2];
 
   vb[0] = coord1[0] - coord3[0];
   vb[1] = coord1[1] - coord3[1];
   vb[2] = coord1[2] - coord3[2];
 
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	float norm[3];
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
 
 
	return norm;
}

void initMesh(char* filename, float meshScale)
{
	totalConnectedTriangles = 0; 
	totalConnectedQuads = 0;
	totalConnectedPoints = 0;
 

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
			//sscanf(buffer,"%i%i%i\n", vertex1,vertex2,vertex3 );
			buffer[0] = ' ';
			sscanf(buffer,"%i%i%i", &vertex1,&vertex2,&vertex3 );
			/*vertex1 -= 1;
			vertex2 -= 1;
			vertex3 -= 1;
			*/
			//  vertex == punt van vertex lijst
			// vertex_buffer -> xyz xyz xyz xyz
			//printf("%f %f %f ", Vertex_Buffer[3*vertex1], Vertex_Buffer[3*vertex1+1], Vertex_Buffer[3*vertex1+2]);

			//+((-meshSize[X]/2)-abs(boundingBox[X+MIN]))) possible pos change to get centered with point 0,0,0
			faces_Triangles[triangle_index]   = (vertex_Buffer[3*vertex1+X])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+1] = (vertex_Buffer[3*vertex1+Y])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+2] = (vertex_Buffer[3*vertex1+Z])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+3] = (vertex_Buffer[3*vertex2+X])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+4] = (vertex_Buffer[3*vertex2+Y])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+5] = (vertex_Buffer[3*vertex2+Z])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+6] = (vertex_Buffer[3*vertex3+X])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+7] = (vertex_Buffer[3*vertex3+Y])*(meshScale/meshSize[Y]);
			faces_Triangles[triangle_index+8] = (vertex_Buffer[3*vertex3+Z])*(meshScale/meshSize[Y]);
 
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

 
void drawMesh(float xRot, float yRot, float zRot)
{
	glPushMatrix();	
	//Mesh Light Test
	glRotatef(xRot, 1, 0, 0);
	glRotatef(yRot, 0, 1, 0);
	glRotatef(zRot, 0, 0, 1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	if(isRightEye) //Allow for different collor for each eye for testing
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	}
	else
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	}

	glColor3f(1.0,1.0,1.0);
	glEnableClientState(GL_VERTEX_ARRAY);	
 	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3,GL_FLOAT,	0,faces_Triangles);	
	glNormalPointer(GL_FLOAT, 0, normals);
	glDrawArrays(GL_TRIANGLES, 0, totalConnectedTriangles);	
	glDisableClientState(GL_VERTEX_ARRAY);    
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glPopMatrix();
}


void spheres()
{

	glTranslatef(0,0,-screenHeightCm);
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
    glEnd();
	glBegin(GL_QUADS); //Back
		glColor3f(1.0,1.0,1.0);
		glNormal3d(0, 0, 1);
		glTexCoord2f(1,-0);		glVertex3f( screenWidthCm,-screenHeightCm,-2*screenHeightCm); 
		glTexCoord2f(1,1);		glVertex3f( screenWidthCm, screenHeightCm,-2*screenHeightCm);  
		glTexCoord2f(-0,1);		glVertex3f(-screenWidthCm, screenHeightCm,-2*screenHeightCm);
		glTexCoord2f(-0,-0);	glVertex3f(-screenWidthCm,-screenHeightCm,-2*screenHeightCm);
    glEnd();

	if(blendsTillTexture == BLURFREQUENCY)
	{
		//cout << "Dont do blend " << textureToLoad << endl;
		blendsTillTexture = 0;

		textureToLoad ++;
	}
	else
	{
		glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
		//glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		//glBlendFunc(GL_SRC_COLOR ,GL_ONE_MINUS_SRC_COLOR );
		glBlendFunc(GL_ONE, GL_ONE);

		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, textureWidth[textureToLoad-1], textureHeight[textureToLoad-1], 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, textureData[textureToLoad-1]);
		glBegin(GL_QUADS); //Back
			glColor3f(1.0,1.0,1.0);
			glNormal3d(0, 0, 1);		
			glTexCoord2f(1,-0);		glVertex3f( screenWidthCm,-screenHeightCm,-screenHeightCm+.01); 
			glTexCoord2f(1,1);		glVertex3f( screenWidthCm, screenHeightCm,-screenHeightCm+.01);  
			glTexCoord2f(-0,1);		glVertex3f(-screenWidthCm, screenHeightCm,-screenHeightCm+.01);
			glTexCoord2f(-0,-0);	glVertex3f(-screenWidthCm,-screenHeightCm,-screenHeightCm+.01);
		glEnd();


		//cout << "Do blend " << blendsTillTexture << endl;
		blendsTillTexture ++;
		

	}	

	if(textureToLoad == TEXTURECOUNT)
	{
		textureToLoad = 0;
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void calculateFPS()
{
    //  Increase frame count
    frameCount++;
 
    //  Get the number of milliseconds since glutInit called
    //  (or first call to glutGet(GLUT ELAPSED TIME)).
    currentTime = glutGet(GLUT_ELAPSED_TIME);
 
    //  Calculate time passed
    int timeInterval = currentTime - previousTime;
 
    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);
 
        //  Set time
        previousTime = currentTime;
 
        //  Reset frame count
        frameCount = 0;
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
			eyeDistCm = -eyeDistCm;
			invertEyes = !invertEyes;
			break;
		case 'r':
			glutPostRedisplay();
			break;
		case 't':
			yzTracking = !yzTracking;
			break;
		case 'c':
			drawCenter = !drawCenter;
			break;
		case 'k':
			kinectToggle = !kinectToggle;
			break;
		case 'f':
			fpsToggle = !fpsToggle;
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
			ofTesting += 1;
			glutPostRedisplay(); 
			break;
		case '4': 
			ofTesting -= 1;
			glutPostRedisplay();
			break;
		case '8': 
			ofTesting1 += .1;
			glutPostRedisplay();
			break;
		case '5': 
			ofTesting1 -= .1;
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
		case '+':
			scale *=2;
			break;
		case '-':
			scale /=2;
			break;
		case ' ':
			glutFullScreen();
			break;
		case 127:
			if(whichDisplayType < 2)
			{
				whichDisplayType++;
			}
			else
			{
				whichDisplayType = 0;
			}
			break;

		default :  printf ("   key = %c -> %d\n", key, key);
   }
}

void display()
{
	float nearPlane = .1+ofTesting;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   
	for(int i = 0; i < 2; i++)
	{
		if(sensor)
		{
			if(kinectToggle)
			{
				getSkeletalData();
			}
			//grab head positions and convert to cm, offset values based on where the kinect is relative to screengithub
			worldHeadLoc[X] = (skeletonPosition[NUI_SKELETON_POSITION_HEAD].x/ofTesting1) * 100 + kinectOffsetCm[X];
			worldHeadLoc[Y] = (skeletonPosition[NUI_SKELETON_POSITION_HEAD].y/ofTesting1) * 100 + kinectOffsetCm[Y];
			worldHeadLoc[Z] = (skeletonPosition[NUI_SKELETON_POSITION_HEAD].z/ofTesting1) * 100 + kinectOffsetCm[Z]; 
		}
		else
		{
			worldHeadLoc[X] = kinectOffsetCm[X];
			worldHeadLoc[Y] = kinectOffsetCm[Y];
			worldHeadLoc[Z] = kinectOffsetCm[Z]; 
		}

		/*if(infoToggle)
		{
			cout << worldHeadLoc[X] << "      " << worldHeadLoc[Y] << "      " << worldHeadLoc[Z] << endl;
	
		}*/
	
		if(whichDisplayType == 0)
		{
			glViewport(0,0, pixelWidth, pixelHeight);
			if(isRightEye) //Draw Which Eye
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
			if(isRightEye) //Draw Which Eye
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
			if(isRightEye) //Draw Which Eye
			{
				glViewport(0,0, pixelWidth, pixelHeight/2);
			}
			else
			{
				glViewport(0,pixelHeight/2, pixelWidth, pixelHeight/2);
		
			}			
		}



		glPushMatrix();
			//Draw Scene
			//spheres();
			//scene();	
			drawMesh(90, 0, 0);
		glPopMatrix();


		
		string framerate =  std::to_string(static_cast<long long>(fps));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Functional
		if(isRightEye)
		{
			worldHeadLoc[X] += eyeDistCm;
		}
		else
		{
			worldHeadLoc[X] -= eyeDistCm;
		}
		if(!yzTracking)
		{
			//Setting these two values gets you 1D tracking, just set to near expected or average values (Likely will make later set to last known/average headLoc[Y] and headLoc[Z]
			worldHeadLoc[Y] = 38*ofTesting1;
			worldHeadLoc[Z] = 243*ofTesting1;
		}
		glFrustum(nearPlane*(-screenWidthCm - worldHeadLoc[X]/1)/worldHeadLoc[Z], nearPlane*(screenWidthCm - worldHeadLoc[X]/1)/worldHeadLoc[Z], nearPlane*(-screenHeightCm - worldHeadLoc[Y]/1)/worldHeadLoc[Z], nearPlane*(screenHeightCm - worldHeadLoc[Y]/1)/worldHeadLoc[Z], nearPlane, 200000.0);
		gluLookAt(worldHeadLoc[X], worldHeadLoc[Y], worldHeadLoc[Z], worldHeadLoc[X], worldHeadLoc[Y], 0, 0, 1, 0);
		
		isRightEye = !isRightEye;
	
		if(drawCenter)
		{
			//Test cylinder to show center of "rotation" during headtracking
			glPushMatrix();
				glRotatef(-90,1,0,0);
				glTranslatef(0, 0, -50);
				glColor3f(0.0,0.0,0.0);
				GLUquadricObj *quadObj = gluNewQuadric();
				gluCylinder(quadObj, 1, 1, 4000, 100, 100);
			glPopMatrix();
		}
		if(fpsToggle)
		{
			glColor3f(1.0, 0.0, 0.0);	
			glRasterPos3f(screenWidthCm - (screenWidthCm/8),screenHeightCm-(screenHeightCm/8),0);
			for(int i = 0; i < framerate.length(); i++)
			{

				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, framerate[i]);
			}
		}
		if(leftRightToggle)
		{
			if(i==0)
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
			else
			{
				if(!invertEyes)
				{
					glColor3f(0.0, 0.0, 1.0);	
					glRasterPos3f(-screenWidthCm + (screenWidthCm/8),-screenHeightCm+(screenHeightCm/8),0);
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'l');
			
				}
				else
				{
					glColor3f(1.0, 0.0, 0.0);
					glRasterPos3f(screenWidthCm - (screenWidthCm/8),-screenHeightCm+(screenHeightCm/8),0);
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'r');
				}
			}
		}
		// Translations
		glTranslatef(xpos, ypos, zpos);
	
		// Rotations
		glRotatef(zrot, 0,0,1);        
		glRotatef(yrot, 0,1,0);
		glRotatef(xrot, 1,0,0);

		glScalef(scale, scale, scale);
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

	try
	{
		glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE|GLUT_STEREO);
		glutInitWindowSize(800, 450);
		pixelRatio = (float)800/450;
		glutInitWindowPosition(0, 0);
		glutCreateWindow("Kinect Headtracking OpenGL");
		glutDisplayFunc(display);
		glutIdleFunc(display); //To limit to whenever kinect is ready set to getSkeletonData
		glutReshapeFunc(reshape);
		glutKeyboardFunc(keyboard);
	
		init();
		initTexturesBmp("textures\\bay\\bayScene");
		initMesh("meshes\\xyzrgb_statuette_simplify.ply", 75);
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
		cout << "Stero ready card detected :)" << endl;
		glEnable(GLUT_STEREO);
	}
	else
	{
		cout << "Stereo ready card not detected :(" << endl;
	}

	glutMainLoop();   
}