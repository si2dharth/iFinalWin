#include "DImage.h"
#include "TrackBar.h"
#include "ImageProcessor.h"
#include <list>

using namespace std;

Camera *Cm;
Window *W;
Window *ShowWin;
int i = 1;
int decolorFactor = 1;
Button *B;
list<PointType> path;


void WinClose(Window *W,EventParams *EV) {
	Cm->Stop();
	W->CloseWin();
	ShowWin->CloseWin();
}

void ProcessPath() {
	if (path.size() < 4) return;
	PointType PathPoints[5];
	for (int i = 0; i<5; i++) 
	{
		PathPoints[i] = (*path.begin());
		if (i<4) path.pop_front();
	}

	for (int i = 1; i < 4; i++) 
		PathPoints[i] = (PathPoints[i -1] + PathPoints[i+1])/2;
	ShowWin->SetColor(Color(Color::Red));
	for (int i = 0; i < 4; i++)
		ShowWin->DrawLine(PathPoints[i].X,PathPoints[i].Y, PathPoints[i+1].X, PathPoints[i+1].Y);
}

void ProcessImage(ImageProcessor *I) {
	I->BlackAndWhite();
	//I->FlipX();
	if (decolorFactor > 1) I->Decolorize(decolorFactor);
	//I->OnlyRed(i);
	//I->BlackAndWhite();
	
	I->Factorize(i);
	//I->BlackAndWhite();
	I->IsolateWhite(decolorFactor);
	Rect R = I->FindWhiteRegion(true);//,decolorFactor);
	
	ShowWin->Clear();
	if (R.Width<0) {
		ShowWin->Clear();
		return;
	}
	static int x = -1, y, oldX, oldY;

	oldX = x;
	oldY = y;

	x = 1600-(((R.X<<1) + R.Width) >> 1)* 1600 / CAM_WIDTH;
	y =  (((R.Y<<1) + R.Height)>>1) * 900 / CAM_HEIGHT;

	if (oldX > 0) ShowWin->DrawLine(oldX,oldY, x, y);

	//path.push_back(PointType(x,y));
	ShowWin->DrawEllipse(x,y, 50,50);

	ShowWin->DrawLine(R.Height,400,R.Height,600);
	ProcessPath();
	I->ReplaceOriginal();
}

void keyPress(Window *W, KeyEventParams *K) {
	if (K->key == 27) W->Close(); 
	if (K->key == 107) i+=5;
	if (K->key == 109) i-=5;
}

void setI(TrackBar *TB) {
	i = TB->value;
}

void PrepareWindow(Window *W) {
	W->Write(10,100,L"Factorize Value:");
	TrackBar *TrB = new TrackBar;
	TrB->Width = 500;
	TrB->Height = 20;
	TrB->Location = PointType(150,100);
	TrB->MaxValue = 255;
	TrB->MinValue = 1;
	TrB->connectTo(i);
	TrB->AddToWindow(W);
	TrB->Draw();
	TrB->value = 200;

	W->Write(10,150,L"Decolorize Factor:");
	TrB = new TrackBar;
	TrB->Width = 500;
	TrB->Height = 20;
	TrB->Location = PointType(150,150);
	TrB->MaxValue = 255;
	TrB->MinValue = 1;
	TrB->connectTo(decolorFactor);
	TrB->AddToWindow(W);
	TrB->Draw();

	Window *W2 = new Window(0,"DisplayWin",1366,768,WindowStyles::PopUp);
	W2->handler(KeyUp)->addHandler(WinClose);
	ShowWin = W2;
}

int Run(char args[]) {
	Window W(PrepareWindow,0,"Settings",700,300);
	Cm = new Camera;
	::W = &W;
	ImageProcessor *IP = new ImageProcessor(Cm);
	
	Cm->SendRealData(true);
	W.handler(EventName::Close)->addHandler(WinClose);
	W.handler(EventName::KeyUp)->addHandler(keyPress);
	W.handler(EventName::RightUp)->addHandler(WinClose);
	//Cm->eHandler(CameraEvent::FeedRecieved)->addHandler(Process);
	IP->handler(ImageProcessor::Events::ImageAcquired)->addHandler(ProcessImage);
	W.Run();
	//delete TrB;
	delete Cm;
	return 0;
}