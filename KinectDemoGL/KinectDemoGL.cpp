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



GLUquadric* qobj;
// Camera Postitions, and tons upon tons of vertices. 
float  xpos = 0.0, ypos = 0.0, xrot = 0.0, yrot = 0.0, zrot = 0.0, scal = 3.0, zpos=0.0;


int counter=0;



void DrawPart1()
{
     /*
   glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, -1.0, -6.0);
    glVertex3f(-3.5, 4.5, -6.0);
    glVertex3f(2.0, 4.5, -6.0);
    glVertex3f(2.0, -1.0, -6.0);
    glVertex3f(-2.0, -1.0, -6.0);
    glEnd();
    glColor3f(0.3, 0.3, 0.3);
     glBegin(GL_POLYGON);
    glVertex3f(-3.5, -1.0, -8.0);
    glVertex3f(-3.5, 4.5, -8.0);
    glVertex3f(2.0, 4.5, -8.0);
    glVertex3f(2.0, -1.0, -8.0);
    glVertex3f(-3.5, -1.0, -8.0);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
     glBegin(GL_POLYGON);
    glVertex3f(-3.5, -1.0, -8.0);
    glVertex3f(-3.5, -1.0, -6.0);
    glVertex3f(-3.5, 4.5, -6.0);
    glVertex3f(-3.5, 4.5, -8.0);
    glVertex3f(-3.5, -1.0, -8.0);
    glEnd();
    glColor3f(0.5, 0.5, 0.5);
     glBegin(GL_POLYGON);
    glVertex3f(2.0, -1.0, -6.0);
    glVertex3f(2.0, -1.0, -8.0);
    glVertex3f(2.0, 4.5, -8.0);
    glVertex3f(2.0, 4.5, -6.0);
    glVertex3f(2.0, -1.0, -6.0);
    glEnd();
    glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5,4.5,-6.0);
    glVertex3f(-3.5,4.5,-8.0);
    glVertex3f(2.0,4.5,-8.0);
    glVertex3f(2.0,4.5,-6.0);
    glEnd();
    //end cube1
    
    //begin cube2
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, -1.0, -6.0);
    glVertex3f(-3.5, 1.25, -6.0);
    glVertex3f(1.5, 1.25, -6.0);
    glVertex3f(1.5, -1.0, -6.0);
    glVertex3f(-3.5, -1.0, -6.0);
    glEnd();
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_POLYGON);
    glVertex3f(-3.75, -1.0, -6.0);
    glVertex3f(-3.75, -1.0, 2.5);
    glVertex3f(-3.75, 1.25, 2.5);
    glVertex3f(-3.75, 1.25, -6.0);
    glVertex3f(-3.75, -1.0, -6.0);
    glEnd();
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(1.75, -1.0, -6.0);
    glVertex3f(1.75, -1.0, 2.5);
    glVertex3f(1.75, 1.25, 2.5);
    glVertex3f(1.75, 1.25, -6.0);
    glVertex3f(1.75, -1.0, -6.0);
    glEnd();
     glColor3f(0.55, 0.55, 0.55);
    glBegin(GL_POLYGON);
    glVertex3f(-3.75, 1.25, -6.0);
    glVertex3f(-3.75, 1.25, 2.5);
    glVertex3f(1.75, 1.25, 2.5);
    glVertex3f(1.75, 1.25, -6.0);
    glVertex3f(-3.75, 1.25, -6.0);
    glEnd();
     glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(-3.75, -1.0, 2.5);
    glVertex3f(1.75, -1.0, 2.5);
    glVertex3f(1.75, 1.25, 2.5);
    glVertex3f(-3.75, 1.25, 2.5);
    glVertex3f(-3.75, -1.0, 2.5);
    glEnd();
    //end cube2
    
     //begin cube3
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 1.25, -6.0);
    glVertex3f(-3.5, 1.75, -6.0);
    glVertex3f(1.5, 1.75, -6.0);
    glVertex3f(1.5, 1.25, -6.0);
    glVertex3f(-3.5, 1.25, -6.0);
    glEnd();
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 1.25, -6.0);
    glVertex3f(-3.5, 1.25, 1.75);
    glVertex3f(-3.5, 1.75, 1.75);
    glVertex3f(-3.5, 1.75, -6.0);
    glVertex3f(-3.5, 1.25, -6.0);
    glEnd();
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 1.25, -6.0);
    glVertex3f(1.5, 1.25, 1.75);
    glVertex3f(1.5, 1.75, 1.75);
    glVertex3f(1.5, 1.75, -6.0);
    glVertex3f(1.5, 1.25, -6.0);
    glEnd();
     glColor3f(0.55, 0.55, 0.55);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 1.75, -6.0);
    glVertex3f(-3.5, 1.75, 1.75);
    glVertex3f(1.5, 1.75, 1.75);
    glVertex3f(1.5, 1.75, -6.0);
    glVertex3f(-3.5, 1.75, -6.0);
    glEnd();
     glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 1.25, 1.75);
    glVertex3f(1.5, 1.25, 1.75);
    glVertex3f(1.5, 1.75, 1.75);
    glVertex3f(-3.5, 1.75, 1.75);
    glVertex3f(-3.5, 1.25, 1.75);
    glEnd();
    //end cube3
    //extra cube
     glBegin(GL_POLYGON);
    glVertex3f(-3.5, 1.75, -6.0);
    glVertex3f(-3.5, 2.5, -6.0);
    glVertex3f(1.5, 2.5, -6.0);
    glVertex3f(1.5, 1.75, -6.0);
    glVertex3f(-3.5, 1.75, -6.0);
    glEnd();
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 1.75, -6.0);
    glVertex3f(-3.5, 1.75, -5.5);
    glVertex3f(-3.5, 2.5, -5.5);
    glVertex3f(-3.5, 2.5, -6.0);
    glVertex3f(-3.5, 1.75, -6.0);
    glEnd();
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 1.75, -6.0);
    glVertex3f(1.5, 1.75, -5.5);
    glVertex3f(1.5, 2.5, -5.5);
    glVertex3f(1.5, 2.5, -6.0);
    glVertex3f(1.5, 1.75, -6.0);
    glEnd();
     glColor3f(0.55, 0.55, 0.55);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 2.5, -6.0);
    glVertex3f(-3.5, 2.5, -5.5);
    glVertex3f(1.5, 2.5, -5.5);
    glVertex3f(1.5, 2.5, -6.0);
    glVertex3f(-3.5, 2.5, -6.0);
    glEnd();
     glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 1.75, -5.5);
    glVertex3f(1.5, 1.75, -5.5);
    glVertex3f(1.5, 2.5, -5.5);
    glVertex3f(-3.5, 2.5, -5.5);
    glVertex3f(-3.5, 1.75, -5.5);
    glEnd();
    //extra cube
    //begin cube4
   glBegin(GL_POLYGON);
    glVertex3f(-3.0, 1.25, -6.0);
    glVertex3f(-3.0, 2.5, -6.0);
    glVertex3f(1.0, 2.5, -6.0);
    glVertex3f(1.0, 1.25, -6.0);
    glVertex3f(-3.0, 1.25, -6.0);
    glEnd();
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_POLYGON);
    glVertex3f(-3.0, 1.25, -6.0);
    glVertex3f(-3.0, 1.25, 1.5);
    glVertex3f(-3.0, 2.5, 1.5);
    glVertex3f(-3.0, 2.5, -6.0);
    glVertex3f(-3.0, 1.25, -6.0);
    glEnd();
    glColor3f(0.45, 0.45, 0.45);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 1.25, -6.0);
    glVertex3f(1.0, 1.25, 1.5);
    glVertex3f(1.0, 2.5, 1.5);
    glVertex3f(1.0, 2.5, -6.0);
    glVertex3f(1.0, 1.25, -6.0);
    glEnd();
     glColor3f(1.6, 0.6, 0.6);
    glBegin(GL_POLYGON);
    glVertex3f(-3.0, 2.5, -6.0);
    glVertex3f(-3.0, 2.5, 1.5);
    glVertex3f(1.0, 2.5, 1.5);
    glVertex3f(1.0, 2.5, -6.0);
    glVertex3f(-3.0, 2.5, -6.0);
    glEnd();
     glColor3f(0.375, 0.375, 0.375);
    glBegin(GL_POLYGON);
    glVertex3f(-3.0, 1.25, 1.5);
    glVertex3f(1.0, 1.25, 1.5);
    glVertex3f(1.0, 2.5, 1.5);
    glVertex3f(-3.0, 2.5, 1.5);
    glVertex3f(-3.0, 2.5, 1.5);
    glEnd();
    //end cube4
    
    //begin cube5
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 2.5, -6.0);
    glVertex3f(-3.5, 3.0, -6.0);
    glVertex3f(1.5, 3.0, -6.0);
    glVertex3f(1.5, 2.5, -6.0);
    glVertex3f(-3.5, 2.5, -6.0);
    glEnd();
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 2.5, -6.0);
    glVertex3f(-3.5, 2.5, 1.75);
    glVertex3f(-3.5, 3.0, 1.75);
    glVertex3f(-3.5, 3.0, -6.0);
    glVertex3f(-3.5, 2.5, -6.0);
    glEnd();
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 2.5, -6.0);
    glVertex3f(1.5, 2.5, 1.75);
    glVertex3f(1.5, 3.0, 1.75);
    glVertex3f(1.5, 3.0, -6.0);
    glVertex3f(1.5, 2.5, -6.0);
    glEnd();
     glColor3f(0.55, 0.55, 0.55);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 3.0, -6.0);
    glVertex3f(-3.5, 3.0, 1.75);
    glVertex3f(1.5, 3.0, 1.75);
    glVertex3f(1.5, 3.0, -6.0);
    glVertex3f(-3.5, 3.0, -6.0);
    glEnd();
     glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(-3.5, 2.5, 1.75);
    glVertex3f(1.5, 2.5, 1.75);
    glVertex3f(1.5, 3.0, 1.75);
    glVertex3f(-3.5, 3.0, 1.75);
    glVertex3f(-3.5, 3.0, 1.75);
    glEnd();
    //end cube5
    
    //begin cube6
     glBegin(GL_POLYGON);
    glVertex3f(-1.5, -1.0, -6.0);
    glVertex3f(-1.5, 3.35, -6.0);
    glVertex3f(-0.5, 3.35, -6.0);
    glVertex3f(-0.5, -1.0, -6.0);
    glVertex3f(-1.5, -1.0, -6.0);
    glEnd();
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_POLYGON);
    glVertex3f(-1.75, -1.0, -6.0);
    glVertex3f(-1.75, -1.0, 2.25);
    glVertex3f(-1.75, 3.35, 2.25);
    glVertex3f(-1.75, 3.35, -6.0);
    glVertex3f(-1.75, -1.0, -6.0);
    glEnd();
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(-0.25, -1.0, -6.0);
    glVertex3f(-0.25, -1.0, 2.25);
    glVertex3f(-0.25, 3.35, 2.25);
    glVertex3f(-0.25, 3.35, -6.0);
    glVertex3f(-0.25, -1.0, -6.0);
    glEnd();
     glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_POLYGON);
    glVertex3f(-1.75, 3.35, -6.0);
    glVertex3f(-1.75, 3.35, 2.25);
    glVertex3f(-0.25, 3.35, 2.25);
    glVertex3f(-0.25, 3.35, -6.0);
    glVertex3f(-1.75, 3.35, -6.0);
    glEnd();
     glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(-1.75, -1.0, 2.25);
    glVertex3f(-0.25, -1.0, 2.25);
    glVertex3f(-0.25, 3.35, 2.25);
    glVertex3f(-1.75, 3.35, 2.25);
    glVertex3f(-1.75, -1.0, 2.25);
    glEnd();
    //end cube6
    // end WOPR
    */
    // begin Table
    //begin cube7
   glBegin(GL_POLYGON);
   glColor3f(0.6, 0.6, 0.6);
    glVertex3f(0.5, 1.50, 4.0);
    glVertex3f(0.5, 1.75, 4.0);
    glVertex3f(7.5, 1.75, 4.0);
    glVertex3f(7.5, 1.50, 4.0);
    glVertex3f(0.5, 1.50, 4.0);
    glEnd();
    glColor3f(0.55, 0.55, 0.55);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 1.50, 4.0);
    glVertex3f(0.5, 1.50, 11.75);
    glVertex3f(0.5, 1.75, 11.75);
    glVertex3f(0.5, 1.75, 4.0);
    glVertex3f(0.5, 1.50, 11.75);
    glEnd();
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_POLYGON);
    glVertex3f(7.5, 1.50, 11.75);
    glVertex3f(7.5, 1.50, 11.75);
    glVertex3f(7.5, 1.75, 11.75);
    glVertex3f(7.5, 1.75, 4.0);
    glVertex3f(7.5, 1.50, 4.0);
    glEnd();
     glColor3f(0.75, 0.75, 0.75);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 1.75, 4.0);
    glVertex3f(0.5, 1.75, 11.75);
    glVertex3f(7.5, 1.75, 11.75);
    glVertex3f(7.5, 1.75, 4.0);
    glVertex3f(0.5, 1.75, 4.0);
    glEnd();
     glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 1.50, 11.75);
    glVertex3f(7.5, 1.50, 11.75);
    glVertex3f(7.5, 1.75, 11.75);
    glVertex3f(0.5, 1.75, 11.75);
    glVertex3f(0.5, 1.50, 11.75);
    glEnd();
    //end cube7
    //begin cube8
  
    //end cube9
    
    //begin cube10
   
    //end cube10
    
    //begin cube11
 glEnd();
    //end cube11
    // end table
    
    //begin about 10 more cubes on top of table... 
   
    //begin cube12
     glColor3f(0.9, 0.9, 0.80);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 1.76, 4.0);
    glVertex3f(1.0, 1.76, 6.5);
    glVertex3f(3.0, 1.76, 6.5);
    glVertex3f(3.0, 1.76, 4.0);
    glVertex3f(1.0, 1.76, 4.0);
    glEnd();
      glColor3f(0.6, 0.6, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 1.76, 4.0);
    glVertex3f(1.0, 1.76, 6.5);
    glVertex3f(1.0, 2.4, 6.5);
    glVertex3f(1.0, 2.4, 4.0);
    glVertex3f(1.0, 1.76, 4.0);
    glEnd();
     glColor3f(0.075, 0.075, 0.075);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 1.76, 6.5);
    glVertex3f(3.0, 1.76, 6.5);
    glVertex3f(3.0, 2.4, 6.5);
    glVertex3f(1.0, 2.4, 6.5);
    glVertex3f(1.0, 1.76, 6.5);
    glEnd();
     glColor3f(0.09, 0.09, 0.09);
    glBegin(GL_POLYGON);
    glVertex3f(3.0, 1.76, 6.5);
    glVertex3f(3.0, 1.76, 4.0);
    glVertex3f(3.0, 2.4, 4.0);
    glVertex3f(3.0, 2.4, 6.5);
    glVertex3f(3.0, 1.76, 6.5);
    glEnd();
   glColor3f(0.075, 0.075, 0.075);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 1.76, 4.0);
    glVertex3f(3.0, 1.76, 4.0);
    glVertex3f(3.0, 2.4, 4.0);
    glVertex3f(1.0, 2.4, 4.0);
    glVertex3f(1.0, 1.76, 4.0);
    glEnd();
   
    
    //end cube12
   //begin cube13
    glColor3f(0.9, 0.9, 0.80);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 2.4, 4.0);
    glVertex3f(1.0, 2.4, 6.5);
    glVertex3f(3.0, 2.4, 6.5);
    glVertex3f(3.0, 2.4, 4.0);
    glVertex3f(1.0, 2.4, 4.0);
    glEnd();
      glColor3f(0.6, 0.6, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 2.4, 4.0);
    glVertex3f(1.0, 2.4, 6.5);
    glVertex3f(1.0, 4.4, 6.5);
    glVertex3f(1.0, 4.4, 4.0);
    glVertex3f(1.0, 2.4, 4.0);
    glEnd();
     glColor3f(0.80, 0.80, 0.75);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 2.4, 6.5);
    glVertex3f(3.0, 2.4, 6.5);
    glVertex3f(3.0, 4.4, 6.5);
    glVertex3f(1.0, 4.4, 6.5);
    glVertex3f(1.0, 1.76, 6.5);
    glEnd();
     glColor3f(0.25, 0.25, 0.25);
    glBegin(GL_POLYGON);
    glVertex3f(3.0, 2.4, 6.5);
    glVertex3f(3.0, 2.4, 4.0);
    glVertex3f(3.0, 4.4, 4.0);
    glVertex3f(3.0, 4.4, 6.5);
    glVertex3f(3.0, 2.4, 6.5);
    glEnd();
   glColor3f(0.075, 0.075, 0.075);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 2.4, 4.0);
    glVertex3f(3.0, 2.4, 4.0);
    glVertex3f(3.0, 4.4, 4.0);
    glVertex3f(1.0, 4.4, 4.0);
    glVertex3f(1.0, 2.4, 4.0);
    glEnd();
    glColor3f(0.75, 0.75, 0.70);
    glBegin(GL_POLYGON);
    glVertex3f(1.0, 4.4, 4.0);
    glVertex3f(1.0, 4.4, 6.5);
    glVertex3f(3.0, 4.4, 6.5);
    glVertex3f(3.0, 4.4, 4.0);
    glVertex3f(1.0, 4.4, 4.0);
    glEnd();
    
    // Sorry about this not actually fitting on the computer the whole way. 
    // Last thing I did, couldn't figure out how to get it to fit right. 
   //TEXT? 
   
   char *s1 = "SHALL WE PLAY A GAME?";
   char *s2 = "Love to. How about Global";
   char *s3 = "Thermonuclear War?";
   char *s4 = "WOUDN'NT YOU PREFER A NICE GAME";
   char *s5 = "OF MARBLES?";
   glRasterPos3f(5.0, 4.2, 6.3);
   int count=0;
   for(count=0; count <= strlen(s1); count++)
   {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s1[count]);        
   }
   
   glRasterPos3f(5.0, 4.0, 6.3);
    count=0;
   for(count=0; count <= strlen(s2); count++)
   {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s2[count]);        
   }
   
   glRasterPos3f(5.0, 3.8, 6.3);
    count=0;
   for(count=0; count <= strlen(s3); count++)
   {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s3[count]);        
   }
   
   glRasterPos3f(5.0, 3.6, 6.3);
    count=0;
   for(count=0; count <= strlen(s4); count++)
   {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s4[count]);        
   }
   
   glRasterPos3f(5.0, 3.4, 6.3);
    count=0;
   for(count=0; count <= strlen(s5); count++)
   {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, s5[count]);        
   }
   //TEXT
   
   
   //end cube13 
   //begin cube14
     glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(3.01, 4.2, 6.5);
    glVertex3f(3.01, 4.2, 4.0);
    glVertex3f(3.01, 4.4, 4.0);
    glVertex3f(3.01, 4.4, 6.5);
    glVertex3f(3.01, 4.2, 6.5);
    glEnd();
     glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(3.01, 2.4, 6.5);
    glVertex3f(3.01, 2.4, 4.0);
    glVertex3f(3.01, 2.6, 4.0);
    glVertex3f(3.01, 2.6, 6.5);
    glVertex3f(3.01, 2.4, 6.5);
    glEnd();
     glBegin(GL_POLYGON);
    glVertex3f(3.01, 2.4, 6.5);
    glVertex3f(3.01, 2.4, 6.3);
    glVertex3f(3.01, 4.4, 6.3);
    glVertex3f(3.01, 4.4, 6.5);
    glVertex3f(3.01, 2.4, 6.5);
    glEnd();
     glBegin(GL_POLYGON);
    glVertex3f(3.01, 2.4, 4.2);
    glVertex3f(3.01, 2.4, 4.0);
    glVertex3f(3.01, 4.4, 4.0);
    glVertex3f(3.01, 4.4, 4.2);
    glVertex3f(3.01, 2.4, 4.2);
    glEnd();
   //end cube14 
   
   
   //begin cube15 (keyboard)
    glColor3f(0.9, 0.9, 0.80);
    glBegin(GL_POLYGON);
    glVertex3f(5.0, 1.76, 4.5);
    glVertex3f(5.0, 1.76, 8.0);
    glVertex3f(7.0, 1.76, 8.0);
    glVertex3f(7.0, 1.76, 4.5);
    glVertex3f(5.0, 1.76, 4.5);
    glEnd();
    glColor3f(0.45, 0.45, 0.45);
    glBegin(GL_POLYGON);
    glVertex3f(5.20, 2.25, 4.6);
    glVertex3f(5.20, 2.25, 7.9);
    glVertex3f(6.75, 1.95, 7.9);
    glVertex3f(6.75, 1.95, 4.6);
    glVertex3f(5.20, 2.25, 4.6);
    glEnd();
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(5.20, 2.25, 4.6);
    glVertex3f(5.20, 2.25, 7.9);
    glVertex3f(5.0, 1.76, 8.0);
    glVertex3f(5.0, 1.76, 4.5);
    glVertex3f(5.20, 2.25, 4.6);
    glEnd();
    glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(6.75, 1.95, 4.6);
    glVertex3f(6.75, 1.95, 7.9);
    glVertex3f(7.0, 1.76, 8.0);
    glVertex3f(7.0, 1.76, 4.5);
    glVertex3f(6.75, 1.95, 4.6);
    glEnd();
     glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(5.0, 1.76, 8.0);
    glVertex3f(7.0, 1.76, 8.0);
    glVertex3f(6.75, 1.95, 7.9);
    glVertex3f(5.20, 2.25, 7.9);
    glVertex3f(5.0, 1.76, 8.0);
    glEnd();
      glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(5.0, 1.76, 4.5);
    glVertex3f(7.0, 1.76, 4.5);
    glVertex3f(6.75, 1.95, 4.6);
    glVertex3f(5.20, 2.25, 4.6);
    glVertex3f(5.0, 1.76, 4.5);
    glEnd();
      glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(5.8, 2.15, 4.8);
    glVertex3f(5.8, 2.15, 7.7);
    glVertex3f(6.5, 2.0, 7.7);
    glVertex3f(6.5, 2.0, 4.8);
    glVertex3f(5.8, 2.15, 4.8);
    glEnd();
   //end cube15
    
    
    //begin cube16
       glColor3f(0.9, 0.9, 0.80);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 1.76, 8.0);
    glVertex3f(0.5, 1.76, 11.5);
    glVertex3f(4.25, 1.76, 11.5);
    glVertex3f(4.25, 1.76, 8.0);
    glVertex3f(0.5, 1.76, 8.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 1.76, 8.0);
    glVertex3f(0.5, 1.76, 11.5);
    glVertex3f(0.5, 3.0, 11.5);
    glVertex3f(0.5, 3.0, 8.0);
    glVertex3f(0.5, 1.76, 8.0);
    glEnd();
     glColor3f(0.3, 0.75, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 1.76, 11.5);
    glVertex3f(4.25, 1.76, 11.5);
    glVertex3f(4.25, 3.0, 11.5);
    glVertex3f(0.5, 3.0, 11.5);
    glVertex3f(0.5, 1.76, 11.5);
    glEnd();
      glColor3f(0.3, 0.75, 0.9);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 1.76, 8.0);
    glVertex3f(4.25, 1.76, 8.0);
    glVertex3f(4.25, 3.0, 8.0);
    glVertex3f(0.5, 3.0, 8.0);
    glVertex3f(0.5, 1.76, 8.0);
    glEnd();
      glColor3f(0.3, 0.85, 0.9);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 3.0, 8.0);
    glVertex3f(0.5, 3.0, 11.5);
    glVertex3f(4.25, 3.0, 11.5);
    glVertex3f(4.25, 3.0, 8.0);
    glVertex3f(0.5, 3.0, 8.0);
    glEnd();
      glColor3f(0.80, 0.80, 0.78);
    glBegin(GL_POLYGON);
    glVertex3f(4.25, 1.76, 8.0);
    glVertex3f(4.25, 1.76, 11.5);
    glVertex3f(4.25, 3.0, 11.5);
    glVertex3f(4.25, 3.0, 8.0);
    glVertex3f(4.25, 1.76, 8.0);
    glEnd();
     glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(4.251, 2.8, 8.0);
    glVertex3f(4.251, 2.8, 11.5);
    glVertex3f(4.251, 3.0, 11.5);
    glVertex3f(4.251, 3.0, 8.0);
    glVertex3f(4.251, 2.8, 8.0);
    glEnd();
      glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(4.0, 2.3, 10.0);
    glVertex3f(4.0, 2.3, 11.25);
    glVertex3f(4.75, 2.3, 11.25);
    glVertex3f(4.75, 2.3, 10.0);
    glVertex3f(4.0, 2.3, 10.0);
    glEnd();
    //end cube16
    
    //begin cube17
     //begin cube16
         glColor3f(0.0, 0.4, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.75, 4.21, 8.0);
    glVertex3f(0.75, 4.21, 11.3);
    glVertex3f(1.00, 4.21, 11.3);
    glVertex3f(1.00, 4.21, 8.0);
    glVertex3f(0.75, 4.21, 8.0);
    glEnd();
       glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.75, 3.01, 8.0);
    glVertex3f(0.75, 3.01, 11.3);
    glVertex3f(1.00, 3.01, 11.3);
    glVertex3f(1.00, 3.01, 8.0);
    glVertex3f(0.75, 3.01, 8.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.75, 3.01, 8.0);
    glVertex3f(0.75, 3.01, 11.3);
    glVertex3f(0.75, 4.21, 11.3);
    glVertex3f(0.75, 4.21, 8.0);
    glVertex3f(0.75, 3.01, 8.0);
    glEnd();
     glColor3f(0.0, 0.3, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.75, 3.01, 11.29);
    glVertex3f(1.00, 3.01, 11.29);
    glVertex3f(1.00, 4.21, 11.29);
    glVertex3f(0.75, 4.21, 11.29);
    glVertex3f(0.75, 3.01, 11.29);
    glEnd();
      glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(0.75, 3.01, 8.0);
    glVertex3f(1.00, 3.01, 8.0);
    glVertex3f(1.00, 4.21, 8.0);
    glVertex3f(0.75, 4.21, 8.0);
    glVertex3f(0.75, 3.01, 8.0);
    glEnd();
   
      glColor3f(0.0, 0.7, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(1.00, 3.01, 8.0);
    glVertex3f(1.00, 3.01, 11.3);
    glVertex3f(1.00, 4.21, 11.3);
    glVertex3f(1.00, 4.21, 8.0);
    glVertex3f(1.00, 3.01, 8.0);
    glEnd();
    
    //
       glColor3f(0.0, 0.4, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 4.21, 8.0);
    glVertex3f(1.5, 4.21, 11.3);
    glVertex3f(1.75, 4.21, 11.3);
    glVertex3f(1.75, 4.21, 8.0);
    glVertex3f(1.5, 4.21, 8.0);
    glEnd();
       glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 3.01, 8.0);
    glVertex3f(1.5, 3.01, 11.3);
    glVertex3f(1.75, 3.01, 11.3);
    glVertex3f(1.75, 3.01, 8.0);
    glVertex3f(1.5, 3.01, 8.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 3.01, 8.0);
    glVertex3f(1.5, 3.01, 11.3);
    glVertex3f(1.5, 4.21, 11.3);
    glVertex3f(1.5, 4.21, 8.0);
    glVertex3f(1.5, 3.01, 8.0);
    glEnd();
     glColor3f(0.0, 0.3, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 3.01, 11.29);
    glVertex3f(1.75, 3.01, 11.29);
    glVertex3f(1.75, 4.21, 11.29);
    glVertex3f(1.5, 4.21, 11.29);
    glVertex3f(1.5, 3.01, 11.29);
    glEnd();
      glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(1.5, 3.01, 8.0);
    glVertex3f(1.75, 3.01, 8.0);
    glVertex3f(1.75, 4.21, 8.0);
    glVertex3f(1.5, 4.21, 8.0);
    glVertex3f(1.5, 3.01, 8.0);
    glEnd();
   
      glColor3f(0.0, 0.7, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(1.75, 3.01, 8.0);
    glVertex3f(1.75, 3.01, 11.3);
    glVertex3f(1.75, 4.21, 11.3);
    glVertex3f(1.75, 4.21, 8.0);
    glVertex3f(1.75, 3.01, 8.0);
    glEnd();
    
    //
    
            glColor3f(0.0, 0.4, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(2.25, 4.21, 8.0);
    glVertex3f(2.25, 4.21, 11.3);
    glVertex3f(2.5, 4.21, 11.3);
    glVertex3f(2.5, 4.21, 8.0);
    glVertex3f(2.25, 4.21, 8.0);
    glEnd();
       glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_POLYGON);
    glVertex3f(2.25, 3.01, 8.0);
    glVertex3f(2.25, 3.01, 11.3);
    glVertex3f(2.5, 3.01, 11.3);
    glVertex3f(2.5, 3.01, 8.0);
    glVertex3f(2.25, 3.01, 8.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(2.25, 3.01, 8.0);
    glVertex3f(2.25, 3.01, 11.3);
    glVertex3f(2.25, 4.21, 11.3);
    glVertex3f(2.25, 4.21, 8.0);
    glVertex3f(2.25, 3.01, 8.0);
    glEnd();
     glColor3f(0.0, 0.3, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(2.25, 3.01, 11.29);
    glVertex3f(2.5, 3.01, 11.29);
    glVertex3f(2.5, 4.21, 11.29);
    glVertex3f(2.25, 4.21, 11.29);
    glVertex3f(2.25, 3.01, 11.29);
    glEnd();
      glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(2.25, 3.01, 8.0);
    glVertex3f(2.5, 3.01, 8.0);
    glVertex3f(2.5, 4.21, 8.0);
    glVertex3f(2.25, 4.21, 8.0);
    glVertex3f(2.25, 3.01, 8.0);
    glEnd();
   
      glColor3f(0.0, 0.7, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(2.5, 3.01, 8.0);
    glVertex3f(2.5, 3.01, 11.3);
    glVertex3f(2.5, 4.21, 11.3);
    glVertex3f(2.5, 4.21, 8.0);
    glVertex3f(2.5, 3.01, 8.0);
    glEnd();

    //end cube17
    
    //begin small cubes
      glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 3.01, 8.0);
    glVertex3f(0.5, 3.01, 11.3);
    glVertex3f(4.05, 3.01, 11.3);
    glVertex3f(4.05, 3.01, 8.0);
    glVertex3f(0.5, 3.01, 8.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 3.01, 8.0);
    glVertex3f(0.5, 3.01, 11.3);
    glVertex3f(0.5, 4.1, 11.3);
    glVertex3f(0.5, 4.1, 8.0);
    glVertex3f(0.5, 3.01, 8.0);
    glEnd();
     glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 3.01, 11.3);
    glVertex3f(4.05, 3.01, 11.3);
    glVertex3f(4.05, 4.1, 11.3);
    glVertex3f(0.5, 4.1, 11.3);
    glVertex3f(0.5, 3.01, 11.3);
    glEnd();
      glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(0.5, 3.01, 8.0);
    glVertex3f(4.05, 3.01, 8.0);
    glVertex3f(4.05, 4.1, 8.0);
    glVertex3f(0.5, 4.1, 8.0);
    glVertex3f(0.5, 3.01, 8.0);
    glEnd();
   
      glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_POLYGON);
    glVertex3f(4.05, 3.01, 8.0);
    glVertex3f(4.05, 3.01, 11.3);
    glVertex3f(4.05, 4.5, 11.3);
    glVertex3f(4.05, 4.5, 8.0);
    glVertex3f(4.05, 3.01, 8.0);
    glEnd();
    
    //
    

}

