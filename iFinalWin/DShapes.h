#include "iGraphics.h"
#include <list>

#define ptPoints D3DPT_POINTLIST
#define ptLines D3DPT_LINELIST
#define ptLineStrip D3DPT_LINESTRIP
#define ptTriangles D3DPT_TRIANGLELIST
#define ptTriangleStrip D3DPT_TRIANGLESTRIP
#define ptTriangleFan D3DPT_TRIANGLEFAN

class DShape {
	DGraphics *_Container;
	ShapeBuffer sBuffer;
	bool buffed,locked;
	list<Vertex> vertices;

	int _x,_y;
	int _width,_height;

	//Calculate the number of shapes based on the drawType
	int getNumShapes() {
		switch (drawType) {
		
		case D3DPT_LINELIST :
			return vertices.size() / 2;

		case D3DPT_LINESTRIP:
			return vertices.size() - 1;

		case D3DPT_POINTLIST:
			return vertices.size();

		case D3DPT_TRIANGLEFAN:
			return vertices.size() - 2;

		case D3DPT_TRIANGLELIST:
			return vertices.size() / 3;

		case D3DPT_TRIANGLESTRIP:
			return vertices.size() - 2;
		}
		return 0;
	}

public:
	bool noUpdate;
	static DGraphics *defContainer;

	//Constructor. Bind to a DirectX Graphics object. If container is not provided, the defContainer is considered
	//Once a container is specified, it is set as the default container if no default is specified.
	DShape(DGraphics *container = 0) {
		if (container == 0 && defContainer == 0) throw("No Default Container set");
		if (container == 0) container = defContainer;
		if (defContainer == 0) defContainer = container;

		_Container = container;
		buffed = false;
		locked = false;
		noUpdate = false;
		curColor = D3DCOLOR_XRGB(0,0,0);
		drawType = D3DPT_LINELIST;
		_x = _y = _width = _height = 0;
	}

	//Standard Destructor. Releases the buffer if needed
	virtual ~DShape() {
		if (buffed && sBuffer != 0) sBuffer->Release();
		sBuffer = 0;
	} 

	//Add a vertex to the shape of clr color. The curColor is not changed
	void addVertex(int x,int y,int clr) {
		if (locked) return;
		
		if (vertices.size() == 0)
		{
			_x = x; _y = y;
			_width = 0;
			_height = 0;
		}

		if (_x > x) _x = x;
		if (_y > y) _y = y;
		if (x - _x > _width) _width = x - _x;
		if (y - _y > _height) _height = y - _y;

		Vertex v = { (float)x, (float)y,1.0,1.0,clr };
		vertices.push_back(v);
		if (buffed && !noUpdate) Process();
	}

	//Add a vertex to the shape. The color is considered as curColor
	void addVertex(int x,int y) {
		addVertex(x,y,curColor);
	}

	//Add a vertex list to the shape. An array to integers is passed in X,Y pairs.
	//Unmatched entry is ignored. The color is considered as curColor
	void addVertices(int *v,int nVertices) {
		addVertices(v,nVertices,curColor);
	}

	//Add a vertex list to the shape. An array to integers is passed in X,Y pairs.
	//Unmatched entry is ignored. The color is specified
	void addVertices(int *v, unsigned long nVertices, DWORD clr) {
		if (locked) return;
		bool nU = noUpdate;
		noUpdate = true;
		for (unsigned long i = 0; i<nVertices*2; i+=2)
			addVertex(v[i],v[i+1],clr);
		noUpdate = nU;
		if (buffed && !noUpdate) Process();
	}

