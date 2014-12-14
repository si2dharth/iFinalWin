#include "Camera.h"

class ImageProcessor {

	Camera *ConnectedCamera;
	CameraFeed *LastFeed;
	bool _modified;
	bool _workOnData;

	BYTE Data[CAM_WIDTH][CAM_HEIGHT][3] , Data2[CAM_WIDTH][CAM_HEIGHT][3];

	BYTE *newData;
	
	void getImageInfo(Camera *C, CameraFeed *CF) {
		if (newData) delete[] newData;
		newData = 0;
		LastFeed = &C->getCurrentData();
		_modified = false;
		_workOnData = true;
		BYTE *RawData = CF->getRawData(true);

		int x,y;
		for (int i = 0; i<CF->getDataLength(); i+=4) {
			x = (i>>1) % CAM_WIDTH;
			y = (i>>1) / CAM_WIDTH;

			Data[x][y][0] = RawData[i];
			Data[x][y][1] = RawData[i + 1];
			Data[x][y][2] = RawData[i + 3];
			Data[x+1][y][0] = RawData[i + 2];
			Data[x+1][y][1] = RawData[i + 1];
			Data[x+1][y][2] = RawData[i + 3];
		}

		handler.raise(ImageProcessor::Events::ImageAcquired);
		handler.raise(ImageProcessor::Events::ImageProcessed);
		delete[] RawData;
	}

	void formNewData() {

    	if (newData) delete[] newData;
		newData = 0;
		newData = new BYTE[LastFeed->getDataLength()];
		int x,y;
		
		for (int i = 0; i<LastFeed->getDataLength(); i+=4) {
			x = (i>>1) % CAM_WIDTH;
			y = (i>>1) / CAM_WIDTH;

			newData[i] = _workOnData?  Data[x][y][0] : Data2[x][y][0];
			newData[i+1] = _workOnData ? (Data[x][y][1] + Data[x+1][y][1]) >> 1 : (Data2[x][y][1] + Data2[x+1][y][1]) >> 1;
			newData[i+3] = _workOnData ? (Data[x][y][2] + Data[x+1][y][2]) >> 1 : (Data2[x][y][2] + Data2[x+1][y][2]) >> 1;
			newData[i+2] = _workOnData ? Data[x+1][y][0] : Data2[x+1][y][0];
		}
	}

	void setData(int i,int j,int k,BYTE DataValue) {
		if (_workOnData) 
			Data2[i][j][k] = DataValue; 
		else
			Data[i][j][k] = DataValue;
	}

	BYTE getData(int i, int j, int k) {
		return _workOnData?Data[i][j][k]:Data2[i][j][k];
	}

public:
	EventHandler<int,ImageProcessor> handler;

	ImageProcessor(Camera *C) : handler(this) {
		ConnectedCamera = C;
		newData = 0;
		C->eHandler(CameraEvent::FeedRecieved)->addHandler(this,&ImageProcessor::getImageInfo);
		C->SendRealData(true);
	}

	void BlackAndWhite() {
		_modified = true;


		for (int i = 0; i < CAM_WIDTH; i++)
			for (int j = 0; j < CAM_HEIGHT; j++) {
				if (_workOnData) {
					Data2[i][j][0] = Data[i][j][0];
					Data2[i][j][1] = Data2[i][j][2] = 128;
				} else {
					Data[i][j][0] = Data2[i][j][0];
					Data[i][j][1] = Data[i][j][2] = 128;
				}
			}
		_workOnData = !_workOnData;
	}

	void Decolorize(int factor) {
		_modified = true;
		BYTE Table[256];
		for (int i = 0; i<256; i++)
			Table[i] = (i-128)/factor + 128;

		for (int i = 0; i < CAM_WIDTH; i++)
			for(int j = 0; j < CAM_HEIGHT; j++)
				if (_workOnData) {
					Data2[i][j][0] = Data[i][j][0];
					Data2[i][j][1] = Table[Data[i][j][1]];
					Data2[i][j][2] = Table[Data[i][j][2]];
				} else {
					Data[i][j][0] = Data2[i][j][0];
					Data[i][j][1] = Table[Data2[i][j][1]];
					Data[i][j][2] = Table[Data2[i][j][2]];
				}
		_workOnData = !_workOnData;
	}

	void Factorize(int factor) {
		_modified = true;
		BYTE Table[256],Table2[256];
		for (int i = 0; i<256; i++) {
			Table[i] = (i < 128 ? 256 - ((128-i)/(factor)) * (factor) : ((i-128)/factor)*factor) + 128;
			Table2[i] = (i/factor)*factor;
		}

		for (int i = 0; i < CAM_WIDTH; i++)
			for(int j = 0; j < CAM_HEIGHT; j++)
				if (_workOnData) {
					Data2[i][j][0] = Table2[Data[i][j][0]];
					Data2[i][j][1] = Table[Data[i][j][1]];
					Data2[i][j][2] = Table[Data[i][j][2]];
				} else {
					Data[i][j][0] = Table2[Data2[i][j][0]];
					Data[i][j][1] = Table[Data2[i][j][1]];
					Data[i][j][2] = Table[Data2[i][j][2]];
				}
		_workOnData = !_workOnData;
	}

	void Redify() {
		_modified = true;

		_workOnData = !_workOnData;
	}

	void Greenify() {
		_modified = true;

		_workOnData = !_workOnData;
	}

	void Blueify() {
		_modified = true;
		
		_workOnData = !_workOnData;
	}

