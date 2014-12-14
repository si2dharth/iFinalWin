//#include "iBase.h"
#include "TrackBar.h"
#include <math.h>

Window *DrawArea;
Window *SettingWindow;
Window *Set3DWin;

#define ox 400
#define oy 100
#define ox3d 0
#define oy3d 0
#define sq(x) (x)*(x)

int d = 70, e = 50, l = 100, b = 100;
int x,y,z;

struct Point3D {
	int x,y,z;

	Point3D(int X,int Y,int Z) {
		x = X;
		y = Y;
		z = Z;
	}

	Point3D() {
		x=y=z=0;
	}
} CameraPos(0,500,10);

void Project3D(Window *W,Point3D P1,Point3D P2) {
	int x1,y1,x2,y2;
	float t;

	t = -P1.y/(CameraPos.y - P1.y);
	x1 = CameraPos.x * t + (1 - t) * P1.x + ox;
	y1 = CameraPos.z * t + (1 - t) * P1.z + oy;

	t = -P2.y/(CameraPos.y - P2.y);
	x2 = CameraPos.x * t + (1 - t) * P2.x + ox;
	y2 = CameraPos.z * t + (1 - t) * P2.z + oy;

	W->DrawLine(x1,y1,x2,y2);
}

void setLoc(Window *W,MouseEventParams *m = 0) {
	if (m) if (!m->lButton) return;

	double A1,A2;
	double cosAlpha, cosBeta, cosGamma, cosTheta;
	static int x ,y;

	if (m) {
		x = m->x - ox;
		y = m->y - oy;
	}

	cosAlpha = (d-x-e)/(sqrt((d-x-e)*(d-x-e) + y*y));
	cosGamma = (l*l - b*b + ((d-x-e)*(d-x-e) + y*y))/(2*l*sqrt((d-x-e)*(d-x-e) + y*y));
	cosBeta = -(e-x-d)/(sqrt((e-x-d)*(e-x-d) + y*y));
	cosTheta = (l*l - b*b + ((e-x-d)*(e-x-d) + y*y))/(2*l*sqrt((e-x-d)*(e-x-d) + y*y));

	if (abs(cosAlpha) > 1 || abs(cosGamma) > 1 || abs(cosBeta) > 1 || abs(cosTheta) > 1) return;

	W->Clear();

	A1 = 3.14 - (acos(cosGamma) + acos(cosAlpha));
	A2 = (acos(cosBeta) + acos(cosTheta));

	W->DrawLine(ox - d, oy, ox + d, oy);
	W->DrawLine(ox + x - e, oy + y, ox + x + e, oy + y);

	W->DrawLine(ox + d, oy, ox + d + l*cos(A1), oy + l*sin(A1));
	W->DrawLine(ox + d + l*cos(A1) , oy + l*sin(A1),ox + x + e, oy + y);

	W->DrawLine(ox - d, oy, ox - d + l*cos(A2), oy + l*sin(A2));
	W->DrawLine(ox - d + l*cos(A2) , oy + l*sin(A2),ox + x - e, oy + y);


	//	wchar_t str[50];

	//_itow(alpha*180/3.14,str,10);
	//W->Write(ox + 200,100,str);

	//_itow(gamma*180/3.14,str,10);
	//W->Write(ox + 200,150,str);

	//_itow(beta*180/3.14,str,10);
	//W->Write(ox + 200,200,str);
	//
	//_itow(theta*180/3.14,str,10);
	//W->Write(ox + 200,250,str);

	W->SetColor(255,0,0);

	W->DrawRectangle(ox + d - 2, oy - 2, ox + d + 2, oy + 2);
	W->DrawRectangle(ox - d - 2, oy - 2, ox - d + 2, oy + 2);
	W->DrawRectangle(ox + x + e - 2, oy + y - 2, ox + x + e + 2, oy + y + 2);
	W->DrawRectangle(ox + x - e - 2, oy + y - 2, ox + x - e + 2, oy + y + 2);

	W->DrawRectangle(ox - d + l*cos(A2) - 2, oy + l*sin(A2) - 2, ox - d + l*cos(A2) + 2, oy + l*sin(A2) + 2);
	W->DrawRectangle(ox + d + l*cos(A1) - 2, oy + l*sin(A1) - 2, ox + d + l*cos(A1) + 2, oy + l*sin(A1) + 2);

	W->SetColor(255,255,255);
}

