#include "DDrawing.h"

class DRect : public DDrawing {
	DShape *Border,*Body;
	bool filled,initializing;
	
public:
	//Create a rectangle at X,Y with specified width and height, of the color Clr, in the specified container or the default container, and fill if specified
	DRect(int X,int Y,int Width, int Height,DWORD Clr,DGraphics *container = 0,bool fill = false) : DDrawing(container),Width(this),Height(this),X(this),Y(this) {
		initializing = true;
		Border = new DShape(container);
		Body = new DShape(container);
		this->X = X;
		this->Y = Y;
		this->Width = Width;
		this->Height = Height;

		int v[] = { X,Y,X+Width,Y,X+Width,Y+Height,X,Y+Height,X,Y};
		Border->addVertices(v,5,Clr);
		Border->drawType = ptLineStrip;
		addShape(Border);

		int w[] = { X,Y,X+Width,Y,X+Width,Y+Height,X,Y+Height,X,Y};
		Body->addVertex(X+(Width>>1) , Y+(Height>>1),Clr);
		Body->addVertices(w,5,Clr);
		Body->drawType = ptTriangleFan;

		initializing = false;
		SetFill(fill);
		Update();
	}

	//Destroy the memory allocated by the rectangle
	virtual ~DRect() {
		removeShape(Border);
		removeShape(Body);
		delete Border;
		delete Body;
	}

	//Update the rectangle drawing if the position or the dimensions are changed
	void Update() {
		if (initializing) return;
		Border->noUpdate = true;
		Border->ChangeLocation(X.old(),Y.old(),X,Y);
		Border->ChangeLocation(X.old() + Width.old(),Y.old(),X+Width,Y);
		Border->ChangeLocation(X.old() + Width.old(),Y.old() + Height.old(),X+Width,Y+Height);
		Border->ChangeLocation(X.old(),Y.old() + Height.old(),X,Y+Height);
		Border->ChangeLocation(X.old(),Y.old(),X,Y);
		Border->noUpdate = false;
		Border->Process();

		Body->noUpdate = true;
		Body->ChangeLocation(X.old(),Y.old(),X,Y);
		Body->ChangeLocation(X.old() + Width.old(),Y.old(),X+Width,Y);
		Body->ChangeLocation(X.old() + Width.old(),Y.old() + Height.old(),X+Width,Y+Height);
		Body->ChangeLocation(X.old(),Y.old() + Height.old(),X,Y+Height);
		Body->ChangeLocation(X.old(),Y.old(),X,Y);
		Body->noUpdate = false;
		Body->Process();

		X.updateOld();
		Y.updateOld();
		Width.updateOld();
		Height.updateOld();
	}

	//Set the rectangle to be filled/not filled
	void SetFill(bool fill) {
		if (filled==fill) return;
		if (fill) shapes.push_front(Body); else removeShape(Body);
		filled = fill;
	}

	//Set the color of the border of the rectangle
	void SetBorderColor(DWORD Clr) {
		Border->setColor(Clr);
	}

	//Set the color of the fill area of the rectangle
	void SetFillColor(DWORD Clr) {
		Body->setColor(Clr);
	}

	//Draw the rectangle
	void Draw() {
		DDrawing::Draw();
	};

	//Draw a rectangle in the specified container or the default container at the given position. The rectangle is created, then destroyed automatically
	static void Draw(int x,int y,int width,int height,DWORD Clr,DGraphics *container = 0, bool fill = false) {
		if (container == 0 && defContainer == 0) throw("No container specified");
		if (container == 0) container = defContainer;
		if (defContainer == 0) defContainer = container;
		if (!container->isDrawing()) return;

		DRect r(x,y,width,height,Clr,container,fill);
		r.Draw();
	}

	//The X - coordinate position of the rectangle. On change, the rectangle is automatically updated.
	DDrawingProp X;
	
	//The Y - coordinate position of the rectangle. On change, the rectangle is automatically updated.
	DDrawingProp Y;
	
	//The Width of the rectangle. On change, the rectangle is automatically updated.
	DDrawingProp Width;
	
	//The height of the rectangle. On change, the rectangle is automatically updated.
	DDrawingProp Height;
};