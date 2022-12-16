#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
void get_color(int r, int b, int g, double color[3]) {
	color[0] = double(r) / 255;
	color[1] = double(b) / 255;
	color[2] = double(g) / 255;
}


void get_normal(double a1[3], double b1[3], double c1[3], double normal[3]) {
	double a[3] = { b1[0] - a1[0], b1[1] - a1[1], b1[2] - a1[2] };
	double b[3] = { c1[0] - a1[0], c1[1] - a1[1], c1[2] - a1[2] };
	double length = 0;
	normal[0] = a[1] * b[2] - b[1] * a[2];
	normal[1] = -a[0] * b[2] + b[0] * a[2];
	normal[2] = a[0] * b[1] - b[0] * a[1];
	length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] = normal[0] / length;
	normal[1] = normal[1] / length;
	normal[2] = normal[2] / length;
}
bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	double A1[] = { -3.5,1.5,0 };
	double B1[] = { -2, -1, 0 };
	double C1[] = { -0.5, 1, 0 };
	double A2[] = { 0,0,0 };
	double B2[] = { -2, -1, 0 };
	double C2[] = { -0.5, 1, 0 };
	double A3[] = { 0,0,0 };
	double B3[] = { 0.5,0.5, 0 };
	double C3[] = { -0.5, 1, 0 };
	double A4[] = { 0,4,0 };
	double B4[] = { 0.5,0.5, 0 };
	double C4[] = { -0.5, 1, 0 };
	double A5[] = { 0,0,0 };
	double B5[] = { 0.5,0.5, 0 };
	double C5[] = { 2, -1.5, 0 };
	double A6[] = { 4.5,0.5,0 };
	double B6[] = { 0.5,0.5, 0 };
	double C6[] = { 2, -1.5, 0 };
	double D1[] = { -3.5,1.5,2 };
	double E1[] = { -2, -1, 2 };
	double F1[] = { -0.5, 1, 2 };
	double D2[] = { 0,0,2 };
	double E2[] = { -2, -1, 2 };
	double F2[] = { -0.5, 1, 2 };
	double D3[] = { 0,0,2 };
	double E3[] = { 0.5,0.5, 2 };
	double F3[] = { -0.5, 1, 2 };
	double D4[] = { 0,4,2 };
	double E4[] = { 0.5,0.5, 2 };
	double F4[] = { -0.5, 1, 2 };
	double D5[] = { 0,0,2 };
	double E5[] = { 0.5,0.5, 2 };
	double F5[] = { 2, -1.5, 2 };
	double D6[] = { 4.5,0.5,2 };
	double E6[] = { 0.5,0.5, 2 };
	double F6[] = { 2, -1.5, 2 };
	glBegin(GL_TRIANGLES);
	glColor3d(0.2, 0.2, 0);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glColor3d(0.7, 0.2, 0);
	glVertex3dv(A2);
	glVertex3dv(B2);
	glVertex3dv(C2);
	glColor3d(0.7, 0.7, 0);
	glVertex3dv(A3);
	glVertex3dv(B3);
	glVertex3dv(C3);
	glColor3d(0.1, 0, 0);
	glVertex3dv(A4);
	glVertex3dv(B4);
	glVertex3dv(C4);
	glColor3d(0.5, 0, 0);
	glVertex3dv(A5);
	glVertex3dv(B5);
	glVertex3dv(C5);
	glColor3d(0.7, 0, 0);
	glVertex3dv(A6);
	glVertex3dv(B6);
	glVertex3dv(C6);
	glColor3d(0.2, 0.2, 0);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glColor3d(0.7, 0.2, 0);
	glVertex3dv(D2);
	glVertex3dv(E2);
	glVertex3dv(F2);
	glColor3d(0.7, 0.7, 0);
	glVertex3dv(D3);
	glVertex3dv(E3);
	glVertex3dv(F3);
	glColor3d(0.1, 0, 0);
	glVertex3dv(D4);
	glVertex3dv(E4);
	glVertex3dv(F4);
	glColor3d(0.5, 0, 0);
	glVertex3dv(D5);
	glVertex3dv(E5);
	glVertex3dv(F5);
	glColor3d(0.7, 0, 0);
	glVertex3dv(D6);
	glVertex3dv(E6);
	glVertex3dv(F6);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(E1);
	glVertex3dv(D1);
	glColor3d(0.2, 0, 0);
	glVertex3dv(A2);
	glVertex3dv(B2);
	glVertex3dv(E2);
	glVertex3dv(D2);
	glColor3d(0.2, 0.7, 0);
	glVertex3dv(A1);
	glVertex3dv(C1);
	glVertex3dv(F1);
	glVertex3dv(D1);
	glColor3d(0.2, 0, 0);
	glVertex3dv(A4);
	glVertex3dv(C4);
	glVertex3dv(F4);
	glVertex3dv(D4);
	glColor3d(0.2, 0.2, 0.6);
	glVertex3dv(A4);
	glVertex3dv(B4);
	glVertex3dv(E4);
	glVertex3dv(D4);
	glColor3d(0.7, 0.7, 0);
	glVertex3dv(A6);
	glVertex3dv(B6);
	glVertex3dv(E6);
	glVertex3dv(D6);
	glColor3d(0.9, 0, 0);
	glVertex3dv(A6);
	glVertex3dv(C6);
	glVertex3dv(F6);
	glVertex3dv(D6);
	glColor3d(0.2, 0, 0);
	glVertex3dv(A5);
	glVertex3dv(C5);
	glVertex3dv(F5);
	glVertex3dv(D5);

	glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}