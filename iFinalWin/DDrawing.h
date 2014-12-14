#pragma once
#include "DShapes.h"

//A drawing for DirectX display container
class DDrawing {
protected:
	list<DDrawing*> drawings;
	list<DShape*> shapes;
	DGraphics *_Container;
	bool locked;
public:
	bool visible;
	//A default container. Every drawing created without specifying the container is added to this container
	static DGraphics *defContainer;

	//A constructor for the drawing. Container points to the DGraphics object that holds the drawing
	DDrawing(DGraphics *container = 0) {
		if (!container && !defContainer) throw ("Container not specified");
		if (!container) container = defContainer;
		if (!defContainer) defContainer = container;
		_Container = container;
		visible = true;
		locked = false;
	};

	//A procedure called when the object is about to be deleted
	virtual void preDelete() {
	}

	//All the contained shapes are deleted when the drawing is deleted
	virtual ~DDrawing() {
		preDelete();
		destroyComponents();
	}

	//Delete all the DShapes and DDrawings contained in the DDrawing
	void destroyComponents() {
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			delete (*I);
		shapes.clear();

		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++)
			delete (*I);
		drawings.clear();
	}

	void addShape(DShape *shape) {
		if (locked) return;
		shapes.push_back(shape);
		shape->ChangeContainer(_Container);
	}

	void addDrawing(DDrawing *drawing) {
		if (locked) return;
		drawings.push_back(drawing);
		drawing->changeContainer(_Container);
	}

	void removeShape(DShape *shape) {
		if (locked) return;
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++)
			if ((*I)==shape) {
				shapes.erase(I);
				return;
			}
	}

	void removeDrawing(DDrawing *Drawing) {
		if (locked) return;
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++)
			if ((*I)==Drawing) {
				drawings.erase(I);
				return;
			}
	}

	void Translate(int dx,int dy) {
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->Translate(dx,dy);
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->Translate(dx,dy);
	}

	void FlipX(int X) {
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->FlipX(X);
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->FlipX(X);
	}

	void FlipY(int Y) {
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->FlipY(Y);
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->FlipY(Y);
	}

	void changeContainer(DGraphics *newContainer) {
		if (locked) return;
		if (_Container == newContainer) return;
		_Container = newContainer;
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->ChangeContainer(newContainer);
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->changeContainer(newContainer);
	}

	void Lock() {
		locked = true;
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->Lock();
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->Lock();
	}

	void Release() {
		locked = false;
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->Release();
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->Release();
	}

	virtual void Update() {
	};

	virtual void Draw() {
		if (!visible) return;
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->Draw();
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->Draw();
	}

	void Process() {
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->Process();
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->Process();
	}

	virtual bool ContainsPoint(int x,int y) {
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			if ((*I)->ContainsPoint(x,y)) return true;
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			if ((*I)->ContainsPoint(x,y)) return true;
		return false;
	}

	virtual int getX() {
		int minX = 65535;

		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			if ((*I)->getX() < minX) minX = (*I)->getX();
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			if ((*I)->getX() < minX) minX = (*I)->getX();
		return minX;
	}

	virtual int getY() {
		int minX = 65535;

		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			if ((*I)->getY() < minX) minX = (*I)->getY();
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			if ((*I)->getY() < minX) minX = (*I)->getY();
		return minX;
	}

	virtual int width() {
		int left = getX();
		int maxW = -65535;

		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) {
			int tmp = (*I)->width() + (*I)->getX() - left ;
			if (tmp > maxW) maxW = tmp;
		}

		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++)  {
			int tmp = (*I)->width() + (*I)->getX() - left ;
			if (tmp > maxW) maxW = tmp;
		}
		return maxW;
	}

	virtual int height() {
		int top = getY();
		int maxW = -65535;

		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) {
			int tmp = (*I)->height() + (*I)->getY() - top ;
			if (tmp > maxW) maxW = tmp;
		}

		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++)  {
			int tmp = (*I)->height() + (*I)->getY() - top ;
			if (tmp > maxW) maxW = tmp;
		}
		return maxW;
	}

	virtual bool Intersects(DDrawing *D,int PaddingLeft = 0,int PaddingRight = 0,int PaddingTop = 0,int PaddingBottom = 0) {
		int x1 = getX(), x2 = getX() + width(), y1 = getY(), y2 = getY() + height();
		int X1 = D->getX() - PaddingLeft, X2 = D->getX() + D->width() + PaddingRight,
			Y1 = D->getY() - PaddingTop, Y2 = D->getY() + D->height() + PaddingBottom;
		return !((y1 > Y2) || (Y1 > y2) || (x1 > X2) || (X1 > x2));
	}

	void setColor(DWORD Clr) {
		for (list<DShape*>::iterator I = shapes.begin(); I != shapes.end(); I++) 
			(*I)->setColor(Clr);
		for (list<DDrawing*>::iterator I = drawings.begin(); I != drawings.end(); I++) 
			(*I)->setColor(Clr);
	}
};

class DDrawingProp{
		int i,oldValue;
		DDrawing *parent;
	public:
		DDrawingProp(DDrawing* prt) {
			parent = prt;
			i = 0;
			oldValue = 0;
		}

		int operator =(int nValue) {
			if (i == nValue) return i;
			oldValue = i;
			i = nValue;
			parent->Update();
			return i;
		}

		int operator +=(int d) {
			oldValue = i;
			i += d;
			parent->Update();
			return i;
		}

		int operator -= (int d) {
			oldValue = i;
			i -= d;
			parent->Update();
			return i;
		}

		int operator *= (int d) {
			oldValue = i;
			i *= d;
			parent->Update();
			return i;
		}

		int operator /= (int d) {
			oldValue = i;
			i /= d;
			parent->Update();
			return i;
		}

		bool operator > (int j) {
			return i>j;
		}

		bool operator < (int j) {
			return i<j;
		}

		bool operator ==(int j) {
			return i==j;
		}

		bool operator >= (int j) {
			return i>=j;
		}

		bool operator <= (int j) {
			return i<=j;
		}

		bool operator != (int j) {
			return i!=j;
		}

		operator int() {
			return i;
		}

		int old() {
			return oldValue;
		}

		void updateOld() {
			oldValue = i;
		}
	} ;