#pragma once
#include "Object.h"
#include "iBase.h"

class ControlManager {
	//The list of all the objects associated with the control manager/window.
	list<Object*> objList;

	//The map to store the objects linked to each event.
	map<int,list<Object*>> handlerList;

	//The container event of the Control Manager
	Window* ParentWindow;

	//This method is called when a Window-Event is raised
	void eventRaised(Window* W,EventParams *E);

	ControlManager(Window* ParentWindow) {
		this->ParentWindow = ParentWindow;
		ManagerList[ParentWindow] = this;
	}

	//Register the object in the control manager.
	void Register(Object* o) {
		objList.push_back(o);
	}

	//Unregister an object from the control manager.
	void UnRegister(Object *o) {
		objList.remove(o);
	}
public:
	static map<Window*, ControlManager*> ManagerList;

	//Connect the handler of an object to the specified Window-Event
	void RegisterEvent(Object *o,int eventName) {
		if (handlerList.find(eventName) == handlerList.end())
			ParentWindow->handler(EventName(eventName))->addHandler(this,&ControlManager::eventRaised);
		handlerList[eventName].push_back(o);
	}

	//Gets the control manager connected to the window.
	static ControlManager* CManager(Window* W);

	//Register an object with a window
	static void Register(Window *W,Object *o) {
		CManager(W)->Register(o);
	}

	//Register an object with a Control Manager
	static void Register(ControlManager *C,Object *o) {
		C->Register(o);
	}

	//Unregister an object from a Control Manager
	static void UnRegister(ControlManager* C,Object* o) {
		C->UnRegister(o);
		if (C->objList.size()==0) delete C;
	}

	//Unregister an object from a Window
	static void UnRegister(Window *W,Object *o) {
		UnRegister(CManager(W),o);
	}

	void Redraw(int X1,int Y1,int X2,int Y2) {
		list<Object*> ::iterator t = objList.begin();
		for (unsigned int i = 0; i<objList.size(); i++) {
			(*t)->Redraw(X1,Y1,X2,Y2);
			t++;
		}
	}
};