	//Remove a vertex at the specified location. In case of multiple vertices, only the first one is removed
	void removeVertex(int x, int y) {
		if (locked) return;
		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++)
			if ((*I).x == x && (*I).y == y) {
				vertices.erase(I);
				return;
			}
		if (buffed && !noUpdate) Process();
	}

	//Remove all the vertices
	void removeAllVertices() {
		if (locked) return;
		vertices.clear();
		//if (buffed && !noUpdate) Process(); NEVER Process empty
	}

	//Get all the vertices in the shape in array form. The array should be deleted after use.
	Vertex *getVertexArray() {
		Vertex *V = new Vertex[vertices.size()];
		int i = 0;
		
		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++,i++)
			V[i] = *I;
		return V;
	}

	//Get the Shape Buffer for the shape
	ShapeBuffer* getShape() {
		Process();
		return &sBuffer;
	}

	//Draw the shape on the DGraphics container, only if the DGraphics is in paint mode
	void Draw() {
		if (!buffed) Process();
		_Container->Draw(&sBuffer,drawType,getNumShapes());
	}

	//Change the container for the shape
	void ChangeContainer(DGraphics *newContainer) {
		if (newContainer == _Container) return;
		if (locked) return;
		bool b = buffed;
		if (buffed) sBuffer->Release();
		buffed = false;
		_Container = newContainer;
		buffed = b;
		if (buffed && !noUpdate) Process();
	}

	//Set the color for all the vertices. The curColor is changed
	void setColor(DWORD clr) {
		if (locked) return;
		curColor = clr;
		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++)
			(*I).color = clr;
		if (buffed && !noUpdate) Process();
	}

	//Update the position of a vertex. Only the first matching entry is changed
	void ChangeLocation(int x,int y,int nX,int nY) {
		if (x==nX && y==nY) return;
		if (locked) return;
		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++)
			if ((*I).x == x && (*I).y==y) {
				(*I).x = (float)nX;
				(*I).y = (float)nY;
				if (buffed && !noUpdate) Process();
				return;
			}
	}

	//Get the actual vertex object for processing it. Throws an exception if vertex is not found or drawing is locked
	//Process() needs to be manually called for updating the shape
	Vertex &getVertex(int x,int y) {
		if (locked) throw("Cannot access a locked shape");
		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++)
			if ((*I).x == x && (*I).y == y) return (*I);
		throw ("Vertex not found");
	}

	//Lock the shape from further changes
	void Lock() {
		if (locked) return;
		Process();
		locked = true;
	}

	//Release the lock from shape
	void Release() {
		locked = false;
	}

	//Process the buffer. Does not draw the shape in the container. Used to update the shape.
	void Process() {
		if (vertices.size() == 0) return;
		if (buffed && locked) return;
		if (buffed) sBuffer->Release();
		Vertex *v = getVertexArray();
		_Container->GetShape(&sBuffer,v,vertices.size());
		delete[] v;
		buffed = true;
	}

	//Move the shape by a certain distance
	void Translate(int dx,int dy) {
		if (locked) return;

		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++) {
			(*I).x += dx;
			(*I).y += dy;
		}
		_x += dx;
		_y += dy;
		if (buffed && !noUpdate) Process();
	}

	//The color to use for all further drawings
	DWORD curColor;

	//The type of Primitive Drawing Type that the shape uses: ptPoints/ptLineStrip/ptLines/ptTriangles/ptTriangleStrip/ptTriangleFan
	ShapeType drawType;

	//Get the left-most X of the shape
	int getX() {
		return _x;
	}

	//Get the upper-most Y of the shape
	int getY() {
		return _y;
	}

	//Get the width of the shape
	int width() {
		return _width;
	}

	//Get the height of the shape
	int height() {
		return _height;
	}

	//Check if the point lies in the bounding box of tShe shape
	bool ContainsPoint(int x,int y) {
		return ( x>=_x && x<=_x+_width && y>=_y && y<=_y + _height);
	}

	//Flip the shape about its middle about the Vertical line through X,0
	void FlipX(int X) {
		if (locked) return;

		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++)
			(*I).x -= 2*((*I).x - X);
		vertices.reverse();		//The vertices order, if clockwise, would become anti-clockwise when flipped. This instructions corrects that bug.
		_x = 2*X - _width - _x;

		if (buffed && !noUpdate) Process();
	}

	//Flip the shape about its middle about the Horizontal line passing through 0,Y
	void FlipY(int Y) {
		if (locked) return;

		for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++)
			(*I).y -= 2*((*I).y - Y);
		vertices.reverse();		//The vertices order, if clockwise, would become anti-clockwise when flipped. This instructions corrects that bug.
		_y -= 2*(_y - Y);

		if (buffed && !noUpdate) Process();
	}

	//Set all the colors in the array to the vertices in the order.
	//The Color array should have the same number of colors as the vertices.
	void SetColorArray(DWORD *Clr) {
		int i = 0;
		try {
			for (list<Vertex>::iterator I = vertices.begin(); I != vertices.end(); I++, i++)
				(*I).color = Clr[i];
		}
		catch (void *ex) {
			throw ("Not enough colors in the array");
		}
		
	}
};