void find3DAngles(Window *W, int x, int y, int z) {
	float cosAlpha, cosBeta, cosGamma, cosTheta, cosPhi, cosDelta;
	float A1,A2,A3;
	float lambda;

	lambda = 0;
	cosAlpha = (d - cos(lambda)*(x + e*cos(lambda)) - sin(lambda)*(y + e*sin(lambda)))/sqrt(sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z));
	cosTheta = (sq(l) + sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z) - sq(b)) / (2 * l * sqrt(sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z)));

	lambda = 120 * 3.14/180;
	cosBeta = (d - cos(lambda)*(x + e*cos(lambda)) - sin(lambda)*(y + e*sin(lambda)))/sqrt(sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z));
	cosPhi = (sq(l) + sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z) - sq(b)) / (2 * l * sqrt(sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z)));

	lambda = 240 * 3.14/180;
	cosGamma = (d - cos(lambda)*(x + e*cos(lambda)) - sin(lambda)*(y + e*sin(lambda)))/sqrt(sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z));
	cosDelta = (sq(l) + sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z) - sq(b)) / (2 * l * sqrt(sq(x + (e - d)*cos(lambda)) + sq(y + (e - d)*sin(lambda)) + sq(z)));

	if (abs(cosAlpha) > 1 || abs(cosTheta) > 1 || abs(cosBeta) > 1 || abs(cosPhi) > 1 || abs(cosGamma) > 1 || abs(cosDelta) > 1) {
		W->Clear();
		W->Write(100,100,L"Inaccessible Point");
		return;
	}

	A1 = cosAlpha + cosTheta;
	A2 = cosBeta + cosPhi;
	A3 = cosGamma + cosDelta;

	W->Clear();

	Point3D l11,l12,l21,l22,l31,l32 , d1,d2,d3;

	lambda = 120 * 3.14/180;

	l11 = Point3D(d,0,0);
	l21 = Point3D(d * cos(lambda), d * sin(lambda), 0);
	l31 = Point3D(d * cos(lambda*2), d * sin(lambda*2), 0);

	l12 = Point3D(d + l * cos(A1), 0, l * sin(A1));
	l22 = Point3D(d * cos(lambda) + l * cos(A2) * cos(lambda), d * sin(lambda) + l * cos(A2) * sin(lambda), l * sin(A2));
	l32 = Point3D(d * cos(lambda*2) + l * cos(A3) * cos(lambda*2), d * sin(lambda*2) + l * cos(A3) * sin(lambda*2), l * sin(A3));

	d1 = Point3D(e + x,y,z);
	d2 = Point3D(e * cos(lambda) + x, y + e * sin(lambda), z);
	d3 = Point3D(e * cos(lambda*2) + x, y + e * sin(lambda*2), z);

	W->SetColor(255,0,0);
	Project3D(W,l11,l12);
	Project3D(W,l12,d1);

	W->SetColor(0,255,0);
	Project3D(W,l21,l22);
	Project3D(W,l22,d2);

	W->SetColor(0,0,255);
	Project3D(W,l31,l32);
	Project3D(W,l32,d3);

	W->SetColor(255,255,255);
	Project3D(W,l11,l21);
	Project3D(W,l21,l31);
	Project3D(W,l31,l11);

	Project3D(W,d1,d2);
	Project3D(W,d2,d3);
	Project3D(W,d3,d1);
	//wchar_t str[50];

	//_itow(A1*180/3.14,str,10);
	//W->Write(ox + 200,100,str);

	//_itow(A2*180/3.14,str,10);
	//W->Write(ox + 200,150,str);

	//_itow(A3*180/3.14,str,10);
	//W->Write(ox + 200,200,str);

}

void set3D(Window *W, MouseEventParams *m) {
	if (!m->lButton) return;
	W->Clear();
	W->DrawLine(m->x - ox3d, m->y - oy3d - d, m->x - ox3d - d*sqrt(3)/2, m->y - oy3d  + d / 2);
	W->DrawLine(m->x - ox3d - d * sqrt(3) / 2, m->y - oy3d  + d / 2, m->x + ox3d + d * sqrt(3) / 2, m->y - oy3d + d / 2);
	W->DrawLine(m->x + ox3d + d * sqrt(3) / 2, m->y - oy3d  + d / 2, m->x - ox3d, m->y - oy3d - d);

	x = m->x - W->GetWidth()/2;
	y = m->y - W->GetHeight()/2;
	find3DAngles(DrawArea,x,y,z);
}

