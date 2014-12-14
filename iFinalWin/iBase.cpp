#include "iBase.h"

//A list of all the window classes ever defined for efficient Window Class Management.
list<string> DefinedClasses;

//A map to all the windows indexed by their Window Handles
map<HWND,Window*> Window::WindowList;

//An internal list to remove windows from the WindowList. Filled temporarily to list the windows to be removed.
list<Window*> Window::RemoveList;

//A list that holds the pointers to all the common timers.
vector<unsigned int> Window::Timers;

//A counter to ID the common timers.
unsigned int Window::curTimerID = 1;

//The traditional WindowProc
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (Window::SendMessage(hWnd,message,wParam,lParam) == 0)			//Send a common message to all the windows.
		return 0;														//Return if message is handled.
	
	return DefWindowProc (hWnd,message,wParam,lParam);					//Handle the message if the message is not handled explicitly.
}

//A function to create a Windows Class.
void CreateClass(HINSTANCE hInstance							//Handle to the instance. Passed in WinMain
				, LPSTR ClassName = "WindowClass"				//Name of the class to be defined
				, LPSTR cursor = IDC_ARROW						//The cursor to be displayed.
				)
{
	DefinedClasses.push_back(*(new string(ClassName)));			//Add the class name to the DefinedClasses list
	WNDCLASSEX wc;												//Make a new Windows Class
	ZeroMemory(&wc, sizeof(WNDCLASSEX));						//Clear all contents of the Class

    wc.cbSize = sizeof(WNDCLASSEX);								//Set the size.
    wc.style = CS_HREDRAW | CS_VREDRAW;							//Set the DrawStyle.
    wc.lpfnWndProc = WindowProc;								//Set the function to be called for messages. Here, set as the internal WindowProc
    wc.hInstance = hInstance;									//The Handle to the instance of the program.
    wc.hCursor = LoadCursor(NULL, cursor);						//The cursor to be displayed.
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		//The common background of the window. Here set as White
    wc.lpszClassName=ClassName;									//Set the name of class

    RegisterClassEx(&wc);										//Register the Class with windows.
}

HWND createWindow(HINSTANCE hInstance, int nCmdShow, LPSTR Title="My Window", int width=800, int height=600,DWORD windowStyle=WS_OVERLAPPED,LPSTR ClassName="WindowClass",int posX=-1,int posY=-1)
{    
    HWND hWnd;
	hWnd = CreateWindowEx(NULL,
                          ClassName,
						  Title,
						  windowStyle,
                          posX,posY,
                          width,height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
	ShowWindow(hWnd, nCmdShow);
	return hWnd;
}

//The handles passed by WinMain
HINSTANCE MainInstance, prevInstance;
int showReq;

template <class Type>
bool listContains(list<Type> &l,Type &o) {
	list<Type>::iterator i = l.begin();
	for (unsigned int I = 0; I<l.size(); I++) {
		if (o==*i) return true;
		i++;
	}
	return false;
};

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE pInstance,LPSTR args,int nCmdShow) {
	MainInstance = hInstance;
	prevInstance = pInstance;
	showReq = nCmdShow;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	Run(args);
	GdiplusShutdown(gdiplusToken);
}

Window::Window(HINSTANCE hi,LPSTR Title, int width, int height,WindowStyle ws, LPSTR ClassName,int PosX, int PosY) :curColor(Color(255,255,0,0)), backColor(255,255,255,255), fontColor(255,0,0,0), name(new char[100]), handler(this)  {
	if (!hi) hi = MainInstance;
	if (!listContains(DefinedClasses,(string)ClassName)) CreateClass(hi,ClassName);
	selfHandle = createWindow(hi,showReq,Title,width,height,ws,ClassName,PosX,PosY);
	LPRECT r = new tagRECT;
	GetWindowRect(selfHandle,r);
	this->width = r->right - r->left;
	this->height = r->bottom - r->top;
	delete r;
	graphics = new Graphics(selfHandle);
	WindowList[selfHandle] = this;
	fontStyle = new wchar_t[wcslen(L"Times New Roman") + 1];
	wcscpy_s(fontStyle,wcslen(L"Times New Roman") + 1,L"Times New Roman");
	font = new Font(fontStyle,24);
	fontBrush = new SolidBrush(fontColor);
	extra = 0;
	allocated = 0;
	deleteExtra = true;
	handler(MouseMove)->addHandler(this,&Window::LogMouseMovePosition);
	WaitingForEvent = false;
	CloseOnClick = true;
}

