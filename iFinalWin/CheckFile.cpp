#include "iBase.h"
#include "iGraphics.h"
#include "DCircle.h"

DCircle *s;
void DrawRect(DGraphics *DG) {
	DG->ClearScreen(DG->Color(255,255,255));
	s->Draw();
}

DGraphics *g;

void drawArc() {
	g->PaintScreen();
}

void moveReg(Window *W) {
	static int i = 1,j=1;
	s->RadiusA += i;
	if (s->Radius >100 || s->Radius < 10)
		i *= -1;
	g->PaintScreen();
}

int CheckFile(char args[]) {
	Window w;
	w.handler(EventName::Close)->addHandler(&w,&Window::CloseWin);
	DGraphics graph(&w);
	
	DCircle r(100,100,20,D3DCOLOR_XRGB(0,255,0),&graph,true);
	r.setFillColor(D3DCOLOR_XRGB(0,0,150));
	
	s = &r;
	g = &graph;
	graph.eHandler(DGraphicsEvents::StartedDraw)->addHandler(DrawRect); 
	w.CreateTimer(10);
	w.handler(EventName::LeftDown)->addHandler(drawArc);
	w.handler(EventName::Timer)->addHandler(moveReg);
	w.Run();
	return 0;
}