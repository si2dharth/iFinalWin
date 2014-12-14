#include "ImgProcessing.h"

Color &BlackWhite(Color &C) {
	return C;
}

CameraFeed &BlackWhite(CameraFeed &F) {
	BYTE *CFData = F.getRawData();
	for (int i = 0; i < F.getDataLength(); i+=4) {
		CFData [i + 1] = 128;
		CFData [i + 3] = 128;
	}
	F.setData(CFData,false);
	return F;
}

CameraFeed &Differenciate(CameraFeed &F,int C, int tolerance, bool deleteOther) {
	BYTE *CFData = F.getRawData();
	for (int i = 0; i < F.getDataLength(); i+=4)
		if (CFData[i] > C - tolerance && CFData[i] < C + tolerance) CFData[i+1] = 0; else if (deleteOther) CFData[i] = CFData[i+2] = 0;
	F.setData(CFData,false);
	return F;
}

CameraFeed &FlipX(CameraFeed &F) {
	BYTE *CFData = F.getRawData(true);
	BYTE tmp[4];
	int wid = F.Width();

	for (DWORD i = 0; i < F.getDataLength(); i += 4)
	{

		int j,x,y;
		F.getLocation(i,x,y);
		j = F.getPos(wid-x,y);
		memcpy(tmp,&CFData[i],sizeof(BYTE) << 2);
		memcpy(&CFData[i],&CFData[j],sizeof(BYTE) << 2);
		memcpy(&CFData[j],tmp,sizeof(BYTE) << 2);
	}
	//F.destroy(true);
	F.setData(CFData);
	//delete [] CFData;
	return F;
}

CameraFeed &FlipY(CameraFeed &F);
CameraFeed &Redify(CameraFeed &F);
CameraFeed &Blueify(CameraFeed &F);
CameraFeed &Greenify(CameraFeed &F);