Window::Window(CreateFunc F,HINSTANCE hi,LPSTR Title, int width, int height,WindowStyle ws, LPSTR ClassName,int PosX, int PosY) :curColor(Color(255,255,0,0)), backColor(255,255,255,255), fontColor(255,0,0,0), name(new char[100]), handler(this) {
	if (!hi) hi = MainInstance;
	if (!listContains(DefinedClasses,(string)ClassName)) CreateClass(hi,ClassName);
	selfHandle = createWindow(hi,showReq,Title,width,height,ws,ClassName,PosX,PosY);
	this->width = width;
	this->height = height;
	graphics = new Graphics(selfHandle);
	WindowList[selfHandle] = this;
	fontStyle = new wchar_t[wcslen(L"Times New Roman") + 1];
	wcscpy_s(fontStyle,wcslen(L"Times New Roman") + 1,L"Times New Roman");
	font = new Font(fontStyle,24);
	fontBrush = new SolidBrush(fontColor);
	extra = 0;
	allocated = 0;
	deleteExtra = true;
	handler(MouseMove)->addHandler(this, &Window::LogMouseMovePosition);
	WaitingForEvent = false;
	CloseOnClick = true;
	F(this);
}

void Window::SaveExtra(void *Data,size_t s, bool deleteLater) {
	if (allocated>0 || extra) RemoveExtra();
	deleteExtra = deleteLater;
	extra = malloc(s);
	allocated = s;
	memcpy(extra,Data,allocated);
}

void *Window::GetExtra() {
	return extra;
}

void Window::GetExtra(void *Data) {
	memcpy(Data,extra,allocated);
}

size_t Window::GetExtraSize() {
	return allocated;
}

void Window::RemoveExtra() {
	if (deleteExtra) free(extra);
	allocated = 0;
	extra = 0;
}

Window::~Window() {
	handler.raise(WindowExiting);
	while (privateTimers.size() > 0) {
		KillTimer(selfHandle,privateTimers[privateTimers.size()-1]);
		privateTimers.pop_back();
	}
	delete font;
	delete[] fontStyle;
	delete fontBrush;
	if (allocated) if (deleteExtra) if (extra) free(extra);
	WindowList.erase(selfHandle);
}

void *Window::operator new(size_t size) {
	void *storage = malloc(size);
	if (storage==NULL)
		throw ("Not enough memory for window creation");
	((Window*)storage)->NotToDelete = false;
	return storage;
}

void Window::Run() {
	MSG msg;
	
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (RemoveList.size()!=0) {
			list<Window*>::iterator i = RemoveList.begin();
			for (unsigned int I=0; I<RemoveList.size(); I++) {
				bool del = !(*i)->NotToDelete;
				WindowList.erase((*i)->selfHandle);
				if (del) delete (*i);
				i++;
			}
		}	
	}
	while (Timers.size() > 0) {
		KillTimer(NULL,Timers[Timers.size()-1]);
		Timers.pop_back();
	}
}

int Window::SendMessage(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) {
	int e;
	Window *RecievingWindow = WindowList[hWindow];

	if (msg == EventName::Close) {
		if (RecievingWindow) if (RecievingWindow->CloseOnClick) RecievingWindow->Close();
	}
	if (RecievingWindow) if (RecievingWindow->WaitingForEvent && RecievingWindow->EN != msg) return 1;
	if (msg == EventName::Timer) {
		if (std::find(Timers.begin(),Timers.end(),wParam) != Timers.end()) {
			map<HWND,Window*>::iterator t = WindowList.begin();
			for (unsigned int i = 0; i<WindowList.size(); i++) {
				t->second->handler.raise(EventName::Timer,GetEvent(EventName(msg),wParam,lParam,e));
				t++;
			}
			return 1;
		}
	}

	
	if (!RecievingWindow) return 1;
	if (!RecievingWindow->handler.HandlerDefined(EventName(msg))) return 1;
	EventParams *E = GetEvent(EventName(msg),wParam,lParam,e);
	int returnValue = RecievingWindow->RaiseEvent(EventName(msg),E);
	return 1;
}

void Window::AddHandler(EventName en,EventFunc ef) {
	handler.event(en)->addHandler(ef);
}

void Window::RemoveHandler(EventName en,EventFunc ef) {
	if (handler.HandlerDefined(en)) handler.event(en)->removeHandler(ef);
}

int Window::RaiseEvent(EventName en,EventParams *e) {
	if (handler.empty()) return 1;
	if (!handler.HandlerDefined(en)) return 1;
	handler(en,e);
	return 0;
}

void Window::SetColor(int r,int g,int b,int A) {
	curColor.SetColor(Color(A,r,g,b));
}

void Window::SetColor(Color C) {
	curColor.SetColor(C);
}

void Window::DrawPixel(int x,int y,Color C) {
	graphics->DrawLine(&Pen(C),x,y,x+1,y);
}

void Window::DrawLine(int x1,int y1,int x2,int y2) {
	graphics->DrawLine(&curColor,x1,y1,x2,y2);	
}

void Window::DrawEllipse(int x1,int y1,int x2,int y2) {
	graphics->DrawEllipse(&curColor,x1,y1,x2,y2);
}

void Window::DrawRectangle(int x1,int y1,int x2,int y2) {
	graphics->DrawRectangle(&curColor,x1,y1,x2-x1,y2-y1);
}