void changeVal(TrackBar *TB, INTTYPE *val) {
	switch (TB->id) {
	case 'l':	
		l = TB->value;
		break;
	case 'b':
		b = TB->value;
		break;
	case 'd':
		d = TB->value;
		break;
	case 'e':
		e = TB->value;
		break;

	case 'x':
		x = *val;
		find3DAngles(DrawArea,x,y,z);
		return;
		break;

	case 'y':
		y = *val;
		find3DAngles(DrawArea,x,y,z);
		return;
		break;

	case 'z':
		z = *val;
		find3DAngles(DrawArea,x,y,z);
		return;
		break;
	}
	setLoc(DrawArea);
}

void InitWin(Window* W) {
	W->handler(MouseMove)->addHandler(setLoc);
	W->handler(RightUp)->addHandler(W, &Window::Close);
	W->SetColor(255,255,255);
	W->SetBackColor(0,0,0);
	W->SetFontColor(255,255,255);
	W->Clear();
}

void InitSettingWindow(Window *W) {
	TrackBar *tB = new TrackBar;

	W->handler(RightUp)->addHandler(W, &Window::Close);

	tB->id = 'l';
	tB->MinValue = 10;
	tB->MaxValue = 300;
	tB->value = l;
	tB->handler(TrackBarValueChanged)->addHandler(changeVal);
	tB->Location = PointType(50,100);
	tB->Width = 200;
	tB->Height = 20;
	tB->AddToWindow(W);
	tB->Draw();

	tB = new TrackBar;
	tB->id = 'b';
	tB->MinValue = 10;
	tB->MaxValue = 300;
	tB->value = b;
	tB->handler(TrackBarValueChanged)->addHandler(changeVal);
	tB->Location = PointType(50,140);
	tB->Width = 200;
	tB->Height = 20;
	tB->AddToWindow(W);
	tB->Draw();

	tB = new TrackBar;
	tB->id = 'd';
	tB->MinValue = 1;
	tB->MaxValue = 200;
	tB->value = d;
	tB->handler(TrackBarValueChanged)->addHandler(changeVal);
	tB->Location = PointType(50,180);
	tB->Width = 200;
	tB->Height = 20;
	tB->AddToWindow(W);
	tB->Draw();

	tB = new TrackBar;
	tB->id = 'e';
	tB->MinValue = 1;
	tB->MaxValue = 200;
	tB->value = e;
	tB->handler(TrackBarValueChanged)->addHandler(changeVal);
	tB->Location = PointType(50,220);
	tB->Width = 200;
	tB->Height = 20;
	tB->AddToWindow(W);
	tB->Draw();

	tB = new TrackBar;
	tB->id = 'x';
	tB->MinValue = -200;
	tB->MaxValue = 200;
	tB->value = 0;
	tB->handler(TrackBarValueChanged)->addHandler(changeVal);
	tB->Location = PointType(50,300);
	tB->Width = 200;
	tB->Height = 20;
	tB->AddToWindow(W);
	tB->Draw();

	tB = new TrackBar;
	tB->id = 'y';
	tB->MinValue = -200;
	tB->MaxValue = 200;
	tB->value = 0;
	tB->handler(TrackBarValueChanged)->addHandler(changeVal);
	tB->Location = PointType(50,340);
	tB->Width = 200;
	tB->Height = 20;
	tB->AddToWindow(W);
	tB->Draw();

	tB = new TrackBar;
	tB->id = 'z';
	tB->MinValue = -300;
	tB->MaxValue = 300;
	tB->value = 100;
	tB->handler(TrackBarValueChanged)->addHandler(changeVal);
	tB->Location = PointType(50,380);
	tB->Width = 200;
	tB->Height = 20;
	tB->AddToWindow(W);
	tB->Draw();
}

void Init3DWin(Window *W) {
	W->handler(MouseMove)->addHandler(set3D);
	W->handler(RightUp)->addHandler(W, &Window::Close);
	W->SetColor(255,255,255);
	W->SetBackColor(0,0,0);
	W->SetFontColor(255,255,255);
	W->Clear();
}

int DeltaIK(char args[]) {
	DrawArea = new Window(InitWin);
	SettingWindow = new Window(InitSettingWindow,0,"Settings",300,500);
	Set3DWin = new Window(Init3DWin,0,"3D Position",500, 500);
	DrawArea->Run();
	SettingWindow->Close();
	return 0 ;
}