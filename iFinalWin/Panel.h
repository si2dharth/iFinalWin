#include "Button.h"

//This is a container class for button.
class Panel : public Button{
	list<Object*> ObjList;
public:
	Panel() {
		hoverEnable = false;
		//hover.Changed.removeHandler(&Button::HoverChanged);
		handler.event(MouseMove)->addHandler(this,&Panel::MouseMoved);
	}

	void MouseMoved(Panel*,MouseEventParams *me) {
		list<Object*>::iterator i = ObjList.begin();
		for (int I=0; I<ObjList.size(); I++) {
			(*i)->handler.raise(MouseMove,me);
		}
	}

	void addObject(Object *o) {
		ObjList.push_back(o);
	}

	void removeObject(Object *o) {
		ObjList.remove(o);
	}
};