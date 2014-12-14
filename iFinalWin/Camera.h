#pragma once

#include <dshow.h>
#include "EventHandler.h"
#include <GdiPlus.h>

using namespace std;
using namespace Gdiplus;

#pragma comment(lib,"Strmiids.lib")
#pragma comment (lib,"Gdiplus.lib")

#define CAM_HEIGHT 480
#define CAM_WIDTH 640

enum CameraEvent {
	CameraStarted,
	FeedRecieved,
	CameraClosed
};

//The feed that the camera returns. Contains common methods.
class CameraFeed {
	BYTE* data;
	DWORD _Len;
	void *_inst;
	IMediaSample *_smp;
	bool _destroy;
	int _width,_height;
public:
	//Construct CameraFeed directly from original feed
	CameraFeed(void *inst,IMediaSample *smp,bool copy = true);

	//Construct a CameraFeed from another CameraFeed
	CameraFeed(CameraFeed &CF);

	//Destroys the data if it was copied.
	~CameraFeed();

	//Copies the data
	CameraFeed &operator = (CameraFeed &CF);

	//Get "inst" field
	void *getInst();

	//Get Pointer to original IMediaSample
	IMediaSample *getSmp();

	//Destroy the data associated. If IgnoreCopy is true, the data will be detroyed even if it has not been copied to the object.
	void destroy(bool IgnoreCopy = false);

	//Change the data associated with the object. Destroys the previous data if it was copied to the feed.
	void setData(BYTE* Data,bool copy = true);

	//Get the Raw Data in BYTE* form. Gives a copy if "copy" is true.
	//If copied,the programmer should delete the data after use.
	BYTE* getRawData(bool copy = false);

	//Get the Length of the raw data
	DWORD getDataLength();

	//Set the resolution of the feed in pixels.
	void setSize(int width,int height);

	//Height of the camera feed
	unsigned int Height();

	//Width of the camera feed
	unsigned int Width();

	//Get R-G-B values for a given pixel. Use the entire feed for a large-scale operation for performance optimization
	void getRGB(int x,int y,int &R,int &G,int &B);

	//Get a Color object for a given pixel. Use the entire feed for large-scale operation.
	Color getColor(int x,int y);

	//Set R-G-B values for a pixel
	void setRGB(int x,int y,int R,int G,int B);

	//Set R-G-B values for a data address in the feed
	void setRGB(BYTE *Data,int R, int G, int B);

	//Set Color for a data address in the feed
	void setColor(BYTE *Data,Color &C);

	//Set Y-U-V values for a data address in the feed
	void setYUV(BYTE *Data, int Y, int U, int V);

	//Set Y-U-V values for a pixel
	void setYUV(int x, int y, int Y, int U, int V);

	//Set Color for a pixel
	void setColor(int x,int y, Color &C);
	
	//Get RGB values from a pointer to a 4-Byte camera data
	static void getRGB(BYTE *data,int &R,int &G,int &B);

	//Get Color object from a pointer to a 4-Byte camera data
	static Color getColor(BYTE *data);

	//Get RGB values from Y-U-V data
	static void getRGB(int Y,int U,int V,int &R,int &G,int &B);

	//Get Color object from Y-U-V data
	static Color getColor(int Y,int U,int V);

	//Get Y-U-V values from pointer to 4-Byte camera data
	static void getYUV(BYTE* data,int &Y,int &U,int &V);
	
	//Get Y-U-V values from RGB values
	static void getYUV(int R,int G,int B,int &Y,int &U,int &V);

	//Get Y-U-V values from Color object
	static void getYUV(Color &C,int &Y,int &U,int &V);

	//Get Y-U-V values at x,y
	void getYUV(int x,int y,int &Y,int &U,int &V);

	//Get Y value at x,y
	int getY(int x,int y);

	//Get Y value for a data
	int getY(BYTE *data);

	//Get the position in buffer according to x,y coordinates
	DWORD getPos(int x,int y);

	//Get the position for a given position of buffer in data
	void getLocation(DWORD pos,int &X, int &Y);

	//Export to an Image object. DOES NOT WORK
	Bitmap *ToBitmap();

	//Export to a 2D array. Exports in higher resolution if HighRes = true. Array to be deleted by prograammer. If Clr memory is not passed, the function creates the memory by itself.
	DWORD **GetArray(bool HighRes = false,DWORD **Clr = 0);

};

//Camera object. Provides basic functions to handle the webcam feed.
class Camera {
	IMediaControl *ctrl;
	bool _isOn;
	bool _realData;
	bool _UpdateWindow;
	CameraFeed *_LastData;
	CameraFeed *_CurData;

	friend HRESULT __stdcall Receive(void *inst,IMediaSample *smp);
	//The internal Receive procedure
	HRESULT inReceive(void *inst,IMediaSample *smp);
public:
	//Initialize data. If Start is true, starts the camera, and shows the feed in a window if Show is true.
	Camera(bool Show = true,bool Start = true);
	
	//Stops and destroys the camera
	~Camera();

	//Starts the camera
	void Start();

	//Stop the camera
	void Stop();

	//Set the window to newData frame
	void setData(CameraFeed *newData);
	
	//Get Last frame camera data. Modifiable.
	CameraFeed &getLastData();

	//Get current frame data. Modifiable.
	CameraFeed &getCurrentData();

	//Set if the original frame data is sent or a copy of the data. Working on original data can reduce performance.
	void SendRealData(bool enable);

	//Set whether the camera feed is shown in its original window
	void ShowInWindow(bool enable);

	//List of functions where every frame of data is sent
	EventHandler<CameraEvent,Camera> eHandler;
};



#define DsHook(a,b,c)												\
	if (!c##_)														\
	{																\
		INT_PTR* p=b+*(INT_PTR**)a;									\
		VirtualProtect(&c##_,4,PAGE_EXECUTE_READWRITE,&no);			\
		*(INT_PTR*)&c##_=*p;										\
		VirtualProtect(p,4,PAGE_EXECUTE_READWRITE,&no);				\
		*p=(INT_PTR)c;												\
	}

