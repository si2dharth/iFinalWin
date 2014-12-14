#include "Camera.h"

CameraFeed &BlackWhite(CameraFeed &F);
CameraFeed &Differenciate(CameraFeed &F,int C, int tolerance = 10,bool deleteOther = false);
CameraFeed &FlipX(CameraFeed &F);
CameraFeed &FlipY(CameraFeed &F);
CameraFeed &Redify(CameraFeed &F);
CameraFeed &Blueify(CameraFeed &F);
CameraFeed &Greenify(CameraFeed &F);