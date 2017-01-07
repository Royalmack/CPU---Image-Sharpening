#include <iostream>
#include <fstream>
#include "Glut/freeglut.h"

//Glut and global functions
void init();
void display();
void drawPoint();
unsigned char** readFile(char[]);
void drawData();
void keyRead(unsigned char, int, int);
void sharpenImage();


//default screen width and height
int screenWidth = 800;
int screenHeight = 600;

//Data arrays to hold pixel value information
unsigned char **imgData = NULL;
unsigned char **imgArray = NULL;
int BPP = 3;
float sharpenValue = 5;

int main(int argc, char** argv)
{		
	//Setting file name
	char fileName[100] = "img.bmp";
	
	//FIll arrays
	imgData = readFile(fileName);
	imgArray = readFile(fileName);

	//Start glut and GL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Image Processing");

	init();	

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyRead);

	glutMainLoop();

	return 0;
}

void init()
{
	//Window settings and field of view
	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screenWidth, 0, screenHeight);
}

void display()
{
	//Clear screen and draw image array
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	drawData();

	glFlush();
	glutSwapBuffers();
}

unsigned char** readFile(char* fileName)
{
	//read in header of file
	char imgHeader[54];
	
	std::ifstream myFile(fileName, std::ifstream::binary);	
	myFile.read(imgHeader, 54);
	
	//Set image width, height and bits per pixel
	int imgWidth = *(int*)&imgHeader[18];
	int imgHeight = *(int*)&imgHeader[22];
	BPP = *(int*)&imgHeader[28];
	BPP /= 8;

	screenWidth = imgWidth;
	screenHeight = imgHeight;

	unsigned char tmp;
	
	//Read in pixel values from file into array
	int rowData = (imgWidth * BPP + BPP) & (~BPP);
	unsigned char **imgData = new unsigned char*[imgHeight];
	unsigned char **imgArray = new unsigned char*[imgHeight];
	for(int i = 0; i < imgHeight; i++)
	{
		imgData[i] = new unsigned char[rowData];
		imgArray[i] = new unsigned char[rowData];
	}
	
	//Swap RGB values as BMP is reversed
	for (int i = 0; i < imgHeight; i++)
	{
		myFile.read((char*)imgData[i], rowData);
	
		for (int j = 0; j < imgWidth * BPP; j += BPP)
		{
			tmp = imgData[i][j];
			imgData[i][j] = imgData[i][j + 2];
			imgData[i][j + 2] = tmp;
		}
	}

	return imgData;
}


void drawData()
{
	//Draw pixels on screen using pixel values stored
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);	

	glBegin(GL_POINTS);

	for (int i = 0; i < screenHeight; i++)
	{
		for (int j = 0; j < screenWidth * BPP; j += BPP)
		{
			
			glColor3f((float)imgArray[i][j] / 255, (float)imgArray[i][j + 1] / 255, (float)imgArray[i][j + 2] / 255);
			glVertex2d(j / BPP, i);
			
		}
	}

	glEnd();
}


//Waits for input before sharpning image
void keyRead(unsigned char k, int x, int y)
{
	switch (k)
	{
		case 'q':
			sharpenImage();
		break;

		default:
		break;
	}
}

//Sharpens image by placing a mask over each pixel.  Looks through entire array minus the border pixels
void sharpenImage()
{	
	int tmpPxl;

	for (int i = 1; i < screenHeight - 1; i++)
	{
		for (int j = BPP; j < screenWidth * BPP - BPP; j+=BPP)
		{
			int r = j;
			int g = j+1;
			int b = j+2;

			//Red
			tmpPxl = (sharpenValue * (int)imgData[i][r]) - ((int)imgData[i][r - BPP]) - ((int)imgData[i][r + BPP]) - ((int)imgData[i - 1][r]) - ((int)imgData[i + 1][r]);
			if (tmpPxl < 0)
				tmpPxl = 0;

			if (tmpPxl > 255)
				tmpPxl = 255;
			imgArray[i][r] = tmpPxl;
			
			//Green
			tmpPxl = (sharpenValue * (int)imgData[i][g]) - ((int)imgData[i][g - BPP]) - ((int)imgData[i][g + BPP]) - ((int)imgData[i - 1][g]) - ((int)imgData[i + 1][g]);
			if (tmpPxl < 0)
				tmpPxl = 0;
			if (tmpPxl > 255)
				tmpPxl = 255;
			imgArray[i][g] = tmpPxl;


			//Blue
			tmpPxl = (sharpenValue * (int)imgData[i][b]) - ((int)imgData[i][b - BPP]) - ((int)imgData[i][b + BPP]) - ((int)imgData[i - 1][b]) - ((int)imgData[i + 1][b]);
			if (tmpPxl < 0)
				tmpPxl = 0;
			if (tmpPxl > 255)
				tmpPxl = 255;
			imgArray[i][b] = tmpPxl;
		}
	}
}