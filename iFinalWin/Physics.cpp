#include "Physics.h"

Coord3D operator-(int i, Coord3D &o) { return Coord3D(i - o.x, i - o.y, i - o.z); }
Coord3D operator+(int i, Coord3D &o) { return Coord3D(i + o.x, i + o.y, i + o.z); }


Physics::Physics(PhysicsEnvironment *ControlEnvironment, int NumOfDimensions) :nDim(NumOfDimensions)
{
	env = ControlEnvironment;
	TimerID = 0;
	AttachedWindow = 0;
}

void Physics::AttachToWindow(Window *W, int TimeStep)
{
	TimerID = W->CreateTimer(TimeStep);
	W->handler(Timer)->addHandler(this, &Physics::Work);
	AttachedWindow = W;
}

void Physics::DettachFromWindow()
{
	AttachedWindow->DeleteTimer(TimerID);
	AttachedWindow = 0;
}

void Physics::Work(Window *W, EventParams *E)
{
	if (E->Param1 != TimerID) return;
	
	PhysicsObject *curObj;
	while (curObj = env->GetNextObject())
	{
		
		if (curObj->FixedObject) continue;
		Coord3D Force(0, nDim == 2 ? curObj->mass * env->gravity : 0, nDim == 3 ? curObj->mass * env->gravity : 0);
		int nHits = 0;

		int i = 0;
		for (PhysicsObject *chkObj = env->GetNextObject(0); chkObj != 0; chkObj=env->GetNextObject(++i))
		{
			if (curObj == chkObj) continue;
			if (curObj->CheckHit(chkObj, nDim))
			{
				nHits++;

				Coord3D Fraction = curObj->getHitFraction(chkObj, nDim);
				if (!chkObj->FixedObject) curObj->NewVelocity = (chkObj->Velocity / chkObj->mass) * curObj->mass;
				else curObj->NewVelocity = curObj->NewVelocity - Fraction * curObj->Velocity * 2;
			}
		}
		if (nHits == 0) curObj->NewVelocity = curObj->Velocity;
		curObj->NewPosition = curObj->Position + curObj->Velocity;
	}
	env->update();
}