/*mouseMovement
컴퓨터그래픽스 과제3 - 3D 오브첵트 구현 및 마우스/키보드 값으로 도형 변형시키기
외부추가요소 - 도로, 운동장, 나무
세종대학교 컴퓨터공학과
13010979, 임지훈
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

/*Trackball를 위한 변수 및 초기화*/
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
GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //안개색상(회색)
GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info);
GLubyte *pBytes; // 데이터를 가리킬 포인터
BITMAPINFO *info; // 비트맵 헤더 저장할 변수
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
int prevMX; // 이전 마우스 클릭 x좌표
int prevMY; // 이전 마우스 클릭 y좌표
int isRMB = 0; // is Right Mouse Button
int isMMB = 0; // is Middle Mouse Button

GLfloat fovv = 45.0f;
GLUquadric* qd_obj;
GLdouble poleX, poleY, poleZ; //회전축
							  //GLdouble angle;  trackball상수와 중복
							  //int startX, startY; trackball 상수와 중복
double xAngle, yAngle, zAngle;

//빛 변수 관련 세팅
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
float color[3] = { 0.2, 0.4, 0.6 }; //기본 색
float color0_1[3] = { 0.6, 0.2, 0.6 }; //윗아래 색
float color0_2[3] = { 0.6, 0.2, 0.6 }; //위아래 색
float color1[3] = { 0.2, 0.4, 0.6 }; //옆면 색
float color2[3] = { 0.2, 0.4, 0.6 }; //옆면 색

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

