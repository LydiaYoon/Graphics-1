#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include <glut.h>

#pragma warning (disable:4996)

int w = 500;
int h = 500;

static GLfloat theta[] = {0.0, 0.0, 0.0};
static GLint axis = 2;
static GLdouble viewer[] = {0.0, 0.0, 5.0}; //�������� �ʱ� ��ġ

float track[3] = {0.0, 0.0, 0.0};
float start_line[3] = {0.25, -0.5, 0.0};
float car[3] = {-0.1, -0.08, 0.015};
float rival[3] = {-0.1, 0.08, 0.015};
float obs1[3] = {0.5, 0.1, -0.045};
float obs2[3] = {0.5, -0.2, -0.045};
float tank[3] = {0.0, 0.0, -0.05};
float ball[3] = {0.0, 0.0, 0.0};

bool up_down = false;

float rival_m[3] = {rival[0], rival[1], rival[2]};
float rival_d[3] = {-0.01, 0, 0};

float obs1_m[3] = {obs1[0], obs1[1], obs1[2]};
float obs2_m[3] = {obs2[0], obs2[1], obs2[2]};
float obs1_d[3] = {0.01, 0.01, 0.0};
float obs2_d[3] = {0.013, 0.013, 0.0};

float tank_m[3] = {tank[0], tank[1], tank[2]};
float tank_d[3] = {0.01, 0.01, 0.0};

float upper_m = 0.0;
float upper_d = 10.0;

float arm_m = 0.0;
float arm_d = 5.0;

float ball_m = 0.0;
float ball_d = 0.03;
float ball_upper = 0.0;
float ball_arm = 0.0;

float build[3][3];
int building_count = 0;
int building_shape = 0;
int build_shape[3];
bool focus = true;
bool menu_P = false;
bool menu_O = true;
int time = 0;

float wheel_theta = 0.0;
float wheel_theta2 = 0.0;

GLfloat mat_Ambient[] = {0.5, 0.5, 0.5, 0.5};
GLfloat mat_Diffuse[] = {0.5, 0.5, 0.5, 0.5};
GLfloat mat_Specular[] = {0.5, 0.5, 0.5, 0.5};
GLfloat mat_Shininess[] = {100.0};

float light0_Position[] = {50.0, 50.0, 50.0, 1.0};
GLfloat light0_Ambient[] = {0.5, 0.5, 0.5, 1.0};
GLfloat light0_Diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light0_Specular[] = {1.0, 1.0, 1.0, 1.0};

float light1_Position[] = {50.0, 50.0, 5.0, 1.0};
GLfloat light1_Ambient[] = {1.0, 0.0, 0.0, 1.0};
GLfloat light1_Diffuse[] = {1.0, 0.0, 0.0, 1.0};
GLfloat light1_Specular[] = {1.0, 0.0, 0.0, 1.0};

GLfloat vertices[][3]=
{
	{-0.1, -0.1, -0.1},
	{0.1, -0.1, -0.1},
	{0.1, 0.1, -0.1},
	{-0.1, 0.1, -0.1},
	{-0.1, -0.1, 0.1},
	{0.1, -0.1, 0.1},
	{0.1, 0.1, 0.1},
	{-0.1, 0.1, 0.1},
};
GLuint MyTextureObject[2];

GLubyte* ReadBMP (const char* filename, int* width, int* height)
{
	FILE * file = fopen(filename, "rb");
	if (file == NULL) return NULL;

	GLubyte info[54];
	fread (info, sizeof(GLubyte), 54, file);

	*width = *(int*)(info+18);
	*height = *(int*)(info+22);

	int size = 3 * (*width) * (*height);
	GLubyte* data = (GLubyte*)malloc(size);
	fread(data, sizeof(GLubyte), size, file);
	fclose(file);

	for (int i=0; i<size; i+=3)
	{
		GLubyte temp = data[i];
		data[i] = data[i+2];
		data[i+2] = temp;
	}

	return data;
}

GLuint LoadTexture(const char* filename)
{
	GLuint texture;

	GLubyte * data;
	int width, height;
	width = height = 0;
	data = ReadBMP(filename, &width, &height);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(data);

	return texture;
}

void FreeTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

void myInit();

