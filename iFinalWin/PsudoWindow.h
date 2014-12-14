#include "Button.h"
#include "PropertyVar.h"

class PseudoWindow : public Button
{
	void PropertiesChanged();
public:
	PropertyVar<int> TitleBarHeight;
	
	PseudoWindow() {
		TitleBarHeight.handler(VariableEvent::Changed)->addHandler(this,&PseudoWindow::PropertiesChanged);
	}
};