void Window::SetBackColor(int r,int g,int b,int A) {
	backColor = Color(A,r,g,b);
}

void Window::SetBackColor(Color C) {
	backColor.SetValue(C.GetValue());
}

void Window::SetFontColor(int r,int g,int b,int a) {
	SetFontColor(Color(a,r,g,b));
}

void Window::SetFontColor(Color C) {
	fontColor = C;
	if (fontBrush->GetType() == BrushType::BrushTypeSolidColor)
		((SolidBrush*)fontBrush)->SetColor(C);
}

void Window::SetFontHatchColor(HatchStyle H,Color Main,Color Back) {
	delete fontBrush;
	fontBrush = new HatchBrush(H,Main,Back);
	SetFontColor(Main);
}

void Window::SetFontGradientColor(Color c1,Color c2,Point P1,Point P2) {
	delete fontBrush;
	fontBrush = new LinearGradientBrush(P1,P2,c1,c2);
	SetFontColor(c1);
}

void Window::SetFontGradientColor(Color c1,Color c2,int x1,int y1,int x2,int y2) {
	SetFontGradientColor(c1,c2,Point(x1,y1),Point(x2,y2));
}

void Window::SetFontStyle(wchar_t *fontName) {
	Font *newFont = new Font(fontName,font->GetSize(),font->GetStyle(),font->GetUnit());
	if (!newFont->IsAvailable()) {
		delete newFont;
		throw("Invalid font style");
	}
	delete [] fontStyle;
	fontStyle = new wchar_t[wcslen(fontName) + 1];
	wcscpy_s(fontStyle,wcslen(fontName) + 1,fontName);
	delete font;
	font = newFont;
}

void Window::SetFontSize(int size) {
	Font *newFont = new Font(fontStyle,float(size),font->GetStyle(),font->GetUnit());
	delete font;
	font = newFont;
}

Color Window::GetColor() {
	return 0;
}

Color Window::GetBackColor() {
	return backColor.GetValue();
}

Color Window::GetFontColor() {
	return fontColor;
}

RectF Window::Write(int x,int y,const wchar_t* s) {
	PointF pointF((float)x,(float)y);
	graphics->DrawString(s,-1,font,pointF,fontBrush);
	RectF r;
	graphics->MeasureString(s,-1,font,pointF,&r);
	return r;
}

void Window::Clear() {
	graphics->Clear(backColor);
}

HDC &Window::StartPaint() {
	hdc = BeginPaint(selfHandle,&ps);
	return hdc;
}

void Window::StopPaint() {
	EndPaint(selfHandle,&ps);
}

void Window::Close() {
	WaitingForEvent = false;
	if (WindowList.size() == 1) {
		PostQuitMessage(0);
	}
	else {
		ShowWindow(selfHandle,HIDE_WINDOW);
		RemoveList.push_back((Window*)this);
	}
}

void Window::CloseWin() {
	Close();
}

unsigned int Window::CreateGlobalTimer(unsigned long int interval, unsigned int TimerID) {
	if (TimerID == 0 || std::find(Timers.begin(),Timers.end(),TimerID) != Timers.end())
		TimerID = curTimerID++;
	SetTimer(NULL,TimerID,interval,(TIMERPROC)NULL);
	Timers.push_back(TimerID);
	return TimerID;
}

unsigned int Window::CreateTimer(unsigned long int interval,unsigned int TimerID) {
	if (TimerID == 0 || std::find(privateTimers.begin(),privateTimers.end(),TimerID) != Timers.end())
		TimerID = curTimerID++;
	SetTimer(selfHandle,TimerID,interval,(TIMERPROC)NULL);
	privateTimers.push_back(TimerID);
	return TimerID;
}

void Window::DeleteGlobalTimer(unsigned int TimerID) {
	Timers.erase(std::find(Timers.begin(),Timers.end(),TimerID));
}

void Window::DeleteTimer(unsigned int TimerID) {
	privateTimers.erase(std::find(privateTimers.begin(),privateTimers.end(),TimerID));
}

Graphics *Window::GetGraphics() {
	return graphics;
}

HWND Window::Handle() {
	return selfHandle;
}

int Window::GetHeight() {
	return height;
}

int Window::GetWidth() {
	return width;
}

void Window::getMousePosition(int &X,int &Y) {
	X = mx;
	Y = my;
}

void Window::WaitForEvent(EventName EN){
	MSG msg;
	WaitingForEvent = true;
	this->EN = EN;
	handler(EN)->addHandler(this, &Window::StopWaiting);
	while (GetMessage(&msg, NULL, 0, 0) && WaitingForEvent)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (RemoveList.size() != 0) {
			list<Window*>::iterator i = RemoveList.begin();
			for (unsigned int I = 0; I<RemoveList.size(); I++) {
				bool del = !(*i)->NotToDelete;
				WindowList.erase((*i)->selfHandle);
				if (del) delete (*i);
				i++;
			}
		}
	}
}