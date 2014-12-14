#include "Object.h"

class Label : public Object {
	wchar_t textToDisplay[100];
public:
	Label(wchar_t *text = L"") : Object() {
		wcsncpy(textToDisplay, text, wcsnlen(text,100));
	}

	virtual void Draw() {
		
	}
};