#include "DArc.h"

class DEllipse : public DArc {
public:
	DEllipse(int centerX, int centerY,int radiusA,int radiusB,DWORD Clr,DGraphics *container = 0,bool fill = false) : DArc(centerX,centerY,radiusA,radiusB,0,360,Clr,container,fill) {
	}

	virtual void Draw() {
		DDrawing::Draw();
	};

	static void Draw(int centerX, int centerY,int radiusA,int radiusB,DWORD Clr,DGraphics *container = 0,bool fill = false) {
		if (container == 0 && defContainer == 0) throw("No container specified");
		if (container == 0) container = defContainer;
		if (defContainer == 0) defContainer = container;
		if (!container->isDrawing()) return;

		DEllipse e(centerX,centerY,radiusA,radiusB,Clr,container,fill);
		e.Draw();
	}

	virtual void Update() {
		if (initializing) return;
		StartAngle = 0;
		StopAngle = 360;
		StartAngle.updateOld();
		StopAngle.updateOld();
		DArc::Update();
	}
};