void polygon(int a, int b, int c, int d)
{
 glBegin(GL_POLYGON);
 {
  glTexCoord2d(0.0, 0.0); glVertex3fv(vertices[a]);
  glTexCoord2d(0.0, 1.0); glVertex3fv(vertices[b]);
  glTexCoord2d(1.0, 1.0); glVertex3fv(vertices[c]);
  glTexCoord2d(1.0, 0.0); glVertex3fv(vertices[d]);
 }
 glEnd();
}

void myCube(int i)
{
	glBindTexture(GL_TEXTURE_2D, MyTextureObject[i]);
	polygon(2, 1, 0, 3);//�Ʒ�
	polygon(3, 7, 6, 2);
	polygon(0, 4, 7, 3);
	polygon(2, 6, 5, 1);
	polygon(5, 6, 7, 4);//��
	polygon(1, 5, 4, 0);
}

void drawXYZ()
{
	///����///
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glColor3ub(125, 125, 125);
	glutSolidSphere(0.01, 14, 15);
	glPopMatrix();

	///X��///
	glPushMatrix();
	glTranslatef(0.8, 0.0, 0.0);
	glColor3ub(255, 0, 0);
	glutSolidSphere(0.01, 14, 15);
	glPopMatrix();

	///Y��///
	glPushMatrix();
	glTranslatef(0.0, 0.8, 0.0);
	glColor3ub(0, 255, 0);
	glutSolidSphere(0.01, 14, 15);
	glPopMatrix();

	///Z��///
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.8);
	glColor3ub(0, 0, 255);
	glutSolidSphere(0.01, 14, 15);
	glPopMatrix();
}

void drawTrack()
{
	glColor3ub(255, 255, 255); 

	///Ʈ��///
	glPushMatrix();
	glTranslatef(0.0, 0.5, -0.1);
	glScalef(6.5, 1.5, 0.5);
	myCube(1);
	glScalef(1/6.5, 1/1.5, 1/0.5);
	glTranslatef(0.0, -0.5, 0.0);

	glTranslatef(0.0, -0.5, 0.0);
	glScalef(6.5, 1.5, 0.5);
	myCube(1);
	glScalef(1/6.5, 1/1.5, 1/0.5);
	glTranslatef(0.0, 0.5, 0.0);

	glTranslatef(0.5, 0.0, 0.0);
	glRotatef(90, 0.0, 0.0, 1.0);
	glScalef(6.5, 1.5, 0.5);
	myCube(1);
	glScalef(1/6.5, 1/1.5, 1/0.5);
	glRotatef(-90, 0.0, 0.0, 1.0);
	glTranslatef(-0.5, 0.0, 0.0);

	glTranslatef(-0.5, 0.0, 0.0);
	glRotatef(90, 0.0, 0.0, 1.0);
	glScalef(6.5, 1.5, 0.5);
	myCube(1);
 
	glPopMatrix();
	//////
}

void drawStartLine()
{
	glColor3ub(0, 0, 0); // ������

	///Ʈ��///
	glPushMatrix();
	glTranslatef(0.0, 0.0, -0.099);
	glScalef(0.3, 1.5, 0.5);
	glutSolidCube(0.2);
	glScalef(1/0.3, 1/1.5, 1/0.5);
	glTranslatef(0.0, 0.0, 0.095);
	glPopMatrix();
}

