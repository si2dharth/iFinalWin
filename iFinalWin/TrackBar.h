#pragma once
#include "button.h"

#define TrackBarValueChanged ButtonEvent::BtnLast+1

class TrackBar :
	public Button
{
	int *connectedTo;

	void valueChanged() {
		value = max(value,MinValue);
		value = min(value,MaxValue);
		if (connectedTo) *connectedTo = value;
		handler.raise(TrackBarValueChanged,&value);
		Draw();
	}

	void Click(TrackBar*,MouseEventParams *MP) {
		if (!MP->lButton) return;
		if (!containsPoint(MP->x,MP->y)) return;
		value = (MP->x - Location.X())*(MaxValue-MinValue)/Width + MinValue;
		Draw();
	}
public:
	INTTYPE value,MinValue,MaxValue;

	virtual void Draw() {
		if (noDraw) return;
		if (!ParentWindow) return;
		drawHandled = false;
		handler.raise(ButtonEvent::BtnDrawing);
		if (!drawHandled) {
			SolidBrush B(backColor);
			ParentWindow->GetGraphics()->FillRectangle(&B,Location.X(),Location.Y(),(int)Width,(int)Height);

			B.SetColor(hover?hoverclr:clr);
			ParentWindow->GetGraphics()->FillRectangle(&B,Location.X(),Location.Y() + (int)Height*0.25, (int)Width, (int)Height*0.5);

			HatchBrush H(HatchStyle::HatchStyle60Percent,hover?hoverclr:clr,backColor);
			ParentWindow->GetGraphics()->FillRectangle(&H,Location.X() + (value - MinValue)* (Width-10)/(MaxValue - MinValue) , Location.Y() + 2, 10, (int)Height-4);
		}
		handler.raise(ButtonEvent::BtnDrawn);
	}

	virtual int checkKeyEvent(KeyEventParams*) {
		return -1;
	}

	void connectTo(int &i) {
		connectedTo = &i;
	}

	TrackBar() {
		value = 0;
		MinValue = 0;
		MaxValue = 100;
		connectedTo = 0;
		value.Changed.addHandler(this,&TrackBar::valueChanged);
		MinValue.Changed.addHandler(this,&TrackBar::valueChanged);
		MaxValue.Changed.addHandler(this, &TrackBar::valueChanged);
		handler(LeftDown)->addHandler(this,&TrackBar::Click);
		handler(MouseMove)->addHandler(this,&TrackBar::Click);
	}

	~TrackBar() {}
};

