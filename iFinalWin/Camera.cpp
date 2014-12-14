#include "Camera.h"

HRESULT ( __stdcall * Receive_ ) ( void* inst, IMediaSample *smp );				//The windows Recieve function that displays the image in its own window.
HRESULT   __stdcall   Receive    ( void* inst, IMediaSample *smp );				//The private Recieve function that may reprocess tha image, before displaying it(if at all displaying)

//Holds the address of the camera connected, if on
Camera* curCamera = 0;

HRESULT __stdcall Receive(void *inst,IMediaSample *smp) {
	if (curCamera) 
		return curCamera->inReceive(inst,smp);
	else
		return 0;
}


//Camera Functions  :

Camera::Camera(bool Show,bool Start) : eHandler(this),_realData(false),_UpdateWindow(Show),_LastData(0),_CurData(0) {
	DWORD no;
	IGraphBuilder *graph = 0;
	ctrl = 0;
	ICreateDevEnum *devs = 0;
	IEnumMoniker *cams = 0;
	IMoniker *mon = 0;
	IBaseFilter *cam = 0;
	IEnumPins *pins = 0;
	IPin *pin = 0;
	IEnumFilters *fil = 0;
	IBaseFilter *rnd = 0;
	IMemInputPin *mem = 0;
	curCamera = this;
	_isOn = Start;

	CoCreateInstance( CLSID_FilterGraph, 0, CLSCTX_INPROC,IID_IGraphBuilder, (void **)&graph );
	graph->QueryInterface( IID_IMediaControl, (void **)&ctrl );
	CoCreateInstance (CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC, IID_ICreateDevEnum, (void **) &devs);
	devs->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &cams, 0);
	cams->Next (1,&mon,0);												// get first found capture device (webcam)    
	mon->BindToObject(0,0,IID_IBaseFilter, (void**)&cam);
	
	graph->AddFilter(cam, L"Capture Source");							// add web cam to graph as source
	cam->EnumPins(&pins);												// we need output pin to autogenerate rest of the graph
	pins->Next(1,&pin, 0);												// via graph->Render
	graph->Render(pin);													// graph builder now builds whole filter chain including MJPG decompression on some webcams
	graph->EnumFilters(&fil);											// from all newly added filters
	fil->Next(1,&rnd,0);												// we find last one (renderer)
	rnd->EnumPins(&pins);												// because data we are intersted in are pumped to renderers input pin 
	pins->Next(1,&pin, 0);												// via Receive member of IMemInputPin interface
	pin->QueryInterface(IID_IMemInputPin,(void**)&mem);

	DsHook(mem,6,Receive);												// so we redirect it to our own proc to grab image data

	if (Start) this->Start();
}

Camera::~Camera() {
	Stop();
	if (_LastData) delete _LastData;
	if (_CurData) delete _CurData;
	//if (ctrl) delete ctrl;
}

void Camera::Start() {
	ctrl->Run();
	eHandler(CameraEvent::CameraStarted);
}

void Camera::Stop() {
	if (!_isOn) return;
	ctrl->Stop();
	_isOn = false;
	curCamera = 0;
	eHandler(CameraEvent::CameraClosed);
}

void Camera::setData(CameraFeed *newData) {
	if (_LastData) delete _LastData;
	_LastData = _CurData;
	_CurData = newData;
	Receive_(newData->getInst(),newData->getSmp());
}

CameraFeed &Camera::getLastData() {
	return *_LastData;
}

CameraFeed &Camera::getCurrentData() {
	return *_CurData;
}

void Camera::SendRealData(bool enable) {
	_realData = enable;
}

void Camera::ShowInWindow(bool enable) {
	_UpdateWindow = enable;
}

HRESULT Camera::inReceive(void *inst,IMediaSample *smp) {
	BYTE *buf;
	smp->GetPointer(&buf);
	DWORD len = smp->GetActualDataLength();

	if (_LastData) delete _LastData;
	_LastData = _CurData;
	_CurData = new CameraFeed(inst,smp,!_realData);
	eHandler(CameraEvent::FeedRecieved,_CurData);

	if (_UpdateWindow)
		return Receive_(inst,smp);
	else
		return 0;
}



//CameraFeed Methods
CameraFeed::CameraFeed(void *inst,IMediaSample *smp,bool copy) {
	_inst = inst;
	_smp = smp;
	_width = CAM_WIDTH;
	_height = CAM_HEIGHT;
	data = 0;
	_destroy = copy;
	_Len = smp->GetActualDataLength();
	BYTE *tData;							//Temporary
	smp->GetPointer(&tData);
	setData(tData,copy);
}

CameraFeed::CameraFeed(CameraFeed &CF) {
	(*this) = CF;
}

CameraFeed::~CameraFeed() {
	destroy();
}

CameraFeed &CameraFeed::operator = (CameraFeed &CF) {
	_Len = CF._Len;
	_inst = CF._inst;
	_smp = CF._smp;
	_destroy = true;
	_width = CF._width;
	_height = CF._height;

	data = new BYTE[_Len];
	memcpy(data,CF.data,_Len*sizeof(BYTE));
	return (*this);
}

void *CameraFeed::getInst() {
	return _inst;
}

IMediaSample *CameraFeed::getSmp() {
	return _smp;
}

void CameraFeed::destroy(bool IgnoreCopy) {
	if (!data) return;
	if (_destroy || IgnoreCopy)
		delete [] data;
	_destroy = false;
	data = 0;
}