void drawCar()
{
	glColor3ub(255, 0, 0); // ������
	///�� ��ü///
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glScalef(0.5, 0.4, 0.4);
	glutSolidCube(0.2);
	//glColor3ub(0, 0, 0);
	//glutWireCube(0.2);
	glPopMatrix();
	//////

	glColor3ub(0, 0, 0); // ������
	///�� ����///
	glPushMatrix();
	glTranslatef(0.05, 0.04, -0.04);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-0.05, -0.04, 0.0);

	glTranslatef(0.05, -0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-0.05, 0.04, 0.0);

	glTranslatef(-0.05, 0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(0.05, -0.04, 0.0);

	glTranslatef(-0.05, -0.04, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glPopMatrix();
	//////
}

void drawRival()
{
	glColor3ub(255, 200, 20); // �����
	///�� ��ü///
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glScalef(0.5, 0.4, 0.4);
	glutSolidCube(0.2);
	//glColor3ub(0, 0, 0);
	//glutWireCube(0.2);
	glScalef(1/0.5, 1/0.4, 1/0.4);
	//////

	///��///
	glColor3ub(255, 200, 20);
	glTranslatef(0.03, 0.02, 0.03);
	glutSolidCone(0.02, 0.03, 10, 5);
	glTranslatef(-0.03, -0.02, -0.03);

	glColor3ub(255, 200, 20);
	glTranslatef(0.0, 0.02, 0.03);
	glutSolidCone(0.02, 0.03, 10, 5);
	glTranslatef(0.0, -0.02, -0.03);

	glColor3ub(255, 200, 20);
	glTranslatef(-0.03, 0.02, 0.03);
	glutSolidCone(0.02, 0.03, 10, 5);
	glTranslatef(0.03, -0.02, -0.03);

	glColor3ub(255, 200, 20);
	glTranslatef(0.03, -0.02, 0.03);
	glutSolidCone(0.02, 0.03, 10, 5);
	glTranslatef(-0.03, 0.02, -0.03);

	glColor3ub(255, 200, 20);
	glTranslatef(0.0, -0.02, 0.03);
	glutSolidCone(0.02, 0.03, 10, 5);
	glTranslatef(0.0, 0.02, -0.03);

	glColor3ub(255, 200, 20);
	glTranslatef(-0.03, -0.02, 0.03);
	glutSolidCone(0.02, 0.03, 10, 5);
	glTranslatef(0.03, 0.02, -0.03);
	//////

	glColor3ub(0, 0, 0); // ������
	///�� ����///
	glPushMatrix();
	glTranslatef(0.05, 0.04, -0.04);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-0.05, -0.04, 0.0);

	glTranslatef(0.05, -0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-0.05, 0.04, 0.0);

	glTranslatef(-0.05, 0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(0.05, -0.04, 0.0);

	glTranslatef(-0.05, -0.04, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glPopMatrix();
	//////
}

void drawObs1()
{
	glColor3ub(30, 170, 0); // �ʷϻ�

	glPushMatrix();
	///����///
	glTranslatef(0.0, 0.0, 0.0);
	glutSolidCone(0.04, 0.06, 10, 5);
	//glColor3ub(0, 0, 0);
	//glutWireCone(0.04, 0.06, 10, 5);
	//glColor3ub(30, 170, 0);
	///��///
	glTranslatef(0.0, 0.0, 0.07);
	glutSolidSphere(0.025, 14, 15);
	//glColor3ub(0, 0, 0);
	//glutWireSphere(0.025, 14, 15);
	//////
	glPopMatrix();
}

void drawObs2()
{
	glColor3ub(180, 255, 30); // ���λ�

	glPushMatrix();
	///����///
	glTranslatef(0.0, 0.0, 0.0);
	glutSolidCone(0.04, 0.06, 10, 5);
	//glColor3ub(0, 0, 0);
	//glutWireCone(0.04, 0.06, 10, 5);
	//glColor3ub(180, 255, 30);
	///ť��///
	glTranslatef(0.0, 0.0, 0.07);
	glRotatef(45.0, 45.0, 45.0, 1.0);
	glutSolidCube(0.05);
	//glColor3ub(0, 0, 0);
	//glutWireCube(0.05);
	//////
	glPopMatrix();
}

void drawTank()
{
	GLUquadricObj * quadric;
	quadric = gluNewQuadric();

	glColor3ub(0, 100, 255); // �ϴû�

	///��ũ �ٵ�///
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glScalef(0.5, 0.4, 0.25);
	glutSolidCube(0.2);
	//glColor3ub(0, 0, 0);
	//glutWireCube(0.2);
	//glColor3ub(0, 100, 255);
	glScalef(1/0.5, 1/0.4, 1/0.25);
	//////

	///��ũ ����///
	glTranslatef(0.0, 0.0, 0.025);
	glRotatef(upper_m, 0.0, 0.0, 1.0);
	glutSolidSphere(0.03, 10, 10);
	//glColor3ub(0, 0, 0);
	//glutWireSphere(0.03, 10, 10);
	//glColor3ub(0, 100, 255);
	//////

	///��ũ ����///
	glTranslatef(0.0, 0.0, 0.01);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(arm_m, 1.0, 0.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
	gluCylinder(quadric, 0.01, 0.01, 0.15, 32, 32);
	glScalef(1/0.5, 1/0.5, 1/0.5);
	glRotatef(-arm_m, 1.0, 0.0, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	//////
	glPopMatrix();


	///��ũ ����///
	glPushMatrix();
	glTranslatef(0.05, 0.04, -0.02);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-0.05, -0.04, 0.0);

	glTranslatef(0.0, -0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-0.0, 0.04, 0.0);

	glTranslatef(0.05, -0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-0.05, 0.04, 0.0);

	glTranslatef(-0.05, 0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(0.05, -0.04, 0.0);

	glTranslatef(-0.0, 0.04, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glRotatef(-wheel_theta2, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(0.0, -0.04, 0.0);

	glTranslatef(-0.05, -0.04, 0.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glRotatef(wheel_theta2, 0.0, 0.0, 1.0);
	glutSolidTorus(0.01, 0.02, 10, 15);
	glPopMatrix();
}

void drawBall()
{

	glPushMatrix();
	glTranslatef(ball[0], ball[1], ball[2]);

	glTranslatef(0.0, 0.0, 0.025);
	glRotatef(ball_upper-90.0, 0.0, 0.0, 1.0);

	glTranslatef(0.0, 0.0, 0.01);
	glRotatef(ball_arm, 0.0, 1.0, 0.0);

	glTranslatef(0.08, 0.0, 0.0);

	glColor3ub(0, 100, 255); // �ϴû�
	glTranslatef(ball_m, 0.0, 0.0);
	glutSolidSphere(0.01, 14, 15);
	//glColor3ub(0, 0, 0);
	//glutWireSphere(0.01, 10, 10);

	glPopMatrix();
}

void drawBuild1()
{
	glColor3ub(255, 255, 255); 
	glPushMatrix();
	glTranslatef(0.0, 0.0, -0.05);

	glScalef(0.6, 0.6, 0.9);
	myCube(0);
	glScalef(1/0.6, 1/0.6, 1/0.9);
	glPopMatrix();
}

void drawBuild2()
{
	glColor3ub(255, 255, 255); 
	glPushMatrix();
	glTranslatef(0.0, 0.0, -0.05);

	glScalef(0.6, 0.6, 0.9);
	myCube(0);
	glScalef(1/0.6, 1/0.6, 1/0.9);
 
	glTranslatef(0.0, 0.0, 0.1);
	glScalef(0.4, 0.4, 0.2);
	glRotatef(45.0, 0.0, 0.0, 1.0);
	myCube(0);
	glScalef(1/0.4, 1/0.4, 1/0.4);
	glPopMatrix();
}

void drawBuild3()
{
	glColor3ub(255, 255, 255); 
	glPushMatrix();
	glTranslatef(0.0, 0.0, -0.05);

	glScalef(0.6, 0.6, 0.9);
	myCube(0);
	glScalef(1/0.6, 1/0.6, 1/0.9);

	glTranslatef(0.0, 0.0, 0.1);
	glScalef(0.4, 0.4, 0.2);
	glRotatef(45.0, 0.0, 0.0, 1.0);
	myCube(0);
	glScalef(1/0.4, 1/0.4, 1/0.4);
 
	glTranslatef(0.0, 0.0, 0.06);
	glRotatef(45.0, 0.0, 0.0, 1.0);
	glScalef(0.2, 0.2, 0.2);
	myCube(0);

	glPopMatrix();
}

void move_obs(float obs[3], float obs_m[3], float obs_d[3])
{
	obs_m[0] += obs_d[0];

	if (obs_m[0] > obs[0]+0.07)
		obs_d[0] = -0.01;
	else if (obs_m[0] < obs[0]-0.07)
		obs_d[0] = +0.01;
}

void move_tank(float tank[3], float tank_m[3], float tank_d[3])
{
	///��ũ �¿� ������///
	tank_m[0] += tank_d[0];

	if (tank_m[0] > tank[0]+0.05)
		tank_d[0] = -0.01;
	else if (tank_m[0] < tank[0]-0.05)
		tank_d[0] = +0.01;
	//////

	///���� �¿� ȸ��///
	upper_m += upper_d;
	//////

	///���� ���� ������///
	arm_m += arm_d;
	if (arm_m > arm_d-10.0)
		arm_d = -5.0;
	else if (arm_m < arm_d-20.0)
		arm_d = +5.0;
	//////
}

void move_ball()
{
	ball_m += ball_d;
}

void move_rival()
{

	if (rival_m[0] < rival[0]-0.55 && rival_d[1] == 0.0)
	{
		rival_d[0] = 0.0;
		rival_d[1] = +0.01;
	}
	else if (rival_m[1] > rival[1]+0.82 && rival_d[0] == 0.0)
	{
		rival_d[0] = +0.01;
		rival_d[1] = 0.0;
	}
	else if (rival_m[0] > rival[0]+0.27 && rival_d[1] == 0.0)
	{
		rival_d[0] = 0.0;
		rival_d[1] = -0.01;
	}
	else if (rival_m[1] < rival[1]-0.0 && rival_d[0] == 0.0)
	{
		rival_d[0] = -0.01;
		rival_d[1] = 0.0;
	}


	for (int i=0; i<2; i++)
		rival_m[i] += rival_d[i];

}

void rotate_wheels()
{
	wheel_theta2 += 10.0;
}

void myDisplay( )
{
	glLineWidth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ��-���� ��� ���� ������ ��ġ�� ����
	glLoadIdentity();
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // ����ü�� ȸ����Ų��
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);

	//drawXYZ();

	glPushMatrix();
	///Ʈ�� �׸���///
	glTranslatef(track[0], track[1], track[2]);
	drawTrack();
	//////

	///������ġ �׸���///
	glTranslatef(start_line[0], start_line[1], start_line[2]);
	drawStartLine();
	glTranslatef(-start_line[0], -start_line[1], -start_line[2]);
	//////

	///�ڵ��� �׸���///
	glTranslatef(start_line[0]+car[0], start_line[1]+car[1], start_line[2]+car[2]);
	if (up_down == false)
		drawCar();
	else if (up_down == true)
	{
		glRotatef(90, 0.0, 0.0, 1.0);
		drawCar();
		glRotatef(-90, 0.0, 0.0, 1.0);
	}
	glTranslatef(-(start_line[0]+car[0]), -(start_line[1]+car[1]), -(start_line[2]+car[2]));
	//////

	///����� �׸���///
	glTranslatef(start_line[0]+rival_m[0], start_line[1]+rival_m[1], start_line[2]+rival_m[2]);
	if (rival_d[0] != 0)
		drawRival();
	else if (rival_d[1] != 0)
	{
		glRotatef(90, 0.0, 0.0, 1.0);
		drawRival();
		glRotatef(-90, 0.0, 0.0, 1.0);
	}
	glTranslatef(-(start_line[0]+rival_m[0]), -(start_line[1]+rival_m[1]), -(start_line[2]+rival_m[2]));
	//////

	///���ع�1 �׸���///
	glTranslatef(obs1_m[0], obs1_m[1], obs1_m[2]);
	drawObs1();
	glTranslatef(-obs1_m[0], -obs1_m[1], -obs1_m[2]);
	//////

	///���ع�2 �׸���///
	glTranslatef(obs2_m[0], obs2_m[1], obs2_m[2]);
	drawObs2();
	glTranslatef(-obs2_m[0], -obs2_m[1], -obs2_m[2]);
	//////

	///��ũ �׸���///
	glTranslatef(tank_m[0], tank_m[1], tank_m[2]);
	drawTank();
	glTranslatef(-tank_m[0], -tank_m[1], -tank_m[2]);
	//////

	///��ź �׸���///
	if ( time % 25 == 0 )
	{
		ball[0] = tank[0];
		ball[1] = tank[1];
		ball[2] = tank[2];

		ball_m = 0.0;
		ball_upper = upper_m;
		ball_arm = arm_m;
		///��ź �׸���///
	}
	drawBall();
	//////

	///���� �׸���///
	if (building_count > 0)
		for (int i=0; i<building_count; i++)
		{
			glTranslatef(build[i][1], build[i][2], 0);
			if (build_shape[i] == 1)
				drawBuild1();
			if (build_shape[i] == 2)
				drawBuild2();
			if (build_shape[i] == 3)
				drawBuild3();
			glTranslatef(-build[i][1], -build[i][2], 0);
		}
	//////

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void myTimer(int value)
{
	move_obs(obs1, obs1_m, obs1_d);
	move_obs(obs2, obs2_m, obs2_d);
	move_tank(tank, tank_m, tank_d);
	move_rival();

	move_ball();

	rotate_wheels();

	glutPostRedisplay();
	time += 1;
	glutTimerFunc(100,myTimer,1);
}

void myMouse(int btn, int state, int x, int y)
{
	if (btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	{
		if (building_count < 3)
		{

			build[building_count][1] = float(x-w/2) / (w/2);
			build[building_count][2] = float(h/2-y) / (h/2);
			build_shape[building_count] = building_shape;

			building_count++;
		}
	}
	myDisplay();
}

void SpecialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		if (car[0] > -0.83)
		car[0] = car[0] - 0.03;
		up_down = false;
			wheel_theta += 10.0;
		myDisplay( );
	}
	if (key == GLUT_KEY_RIGHT) 
	{
		if (car[0] < 0.33)
		car[0] = car[0] + 0.03;
		up_down = false;
			wheel_theta += 10.0;
		myDisplay( );
	}
	if (key == GLUT_KEY_DOWN)  
	{
		if (car[1] > -0.07)
		car[1] = car[1] - 0.03;
		up_down = true;
			wheel_theta += 10.0;
		myDisplay( );
	}
	if (key == GLUT_KEY_UP) 
	{
		if (car[1] < 1.07)
		car[1] = car[1] + 0.03;
		up_down = true;
			wheel_theta += 10.0;
		myDisplay( );
	}

}

void myKey(unsigned char key, int x, int y)
{
	// x�� : q,a // y�� : w,s // z�� : e,d //
	if (key == 'q' || key == 'Q')	viewer[0] -= 1.0;	focus = false;
	if (key == 'a' || key == 'A') viewer[0] += 1.0;	focus = false;
	if (key == 'w' || key == 'W') viewer[1] -= 1.0;	focus = false;
	if (key == 's' || key == 'S') viewer[1] += 1.0;	focus = false;
	if (key == 'e' || key == 'E') viewer[2] -= 1.0;	focus = false;
	if (key == 'd' || key == 'D') viewer[2] += 1.0;	focus = false;

	// x�� : z // y�� : x // z�� : c //
	if (key == 'z' || key == 'Z')
	{
		axis = 0;
		theta[axis] -= 2.0;
		focus = false;
	}
	if (key == 'x' || key == 'X')
	{
		axis = 1;
		theta[axis] -= 2.0;
		focus = false;
	}
	if (key == 'c' || key == 'C')
	{
		axis = 2;
		theta[axis] -= 2.0;
		focus = false;
	}
	if ( theta[axis] > 360.0 ) theta[axis] -= 360.0;


	///���� �̵�///
	if (key == 'j' || key == 'J') // ����
	{
		light1_Position[0] -= 50.0;
	}

	if (key == 'l' || key == 'L') // ������
	{
		light1_Position[0] += 50.0;
	}

	if (key == 'k' || key == 'K') // �Ʒ���  
	{
		light1_Position[1] -= 50.0;
	}

	if (key == 'i' || key == 'I') // ���� 
	{
		light1_Position[1] += 50.0;
	}

	myInit();
	myDisplay();

}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h); // ���� ������ ���
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (menu_O == true)
	{
		viewer[0] = 0;
		viewer[1] = 0;
		viewer[2] = 5.0;
		theta[0] = 0;
		theta[1] = 0;
		theta[2] = 0;

		if (w <= h)
			glOrtho(-1.0, 1.0, -1.0*(GLfloat)h/(GLfloat)w, 1.0*(GLfloat)h/(GLfloat)w, -50.0, 50.0);
		else
			glOrtho(-1.0, 1.0, -1.0*(GLfloat)w/(GLfloat)h, 1.0*(GLfloat)w/(GLfloat)h, -50.0, 50.0);
	}
	if (menu_P == true)
	{
		viewer[0] = 0;
		viewer[1] = 0;
		viewer[2] = 5.0;
		theta[0] = 0;
		theta[1] = 0;
		theta[2] = 0;

		gluPerspective(20.0, w/h, 1.0, 10.0);
	}
	glMatrixMode(GL_MODELVIEW);
}

void myInit()
{
	//glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST); // ������ �� ����
	glEnable(GL_CULL_FACE); // �ĸ� ����

	// ������ ����
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_Ambient);       
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_Diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_Specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_Shininess);
	// LIGHT0 ����
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_Ambient); // �ֺ��� ���� ����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_Diffuse); // �л걤 ���� ����
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_Specular);// �ݻ籤 ���� ����
	glLightfv(GL_LIGHT0, GL_POSITION, light0_Position); // ���� ��ġ ����

	// LIGHT1 ����
	//glLightfv(GL_LIGHT1, GL_AMBIENT, light1_Ambient); // �ֺ��� ���� ����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_Diffuse); // �л걤 ���� ����
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_Specular);// �ݻ籤 ���� ����
	glLightfv(GL_LIGHT1, GL_POSITION, light1_Position); // ���� ��ġ ����

	glEnable(GL_LIGHTING); // ��ü ����Ʈ�� ��ȿ�ϰ� ��
	glEnable(GL_COLOR_MATERIAL); // ���� ���߾ ������ ������Ŵ
	glEnable(GL_LIGHT0); // LIGHT0�� �Ҵ�.
	glEnable(GL_LIGHT1); // LIGHT0�� �Ҵ�.
	glEnable(GL_DEPTH_TEST); // z buffer�� ����Ѵ�.


	glEnable(GL_TEXTURE_2D);
	
	MyTextureObject[0] = LoadTexture("d:\\a.bmp");
	MyTextureObject[1] = LoadTexture("d:\\track.bmp");
}

