#include "iBase.h"
#include "PhyEnvironment.h"

class Physics
{
	PhysicsEnvironment *env;
	long TimerID;
	const int nDim;
	Window *AttachedWindow;
public:
	Physics(PhysicsEnvironment *ControlEnvironment, int NumberOfDimensions = 3);
	void AttachToWindow(Window *W, int TimeStep);
	void DettachFromWindow();

	void Work(Window *W, EventParams *E);
};