void cylinders()
{
   glColor3f(0.2, 0.2, 0.2);
   glTranslatef (1.0, 1.75, 4.5);  // Translations. 
   glRotatef (90, 1,0,0);
   gluCylinder(qobj, 0.2, 0.2, 4.0, 600, 1);
   
   glTranslatef (6.0, 0.0, 0.0);  // Translations. 
   gluCylinder(qobj, 0.2, 0.2, 4.0, 600, 1);
   
   glTranslatef (0.0, 6.5, 0.0);
   gluCylinder(qobj, 0.2, 0.2, 4.0, 600, 1);
   
   glTranslatef (-6.0, 0.0, 0.0);
   gluCylinder(qobj, 0.2, 0.2, 4.0, 600, 1);
   
   
   glColor3f(0.75, 0.0, 0.0);
   glTranslatef (4.5, 0.0, -0.80);
   gluCylinder(qobj, 0.3, 0.3, 1.0, 600, 1);
   
   glColor3f(1.0, 0.0, 0.0);
   glTranslatef (0.0, 0.0, -0.10);
   gluCylinder(qobj, 0.25, 0.3, 0.1, 600, 1);
   
   glColor3f(0.5, 0.5, 0.5);
   glTranslatef (0.0, 0.0, -0.01);
   gluCylinder(qobj, 0.25, 0.0, 0.1, 600, 1);
   
   glColor3f(0.0, 1.0, 0.0);
   glTranslatef (0.0, -2.0, 0.75);
   glutSolidSphere(0.15, 16, 100);
   
    glColor3f(0.0, 0.0, 1.0);
   glTranslatef (1.0, 0.0, 0.0);
   glutSolidSphere(0.15, 16, 100);
   
    glColor3f(0.0, 0.5, 1.0);
   glTranslatef (0.0, 1.0, 0.0);
   glutSolidSphere(0.15, 16, 100);
   
    glColor3f(0.5, 0.0, 0.5);
   glTranslatef (0.75, 0.0, 0.0);
   glutSolidSphere(0.15, 16, 100);
   
}