/*Trackball 관련 구현 함수*/
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
	
    //땅
	glColor3f(0.6f, 0.6f, 0.6f);
	m_diffuse[0] = 0.3f; m_diffuse[1] = 0.3f; m_diffuse[2] = 0.3f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	makeBuilding(-30, -1, 20, 60, 0.2f, 120, 0, 4);
	//운동장
	glColor3f(0.0f, 0.8f, 0.0f);
	m_diffuse[0] = 0.0f; m_diffuse[1] = 0.8f; m_diffuse[2] = 0.0f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	makeBuilding(-23, 0, -67, 40, -0.5f, 15, 0, 5);

	//나무
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

	//도로
	glColor3f(0.3f, 0.3f, 0.3f);
	m_diffuse[0] = 0.35f; m_diffuse[1] = 0.35f; m_diffuse[2] = 0.35f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	makeBuilding(27, 0, 20.0f, 2, -0.5f, 110, 0, 6);
	makeBuilding(28.0f, 0, -91.0f, 2, -0.5f, 58, 90, 6);
	//건물
	glColor3f(0.77f, 0.77f, 0.7f);
	m_diffuse[0] = 0.75f; m_diffuse[1] = 0.75f; m_diffuse[2] = 0.75f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);

	/*
	//모드 숫자에 따른 텍스쳐(맨 마지막 파라미터 값)
	0 - 나무판자
	1 - 흰색 대리석
	2 - 유리
	3 - 갈색 벽돌
	4 - 돌맹이
	5 - 풀
	6 - 도로
	7 - 나무기둥
	8 - 잎사귀
	*/

	makeBuilding(10, 0, -2, 6, 15, 5, 0, 0); //a건물
	makeBuilding(-24, 0, 16, 3, 9, 13, 0, 0); //b건물
	makeBuilding(-14, 0, 16, 3, 9, 13, 0, 1); //c건물
	makeBuilding(-4, 0, 16, 3, 9, 13, 0, 2); //d건물
	makeBuilding(-26, 0, -15, 12, 5, 4, 0, 3); //e건물
	makeBuilding(-26, 0, -27, 12, 5, 4, 0, 1); //f건물
	makeBuilding(-26, 0, -39, 12, 5, 4, 0, 3); //g건물
	makeBuilding(-7, 0, -20, 2, 4, 10, 0, 2); //k건물
	makeBuilding(-7, 0, -30, 5, 18, 12, 0, 0); //h건물
	makeBuilding(-2, 0, -30, 11, 3, 2, 0, 1); //h건물 옆 이음새a
	makeBuilding(-2, 0, -40, 11, 3, 2, 0, 2); //h건물 옆 이음새b
	makeBuilding(9, 0, -30, 9, 10, 12, 0, 1); //j건물
	makeBuilding(0, 0, -50, 14, 7, 10, 0, 3); //i건물
	makeBuilding(18, 0, 2, 1, 3, 8, 0, 2); //추가a건물

	glPopMatrix();

	glutSwapBuffers();  // 더블버퍼링 사용시엔 glFlush() 대신 이거 써야 함
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0); // black clear color, opaque window
	glClearDepth(1.0f); // 깊이를 1로 초기화한다.
						//glColor3f(1.0, 1.0, 1.0); // white
						//glMatrixMode(GL_MODELVIEW);
	glEnable(GL_FOG);
	glHint(GL_FOG_HINT, GL_DONT_CARE);

	camera_init_x = camera_x;
	camera_init_y = camera_y;

	// 텍스쳐 매핑을 위한 코딩
	// 텍스쳐0 설정
	//glGenTextures(1, textures);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//pBytes = LoadDIBitmap("texture1.bmp", &info);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 100, 100, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	//glEnable(GL_TEXTURE_2D); //텍스쳐 매핑이 가능하게 한다. 
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);  // 내부는 잘라낸다  


	glEnable(GL_DEPTH_TEST); //깊이 테스트를 가능한게 설정한다.
	//setFog(0.3f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING); //조명을 사용하기 위해서 ENABLE 시켜준다.
	glEnable(GL_LIGHT0); // 조명 중 0번 조명을 사용한다.

	glShadeModel(GL_SMOOTH); //부드러운 쉐이딩이 가능하다.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// 개선된 관점으로 설정
													  //glLoadIdentity();
													  //glOrtho3D(-1.0, 1.0, -1.0, 1.0); // screen size (-1.0,-1.0) to (1.0,1.0)

													  //조명의 성질 설정
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


	// 빛 설정(참고 - https://skyfe79.gitbooks.io/opengl-tutorial/content/chapter10.html)
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight[0]); //Ambient 조명의 성질을 설정
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight[0]); //Diffuse 조명의 성질을 설정
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular[0]); //Specular 조명의 성질을 설정 
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition[0]); //조명의 위치를 설정

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight[1]); //Ambient 조명의 성질을 설정
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight[1]); //Diffuse 조명의 성질을 설정
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular[1]); //Specular 조명의 성질을 설정 
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition[1]); //조명의 위치를 설정

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight[2]); //Ambient 조명의 성질을 설정
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight[2]); //Diffuse 조명의 성질을 설정
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular[2]); //Specular 조명의 성질을 설정 
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition[2]); //조명의 위치를 설정

	//기본 설정 값 - 빛 3개
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	//재질 설정
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
	GLfloat aspect = (GLfloat)width / (GLfloat)height; //윈도우 창의 비율 계산
	glViewport(0, 0, width, height);       // 뷰포트를 지정한다.
	winWidth = width;
	winHeight = height;

	//setCamera(aspect);
	glMatrixMode(GL_PROJECTION);  // 매트릭스모드를 프로젝션으로 설정
	glLoadIdentity();             // 재설정함        
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
	printf("이전 %d, %d", prevMX, mouseCurPositionX);
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
	glMatrixMode(GL_PROJECTION);  // 매트릭스모드를 프로젝션으로 설정
	glLoadIdentity();             // 재설정함        
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
			//마우스가 오른쪽으로 이S동
			printf("마우스 오른쪽으로 이동 -> x축으로 회전 중\n");
			if (fovv >= 0) fovv = fovv - 0.8f;
			printf("%f", fovv);
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			//myreshape(winWidth, winHeight);
			//xAngle += (x - startX) / 3.6;
			glutPostRedisplay();
		}
		if (prevMX > x) {
			//마우스가 왼쪽으로 이동
			printf("마우스 왼쪽으로 이동 -> y축으로 회전 중\n");
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
			//마우스가 오른쪽으로 이S동
			camera_x -= (x - startX) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}
		if (prevMX > x) {
			//마우스가 왼쪽으로 이동
			camera_x -= (x - startX) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}
		if (prevMX < y) {
			//마우스가 위로 이동
			camera_y += (y - startY) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}
		if (prevMX > y) {
			//마우스가 아래쪽으로 이동
			camera_y += (y - startY) / 3.6;
			setCamera((GLfloat)winWidth / (GLfloat)winHeight);
			glutPostRedisplay();
		}


	}

	//마지막 마우스 커서 위치를 저장
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
		// 빛 없음
		printf("0번키가 눌렸습니다. - 빛 없음 모드\n");
		glDisable(GL_LIGHTING);
		glutPostRedisplay(); // 변경된 사항을 다시 그림
		break;
	case '1':
		// 빛 1
		printf("1번키가 눌렸습니다. - 빛 1개 모드\n");
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT0);

		glutPostRedisplay(); // 변경된 사항을 다시 그림
		break;
	case '2':
		// 빛 1 + 2
		printf("2번키가 눌렸습니다. - 빛 2개 모드\n");
		glEnable(GL_LIGHTING);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glutPostRedisplay(); // 변경된 사항을 다시 그림
		break;
	case '3':
		// 빛 1 + 2 + 3
		printf("3번키가 눌렸습니다. - 빛 3개 모드\n");
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glutPostRedisplay(); // 변경된 사항을 다시 그림
		break;

	case '5':
		// 5번을 누르면 텍스쳐 모드 중단 또는 실행
		(count_texture++ % 2 == 0) ? glDisable(GL_TEXTURE_2D) : glEnable(GL_TEXTURE_2D);
		glutPostRedisplay(); // 변경된 사항을 다시 그림
		break;
	case '8':
		// 8번을 누르면 필터링 모드 변경
		//textures 4 setting - 돌맹이 이미지(기본밑판 - 'c' 키를 이용해서 심플과 스무스 변경)
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		pBytes = LoadDIBitmap("row_resolution.bmp", &info);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (count_filtering++ % 2 == 0) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			printf("돌맹이 이미지(밑바닥) - 필터링 모드 : GL_LINEAR\n");
		}
		else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			printf("돌맹이 이미지(밑바닥) - 필터링 모드 : GL_NEAREST\n");
		}
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glutPostRedisplay(); // 변경된 사항을 다시 그림
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
		printf("0~3중에 하나를 눌러주십시오.\n");
		break;
	}
}

