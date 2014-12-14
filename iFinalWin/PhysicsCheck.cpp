#include "Physics.h"

PhysicsObject ob1,ob2,wall1,wall2,wall3,wall4;
PhysicsEnvironment env;
Physics ph(&env,1);

Window *MainWindow;

void DrawOb(PhysicsObject *obj)
{
	if (obj == &ob1) MainWindow->Clear();
	MainWindow->SetColor(255,50,255);
	MainWindow->DrawRectangle(obj->Position.x+5, obj->Position.y+5, obj->Position.x + obj->BoundingBoxDimension.x-5, obj->Position.y + obj->BoundingBoxDimension.y-5);
}

void Start(Window *W)
{
	ob1.Position.set(300, 400, 0);
	ob1.handler(PhysicsEvent::Update)->addHandler(DrawOb);
	ob1.Velocity.set(5, 0, 0);
	ob2.Position.set(500, 400, 0);
	ob2.handler(PhysicsEvent::Update)->addHandler(DrawOb);
	wall1.Position.set(0, 0, 0);
	wall1.FixedObject = true;
	wall1.BoundingBoxDimension.set(50, 900, 0);
	wall1.handler(PhysicsEvent::Update)->addHandler(DrawOb);
	wall2.Position.set(750, 0, 0);
	wall2.FixedObject = true;
	wall2.BoundingBoxDimension.set(50, 900, 0);
	wall2.handler(PhysicsEvent::Update)->addHandler(DrawOb);

	env.addObject(&ob1);
	env.addObject(&ob2);
	env.addObject(&wall1);
	env.addObject(&wall2);
	ph.AttachToWindow(W, 200);
}

int PhyCheck(char args[])
{
	Window W(Start, 0, "Physics", 800, 600);
	MainWindow = &W;
	W.handler(Close)->addHandler(&ph, &Physics::DettachFromWindow);
	W.handler(Close)->addHandler(&W, &Window::Close);
	W.Run();
	return 0;
}