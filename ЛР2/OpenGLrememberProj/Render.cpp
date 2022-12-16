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

double* norm(double A[], double B[], double C[])
{
	double x, y, h;
	double a[] = { A[0] - B[0],A[1] - B[1],A[2] - B[2] };
	double c[] = { C[0] - B[0],C[1] - B[1],C[2] - B[2] };
	x = a[1] * c[2] - c[1] * a[2];
	y = c[0] * a[2] - a[0] * c[2];
	h = a[0] * c[1] - c[0] * a[1];
	double l = sqrt(x * x + y * y + h * h);
	double n[] = { x / l, y / l, h / l };
	return n;
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

	//������ ��������� ������

	double h = 2; 
	//���������� �����
	double A[] = { 0, 0, h };
	double A1[] = { 0, 0, 0 };
	double B[] = { -1, 8, h };
	double B1[] = { -1, 8, 0 };
	double C[] = { 4, 8, h };
	double C1[] = { 4, 8, 0 };
	double D[] = { 3, 13, h };
	double D1[] = { 3, 13, 0 };
	double E[] = { 8, 14, h };
	double E1[] = { 8, 14, 0 };
	double F[] = { 7, 7, h };
	double F1[] = { 7, 7, 0 };
	double G[] = { 11, 4, h };
	double G1[] = { 11, 4, 0 };
	double H[] = { 6, 6, h };
	double H1[] = { 6, 6, 0 };

	//������ ����� 
	double* N = norm(A1, B1, H1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(H1);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(H1);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(C1);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(C1);
	glVertex3dv(H1);
	glVertex3dv(F1);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(H1);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glEnd();

	//������� �����
	N = norm(A, B, H);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(H);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(H);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(C);
	glVertex3dv(E);
	glVertex3dv(F);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(C);
	glVertex3dv(H);
	glVertex3dv(F);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 0.1);
	glVertex3dv(H);
	glVertex3dv(F);
	glVertex3dv(G);
	glEnd();

	//������� �����	
	//����� 1 - AB
	double A13[] = { 0, 0, 0 };  //A1
	double B13[] = { 0, 0, h };  //A
	double C13[] = { -1, 8, h }; //B
	double D13[] = { -1, 8, 0 }; //B1
	N = norm(A13, B13, C13);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A13);
	glVertex3dv(B13);
	glVertex3dv(C13);
	glVertex3dv(D13);
	glEnd();
		
	//����� 2 - BC
	double A23[] = { -1, 8, 0 };
	double B23[] = { -1, 8, h };
	double C23[] = { 4, 8, h };
	double D23[] = { 4, 8, 0 };
	N = norm(A23, B23, C23);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A23);
	glVertex3dv(B23);
	glVertex3dv(C23);
	glVertex3dv(D23);
	glEnd();
		
	//����� 3 - CD
	double A33[] = { 4, 8, 0 };
	double B33[] = { 4, 8, h };
	double C33[] = { 3, 13, h };
	double D33[] = { 3, 13, 0 };
	N = norm(A33, B33, C33);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A33);
	glVertex3dv(B33);
	glVertex3dv(C33);
	glVertex3dv(D33);
	glEnd();
		
	//����� 4 - DE
	double A43[] = { 3, 13, 0 };
	double B43[] = { 3, 13, h };
	double C43[] = { 8, 14, h };
	double D43[] = { 8, 14, 0 };
	N = norm(A43, B43, C43);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A43);
	glVertex3dv(B43);
	glVertex3dv(C43);
	glVertex3dv(D43);
	glEnd();
		
	//����� 5 - EF
	double A53[] = { 8, 14, 0 };
	double B53[] = { 8, 14, h };
	double C53[] = { 7, 7, h };
	double D53[] = { 7, 7, 0 };
	N = norm(A53, B53, C53);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A53);
	glVertex3dv(B53);
	glVertex3dv(C53);
	glVertex3dv(D53);
	glEnd();
		
	//����� 6 - FG
	double A63[] = { 7, 7, 0 };
	double B63[] = { 7, 7, h };
	double C63[] = { 11, 4, h };
	double D63[] = { 11, 4, 0 };
	N = norm(A63, B63, C63);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A63);
	glVertex3dv(B63);
	glVertex3dv(C63);
	glVertex3dv(D63);
	glEnd();
			
	//����� 7 - GH
	double A73[] = { 11, 4, 0 };
	double B73[] = { 11, 4, h };
	double C73[] = { 6, 6, h };
	double D73[] = { 6, 6, 0 };
	N = norm(A73, B73, C73);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A73);
	glVertex3dv(B73);
	glVertex3dv(C73);
	glVertex3dv(D73);
	glEnd();
			
	//����� 8 - HA
	double A83[] = { 6, 6, 0 };
	double B83[] = { 6, 6, h };
	double C83[] = { 0, 0, h };
	double D83[] = { 0, 0, 0 };
	N = norm(A83, B83, C83);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_QUADS);
	glColor3d(1, 0, 0);
	glNormal3dv(N);
	glVertex3dv(A83);
	glVertex3dv(B83);
	glVertex3dv(C83);
	glVertex3dv(D83);
	glEnd();
	//����� ��������� ������

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
	ss << "T - ���������/���������� �������" << std::endl;
	ss << "L - ���������/���������� ���������" << std::endl;
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