void GetNormal(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat normal[3])
{
	GLfloat ba[3];
	GLfloat ca[3];
	GLfloat n[3];

	// 두 정점간에 벡터를 계산함
	ba[0] = b[0] - a[0]; ba[1] = b[1] - a[1]; ba[2] = b[2] - a[2];
	ca[0] = c[0] - a[0]; ca[1] = c[1] - a[1]; ca[2] = c[2] - a[2];

	// 벡터 외적 곱 합
	n[0] = ba[1] * ca[2] - ca[1] * ba[2];
	n[1] = ca[0] * ba[2] - ba[0] * ca[2];
	n[2] = ba[0] * ca[1] - ca[0] * ba[1];

	// 정규화함
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
	glTranslatef(tx, ty, tz); //좌표를 x축으로 2만큼 옮김
	glBindTexture(GL_TEXTURE_2D, textures[7]); 
	glBegin(GL_QUADS); //정육면체 시작
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	m_diffuse[0] = 0.7f; m_diffuse[1] = 0.25f; m_diffuse[2] = 0.12f; m_diffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
	//윗면
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

	//왼쪽면
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

	//오른쪽면
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

	//밑면
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

	//뒷면
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

	//앞면
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

	glEnd();  // 정육면체 끝

    // 나무 윗 부분 시작
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

	glTranslatef(-tx, -ty, -tz); //좌표를 x축으로 -2만큼 옮김(원상복귀)
	glPopMatrix();

}

