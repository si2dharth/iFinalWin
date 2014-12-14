#include "DDrawing.h"

class DImage : public DDrawing {
protected:
	DShape Picture;
	bool _editing;
	int _height,_width;
public:
	DImage(int Width, int Height, DWORD Backclr = D3DCOLOR_ARGB(255,0,0,0), DGraphics *container = 0) : DDrawing(container),Picture(container) {
		Picture.setColor(Backclr);
		int *Vertices = new int[2*Width*Height];
		_height = Height;
		_width = Width;
		
		for (unsigned long i = 0; i < Height; i++) 
			for (unsigned long j = 0; j < Width; j++) {
				Vertices[(i*Width + j)<<1] = j;
				Vertices[(i*Width + j)<<1 | 1] = i;
			}
	
		Picture.addVertices(Vertices,Width*Height);
		delete[] Vertices;
		_editing = false;
	}

	void startEdit() {
		_editing = true;
	}

	void stopEdit() {
		_editing = false;
		Picture.Process();
	}

	void setVertex(int x,int y, DWORD Clr) {
		Picture.getVertex(x,y).color = Clr;
		if (!_editing) Picture.Process();
	}

	void setPictureFromArray(DWORD **Clr) {
		DWORD *newAr = new DWORD[_height*_width];
		for (int i = 0; i <= _height; i++)
			memcpy(&newAr[i*_width],Clr[i],_width);
		Picture.SetColorArray(newAr);
		delete [] newAr;
		
		for (int i = 0; i<=_height; i++)
			delete [] Clr[i];
		delete[] Clr;

	}

	virtual void preDelete() {
		removeShape(&Picture);
	}

	virtual ~DImage() {
		preDelete();
	}

	void Draw() {
		DDrawing::Draw();
	}
};