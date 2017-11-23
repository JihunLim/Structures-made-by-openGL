/*mouseMovement
��ǻ�ͱ׷��Ƚ� ����3 - 3D ����ýƮ ���� �� ���콺/Ű���� ������ ���� ������Ű��
�ܺ��߰���� - ����, ���, ����
�������б� ��ǻ�Ͱ��а�
13010979, ������
*/
#define _CRT_SECURE_NO_WARNINGS
#include <cmath> 
#include <cstdlib>
#include <cstdio>
#include <GL/glut.h>

#include <iostream>
#include <GL\freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

/*Trackball�� ���� ���� �� �ʱ�ȭ*/
#define bool int 
#define false 0
#define true 1
#define M_PI 3.141592

int winWidth, winHeight;
GLfloat file_width, file_height;
int count_texture = 0;
int count_filtering = 0;
float angle = 0.0, axis[3], trans[3];
bool trackingMouse = false;
bool redrawContinue = false;
bool trackballMove = false;

float lastPos[3] = { 0.0F, 0.0F, 0.0F };
int curx, cury;
int startX, startY;
GLfloat camera_x = 45;
GLfloat camera_init_x = 45;
GLfloat camera_y = 70;
GLfloat camera_init_y = 70;
GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //�Ȱ�����(ȸ��)
GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info);
GLubyte *pBytes; // �����͸� ����ų ������
BITMAPINFO *info; // ��Ʈ�� ��� ������ ����
GLfloat fog_density = 0.01f;

int width, height;
//unsigned char* image;
GLuint textures[9];
void initializeTextures(void);
void drw_polygon(int, int, float, float);
void mydisplay();
void init();
//void glutMouseFunc(mouseMovement);
void myreshape(GLsizei, GLsizei);
void mouseMovement(int, int, int, int);
void mouseMotion(int, int);
void checkInObjects();
void myKeyboard(unsigned char, int, int);
void GetNormal(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat normal[3]);
void makeBuilding(GLfloat tx, GLfloat ty, GLfloat tz, GLfloat sx, GLfloat h, GLfloat sz, double ry, int mode); // make 6-face box
void trackball_ptov(int x, int y, int width, int height, float v[3]);
void startMotion(int x, int y);
void stopMotion(int x, int y);
void spinCube();
void makeTree(GLfloat tx, GLfloat ty, GLfloat tz);
void createCylinder(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h);
void setCamera(GLfloat aspect);
void setFog(GLfloat);


int numOfQuad = 0;  // 
int numOfTri = 0;  // 
int mouseCurPositionX;
int mouseCurPositionY;
int mouseCurButton;
int startMovement;
int prevMX; // ���� ���콺 Ŭ�� x��ǥ
int prevMY; // ���� ���콺 Ŭ�� y��ǥ
int isRMB = 0; // is Right Mouse Button
int isMMB = 0; // is Middle Mouse Button

GLfloat fovv = 45.0f;
GLUquadric* qd_obj;
GLdouble poleX, poleY, poleZ; //ȸ����
							  //GLdouble angle;  trackball����� �ߺ�
							  //int startX, startY; trackball ����� �ߺ�
double xAngle, yAngle, zAngle;

//�� ���� ���� ����
GLfloat ambientLight[4][4];
GLfloat diffuseLight[4][4];
GLfloat lightPosition[4][4];
GLfloat specular[4][4];
GLfloat yrot;

GLfloat m_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat m_diffuse[] = { 0.3, 0.6, 0.2, 1.0 };
GLfloat m_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat m_shine = 35.0;

double pickedX, pickedY, pickedZ;
float color[3] = { 0.2, 0.4, 0.6 }; //�⺻ ��
float color0_1[3] = { 0.6, 0.2, 0.6 }; //���Ʒ� ��
float color0_2[3] = { 0.6, 0.2, 0.6 }; //���Ʒ� ��
float color1[3] = { 0.2, 0.4, 0.6 }; //���� ��
float color2[3] = { 0.2, 0.4, 0.6 }; //���� ��

