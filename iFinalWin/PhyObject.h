#pragma once
#include <math.h>
#include "EventHandler.h"

enum PhysicsEvent
{
	Update,
};

struct Coord3D
{
	float x, y, z;
	Coord3D() { x = y = z = 0; }
	Coord3D(float X, float Y, float Z) { set(X, Y, Z); }
	void set(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	Coord3D operator+(float i) { return Coord3D(x + i, y + i, z + i); }
	Coord3D operator+(Coord3D &i) { return Coord3D(x + i.x, y + i.y, z + i.z); }
	Coord3D operator-() { return Coord3D(-x, -y, -z); }
	Coord3D operator-(Coord3D &i) { return Coord3D(x - i.x, y - i.y, z - i.z); }
	Coord3D operator*(float i) { return Coord3D(x * i, y * i, z * i); }
	Coord3D operator*(Coord3D &i) { return Coord3D(x * i.x, y * i.y, z * i.z); }
	Coord3D operator/(float i) { return Coord3D(x / i, y / i, z / i); }
	float val(int nDim) { return sqrt(x*x + (nDim > 1 ? y*y : 0) + (nDim > 2 ? z*z : 0)); }
};

class PhysicsObject
{
public:
	Coord3D Position, Velocity, Acceleration;
	Coord3D TerminalVelocity, TerminalAcceleration;
	Coord3D Force;
	Coord3D BoundingBoxDimension;
	Coord3D CenterOfMass;

	Coord3D NewPosition, NewVelocity, NewAcceleration;
	EventHandler<int, PhysicsObject> handler;

	int mass;
	float elasticity;
	bool FixedObject;
	PhysicsObject() : handler(this)
	{
		mass = elasticity = 1;
		FixedObject = false;
		BoundingBoxDimension.set(50, 50, 50);
		TerminalVelocity.set(50, 50, 50);
		TerminalAcceleration.set(10, 10, 10);
	}
	void SetPosition(float x, float y, float z)
	{
		Position.set(x, y, z);
	}

	void SetVelocity(float x, float y, float z)
	{
		x = abs(x) > TerminalVelocity.x ? TerminalVelocity.x * (x / abs(x)) : x;
		y = abs(y) > TerminalVelocity.y ? TerminalVelocity.y * (y / abs(y)) : y;
		z = abs(z) > TerminalVelocity.z ? TerminalVelocity.z * (z / abs(z)) : z;
		Velocity.set(x, y, z);
	}

	void SetAcceleration(float x, float y, float z)
	{
		x = abs(x) > TerminalAcceleration.x ? TerminalAcceleration.x * (x / abs(x)) : x;
		y = abs(y) > TerminalAcceleration.y ? TerminalAcceleration.y * (y / abs(y)) : y;
		z = abs(z) > TerminalAcceleration.z ? TerminalAcceleration.z * (z / abs(z)) : z;
		Acceleration.set(x, y, z);
	}

	void SetTerminalVelocity(float x, float y, float z)
	{
		TerminalVelocity.set(abs(x), abs(y), abs(z));
	}

	void SetTerminalAcceleration(float x, float y, float z)
	{
		TerminalAcceleration.set(abs(x), abs(y), abs(z));
	}

	void ApplyForce(float x, float y, float z)
	{
		Force.set(x, y, z);
	}

	void SetSize(float x, float y, float z)
	{
		BoundingBoxDimension.set(x, y, z);
	}

	void SetCenterOfMass(float x, float y, float z)
	{
		CenterOfMass.set(x, y, z);
	}

	bool CheckHit(PhysicsObject *obj, int NumberOfDimensions)
	{
		if (obj->Position.x > Position.x + BoundingBoxDimension.x) return false;
		if (obj->Position.x + obj->BoundingBoxDimension.x < Position.x) return false;
		if (NumberOfDimensions == 1) return true;
		if (obj->Position.y > Position.y + BoundingBoxDimension.y) return false;
		if (obj->Position.y + obj->BoundingBoxDimension.y > Position.y) return false;
		if (NumberOfDimensions == 2) return true;
		if (obj->Position.z > Position.z + BoundingBoxDimension.z) return false;
		if (obj->Position.z + obj->BoundingBoxDimension.z > Position.z) return false;
		return true;
	}

	Coord3D getHitFraction(PhysicsObject *obj, int NumberOfDimensions)
	{
		int x1 = Position.x + CenterOfMass.x,
			y1 = Position.y + CenterOfMass.y,
			z1 = Position.z + CenterOfMass.z,
			x2 = obj->Position.x + obj->CenterOfMass.x,
			y2 = obj->Position.y + obj->CenterOfMass.y,
			z2 = obj->Position.z + obj->CenterOfMass.z;
		Coord3D result;
		if (obj->FixedObject) {
			if (NumberOfDimensions == 1) result.x = 1;
			else if (NumberOfDimensions == 2)
			{
				int xOverlap = max(Position.x, obj->Position.x) - min(Position.x + BoundingBoxDimension.x, obj->Position.x + BoundingBoxDimension.x);
				int yOverlap = max(Position.y, obj->Position.y) - min(Position.y + BoundingBoxDimension.y, obj->Position.y + BoundingBoxDimension.y);
				if (xOverlap > yOverlap) result.y = 1; else result.x = 1;
			}
			else
			{
				int xOverlap = max(Position.x, obj->Position.x) - min(Position.x + BoundingBoxDimension.x, obj->Position.x + BoundingBoxDimension.x);
				int yOverlap = max(Position.y, obj->Position.y) - min(Position.y + BoundingBoxDimension.y, obj->Position.y + BoundingBoxDimension.y);
				int zOverlap = max(Position.z, obj->Position.z) - min(Position.z + BoundingBoxDimension.z, obj->Position.z + BoundingBoxDimension.z);
				if (xOverlap < yOverlap && xOverlap < zOverlap) result.x = 1;
				else if (yOverlap < zOverlap && yOverlap < xOverlap) result.y = 1;
				else result.z = 1;
			}
		}
		else
		{
			if (NumberOfDimensions == 1)
			{
				result.x = 1;
			}
			else if (NumberOfDimensions == 2)
			{
				float hyp = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
				hyp = sqrt(hyp);
				if (hyp == 0) return result;
				result.x = abs(x2 - x1) / hyp;
				result.y = abs(y2 - y1) / hyp;
			}
			else
			{
				float hyp = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
				hyp = sqrt(hyp);
				if (hyp == 0) return result;
				result.x = abs(x2 - x1) / hyp;
				result.y = abs(y2 - y1) / hyp;
				result.z = abs(z2 - z1) / hyp;
			}
		}
		return result;
	}

	void update()
	{
		handler(PhysicsEvent::Update, 0);
		if (FixedObject) return;
		SetVelocity(NewVelocity.x, NewVelocity.y, NewVelocity.z);
		SetPosition(NewPosition.x, NewPosition.y, NewPosition.z);
		SetAcceleration(NewAcceleration.x, NewAcceleration.y, NewAcceleration.z);
		NewVelocity.set(0,0,0);
		NewPosition.set(0, 0, 0);
		NewAcceleration.set(0, 0, 0);
	}
	~PhysicsObject() {}
};