/*
void spiralAnimation()
{
 if(counter<270)
 {
   yrot += 4.0; 
   zrot += .25; 
   scal *= 1.005; 
 }
 else if(counter>=270)
 {
   yrot += 0.0;
   zrot += 0.0;
   scal *= 1.0;     
 }
 //printf("%d, /n", count); 
 counter++;   
 glutPostRedisplay();  
 glutTimerFunc(50, spiralAnimation, 1);    
}
*/

void keyboard (unsigned char key, int x, int y) {
 
	switch (key) {
 
 
		//case ' ':  spiralAnimation();
		//           break; 
		case  27:  
			exit (0);

		//no real depth effect
		case 'w': 
			ypos += 1;
			glutPostRedisplay();  
			break;
		case 's': 
			ypos -= 1;
			glutPostRedisplay();  
			break;
		case 'a': 
			zpos += 1;
			glutPostRedisplay();  
			break;
		case 'd': 
			zpos -= 1;
			glutPostRedisplay();  
			break;

		case 'W': 
			zrot += 1;
			glutPostRedisplay();  
			break;
		case 'S': 
			zrot -= 1;
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

		default :  printf ("   key = %c -> %d\n", key, key);
   }
}

void display()
{ 
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(10.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glTranslatef (xpos, ypos, zpos);  // Translations.
 
   glRotatef (zrot, 0,0,1);        // Rotations.
   glRotatef (yrot, 0,1,0);
   glRotatef (xrot, 1,0,0);
    glScalef (scal, scal, scal);
    glTranslatef (-3.0, 0.0, -8.0); 
   // Sizing.
        DrawPart1();
    cylinders();
  
    // begin WOPR
     //begin cube1    
   //glTranslatef (xpos, ypos, zpos);  // Translations.
 
   glRotatef (zrot, 0,0,1);        // Rotations.
   glRotatef (yrot, 0,1,0);
   glRotatef (xrot, 1,0,0);
 
  
   // glLoadIdentity();
    
   // glPushMatrix();
    //glTranslatef(10.0, 12.0, 8.0);
    //glPopMatrix();
        glutSwapBuffers();
    
}

// reshapes window, gives the actual size of the xzy coordinate units
void reshape(int w, int h)
{
glViewport(0,0, w, h);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(-20.0, 20.0, -20.0, 20.0, -20.0, 20.0);
    
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
glutInit(&argc, argv);
glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);

glutInitWindowSize(1000, 1000);
glutInitWindowPosition(0, 0);
glutCreateWindow("WarGames");
glutDisplayFunc(display);
glutReshapeFunc(reshape);
glutKeyboardFunc(keyboard);
init();
glutMainLoop();   
}




