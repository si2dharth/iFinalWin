#pragma once
#include "EventHandler.h"
#include "Events.h"

enum ObjectEvents {
	ObjMouseEnter = EventName::LastEvent + 1,
	ObjMouseLeave,
	ObjMouseMove,
	ObjLeftDown = EventName::LeftDown,
	ObjLeftUp = EventName::LeftUp,
	ObjRightDown = EventName::RightDown,
	ObjRightUp = EventName::RightUp,
	ObjMiddleDown = EventName::MiddleDown,
	ObjMiddleUp = EventName::MiddleUp,
	ObjLeftDoubleClick = EventName::LeftDblClick,
	ObjRightDoubleClick = EventName::RightDblClick,
	ObjMiddleDouleClick = EventName::MiddleDblClick,
	ObjKeyDown = ObjMouseMove + 1,
	ObjKeyUp,
	ObjMoving,
	ObjMoved,
	ObjResizing,
	ObjResized,
	ObjDrawing,
	ObjDrawn,
	ObjAddingToWindow,
	ObjAddedToWindow,
	ObjRemoving,
	ObjRemoved,
	ObjDeleting,
	ObjInActive,
	ObjActive,
	ObjLast = ObjActive
};

struct PointType {
	int X,Y;

	PointType() {
		X = 0;
		Y = 0;
	}

	PointType(int x, int y) {
		X = x;
		Y = y;
	}

	PointType operator +(PointType &P) {
		return PointType(this->X + P.X,this->Y + P.Y);
	}

	PointType operator -(PointType &P) {
		return PointType(this->X - P.X,this->Y - P.Y);
	}

	PointType operator *(float n) {
		return PointType(this->X * n, this->Y * n);
	}

	PointType operator /(float n) {
		return PointType(this->X / n, this->Y / n);
	}
};

struct INTTYPE {
		INTTYPE() : Changed(this),Accessed(this),Changing(this) {i = 0;}

		INTTYPE(int I) : Changed(this),Accessed(this),Changing(this) { I = 0; }

		int operator =(int WD) {
			if (i==WD) return i;
			Changing.raise();
			i = WD;
			Changed.raise();
			return i;
		};

		operator int() {
			Accessed.raise();
			return i;
		}

		Event<INTTYPE> Changing;
		Event<INTTYPE> Changed;
		Event<INTTYPE> Accessed;
	private:
		int i;
};

//Events, by default, are int type. So, any enum can use them.

//Object : anything having a size and a location. Also supports events. Can be registered in a window.
class Object {
public:
	EventHandler<int,Object> handler;
	struct LOCTYPE: private PointType{

		LOCTYPE(): Changed(this),Accessed(this),Changing(this),PointType() { }

		PointType operator = (PointType &P) {
			if (PointType::X==P.X && PointType::Y==P.Y) return (*this);
			Changing.raise();
			PointType::X = P.X;
			PointType::Y = P.Y;
			Changed.raise();
			return (*this);
		}

		operator PointType() {
			Accessed.raise();
			return (*this);
		}

		int Y() {
			return PointType::Y;
		}

		int X() {
			return PointType::X;
		}

		Event<PointType> Changing;
		Event<PointType> Changed;
		Event<PointType> Accessed;
	} Location;
	INTTYPE Width;
	INTTYPE Height;
	int id;

	void *Active;
	bool ActivePossible;

	Object() : handler(this) {
		id = 0;
		Active = 0;
		ActivePossible = false;
	}

	virtual void Draw() {
	}

	virtual bool intersectsRegion(int X1,int Y1,int X2,int Y2) {
		if (Location.Y() > Y2) return false;
		if (Location.Y() + Height < Y1) return false;
		if (Location.X() > X2) return false;
		if (Location.X() + Width < X1) return false;
		return true;
	}

	virtual bool containsPoint(int X,int Y) {
		if (X >= Location.X() && X<=Location.X() + Width)
			if (Y >= Location.Y() && Y <= Location.Y() + Height) 
				return true;
		return false;
	}

	virtual void Redraw(int X1,int Y1,int X2,int Y2) {
		if (intersectsRegion(X1,Y1,X2,Y2)) Draw();
	}

	//Check if an event applies to the button, given the parameters E
	bool eventApplicable(EventParams *EP) {
		int customRes;
		switch (EP->EventType) {
		case NormalEvent :
			customRes = checkNormalEvent(EP);
			return customRes == 0 ? true : (customRes > 0 ? true : false);
			break;

		case KeyEvent:
			customRes = checkKeyEvent((KeyEventParams*)EP);
			return customRes == 0 ? true : (customRes > 0 ? true : false);
			break;

		case MouseEvent:
			customRes = checkMouseEvent((MouseEventParams*)EP);
			if (customRes != 0) return (customRes > 0);
			MouseEventParams *MP = (MouseEventParams*)EP;
			return containsPoint(MP->x,MP->y);
			break;
		}
		return false;
	}

	//Inheritable method for eventApplicable for a normal event. Return 0 for default, positive for true, negative for false.
	virtual int checkNormalEvent(EventParams *EP) {
		return 0;
	}

	//Inheritable method for eventApplicable for a key event. Return 0 for default, positive for true, negative for false.
	virtual int checkKeyEvent(KeyEventParams *KP) {
		return 0;
	}

	//Inheritable method for eventApplicable for a mouse event. Return 0 for default, positive for true, negative for false.
	virtual int checkMouseEvent(MouseEventParams *MP) {
		return 0;
	}
};