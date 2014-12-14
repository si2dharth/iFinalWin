#include "iGraphics.h"

#define VertexFormat (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
#define GetColor(r,g,b) D3DCOLOR_XRGB(r,g,b)

DGraphics::DGraphics(Window *PWin,bool fullScreen):eHandler(this) {
	ParentWindow = PWin;
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = !fullScreen;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = PWin->Handle();

	d3d->CreateDevice  (D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						PWin->Handle(),
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						&d3dpp,
						&d3ddev);
	PWin->handler(EventName::Paint)->addHandler(this,&DGraphics::PaintScreen);
	eHandler.raise(DGraphicsEvents::Initialized);
	smoothness = 20;
}

DGraphics::~DGraphics() {
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
	d3ddev = 0;
	d3d = 0;
	eHandler.raise(DGraphicsEvents::Destroyed);
};

DWORD DGraphics::Color(int r, int g, int b) {
	return GetColor(r,g,b);
}

ShapeBuffer *DGraphics::GetShape(ShapeBuffer *vertexBuffer,Vertex vertexList[],int nVertices) {
	
	d3ddev->CreateVertexBuffer (nVertices*sizeof(Vertex),
								0,
								VertexFormat,
								D3DPOOL_MANAGED,
								vertexBuffer,
								NULL);

	VOID* pVoid;

	(*vertexBuffer)->Lock (0, 0, (void**) &pVoid, 0);
	memcpy(pVoid, vertexList, sizeof(Vertex) * nVertices);
	(*vertexBuffer)->Unlock();
	return vertexBuffer;
}

void DGraphics::Draw(ShapeBuffer *vertexBuffer,ShapeType drawType,int nShapes, int startCount) {
	if (nShapes<1) return;
	if (!_DrawActive) return;
	d3ddev->SetFVF(VertexFormat);
	d3ddev->SetStreamSource(0,*vertexBuffer,startCount*sizeof(Vertex),sizeof(Vertex));
	d3ddev->DrawPrimitive(drawType, startCount , nShapes);
}

void DGraphics::Draw(Vertex vertexList[],int nVertices,ShapeType drawType,int nShapes,int startCount) {
	if (!_DrawActive) return;
	ShapeBuffer vBuffer;
	Draw(GetShape(&vBuffer,vertexList,nVertices),drawType,nShapes);
	vBuffer->Release();
}

void DGraphics::StartPaint() {
	if (_DrawActive) return;
	eHandler.raise(DGraphicsEvents::StartingDraw);
	d3ddev->BeginScene();
	_DrawActive = true;
	eHandler.raise(DGraphicsEvents::StartedDraw);
}

void DGraphics::StopPaint() {
	if (!_DrawActive) return;
	eHandler.raise(DGraphicsEvents::EndingDraw);
	d3ddev->EndScene();
	d3ddev->Present(0,0,0,0);
	_DrawActive = false;
	eHandler.raise(DGraphicsEvents::EndedDraw);
}

void DGraphics::PaintScreen() {
	StartPaint();
	StopPaint();
}

void DGraphics::ClearScreen() {
	ClearScreen(curcolor);
}

void DGraphics::ClearScreen(DWORD clr) {
	d3ddev->Clear(0,NULL,D3DCLEAR_TARGET,clr,1.0f,0);
}

bool DGraphics::isDrawing() {
	return _DrawActive;
}

Window *DGraphics::GetWindow() {
	return ParentWindow;
}