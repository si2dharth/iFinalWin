#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <d3d9.h>
#include "iBase.h"

#pragma comment (lib,"d3d9.lib")

struct Vertex {
	FLOAT x,y,z,rhw;
	DWORD color;
};

typedef LPDIRECT3DVERTEXBUFFER9 ShapeBuffer;
typedef D3DPRIMITIVETYPE ShapeType;

enum DGraphicsEvents {
	Initialized,
	Destroyed,
	StartingDraw,
	StartedDraw,
	EndingDraw,
	EndedDraw
};


class DGraphics {
	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 d3ddev;
	Window *ParentWindow;

	bool _DrawActive;
public:
	DGraphics(Window *ParentWindow,bool FullScreen = false);
	~DGraphics();

	EventHandler<int,DGraphics> eHandler;

	void StartPaint();
	void StopPaint();
	void PaintScreen();
	void ClearScreen();
	void ClearScreen(DWORD Clr);
	bool isDrawing();
	Window *GetWindow();

	DWORD Color(int r,int g,int b);

	ShapeBuffer *GetShape(ShapeBuffer *vertexBuffer,Vertex vertexList[],int nVertices);
	void Draw(ShapeBuffer *vertexBuffer,ShapeType drawType,int nShapes,int startCount = 0);
	void Draw(Vertex vertexList[],int nVertices,ShapeType drawType,int nShapes,int startCount = 0);

	DWORD curcolor;
	DWORD fillcolor;
	int smoothness;		//The higher, the smoother
};