void setFog() {
	int num = 95.0f;

	//glEnable(GL_FOG);
	//glHint(GL_FOG_HINT, GL_DONT_CARE);

	glFogfv(GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogf(GL_FOG_DENSITY, fog_density);
	//glFogf(GL_FOG_START, 0);
	//glFogf(GL_FOG_END, 30);
	glutPostRedisplay();
	printf("fog_density -> %f\n\n", fog_density);
}

/*Trackball ���� ���� �Լ�*/
void trackball_ptov(int x, int y, int width, int height, float v[3])
{
	float d, a;

	v[0] = (2.0F*x - width) / width;
	v[1] = (height - 2.0F*y) / height;
	d = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
	v[2] = (float)cos((M_PI / 2.0F) * ((d < 1.0F) ? d : 1.0F));
	a = 1.0F / (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

void startMotion(int x, int y)
{
	trackingMouse = true;
	redrawContinue = false;
	startX = x; startY = y;
	curx = x; cury = y;
	trackball_ptov(x, y, winWidth, winHeight, lastPos);
	trackballMove = true;
}

void stopMotion(int x, int y)
{
	trackingMouse = false;

	if (startX != x || startY != y) {
		redrawContinue = false;

	}
	else {
		angle = 0.0F;
		redrawContinue = false;
		trackballMove = false;

	}
}

void spinCube()
{
	if (redrawContinue) glutPostRedisplay();
}


void mydisplay()
{
	GLfloat normal[3];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

	if (trackballMove)
		glRotatef(angle, axis[0], axis[1], axis[2]);

	glPushMatrix();
	
    //��
	glColor3f(0.6f, 0.6f, 0.6f);
	m_diffuse[0] = 0.3f; m_diffuse[1] = 0.3f; m_diffuse[2] = 0.3f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	makeBuilding(-30, -1, 20, 60, 0.2f, 120, 0, 4);
	//���
	glColor3f(0.0f, 0.8f, 0.0f);
	m_diffuse[0] = 0.0f; m_diffuse[1] = 0.8f; m_diffuse[2] = 0.0f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	makeBuilding(-23, 0, -67, 40, -0.5f, 15, 0, 5);

	//����
	makeTree(24, 0, 15);
	makeTree(24, 0, 0);
	makeTree(24, 0, -15);
	makeTree(24, 0, -30);
	makeTree(24, 0, -45);
	makeTree(24, 0, -60);
	makeTree(24, 0, -75);
	makeTree(24, 0, -85);
	makeTree(14, 0, -85);
	makeTree(6, 0, -85);
	makeTree(-2, 0, -85);
	makeTree(-10, 0, -85);
	makeTree(-18, 0, -85);
	makeTree(-26, 0, -85);

	//����
	glColor3f(0.3f, 0.3f, 0.3f);
	m_diffuse[0] = 0.35f; m_diffuse[1] = 0.35f; m_diffuse[2] = 0.35f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	makeBuilding(27, 0, 20.0f, 2, -0.5f, 110, 0, 6);
	makeBuilding(28.0f, 0, -91.0f, 2, -0.5f, 58, 90, 6);
	//�ǹ�
	glColor3f(0.77f, 0.77f, 0.7f);
	m_diffuse[0] = 0.75f; m_diffuse[1] = 0.75f; m_diffuse[2] = 0.75f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);

	/*
	//��� ���ڿ� ���� �ؽ���(�� ������ �Ķ���� ��)
	0 - ��������
	1 - ��� �븮��
	2 - ����
	3 - ���� ����
	4 - ������
	5 - Ǯ
	6 - ����
	7 - �������
	8 - �ٻ��
	*/

	makeBuilding(10, 0, -2, 6, 15, 5, 0, 0); //a�ǹ�
	makeBuilding(-24, 0, 16, 3, 9, 13, 0, 0); //b�ǹ�
	makeBuilding(-14, 0, 16, 3, 9, 13, 0, 1); //c�ǹ�
	makeBuilding(-4, 0, 16, 3, 9, 13, 0, 2); //d�ǹ�
	makeBuilding(-26, 0, -15, 12, 5, 4, 0, 3); //e�ǹ�
	makeBuilding(-26, 0, -27, 12, 5, 4, 0, 1); //f�ǹ�
	makeBuilding(-26, 0, -39, 12, 5, 4, 0, 3); //g�ǹ�
	makeBuilding(-7, 0, -20, 2, 4, 10, 0, 2); //k�ǹ�
	makeBuilding(-7, 0, -30, 5, 18, 12, 0, 0); //h�ǹ�
	makeBuilding(-2, 0, -30, 11, 3, 2, 0, 1); //h�ǹ� �� ������a
	makeBuilding(-2, 0, -40, 11, 3, 2, 0, 2); //h�ǹ� �� ������b
	makeBuilding(9, 0, -30, 9, 10, 12, 0, 1); //j�ǹ�
	makeBuilding(0, 0, -50, 14, 7, 10, 0, 3); //i�ǹ�
	makeBuilding(18, 0, 2, 1, 3, 8, 0, 2); //�߰�a�ǹ�

	glPopMatrix();

	glutSwapBuffers();  // ������۸� ���ÿ� glFlush() ��� �̰� ��� ��
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0); // black clear color, opaque window
	glClearDepth(1.0f); // ���̸� 1�� �ʱ�ȭ�Ѵ�.
						//glColor3f(1.0, 1.0, 1.0); // white
						//glMatrixMode(GL_MODELVIEW);
	glEnable(GL_FOG);
	glHint(GL_FOG_HINT, GL_DONT_CARE);

	camera_init_x = camera_x;
	camera_init_y = camera_y;

	// �ؽ��� ������ ���� �ڵ�
	// �ؽ���0 ����
	//glGenTextures(1, textures);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//pBytes = LoadDIBitmap("texture1.bmp", &info);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 100, 100, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	//glEnable(GL_TEXTURE_2D); //�ؽ��� ������ �����ϰ� �Ѵ�. 
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);  // ���δ� �߶󳽴�  


	glEnable(GL_DEPTH_TEST); //���� �׽�Ʈ�� �����Ѱ� �����Ѵ�.
	//setFog(0.3f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING); //������ ����ϱ� ���ؼ� ENABLE �����ش�.
	glEnable(GL_LIGHT0); // ���� �� 0�� ������ ����Ѵ�.

	glShadeModel(GL_SMOOTH); //�ε巯�� ���̵��� �����ϴ�.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// ������ �������� ����
													  //glLoadIdentity();
													  //glOrtho3D(-1.0, 1.0, -1.0, 1.0); // screen size (-1.0,-1.0) to (1.0,1.0)

													  //������ ���� ����
	ambientLight[0][0] = 0.3f; ambientLight[1][0] = 0.2f; ambientLight[2][0] = 0.3f;
	ambientLight[0][1] = 0.3f; ambientLight[1][1] = 0.4f; ambientLight[2][1] = 0.2f;
	ambientLight[0][2] = 0.3f; ambientLight[1][2] = 0.2f; ambientLight[2][2] = 0.6f;
	ambientLight[0][3] = 1.0f; ambientLight[1][3] = 1.0f; ambientLight[2][3] = 1.0f;

	diffuseLight[0][0] = 0.7f; diffuseLight[1][0] = 0.2f; diffuseLight[2][0] = 0.8f;
	diffuseLight[0][1] = 0.7f; diffuseLight[1][1] = 0.7f; diffuseLight[2][1] = 0.3f;
	diffuseLight[0][2] = 0.7f; diffuseLight[1][2] = 0.3f; diffuseLight[2][2] = 0.1f;
	diffuseLight[0][3] = 1.0f; diffuseLight[1][3] = 1.0f; diffuseLight[2][3] = 1.0f;

	lightPosition[0][0] = 0.0f; lightPosition[1][0] = 7.0f; lightPosition[2][0] = 15.0f;
	lightPosition[0][1] = 55.0f; lightPosition[1][1] = 60.0f; lightPosition[2][1] = 50.0f;
	lightPosition[0][2] = 10.0f; lightPosition[1][2] = 10.0f; lightPosition[2][2] = 10.0f;
	lightPosition[0][3] = 1.0f; lightPosition[1][3] = 1.0f; lightPosition[2][3] = 1.0f;

	specular[0][0] = 1.0f; specular[1][0] = 1.0f; specular[2][0] = 1.0f;
	specular[0][1] = 1.0f; specular[1][1] = 1.0f; specular[2][1] = 1.0f;
	specular[0][2] = 1.0f; specular[1][2] = 1.0f; specular[2][2] = 1.0f;
	specular[0][3] = 1.0f; specular[1][3] = 1.0f; specular[2][3] = 1.0f;


	// �� ����(���� - https://skyfe79.gitbooks.io/opengl-tutorial/content/chapter10.html)
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight[0]); //Ambient ������ ������ ����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight[0]); //Diffuse ������ ������ ����
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular[0]); //Specular ������ ������ ���� 
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition[0]); //������ ��ġ�� ����

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight[1]); //Ambient ������ ������ ����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight[1]); //Diffuse ������ ������ ����
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular[1]); //Specular ������ ������ ���� 
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition[1]); //������ ��ġ�� ����

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight[2]); //Ambient ������ ������ ����
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight[2]); //Diffuse ������ ������ ����
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular[2]); //Specular ������ ������ ���� 
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition[2]); //������ ��ġ�� ����

	//�⺻ ���� �� - �� 3��
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	//���� ����
	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, m_shine);
	
	/*initialise the texture map*/
	initializeTextures();
	
	setFog();
}

