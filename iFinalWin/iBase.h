#pragma once
#include "Events.h"
#include "EventHandler.h"
#include "enums.h"
#include "MemoryObjectManager.h"
#include <string>
#include <map>
#include <vector>
#include <list>
#include <ObjIdl.h>
#include <GdiPlus.h>
#pragma comment (lib,"Gdiplus.lib")

using namespace std;
using namespace Gdiplus;

typedef int ShowStyle;
typedef DWORD WindowStyle;

class Window;
typedef void (*EventFunc)(Window*,EventParams*);
typedef void (*CreateFunc)(Window* sender);

//An advanced class to  handle Windows easily
class Window : public ManagedObject {
	HDC hdc;
	PAINTSTRUCT ps;
	static Window *currentWindow;
	static map<HWND,Window*> WindowList;
	wchar_t *className;
	Graphics *graphics;
	Pen curColor;
	Brush *fontBrush;
	wchar_t* fontStyle;
	Font *font;
	Color backColor;
	Color fontColor;

	Window *callingWindow;

	void *extra;
	size_t allocated;
	static unsigned int curTimerID;
	bool WaitingForEvent;
	EventName EN;

	//Last mouse X and Y position
	int mx,my;

	void StopWaiting() { WaitingForEvent = false; }

	//Internal mouse logger
	void LogMouseMovePosition(Window *W,MouseEventParams *MP) {
		mx = MP->x;
		my = MP->y;
	}
protected:
	static vector<unsigned int> Timers;
	vector<unsigned int> privateTimers;
	static list<Window*> RemoveList;
	bool NotToDelete;
	bool deleteExtra;
	HWND selfHandle;
	int width;
	int height;

	BITMAP *b;

public:
	//Directly Close Window when Close button is pressed
	bool CloseOnClick;

	//The handler for Windows events
	EventHandler<EventName,Window,EventParams> handler;
	
	//The name of the window(not title)
	string name;

	//Normal Constructor. hi is the instance of the calling WinMain, Title is the required title of window, width,height are sizes, WindowStyle is the style of window according to standard window, ClassName is the name of class, PosX,PosY specify the location of window
	Window(HINSTANCE hi=0,LPSTR Title="My Window", int width=CW_USEDEFAULT, int height=CW_USEDEFAULT,WindowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, LPSTR ClassName = "WindowClass",int PosX = CW_USEDEFAULT, int PosY = CW_USEDEFAULT);
	
	//Same as normal constructor. The function F is called as soon as the window is constructed. Function takes one argument : the pointer to the window created
	Window(CreateFunc F,HINSTANCE hi=0,LPSTR Title="My Window", int width=CW_USEDEFAULT, int height=CW_USEDEFAULT,WindowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, LPSTR ClassName = "WindowClass",int PosX = CW_USEDEFAULT, int PosY = CW_USEDEFAULT);
	
	//Normal Destructor.
	virtual ~Window();

	//Automatically called, so as to register the windows that are dynamically created.
	void* operator new(size_t);

	//
	void SaveExtra(void *Data,size_t,bool deleteLater = true);
	
	
	void GetExtra(void *Data);
	size_t GetExtraSize();
	void *GetExtra();
	void RemoveExtra();

	//Create a timer common to all windows that ticks at specified interval. Returns the ID of the timer for future reference
	static unsigned int CreateGlobalTimer(unsigned long int interval,unsigned int TimerID = 0);
	
	//Create a timer private to the window that ticks at specified interval. Returns ID of the timer for future reference.
	unsigned int CreateTimer(unsigned long int interval,unsigned int TimerID = 0);
	
	//Delete the specified timer common to all the windows.
	static void DeleteGlobalTimer(unsigned int TimerID);

	//Delete the specified private timer.
	void DeleteTimer(unsigned int TimerID);
	
	//Returns the handle to the window.
	HWND Handle();

	//Returns the height of the window.
	int GetHeight();

	//Returns the width of the window.
	int GetWidth();

	//Send a common message to all windows. The associated window responds.
	static int SendMessage(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

	//Add a Handler function for an event. Use the handler object for better features.
	virtual void AddHandler(EventName, EventFunc);

	//Remove a handler function for an event. Use the handler object for better features.
	virtual void RemoveHandler(EventName, EventFunc);

	//Run. Function runs and controls all the windows.
	static void Run();

	//Explicitly raise an event with given parameters.
	int RaiseEvent(EventName,EventParams*);

	//Set a RGB color for future draw commands.
	void SetColor(int r,int g,int b,int A = 255);

	//Set a Color for future draw commands.
	void SetColor(Color);

	//Set a RGB back color for future draw commands.
	void SetBackColor(int r,int g,int b,int A = 255);

	//Set a back color for future draw commands.
	void SetBackColor(Color);

	//Get the current color.
	Color GetColor();

	//Get the current back color.
	Color GetBackColor();		

	//Clear the entire window.
	void Clear();

	//Put a pixel of color C at location X,Y
	void DrawPixel(int X,int Y,Color C);

	//Draw a Line from X1,Y1 to X2,Y2 . The Color is set by using SetColor
	void DrawLine(int x1, int y1, int x2, int y2);

	//Draw an ellipse bounded by rectangle by coordinates x1,y1,x2,y2. The Color is set by using SetColor
	void DrawEllipse(int x1,int y1,int x2,int y2);

	//Draw a rectangle with opposite corners at x1,y1 and x2,y2. The color is set by using SetColor
	void DrawRectangle(int x1,int y1,int x2,int y2);

	//Set a RGB color for writing text.
	void SetFontColor(int r,int g,int b,int alpha = 255);

	//Set a color for wriing text.
	void SetFontColor(Color);

	//Draw font with hatch style H. MainColor and BackColor are colors for the hatch.
	void SetFontHatchColor(HatchStyle H,Color MainColor,Color BackColor);

	//Set Font Color Gradient from Color1 to Color2, changing from Point Structure P1 to Point Structure P2.
	void SetFontGradientColor(Color Color1,Color Color2,Point P1,Point P2);

	//Set Font Color Gradient from Color1 to Color2, changing from x1,y1 to x2,y2.
	void SetFontGradientColor(Color Color1,Color Color2,int x1,int y1,int x2,int y2);

	//Set the font for writing text. Throws "Invalid Font Style" is style fontName does not exist.
	void SetFontStyle(wchar_t *fontName);

	//Set the size of the font.
	void SetFontSize(int size);

	//Get the current font color.
	Color GetFontColor();

	//Write wchar_t string s1 at coordinates x,y
	RectF Write(int x,int y,const wchar_t* s1);

	//Starts Window paint. Return the handle to Device Context for drawing use.
	HDC &StartPaint();

	//Stops Window paint.
	void StopPaint();

	//Get a pointer to the graphics object. Can be used without starting paint.
	Graphics *GetGraphics();

	//Get the last mouse position
	void getMousePosition (int &X, int &Y);

	//Ignore all events until a specific event occurs and get parameters of the event occured
	void WaitForEvent(EventName);

	//Closes the window.
	void Close();

	//Inheritable Close procedure for last minute tasks.
	virtual void CloseWin();
};

//A function as a replacement to main/WinMain.
int Run(char args[]);