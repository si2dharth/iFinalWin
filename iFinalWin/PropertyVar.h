#include "EventHandler.h"

enum VariableEvent
{
	Assessed = 1,
	Changing = 2,
	Changed = 3
};

template <class t>
class PropertyVar
{
	t val;
public:
	EventHandler<VariableEvent, t> handler;
	PropertyVar() : handler(this){}

	t operator = (t newValue)
	{
		handler.raise(VariableEvent::Changing);
		val = newValue;
		handler.raise(VariableEvent::Changed);
		return val;
	}

	t get()
	{
		handler.raise(VariableEvent::Assessed);
		return val;
	}
};