void myreshape(GLsizei width, GLsizei height) {
	printf("myreshape");
	if (height == 0) height = 1;
	GLfloat aspect = (GLfloat)width / (GLfloat)height; //������ â�� ���� ���
	glViewport(0, 0, width, height);       // ����Ʈ�� �����Ѵ�.
	winWidth = width;
	winHeight = height;

	//setCamera(aspect);
	glMatrixMode(GL_PROJECTION);  // ��Ʈ������带 ������������ ����
	glLoadIdentity();             // �缳����        
	gluPerspective(fovv, aspect, .1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x, camera_y, 95.0f, camera_x - camera_init_x, camera_y - camera_init_y, -5.0f, .0f, 1.0f, .0f);

	glutPostRedisplay();

}

void mouseMovement(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON)
		switch (state)
		{
		case GLUT_DOWN:
			y = winHeight - y;
			startMotion(x, y);
			break;
		case GLUT_UP:
			stopMotion(x, y);
			break;
		}

	prevMX = mouseCurPositionX;
	prevMY = mouseCurPositionY;
	printf("���� %d, %d", prevMX, mouseCurPositionX);
	//mouseCurButton = button;

	if (button == GLUT_RIGHT_BUTTON)
		switch (state)
		{
		case GLUT_DOWN:
			isRMB = 1;
			startX = x; startY = y;
			prevMX = x; prevMY = y;
			glutPostRedisplay();
			break;
		case GLUT_UP:
			isRMB = 0;
			break;
		}

	if (button == GLUT_MIDDLE_BUTTON)
		switch (state)
		{
		case GLUT_DOWN:
			isMMB = 1;
			startX = x; startY = y;
			prevMX = x; prevMY = y;
			glutPostRedisplay();
			break;
		case GLUT_UP:
			isMMB = 0;
			break;
		}

	prevMX = mouseCurPositionX;
	prevMY = mouseCurPositionY;
	mouseCurPositionX = x;
	mouseCurPositionY = y;

	glutPostRedisplay();

}

