#include "CV.h"

list<CameraProcessFunction> CPF;
bool _ShowWindow = false;

HRESULT ( __stdcall * Receive_ ) ( void* inst, IMediaSample *smp );				//The windows Recieve function that displays the image in its own window.
HRESULT   __stdcall   Receive    ( void* inst, IMediaSample *smp );				//The private Recieve function that may reprocess tha image, before displaying it(if at all displaying)

void StartCamera() {
	DWORD no;

	IGraphBuilder* graph = 0;
	IMediaControl* ctrl = 0;
	ICreateDevEnum* devs = 0;
	IEnumMoniker* cams = 0;
	IMoniker* mon  = 0;
	IBaseFilter* cam  = 0;
	IEnumPins* pins = 0;
	IPin* pin = 0;
	IEnumFilters* fil  = 0;
	IBaseFilter* rnd  = 0;
	IMemInputPin*   mem  = 0;

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

    ctrl->Run();
};

void RunCPF(BYTE *TmpBuf,DWORD len){
	list<CameraProcessFunction>::iterator I = CPF.begin();
	for (int i = 0; i<CPF.size(); i++) {
		(*I)(TmpBuf,len);
	}
}

HRESULT   __stdcall   Receive    ( void* inst, IMediaSample *smp ) {
	BYTE* buf,*mBuf;
	smp->GetPointer(&buf);
	DWORD len = smp->GetActualDataLength();
	mBuf = (BYTE*)malloc(sizeof(BYTE)*len);
	memcpy(mBuf,buf,sizeof(BYTE)*len);
	RunCPF(mBuf,len);
	free(mBuf);

	if (_ShowWindow) {
		HRESULT   ret  =  Receive_   ( inst, smp );   
		return ret;
	} else
		return 0;
}

void CloseCamera() {
	//NOTHING
}

list<CameraProcessFunction> &ImageGetH() {
	return CPF;
}

void SetImageShow(bool enable) {
	_ShowWindow = enable;
}

void getRGB(BYTE* Clr,BYTE &R,BYTE &G,BYTE &B){
	BYTE Cr,Cb;
	unsigned int Y;
	 Y = (Clr[0] + Clr[2]) >> 1;
	Cb = Clr[1];
	Cr = Clr[3];

	int r,g,b;
	r = Y                        + 1.402   * (Cr - 128);
	g = Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128);
	b = Y + 1.772   * (Cb - 128);

	if (r > 0)
		if (r < 255)
			R = BYTE(r);
		else
			R = 255;
	else
		R = 0;

	if ( g > 0)
		if ( g <255)
			G = BYTE(g);
		else
			G = 255;
	else
		G = 0;

	if (b > 0)
		if (b < 255)
			B = BYTE(b);
		else
			B = 255;
	else
		B = 0;
}

void getRGB(BYTE Y,BYTE Cb,BYTE Cr,BYTE &R,BYTE &G,BYTE &B)
{
	 R = BYTE( Y                        + 1.402   * (Cr - 128));
	 G = BYTE( Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
	 B = BYTE( Y + 1.772   * (Cb - 128));
}

BYTE* ConvertYCbCr(int R,int G,int B,BYTE *Clr){
	BYTE Y,Cb,Cr;
	 Y = BYTE(      0.299    *R + 0.587    *G + 0.114    *B);
	Cb = BYTE(128 - 0.168736 *R - 0.331264 *G + 0.5      *B);
	Cr = BYTE(128 + 0.5      *R - 0.418688 *G - 0.081312 *B);

	Clr[0]=Y; Clr[1]=Cb; Clr[2]=Y; Clr[3]=Cr;
	return Clr;
}

int diff(int a,int b)
{
	return (a>b)?(a-b):(b-a);
}