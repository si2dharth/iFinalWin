#include "DEllipse.h"

class DCircle : public DEllipse {
public:
	DDrawingProp Radius;
	
	DCircle(int centerX,int centerY,int radius,DWORD Clr, DGraphics *container = 0,bool fill = false) : DEllipse(centerX,centerY,radius,radius,Clr,container,fill),Radius(this) { 
		Radius = radius;
		Radius.updateOld();
	}

	virtual bool ContainsPoint(int x,int y) { 
		return (sqrt((CenterX - x)*(CenterX - x) + (CenterY - y)*(CenterY - y)) <= Radius) ;
	}

	virtual void Draw() {
		DDrawing::Draw();
	}

	void Draw(int centerX,int centerY,int radius,DWORD Clr, DGraphics *container = 0,bool fill = false) {
		if (container == 0 && defContainer == 0) throw("No container specified");
		if (container == 0) container = defContainer;
		if (defContainer == 0) defContainer = container;
		if (!container->isDrawing()) return;

		DCircle C(centerX,centerY,radius,Clr,container,fill);
		C.Draw();
	}

	virtual void Update() {
		if (initializing) return;
		if (RadiusA != RadiusA.old()) {
			RadiusA.updateOld();
			Radius = RadiusA;
			RadiusB = Radius;
		}

		if (RadiusB != RadiusB.old()) {
			RadiusB.updateOld();
			Radius = RadiusB;
			RadiusA = Radius;
		}

		RadiusA = RadiusB = Radius;
		DEllipse::Update();
	};
};