void setCamera(GLfloat aspect) {
	glMatrixMode(GL_PROJECTION);  // ��Ʈ������带 ������������ ����
	glLoadIdentity();             // �缳����        
	gluPerspective(fovv, aspect, .1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x, camera_y, 95.0f, camera_x - camera_init_x, camera_y - camera_init_y, -5.0f, .0f, 1.0f, .0f);

	glutPostRedisplay();
}

void mouseMotion(int x, int y) {

	if (isRMB == 1) {
		printf("dd");
		if (prevMX < x) {
			//���콺�� ���������� ��S��
			printf("���콺 ���������� �̵� -> x������ ȸ�� ��\n");
			if (fovv >= 0) fovv = fovv - 0.8f;
			printf("%f", fovv);
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			//myreshape(winWidth, winHeight);
			//xAngle += (x - startX) / 3.6;
			glutPostRedisplay();
		}
		if (prevMX > x) {
			//���콺�� �������� �̵�
			printf("���콺 �������� �̵� -> y������ ȸ�� ��\n");
			fovv = fovv + 0.8f;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			//myreshape(winWidth, winHeight);
			//yAngle += (x - startX) / 3.6;
			glutPostRedisplay();
		}
	}

	if (isMMB == 1) {
		printf("middle mouse button clicked");
		if (prevMX < x) {
			//���콺�� ���������� ��S��
			camera_x -= (x - startX) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}
		if (prevMX > x) {
			//���콺�� �������� �̵�
			camera_x -= (x - startX) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}
		if (prevMX < y) {
			//���콺�� ���� �̵�
			camera_y += (y - startY) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}
		if (prevMX > y) {
			//���콺�� �Ʒ������� �̵�
			camera_y += (y - startY) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}


	}

	//������ ���콺 Ŀ�� ��ġ�� ����
	prevMX = x;
	prevMY = y;
	startX = x;
	startY = y;

	float curPos[3], dx, dy, dz;
	mouseCurPositionX = x;
	trackball_ptov(x, y, winWidth, winHeight, curPos);
	if (trackingMouse)
	{
		dx = curPos[0] - lastPos[0];
		dy = curPos[1] - lastPos[1];
		dz = curPos[2] - lastPos[2];

		if (dx || dy || dz) {
			angle = 90.0F * sqrt(dx*dx + dy*dy + dz*dz);

			axis[0] = lastPos[1] * curPos[2] - lastPos[2] * curPos[1];
			axis[1] = lastPos[2] * curPos[0] - lastPos[0] * curPos[2];
			axis[2] = lastPos[0] * curPos[1] - lastPos[1] * curPos[0];

			lastPos[0] = curPos[0];
			lastPos[1] = curPos[1];
			lastPos[2] = curPos[2];

		}
	}
	glutPostRedisplay();

}

