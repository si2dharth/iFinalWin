#include "PhyObject.h"
#include <vector>
using namespace std;

class PhysicsEnvironment
{
	Coord3D PositiveLimits, NegativeLimits;
	vector<PhysicsObject*> ContainedObjects;
	int lastObject;
public:
	float gravity;
	PhysicsEnvironment()
	{
		lastObject = -1;
		gravity = 9.81;
	}

	void addObject(PhysicsObject *obj)
	{
		for (vector<PhysicsObject*>::iterator I = ContainedObjects.begin(); I != ContainedObjects.end(); I++) {
			if (obj == *I) return;
		}
		ContainedObjects.push_back(obj);
	}

	void removeObject(PhysicsObject *obj)
	{
		for (vector<PhysicsObject*>::iterator I = ContainedObjects.begin(); I != ContainedObjects.end(); I++) {
			if (obj == *I) ContainedObjects.erase(I);
		}
	}

	PhysicsObject *GetNextObject(int index = -1)
	{
		if (index == -1)
		{
			index = ++lastObject;
			if (index >= ContainedObjects.size()) {
				lastObject = -1;
				return 0;
			}
		}
		if (index >= ContainedObjects.size()) return 0;
		return ContainedObjects[index];
	}

	void SetLimits(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		PositiveLimits.set(x1, y1, z1);
		NegativeLimits.set(x2, y2, z2);
	}

	void update()
	{
		lastObject = -1;
		while (PhysicsObject *curObj = GetNextObject())
			curObj->update();
	}
};