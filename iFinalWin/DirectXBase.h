#include <Windows.h>
#include <windowsx.h>
#include <d3d9.h>

#pragma comment (lib,"d3d9.lib")

struct Point {
	FLOAT x, y, z, rhw;
	DWORD color;
};

typedef LPDIRECT3DVERTEXBUFFER9 ShapeBuffer;
typedef D3DPRIMITIVETYPE ShapeType;

class DirectX{
	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 d3ddev;
	bool _DrawActive;
public:
	DirectX(HWND WindowHandle, bool FullScreen = false);
	~DirectX();

	DWORD Color(int r, int g, int b);

	ShapeBuffer *GetShape(ShapeBuffer *vertexBuffer, Point vertexList[], int nVertices);
	void StartPaint();
	void Draw(ShapeBuffer *vertexBuffer, ShapeType drawType, int nShapes, int startCount = 0);
	void Draw(Point vertexList[], int nVertices, ShapeType drawType, int nShapes, int startCount = 0);
	void StopPaint();

	void ClearScreen();
	void ClearScreen(DWORD clr);
	bool isDrawing();

	DWORD curcolor;
	DWORD fillcolor;
	int smoothness;		//The higher, the smoother
};