void myKeyboard(unsigned char key , int x, int y) {
	
	switch (key) {
	case '0':
		// �� ����
		printf("0��Ű�� ���Ƚ��ϴ�. - �� ���� ���\n");
		glDisable(GL_LIGHTING);
		glutPostRedisplay(); // ����� ������ �ٽ� �׸�
		break;
	case '1':
		// �� 1
		printf("1��Ű�� ���Ƚ��ϴ�. - �� 1�� ���\n");
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT0);

		glutPostRedisplay(); // ����� ������ �ٽ� �׸�
		break;
	case '2':
		// �� 1 + 2
		printf("2��Ű�� ���Ƚ��ϴ�. - �� 2�� ���\n");
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glutPostRedisplay(); // ����� ������ �ٽ� �׸�
		break;
	case '3':
		// �� 1 + 2 + 3
		printf("3��Ű�� ���Ƚ��ϴ�. - �� 3�� ���\n");
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glutPostRedisplay(); // ����� ������ �ٽ� �׸�
		break;

	case '5':
		// 5���� ������ �ؽ��� ��� �ߴ� �Ǵ� ����
		(count_texture++ % 2 == 0) ? glDisable(GL_TEXTURE_2D) : glEnable(GL_TEXTURE_2D);
		glutPostRedisplay(); // ����� ������ �ٽ� �׸�
		break;
	case '8':
		// 8���� ������ ���͸� ��� ����
		//textures 4 setting - ������ �̹���(�⺻���� - 'c' Ű�� �̿��ؼ� ���ð� ������ ����)
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		pBytes = LoadDIBitmap("row_resolution.bmp", &info);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (count_filtering++ % 2 == 0) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			printf("������ �̹���(�عٴ�) - ���͸� ��� : GL_LINEAR\n");
		}
		else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			printf("������ �̹���(�عٴ�) - ���͸� ��� : GL_NEAREST\n");
		}
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glutPostRedisplay(); // ����� ������ �ٽ� �׸�
		break;
	case '4':
		 fog_density -= 0.0005f;
		 if (fog_density <= 0) fog_density = 0.0f;
		setFog();
		
		break;
	case '6':
		fog_density += 0.0005f;
		if (fog_density >= 1) fog_density = 1.0f;
		setFog();
		
		break;
	default:
		printf("0~3�߿� �ϳ��� �����ֽʽÿ�.\n");
		break;
	}
}

void GetNormal(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat normal[3])
{
	GLfloat ba[3];
	GLfloat ca[3];
	GLfloat n[3];

	// �� �������� ���͸� �����
	ba[0] = b[0] - a[0]; ba[1] = b[1] - a[1]; ba[2] = b[2] - a[2];
	ca[0] = c[0] - a[0]; ca[1] = c[1] - a[1]; ca[2] = c[2] - a[2];

	// ���� ���� �� ��
	n[0] = ba[1] * ca[2] - ca[1] * ba[2];
	n[1] = ca[0] * ba[2] - ba[0] * ca[2];
	n[2] = ba[0] * ca[1] - ca[0] * ba[1];

	// ����ȭ��
	GLfloat l = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	normal[0] = n[0] / l; normal[1] = n[1] / l; normal[2] = n[2] / l;
}

