#include "PropertyVar.h"

typedef PropertyVar < float > Float;

class Vector3D {
	float _x, _y, _z;
public:
	Vector3D();
	Vector3D(float x, float y, float z);

	void translate(Vector3D);
	void translate(float x, float y, float z);
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
};