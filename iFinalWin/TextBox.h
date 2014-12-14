#include "Button.h"

enum TextBoxEvent {
	TextChanged = ButtonEvent::BtnLast + 1,
	TextLast = TextChanged,
};

class TextBox : public Button
{
protected:
	void AddTextBox(TextBox *B, Window *W) {
		W->handler(EventName::KeyDown)->addHandler(this, &TextBox::KeyPress);
	}
public:
	TextBox() : Button() {
		ActivePossible = true;
		handler(ButtonEvent::BtnAddingToWindow)->addHandler(this, &TextBox::AddTextBox);
	}

	void KeyPress(Window *W, KeyEventParams *KE) {
		if (Active != this) return;
	}	
};