void makeTree(GLfloat tx, GLfloat ty, GLfloat tz)
{
	m_ambient[0] = 0.2f; m_ambient[1] = 0.2f; m_ambient[2] = 0.2f; m_ambient[3] = 0.2f;
	m_diffuse[0] = 0.0f; m_diffuse[1] = 1.0f; m_diffuse[2] = 0.0f; m_diffuse[3] = 0.0f;
	m_specular[0] = 1; m_specular[1] = 1; m_specular[2] = 1; m_specular[3] = 1;
	m_shine = 35.0;


	GLfloat normal[3];
	GLfloat h = 4;
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.3f);
	glTranslatef(tx, ty, tz); //��ǥ�� x������ 2��ŭ �ű�
	glBindTexture(GL_TEXTURE_2D, textures[7]); 
	glBegin(GL_QUADS); //������ü ����
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	m_diffuse[0] = 0.7f; m_diffuse[1] = 0.25f; m_diffuse[2] = 0.12f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	//����
	GLfloat f_up[4][3] = {
		{ 1, h, -1 },
		{ -1.0f, h, -1 },
		{ -1.0f, h, 1.0f },
		{ 1, h, 1.0f },
	};
	GetNormal(f_up[0], f_up[1], f_up[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_up[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_up[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_up[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_up[3]);

	//���ʸ�
	GLfloat f_left[4][3] = {
		{ -1.0f, h, 1.0f },
		{ -1.0f, h, -1 },
		{ -1.0f, -1.0f, -1 },
		{ -1.0f, -1.0f, 1.0f },
	};
	GetNormal(f_left[0], f_left[1], f_left[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_left[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_left[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_left[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_left[3]);

	//�����ʸ�
	GLfloat f_right[4][3] = {
		{ 1, h, -1 },
		{ 1, h, 1.0f },
		{ 1, -1.0f, 1.0f },
		{ 1, -1.0f, -1 },
	};
	GetNormal(f_right[0], f_right[1], f_right[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_right[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_right[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_right[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_right[3]);

	//�ظ�
	GLfloat f_bottom[4][3] = {
		{ 1, -1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1 },
		{ 1, -1.0f, -1 },
	};
	GetNormal(f_bottom[0], f_bottom[1], f_bottom[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_bottom[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_bottom[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_bottom[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_bottom[3]);

	//�޸�
	GLfloat f_behind[4][3] = {
		{ 1, -1.0f, -1 },
		{ -1.0f, -1.0f, -1 },
		{ -1.0f, h, -1 },
		{ 1, h, -1 },
	};
	GetNormal(f_behind[0], f_behind[1], f_behind[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(0, 0);
	glVertex3fv(f_behind[0]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_behind[1]);
	glTexCoord2f(1, 1);
	glVertex3fv(f_behind[2]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_behind[3]);

	//�ո�
	GLfloat f_front[4][3] = {
		{ 1, h, 1.0f },
		{ -1.0f, h, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1, -1.0f, 1.0f },
	};
	GetNormal(f_front[0], f_front[1], f_front[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_front[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_front[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_front[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_front[3]);

	glEnd();  // ������ü ��

    // ���� �� �κ� ����
	glColor3f(0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	glBegin(GL_TRIANGLES);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	m_diffuse[0] = 0.3f; m_diffuse[1] = 0.7f; m_diffuse[2] = 0; m_diffuse[3] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	GLfloat width = 2;
	//{ Front }

	GLfloat front[3][3] = {
		{ 0.0f, 8.0f, 0.0f },
		{ -width, 4.0f, width },
		{ width, 4.0f, width },
	};
	GetNormal(front[0], front[1], front[2], normal);
	glNormal3fv(normal);

	//printf((char)normal[1], (char)normal[2]);
	glTexCoord2f(0.5, 0.5);
	glVertex3fv(front[0]);
	glTexCoord2f(0, 0);
	glVertex3fv(front[1]);
	glTexCoord2f(1, 0);
	glVertex3fv(front[2]);

	//{ Right }
	GLfloat right[3][3] = {
		{ 0.0f, 8.0f, 0.0f },
		{ width, 4.0f, width },
		{ width, 4.0f, -width },
	};
	GetNormal(right[0], right[1], right[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(0.5, 0.5);
	glVertex3fv(right[0]);
	glTexCoord2f(0, 0);
	glVertex3fv(right[1]);
	glTexCoord2f(1, 0);
	glVertex3fv(right[2]);

	//{ Back }
	GLfloat back[3][3] = {
		{ 0.0f, 8.0f, 0.0f },
		{ width, 4.0f, -width },
		{ -width, 4.0f, -width }
	};
	GetNormal(back[0], back[1], back[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(0.5, 0.5);
	glVertex3fv(back[0]);
	glTexCoord2f(0, 0);
	glVertex3fv(back[1]);
	glTexCoord2f(1, 0);
	glVertex3fv(back[2]);

	//{ Left }
	GLfloat left[3][3] = {
		{ 0.0f, 8.0f, 0.0f },
		{ -width, 4.0f, -width },
		{ -width, 4.0f, width },
	};
	GetNormal(left[0], left[1], left[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(0.5, 0.5);
	glVertex3fv(left[0]);
	glTexCoord2f(0, 0);
	glVertex3fv(left[1]);
	glTexCoord2f(1, 0);
	glVertex3fv(left[2]);

	glEnd();

	glTranslatef(-tx, -ty, -tz); //��ǥ�� x������ -2��ŭ �ű�(���󺹱�)
	glPopMatrix();

}

void makeBuilding(GLfloat tx, GLfloat ty, GLfloat tz, GLfloat sx, GLfloat h, GLfloat sz, double ry, int mode)  //�Ű����� - �������� �̵��� ��ġ, ũ��, ȸ���� x, y -> �����ʰ� ������ �þ
{
	GLfloat normal[3];

	glPushMatrix();
	//glColor3fv(color1); //�� ����
	glTranslatef(tx, ty, tz); //��ǥ�� x������ 2��ŭ �ű�
							  //glRotatef(rx, 1.0f, 0.0f, 0.0f);
	glRotatef(ry, 0.0f, 1.0f, 0.0f);
	//glRotatef(zAngle, 0.0f, 0.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, textures[mode]); // mode�� 0���� 5����
	glBegin(GL_QUADS); //������ü ����
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	//����
	GLfloat f_up[4][3] = {
		{ sx, h, -sz },
		{ -1.0f, h, -sz },
		{ -1.0f, h, 1.0f },
		{ sx, h, 1.0f },
	};
	GetNormal(f_up[0], f_up[1], f_up[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_up[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_up[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_up[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_up[3]);

	//���ʸ�
	GLfloat f_left[4][3] = {
		{ -1.0f, h, 1.0f },
		{ -1.0f, h, -sz },
		{ -1.0f, -1.0f, -sz },
		{ -1.0f, -1.0f, 1.0f },
	};
	GetNormal(f_left[0], f_left[1], f_left[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_left[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_left[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_left[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_left[3]);

	//�����ʸ�
	GLfloat f_right[4][3] = {
		{ sx, h, -sz },
		{ sx, h, 1.0f },
		{ sx, -1.0f, 1.0f },
		{ sx, -1.0f, -sz },
	};
	GetNormal(f_right[0], f_right[1], f_right[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_right[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_right[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_right[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_right[3]);

	//�ظ�
	GLfloat f_bottom[4][3] = {
		{ sx, -1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -sz },
		{ sx, -1.0f, -sz },
	};
	GetNormal(f_bottom[0], f_bottom[1], f_bottom[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 0);
	glVertex3fv(f_bottom[0]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_bottom[1]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_bottom[2]);
	glTexCoord2f(1, 1);
	glVertex3fv(f_bottom[3]);

	//�޸�
	GLfloat f_behind[4][3] = {
		{ sx, -1.0f, -sz },
		{ -1.0f, -1.0f, -sz },
		{ -1.0f, h, -sz },
		{ sx, h, -sz },
	};
	GetNormal(f_behind[0], f_behind[1], f_behind[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(0, 0);
	glVertex3fv(f_behind[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_behind[1]);
	glTexCoord2f(1, 1);
	glVertex3fv(f_behind[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_behind[3]);

	//�ո�
	GLfloat f_front[4][3] = {
		{ sx, h, 1.0f },
		{ -1.0f, h, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ sx, -1.0f, 1.0f },
	};
	GetNormal(f_front[0], f_front[1], f_front[2], normal);
	glNormal3fv(normal);
	glTexCoord2f(1, 1);
	glVertex3fv(f_front[0]);
	glTexCoord2f(0, 1);
	glVertex3fv(f_front[1]);
	glTexCoord2f(0, 0);
	glVertex3fv(f_front[2]);
	glTexCoord2f(1, 0);
	glVertex3fv(f_front[3]);

	glEnd();  // ������ü ��
	glTranslatef(-tx, -ty, -tz); //��ǥ�� x������ -2��ŭ �ű�(���󺹱�)
	glPopMatrix();

}

GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	// ���̳ʸ� �б� ���� ������ ����
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	// ��Ʈ�� ���� ����� �д´�.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}

	// ������ BMP �������� Ȯ���Ѵ�.
	if (header.bfType != 'MB') {
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER ��ġ�� ����.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// ��Ʈ�� �̹��� �����͸� ���� �޸� �Ҵ��� �Ѵ�.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		fclose(fp);
		exit(0);
		return NULL;
	}

	// ��Ʈ�� ���� ����� �д´�.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// ��Ʈ���� ũ�� ����
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
		(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);

	file_width = (*info)->bmiHeader.biWidth;
	file_height = (*info)->bmiHeader.biHeight;

	// ��Ʈ���� ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// ��Ʈ�� �����͸� bit(GLubyte Ÿ��)�� �����Ѵ�.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	return bits;
}

void initializeTextures(void)
{
	GLint level = 0;      /*only one level - no level of detail*/
	GLint components = 3; /*3 means R, G, and B components only*/
	GLint border = 0;     /*no border around the image*/

	glGenTextures(7, textures);

	//textures 0 setting - ��������
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	pBytes = LoadDIBitmap("plank.bmp", &info);
	/*each pixelrow on a byte alignment boundary*/
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	/*define information about the image*/
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	/*ensures that image is not wrapped*/
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	/*chooses mapping type from texels to pixels*/
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/*this says for minification and magnfication choose texel that
	GL_NEAREST chooses the texel nearest the centre of the pixel
	is nearest to the centre of the pixel, rather than GL_LINEAR which
	performs a linear interpolation on the 4 surrounding texels*/
	/*GL_DECAL - this says overwrite pixel with texture colour*/
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	/*an alternative is GL_MODULATE which modulates the lighting
	by the texel value by multiplication*/

	//textures 1 setting - ��� �븮��  
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("marble.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 2 setting - ���� �̹���
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	pBytes = LoadDIBitmap("glass.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 3 setting - ���� ���� �̹���
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	pBytes = LoadDIBitmap("block.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 4 setting - ������ �̹���(�⺻���� - 'c' Ű�� �̿��ؼ� ���ð� ������ ����)
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	pBytes = LoadDIBitmap("row_resolution.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 5 setting - Ǯ �̹���(���)
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	pBytes = LoadDIBitmap("grass.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 6 setting - ���� �̹���(����)
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	pBytes = LoadDIBitmap("road.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 7 setting - ���� �Ʒ�(���)
	glBindTexture(GL_TEXTURE_2D, textures[7]);
	pBytes = LoadDIBitmap("tree_below.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 8 setting - ���� ��(��)
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	pBytes = LoadDIBitmap("leaf.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*this enables texturing*/
	glEnable(GL_TEXTURE_2D);
	
	//http://www0.cs.ucl.ac.uk/staff/a.steed/book_tmp/CGVE/code/Lib3D1.0/maintexture.c ����

	// �ؽ��� ������ ���� �ڵ�
	// �ؽ���0 ����
	//glGenTextures(1, textures);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//pBytes = LoadDIBitmap("texture1.bmp", &info);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 100, 100, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	//glEnable(GL_TEXTURE_2D); //�ؽ��� ������ �����ϰ� �Ѵ�. 
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);  // ���δ� �߶󳽴�  
}

int main(int argc, char** argv)
{
	// glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// actual window size
	glutInitWindowSize(1000, 1000);
	// initial window location, top-left corner
	glutInitWindowPosition(0, 0);
	// create window with title ��simple��
	glutCreateWindow("HW3_13010979_������");
	printf("������ â�� �����ƽ��ϴ�.\n");
	// control the mouse Movement; callback Func
	glutMouseFunc(mouseMovement);
	// ���콺 ��� �Լ�
	glutMotionFunc(mouseMotion);
	// Ű���� �̺�Ʈ ȣ��
	glutKeyboardFunc(myKeyboard);
	// call mydisplay() function
	glutDisplayFunc(mydisplay);
	// ������ resize �̺�Ʈ�� ���� �ݹ��Լ��� �߰��Ѵ�.
	glutReshapeFunc(myreshape);
	// call init() function - Run only one time at the beginning

	glutIdleFunc(spinCube);


	init();
	// main event loop, do not use exit()
	glutMainLoop();
}
