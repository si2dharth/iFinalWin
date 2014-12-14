#include "DDrawing.h"

class DArc :public DDrawing{
protected:
	bool initializing;
	bool filled;
	DShape Border,Body;
public:
	DDrawingProp CenterX,CenterY,RadiusA,RadiusB,StartAngle,StopAngle;

	DArc(int centerX,int centerY,int radiusA,int radiusB,int startAngle,int stopAngle,DWORD clr,DGraphics *container = 0, bool fill = false) : DDrawing(container),CenterX(this),CenterY(this),RadiusA(this),RadiusB(this),StartAngle(this),StopAngle(this),Border(container),Body(container) {
		initializing = true;
		Body.setColor(clr);
		Border.setColor(clr);
		Body.addVertex(centerX,centerY);
		
		int j = _Container->smoothness;
		float dAngle = (stopAngle - startAngle)/(float)j;
		int x,y;
		const double Converter = (22.0/7)/180;

		for (float i = (float)startAngle; i <= (float)stopAngle;  i += dAngle) {
			x = (int)(centerX + radiusA * cos(i*Converter));
			y = (int)(centerY + radiusB * sin(i*Converter));
			Body.addVertex(x,y);
			Border.addVertex(x,y);
		}
		Body.drawType = ptTriangleFan;
		Border.drawType = ptLineStrip;
		addShape(&Border);
		filled = false;
		setFill(fill);

		CenterX = centerX;
		CenterY = centerY;
		RadiusA = radiusA;
		RadiusB = radiusB;
		StartAngle = startAngle;
		StopAngle = stopAngle;
		CenterX.updateOld();
		CenterY.updateOld();
		RadiusA.updateOld();
		RadiusB.updateOld();
		StartAngle.updateOld();
		StopAngle.updateOld();

		initializing = false;
	}

	virtual void preDelete() {
		removeShape(&Body);
		removeShape(&Border);
	}

	virtual ~DArc() {
		preDelete();
	}

	void setFill(bool fill) {
		if (fill == filled) return;
		filled = fill;
		if (fill) shapes.push_front(&Body); else removeShape(&Body);
	}

	void setFillColor(DWORD Clr) {
		Body.setColor(Clr);
	}

	void setBorderColor(DWORD Clr) {
		Border.setColor(Clr);
	}

	void Draw() {
		DDrawing::Draw();
	}

	static void Draw(int centerX,int centerY,int radiusA,int radiusB,int startAngle,int stopAngle,DWORD clr,DGraphics *container = 0,bool fill = false) {
		if (container == 0 && defContainer == 0) throw("No container specified");
		if (container == 0) container = defContainer;
		if (defContainer == 0) defContainer = container;
		if (!container->isDrawing()) return;

		DArc a(centerX,centerY,radiusA,radiusB,startAngle,stopAngle,clr,container,fill);
		a.Draw();
	}

	virtual void Update() {
		if (initializing) return;

		Body.noUpdate = true;
		Border.noUpdate = true;

		Body.removeAllVertices();
		Border.removeAllVertices();

		Body.addVertex(CenterX,CenterY);
		
		int j = _Container->smoothness;
		float dAngle = (StopAngle - StartAngle)/(float)j;
		int x,y;
		const double Converter = (22.0/7)/180;

		for (float i = (float)StartAngle; i <= (float)StopAngle;  i += dAngle) {
			x = (int)(CenterX + RadiusA * cos(i*Converter));
			y = (int)(CenterY + RadiusB * sin(i*Converter));
			Body.addVertex(x,y);
			Border.addVertex(x,y);
		}

		Body.noUpdate = false;
		Border.noUpdate = false;
		Body.Process();
		Border.Process();
		
		CenterX.updateOld();
		CenterY.updateOld();
		RadiusA.updateOld();
		RadiusB.updateOld();
		StartAngle.updateOld();
		StopAngle.updateOld();
	}
};