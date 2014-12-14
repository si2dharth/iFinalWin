#include "DirectXBase.h"
#include <list>
using namespace std;

class DirectXObject {
	DirectX *Screen;
	list<Point> PointList;
public:
	DirectXObject(DirectX *screen) {
		Screen = screen;
	}
};