void light_color(int id)
{
	switch(id)
	{
	case 2: 
		{
			light1_Ambient[0] = 0.5;
			light1_Ambient[1] = 0.0;
			light1_Ambient[2] = 0.0;
			light1_Diffuse[0] = 0.5;
			light1_Diffuse[1] = 0.0;
			light1_Diffuse[2] = 0.0;
			light1_Specular[0] = 0.5;
			light1_Specular[1] = 0.0;
			light1_Specular[2] = 0.0;
			myInit();
		}
		break;
	case 3:
		{
			light1_Ambient[0] = 0.0;
			light1_Ambient[1] = 0.5;
			light1_Ambient[2] = 0.0;
			light1_Diffuse[0] = 0.0;
			light1_Diffuse[1] = 0.5;
			light1_Diffuse[2] = 0.0;
			light1_Specular[0] = 0.0;
			light1_Specular[1] = 0.5;
			light1_Specular[2] = 0.0;
			myInit();
		}
		break;
	case 4:
		{
			light1_Ambient[0] = 0.0;
			light1_Ambient[1] = 0.0;
			light1_Ambient[2] = 0.5;
			light1_Diffuse[0] = 0.0;
			light1_Diffuse[1] = 0.0;
			light1_Diffuse[2] = 0.5;
			light1_Specular[0] = 0.0;
			light1_Specular[1] = 0.0;
			light1_Specular[2] = 0.5;
			myInit();
		}
		break;
	}
	glutPostRedisplay( );
}

