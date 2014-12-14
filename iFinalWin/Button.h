#pragma once
#include "iBase.h"
#include "Object.h"
#include "ControlManager.h"

//Events of buttons. Events for Object inherited from button should start with BtnLast + 1
//Used for compatibility. Use ObjectEvents for more events
enum ButtonEvent {
	BtnMouseEnter = EventName::LastEvent + 1,
	BtnMouseLeave,
	BtnMouseMove,
	BtnLeftDown = EventName::LeftDown,
	BtnLeftUp = EventName::LeftUp,
	BtnRightDown = EventName::RightDown,
	BtnRightUp = EventName::RightUp,
	BtnMiddleDown = EventName::MiddleDown,
	BtnMiddleUp = EventName::MiddleUp,
	BtnLeftDoubleClick = EventName::LeftDblClick,
	BtnRightDoubleClick = EventName::RightDblClick,
	BtnMiddleDouleClick = EventName::MiddleDblClick,
	BtnMoving = ObjectEvents::ObjMoving,
	BtnMoved,
	BtnResizing,
	BtnResized,
	BtnDrawing,
	BtnDrawn,
	BtnAddingToWindow,
	BtnAddedToWindow,
	BtnRemoving,
	BtnRemoved,
	BtnDeleting,
	BtnLast = ObjectEvents::ObjLast
};

//Has Hover-feature and supports colors. Also, see enum "ButtonEvent"
class Button : public Object {
protected:
	Window* ParentWindow;
	ControlManager* ParentManager;
	Color backColor;
	Color clr;
	Color hoverclr;
	INTTYPE hover;
	bool hoverEnable;
	bool noDraw;
	bool drawHandled;
	wchar_t text[100];

	MouseEventParams *mp;

	//Automatically called when button is going to move.
	void LocationChanging(PointType *P) {
		Erase();
		handler.raise(BtnMoving, P);
	}

	//Automatically called when button is moved.
	void LocationChanged(PointType*P) {
		Draw();

		handler.raise(BtnMoved, P);
		if (mp) HoverCheck(this, mp);
	}

	//Automatically called when button is going to be resized.
	void SizeChanging(INTTYPE *I) {
		Erase();
		handler.raise(BtnResizing, I);
	};

	//Automatically called when button is resized.
	void SizeChanged(INTTYPE *I) {
		Draw();
		handler.raise(BtnResized, I);
		if (mp) HoverCheck(this, mp);
	}

	//Called when the mouse is moved in the parent window
	void HoverCheck(Button*, MouseEventParams *me) {
		hover = (containsPoint(me->x, me->y));
		mp = me;
		if (hover) handler.raise(ButtonEvent::BtnMouseMove, me);
	}

	//Automatically called when the hover status changed
	void HoverChanged() {
		if (hover)
			handler.raise(ButtonEvent::BtnMouseEnter, mp);
		else
			handler.raise(ButtonEvent::BtnMouseLeave, mp);
		Draw();
	}

public:
	//Button Constructor. Takes no parameters.
	Button() : backColor(Color(255, 255, 255)), clr(Color(0, 0, 125)), hoverclr(190, 0, 0) , Object() {
		ParentWindow = 0;
		ParentManager = 0;
		mp = 0;
		hoverEnable = true;
		Location.Changing.addHandler(this, &Button::LocationChanging);
		Location.Changed.addHandler(this, &Button::LocationChanged);
		Width.Changing.addHandler(this, &Button::SizeChanging);
		Width.Changed.addHandler(this, &Button::SizeChanged);
		Height.Changing.addHandler(this, &Button::SizeChanging);
		Height.Changed.addHandler(this, &Button::SizeChanged);
		hover.Changed.addHandler(this, &Button::HoverChanged);
		handler.event(EventName::MouseMove)->addHandler(this, &Button::HoverCheck);
		noDraw = false;
	};

	//Erase the button from the screen. Does not hide the button.
	void Erase() {
		if (!ParentWindow) return;
		noDraw = true;
		SolidBrush B(ParentWindow->GetBackColor());
		ParentWindow->GetGraphics()->FillRectangle(&B, Location.X(), Location.Y(), (int)Width, (int)Height);
		ParentManager->Redraw(Location.X(), Location.Y(), Location.X() + Width, Location.Y() + Height);
		noDraw = false;
	}

	//Draw the button on its location, according to its size.
	void Draw() {
		if (noDraw) return;
		if (!ParentWindow) return;
		drawHandled = false;
		handler.raise(ButtonEvent::BtnDrawing);
		if (!drawHandled) {
			SolidBrush B(hover ? hoverclr : clr);
			ParentWindow->GetGraphics()->FillRectangle(&B, Location.X(), Location.Y(), (int)Width, (int)Height);
			ParentWindow->Write(Location.X() + 3, Location.Y() + Height / 2 - 5, text);
		}
		handler.raise(ButtonEvent::BtnDrawn);
	}

	//Add the button to a window.
	void AddToWindow(Window *W, bool Register = true) {
		handler.raise(ButtonEvent::BtnAddingToWindow, W);
		ParentWindow = W;
		ParentManager = ControlManager::CManager(W);
		ControlManager::Register(ParentManager, this);
		if (Register) {
			ParentManager->RegisterEvent(this, MouseMove);
			ParentManager->RegisterEvent(this, LeftDown);
			ParentManager->RegisterEvent(this, LeftUp);
			ParentManager->RegisterEvent(this, RightDown);
			ParentManager->RegisterEvent(this, RightUp);
			ParentManager->RegisterEvent(this, MiddleDown);
			ParentManager->RegisterEvent(this, MiddleUp);
			ParentManager->RegisterEvent(this, LeftDblClick);
			ParentManager->RegisterEvent(this, RightDblClick);
			ParentManager->RegisterEvent(this, MiddleDblClick);
		}
		handler.raise(ButtonEvent::BtnAddedToWindow, W);
	}

	//Additional check procedure
	virtual int checkMouseEvent(MouseEventParams *MP) {
		if (MP->en == EventName::MouseMove) return 1; else return 0;
	}

	//Set Text
	void SetText(char Text[])
	{ 
		Erase();
		mbstowcs(text, Text, 100);
	}

	void Remove() {
		handler.raise(BtnRemoving);
		Erase();
		if (ParentManager) ControlManager::UnRegister(ParentManager,this);
		handler.raise(BtnRemoved);
	}

	~Button() {
		Remove();
		handler.raise(BtnDeleting);
	}
};