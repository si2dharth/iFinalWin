#include "Events.h"

EventParams e;
KeyEventParams ke;
MouseEventParams me;

EventParams* GetEvent(EventName en, WPARAM P1, LPARAM P2, int &eventType) {
	if ((en>=Create && en<=SyncPaint) || (en>=ImeStartComposition && en<=CtlColorStatic) || (en>=ParentNotify && en<=ImeRequest) || (en>=WTSSessionChange && en<=App)) {
		eventType = NormalEvent;
		e.en = en;
		e.Param1 = P1;
		e.Param2 = P2;
		e.EventType = eventType;
		return (&e);
	}
	else if ((en>=NCMouseMove && en<=NCXDblClick) || (en>=MouseMove && en<=XDblClick) || (en>=NCMouseHover && en<=MouseLeave)) {
		eventType = MouseEvent;
		me.en = en;
		me.Param1 = P1;
		me.Param2 = P2;
		me.getAllParams();
		me.EventType = eventType;
		return &me;
	}
	else if ((en>=KeyDown && en<=UniChar) || en==ImeKeyDown || en==ImeKeyUp) {
		eventType = KeyEvent;
		ke.en = en;
		ke.Param1 = P1;
		ke.Param2 = P2;
		ke.getAllParams();
		ke.EventType = eventType;
		return &ke;
	}
	return 0;
};