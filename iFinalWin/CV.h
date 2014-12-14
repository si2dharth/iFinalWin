#pragma once
#include <DShow.h>
#include <list>
using namespace std;

#pragma comment(lib,"Strmiids.lib")

typedef void(*CameraProcessFunction)(BYTE*,DWORD);

void StartCamera();

list<CameraProcessFunction> &ImageGetH();
void CloseCamera();

void getOriginal(bool enable);
void SetImageShow(bool enable);
void getRGB(BYTE* Clr,BYTE &R,BYTE &G,BYTE &B);
void getRGB(BYTE Y,BYTE Cb,BYTE Cr,BYTE &R,BYTE &G,BYTE &B);
BYTE* ConvertYCbCr(int R,int G,int B,BYTE *Clr);
int diff(int a,int b);


#define DsHook(a,b,c)												\
	if (!c##_)														\
	{																\
		INT_PTR* p=b+*(INT_PTR**)a;									\
		VirtualProtect(&c##_,4,PAGE_EXECUTE_READWRITE,&no);			\
		*(INT_PTR*)&c##_=*p;										\
		VirtualProtect(p,4,PAGE_EXECUTE_READWRITE,&no);				\
		*p=(INT_PTR)c;												\
	}