void makeBuilding(GLfloat tx, GLfloat ty, GLfloat tz, GLfloat sx, GLfloat h, GLfloat sz, double ry, int mode)  //매개변수 - 원점에서 이동할 위치, 크기, 회전각 x, y -> 오른쪽과 뒤쪽이 늘어남
{
	GLfloat normal[3];

	glPushMatrix();
	//glColor3fv(color1); //색 설정
	glTranslatef(tx, ty, tz); //좌표를 x축으로 2만큼 옮김
							  //glRotatef(rx, 1.0f, 0.0f, 0.0f);
	glRotatef(ry, 0.0f, 1.0f, 0.0f);
	//glRotatef(zAngle, 0.0f, 0.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, textures[mode]); // mode는 0부터 5까지
	glBegin(GL_QUADS); //정육면체 시작
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	//윗면
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

	//왼쪽면
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

	//오른쪽면
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

	//밑면
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

	//뒷면
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

	//앞면
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

	glEnd();  // 정육면체 끝
	glTranslatef(-tx, -ty, -tz); //좌표를 x축으로 -2만큼 옮김(원상복귀)
	glPopMatrix();

}

GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	// 바이너리 읽기 모드로 파일을 연다
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	// 비트맵 파일 헤더를 읽는다.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}

	// 파일이 BMP 파일인지 확인한다.
	if (header.bfType != 'MB') {
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER 위치로 간다.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// 비트맵 이미지 데이터를 넣을 메모리 할당을 한다.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		fclose(fp);
		exit(0);
		return NULL;
	}

	// 비트맵 인포 헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵의 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
		(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);

	file_width = (*info)->bmiHeader.biWidth;
	file_height = (*info)->bmiHeader.biHeight;

	// 비트맵의 크기만큼 메모리를 할당한다.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵 데이터를 bit(GLubyte 타입)에 저장한다.
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

	//textures 0 setting - 나무판자
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

	//textures 1 setting - 흰색 대리석  
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	pBytes = LoadDIBitmap("marble.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 2 setting - 유리 이미지
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	pBytes = LoadDIBitmap("glass.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 3 setting - 갈색 벽돌 이미지
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	pBytes = LoadDIBitmap("block.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 4 setting - 돌맹이 이미지(기본밑판 - 'c' 키를 이용해서 심플과 스무스 변경)
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	pBytes = LoadDIBitmap("row_resolution.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 5 setting - 풀 이미지(운동장)
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	pBytes = LoadDIBitmap("grass.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 6 setting - 도로 이미지(도로)
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	pBytes = LoadDIBitmap("road.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 7 setting - 나무 아래(기둥)
	glBindTexture(GL_TEXTURE_2D, textures[7]);
	pBytes = LoadDIBitmap("tree_below.bmp", &info);
	glTexImage2D(GL_TEXTURE_2D, level, components, 256, 256, border, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//textures 8 setting - 나무 위(잎)
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
	
	//http://www0.cs.ucl.ac.uk/staff/a.steed/book_tmp/CGVE/code/Lib3D1.0/maintexture.c 참고

	// 텍스쳐 매핑을 위한 코딩
	// 텍스쳐0 설정
	//glGenTextures(1, textures);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//pBytes = LoadDIBitmap("texture1.bmp", &info);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 100, 100, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);
	//glEnable(GL_TEXTURE_2D); //텍스쳐 매핑이 가능하게 한다. 
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);  // 내부는 잘라낸다  
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
	// create window with title “simple”
	glutCreateWindow("HW3_13010979_임지훈");
	printf("윈도우 창이 생성됐습니다.\n");
	// control the mouse Movement; callback Func
	glutMouseFunc(mouseMovement);
	// 마우스 모션 함수
	glutMotionFunc(mouseMotion);
	// 키보드 이벤트 호출
	glutKeyboardFunc(myKeyboard);
	// call mydisplay() function
	glutDisplayFunc(mydisplay);
	// 윈도우 resize 이벤트를 위한 콜백함수를 추가한다.
	glutReshapeFunc(myreshape);
	// call init() function - Run only one time at the beginning

	glutIdleFunc(spinCube);


	init();
	// main event loop, do not use exit()
	glutMainLoop();
}