void shading(int id)
{
	switch(id)
	{
	case 5: glShadeModel(GL_SMOOTH);
		break;
	case 6: glShadeModel(GL_FLAT);
		break;
	}
	glutPostRedisplay( );
}

void projection(int id)
{
	switch(id)
	{
	case 7:
		{
			menu_P = true;
			menu_O = false;
		}
		break;
	case 8: 
		{
			menu_P =  false;
			menu_O = true;
		}
		break;
	}
	myReshape(w,h);
	glutPostRedisplay();
}

void building(int id)
{
	switch(id)
	{
	case 9: building_shape = 1;
		break;
	case 10: building_shape = 2;
		break;
	case 11: building_shape = 3;
		break;
	}
	//glutPostRedisplay( );
}

void top_menu(int id)
{
}

int main(int argc, char** argv)
{
	int sub_light;
	int sub_shading;
	int sub_projection;
	int sub_building;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(w, h);
	//glutInitWindowPosition(0, 0);
	glutCreateWindow("122206");

	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	myInit();

	glutTimerFunc(100,myTimer,1);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKey);
	glutSpecialFunc(SpecialKey);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	sub_light = glutCreateMenu(light_color);
	glutAddMenuEntry("RED",2);
	glutAddMenuEntry("GREEN",3);
	glutAddMenuEntry("BLUE",4);

	sub_shading = glutCreateMenu(shading);
	glutAddMenuEntry("Smooth",5);
	glutAddMenuEntry("Flat",6);

	sub_projection = glutCreateMenu(projection);
	glutAddMenuEntry("Perspective",7);
	glutAddMenuEntry("Orthographic",8);

	sub_building = glutCreateMenu(building);
	glutAddMenuEntry("Shape 1", 9);
	glutAddMenuEntry("Shape 2", 10);
	glutAddMenuEntry("Shape 3", 11);

	glutCreateMenu(top_menu);
	glutAddSubMenu("Light Color", sub_light);
	glutAddSubMenu("Shading", sub_shading);
	glutAddSubMenu("Projection", sub_projection);
	glutAddSubMenu("Building", sub_building);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	FreeTexture(MyTextureObject[0]);
	FreeTexture(MyTextureObject[1]);
	return 0;
}