	void OnlyRed(int val = 128) {
		_modified = true;
		int R,G,B,Y,U,V;
		for (int i = 0; i < CAM_WIDTH; i++)
			for (int j = 0; j < CAM_HEIGHT; j++)
			{
				if (_workOnData)
					LastFeed->getRGB(Data[i][j][0],Data[i][j][1],Data[i][j][2],R,G,B);
				else
					LastFeed->getRGB(Data2[i][j][0],Data2[i][j][1],Data2[i][j][2],R,G,B);

				if (R >= val + max(G,B)) R = G = B = 255; else R = G = B = 0;

				LastFeed->getYUV(R,G,B,Y,U,V);
				if (_workOnData) {
					Data2[i][j][0] = Y;
					Data2[i][j][1] = U;
					Data2[i][j][2] = V;
				} else  {
					Data[i][j][0] = Y;
					Data[i][j][1] = U;
					Data[i][j][2] = V;
				}
			}
		_workOnData = !_workOnData;
	}

	void IsolateWhite(int value = 128) {
		_modified = true;
		if (_workOnData)
				memcpy(Data2,Data,CAM_WIDTH*CAM_HEIGHT*3*sizeof(BYTE));
			else
				memcpy(Data,Data2,CAM_WIDTH*CAM_HEIGHT*3*sizeof(BYTE));

		for (int i = 0; i < CAM_WIDTH; i++)
			for (int j = 0; j < CAM_HEIGHT; j++)
				if (value > (_workOnData? Data[i][j][0] : Data2[i][j][0]))
					if (_workOnData) Data2[i][j][0] = 0; else Data[i][j][0] = 0;
		_workOnData = !_workOnData;
	}

	void AdjustBrightness(int changeAmount) {
		_modified = true;

		_workOnData = !_workOnData;
	}

	void Extract(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
		_modified = true;

		_workOnData = !_workOnData;
	}

	void FlipX() {
		_modified = true;
		for (int i = 0; i < CAM_WIDTH; i++)
			for (int j = 0; j < CAM_HEIGHT; j++) {
				if (_workOnData) 
					memcpy(Data2[i][j],Data[CAM_WIDTH-1-i][j],3*sizeof(BYTE));
				else
					memcpy(Data[i][j],Data2[CAM_WIDTH-1-i][j],3*sizeof(BYTE));
			}
		_workOnData = !_workOnData;
	}

	void FlipY() {
		_modified = true;
		for (int i = 0; i < CAM_WIDTH; i++)
			for (int j = 0; j < CAM_HEIGHT; j++) {
				if (_workOnData) 
					memcpy(Data2[i][j],Data[i][CAM_HEIGHT-1-j],3*sizeof(BYTE));
				else
					memcpy(Data[i][j],Data2[i][CAM_HEIGHT-1-j],3*sizeof(BYTE));
			}
		_workOnData = !_workOnData;
	}

	Rect FindWhiteRegion(bool markOnImage = false,int threshold = 128,Color C = Color(Color::LimeGreen)) {
		Rect region;
		region.X = CAM_WIDTH;
		region.Y = CAM_HEIGHT;
		region.Width = -CAM_WIDTH;
		region.Height = -CAM_HEIGHT;

		for (int i = 0; i < CAM_WIDTH; i++)
			for (int j = 0; j < CAM_HEIGHT; j++)
				if (threshold < (_workOnData ? Data[i][j][0] : Data2[i][j][0])) {
					if (region.X > i) region.X = i;
					if (region.Width < i - region.X) region.Width = i - region.X;
					if (region.Y > j) region.Y = j;
					if (region.Height < j - region.Y) region.Height = j - region.Y;
				}

		if (markOnImage) {
			_modified = true;
			int Y,U,V;
			CameraFeed::getYUV(C,Y,U,V);
			BYTE Clr[] = {Y,U,V};
			if (_workOnData)
				memcpy(Data2,Data,CAM_WIDTH*CAM_HEIGHT*3*sizeof(BYTE));
			else
				memcpy(Data,Data2,CAM_WIDTH*CAM_HEIGHT*3*sizeof(BYTE));
			for (int i = region.X; i < region.X + region.Width; i++)
				if (_workOnData)
				{
					memcpy(Data2[i][region.Y],Clr,3*sizeof(BYTE));
					memcpy(Data2[i][region.Y + region.Height],Clr,3*sizeof(BYTE));
				} else	{
					memcpy(Data[i][region.Y],Clr,3*sizeof(BYTE));
					memcpy(Data[i][region.Y + region.Height],Clr,3*sizeof(BYTE));
				}

			for (int j = region.Y; j < region.Y + region.Height; j++)
				if (_workOnData)
				{
					memcpy(Data2[region.X][j],Clr,3*sizeof(BYTE));
					memcpy(Data2[region.X + region.Width][j],Clr,3*sizeof(BYTE));
				} else	{
					memcpy(Data[region.X][j],Clr,3*sizeof(BYTE));
					memcpy(Data[region.X + region.Width][j],Clr,3*sizeof(BYTE));
				}
			_workOnData = !_workOnData;
		}
		return region;
	}

	void Rotate(int angle) {
		_modified = true;

		_workOnData = !_workOnData;
	}

	void ReplaceOriginal() {
		if (!_modified) return;
		formNewData();
		
		LastFeed->setData(newData);

		//for (int i = 0; i < 100; i++)
		//LastFeed->setColor(i,i,*(new Color(Color::Red)));
	}

	enum Events {
		Loaded,
		ImageAcquired,
		ImageProcessed,
		ImageDeleted
	};
};