void CameraFeed::setData(BYTE *Data,bool copy) {
	
	if (copy) {
//		destroy();
//		data = new BYTE[_Len];
		memcpy(data,Data,_Len*sizeof(BYTE));
	} else
	{
		if (data) delete []data;
		data = 0;
		data = Data;
	}
}

BYTE *CameraFeed::getRawData(bool copy) {
	if (copy)
	{
		BYTE *tData;
		tData = new BYTE[_Len];
		memcpy(tData,data,_Len*sizeof(BYTE));
		return tData;
	}
	return data;
}

DWORD CameraFeed::getDataLength() {
	return _Len;
}

void CameraFeed::setSize(int width,int height) {
	_width = width;
	_height = height;
}

unsigned int CameraFeed::Height() {
	return _height;
}

unsigned int CameraFeed::Width() {
	return _width;
}

void CameraFeed::getRGB(int x,int y,int &R, int &G, int &B) {
	if (x>_width || y>_height) return;
	DWORD Pix = getPos(x,y);
	int Y,U,V;
	getYUV(&data[Pix],Y,U,V);
	getRGB(Y,U,V,R,G,B);
}

Color CameraFeed::getColor(int x,int y) {
	int R,G,B;
	getRGB(x,y,R,G,B);
	return Color(R,G,B);
}

void CameraFeed::setRGB(int x,int y,int R,int G,int B) {
	setRGB(&data[getPos(x,y)],R,G,B);
}

void CameraFeed::setColor(int x,int y,Color &C) {
	setRGB(x,y,C.GetR(),C.GetG(),C.GetB());
}

void CameraFeed::setRGB(BYTE *Data,int R, int G, int B) {
	int Y,U,V;
	getYUV(R,G,B,Y,U,V);
	setYUV(Data,Y,U,V);
}

void CameraFeed::setColor(BYTE *Data,Color &C) {
	setRGB(Data,C.GetR(),C.GetG(),C.GetB());
}

void CameraFeed::setYUV(BYTE *Data, int Y, int U, int V) {
	Data[0]	= Y;
	Data[1] = U;
	Data[2] = Y;
	Data[3] = V;
}

void CameraFeed::setYUV(int x,int y, int Y, int U, int V) {
	setYUV(&data[getPos(x,y)],Y,U,V);
}

void CameraFeed::getRGB(BYTE *data,int &R,int &G,int &B) {
	int Y,U,V;
	getYUV(data,Y,U,V);
	getRGB(Y,U,V,R,G,B);
}

Color CameraFeed::getColor(BYTE *data) {
	int R,G,B;
	getRGB(data,R,G,B);
	return Color(R,G,B);
}

#define clip(x) x<0? 0 : (x>255? 255 : x)
void CameraFeed::getRGB(int Y,int U,int V,int &R,int &G,int &B) {
	int C,E,D;

	C = Y - 16;
	D = U - 128;
	E = V - 128;
	R = clip(( 298 * C           + 409 * E + 128) >> 8);
	G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8);
	B = clip(( 298 * C + 516 * D           + 128) >> 8);
}

Color CameraFeed::getColor(int Y,int U,int V) {
	int R,G,B;
	getRGB(Y,U,V,R,G,B);
	return Color(R,G,B);
}

void CameraFeed::getYUV(BYTE *data,int &Y,int &U,int &V) {
	Y = (data[0] + data[2]) >> 1;
	U = data[1];
	V = data[3];
}

void CameraFeed::getYUV(int R,int G,int B,int &Y,int &U,int &V) {
	Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
	U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
	V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;
}

void CameraFeed::getYUV(Color &C,int &Y,int &U,int &V) {
	getYUV(C.GetR(),C.GetG(),C.GetB(),Y,U,V);
}

void CameraFeed::getYUV(int x,int y,int &Y,int &U,int &V) {
	DWORD Pix = getPos(x,y);
	getYUV(&data[Pix],Y,U,V);
}

int CameraFeed::getY(int x,int y) {
	DWORD Pix = getPos(x,y);
	return (data[Pix] + data[Pix+2])>>1;
}

int CameraFeed::getY(BYTE* data) {
	return (data[0] + data[2])>>1;
}

DWORD CameraFeed::getPos(int x,int y) {
	x = x % 2 ? x - 1 : x;					//Make x even
	return (y * _width + x) << 1;
}

void CameraFeed::getLocation(DWORD pos,int &X,int &Y) {
	pos >>= 1;				//As the data is divided in 2 blocks of info per pixel
	X = pos % _width;
	Y = pos / _width;
}

Bitmap *CameraFeed::ToBitmap() {
	Bitmap *Bmp = new Bitmap(_width,_height,16,PixelFormat16bppRGB565,data);
	return Bmp;
}

DWORD **CameraFeed::GetArray(bool HighRes, DWORD **Clr) {
	
	if (!Clr) {
		Clr = new DWORD*[_height];
	
		for (int i = 0; i<_height; i++) {
			DWORD *xClr = new DWORD[_width];
			Clr[i] = xClr;
		}
	}

	BYTE *dataCopy = getRawData(true);
	DWORD curByte = 0;

	for (int i = 0; i < _height; i++)
		for (int j = 0; j < _width; j+=2, curByte += 4)
		{
			int R,G,B;
			getRGB(&dataCopy[curByte],R,G,B);
			Clr[i][j] = ((1<<24)|(((R)&0xff)<<16)|(((G)&0xff)<<8)|((B)&0xff));
		}

	delete []dataCopy;
	return Clr;
}

