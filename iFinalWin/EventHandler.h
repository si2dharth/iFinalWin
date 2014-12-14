#pragma once
#include <list>
#include <map>
using namespace std;

#define FT(x) void(SuperClass::*##x##)(SuperClass*,ParamType*)
#define FuncType void(SuperClass::*)(SuperClass*,ParamType*)
#define FuncNoParam void (SuperClass::*)(SuperClass*)
#define FNP(x) void (SuperClass::*##x##)(SuperClass*)
#define SimpleFunc void(*)(SuperClass*,ParamType*)
#define SF(x)  void(*##x##)(SuperClass*,ParamType*)
#define SimpleFuncNoParam void (*)(SuperClass*)
#define SFNP(x) void (*##x##)(SuperClass*)

class Function {
public:
	unsigned short int Type;
	
	virtual void operator () (void) = 0;
	virtual void operator () (void*)= 0;
	virtual void operator () (void*,void*) = 0;
};

template <class Any,class SuperClass,class ParamType = void>
class FUNCTION : public Function{
	void Call(void* obj = 0,void* param = 0) {
		switch(Type) {
		case 0:
			simplef0();
			break;
		case 1:
			simplef1((SuperClass*)obj);
			break;
		case 2:
			simplef2((SuperClass*)obj,(ParamType*)param);
			break;
		case 3:
			(lObj->*f0)();
			break;
		case 4:
			(lObj->*f1)((SuperClass*)obj);
			break;
		case 5:
			(lObj->*f2)((SuperClass*)obj,(ParamType*)param);
			break;
		}
	}

public:
	Any* lObj;
	
	void (*simplef0)();
	void (*simplef1)(SuperClass*);
	void (*simplef2)(SuperClass*,ParamType*);
	void (Any::*f0)();
	void (Any::*f1)(SuperClass*);
	void (Any::*f2)(SuperClass*,ParamType*);

	FUNCTION (void (*f)()) {
		Type = 0;
		simplef0 = f;
	}

	FUNCTION(void (*f)(SuperClass*)) {
		Type = 1;
		simplef1 = f;
	}

	FUNCTION(void (*f)(SuperClass*,ParamType*)) {
		Type = 2;
		simplef2 = f;
	}
	
	FUNCTION(Any* obj,void (Any::*f)()) {
		Type = 3;
		lObj = obj;
		f0 = f;
	}

	FUNCTION (Any* obj, void (Any::*f)(SuperClass*)) {
		Type = 4;
		lObj = obj;
		f1 = f;
	}

	FUNCTION (Any* obj,void (Any::*f)(SuperClass*,ParamType*)) {
		Type = 5;
		lObj = obj;
		f2 = f;
	}

	void operator()() {
		Call();
	}

	void operator() (void* obj) {
		Call(obj);
	};

	void operator() (void* obj,void* param) {
		Call(obj,param);
	}
};

template <class SuperClass,class ParamType=void>
class Event {
	SuperClass *linkedObject;
	list<list<Function*>::iterator> removeList;
public:
	//Raise an event with address of someData as parameters.
	void raise(ParamType *someData=0) {
		
		if (FuncList.size()==0) return;
		list<Function*>::iterator i = FuncList.begin();
		for (unsigned int I=0; I<FuncList.size(); I++) {
			(*(*i))(linkedObject,someData);
			i++;
		}

		while (removeList.size()>0) {
			FuncList.erase(*removeList.begin());
			removeList.pop_front();
		}
	}
	
	//A list of the handling functions..
	list <Function*> FuncList;

	//Event is constructed and linked with the object specified. This object is passed as argument to all the handling functions.
	Event (SuperClass *linkedObject) {
		this->linkedObject = linkedObject;
	}

	//Function list is emptied.
	~Event () {
		while (FuncList.size()>0) {
			delete (*FuncList.begin());
			FuncList.pop_front();
		}
	}

	//Add a handler to the event.
	void addHandler(void (*func)()									//Normal function with no return type or parameters.
					) {
		FUNCTION<SuperClass,SuperClass> *f = new FUNCTION<SuperClass,SuperClass>(func);
		FuncList.push_back(f);
	}

	template <class AnyClass>
	//Add a handler to the event.
	void addHandler(void (*func)(AnyClass*)							//Normal function with no return and a parameter which will take the address of the object.
					) {
		FUNCTION<AnyClass,AnyClass> *f = new FUNCTION<AnyClass,AnyClass>(func);
		FuncList.push_back(f);
	}

	template <class AnyClass,class FuncParam>
	//Add a handler to the event.
	void addHandler(void (*func)(AnyClass*,FuncParam*)				//Normal function with no return type and parameters as linkedObject type, and a variable to recieve address of event parameters.
					) {
		FUNCTION<AnyClass,AnyClass,FuncParam> *f = new FUNCTION<AnyClass,AnyClass,FuncParam>(func);
		FuncList.push_back(f);
	}

	template <class Any>
	//Add a handler to the event.
	void addHandler(Any* obj,void (Any::*func)()					//Some member function with no return type or parameters.
					){
		FUNCTION<Any,SuperClass> *f = new FUNCTION<Any,SuperClass>(obj,func);
		FuncList.push_back(f);
	}

	template <class Any,class AnyClass>
	//Add a handler to the event.
	void addHandler(Any* obj,void (Any::*func)(AnyClass*)			//Some member function with no return type and a parameter for address of linkedObject
					){
		FUNCTION<Any,AnyClass> *f = new FUNCTION<Any,AnyClass>(obj,func);
		FuncList.push_back(f);
	}

	template <class Any,class AnyClass, class FuncParam>
	//Add a handler to the event
	void addHandler(Any* obj,void (Any::*func)(AnyClass*,FuncParam*)	//Some member function with no return type and 2 parameter for address of linkedObject and address to event parameters.
					) {
		FUNCTION<Any,AnyClass,FuncParam> *f = new FUNCTION<Any,AnyClass,FuncParam>(obj,func);
		FuncList.push_back(f);
	}

	//Remove a function from handler list
	void removeHandler(void (*func)()) {
		list<Function*>::iterator i = FuncList.begin();
		for (int I = 0; I<FuncList.size(); I++) {
			if ((*i)->Type == 0)
				if (((FUNCTION<SuperClass,SuperClass>*)(*i))->simplef0==func) {
					delete (*i);
					removeList.push_back(i);
					return;
				}
			i++;
		}
	}

	//Remove a function from handler list
	template <class AnyClass>
	void removeHandler(void (*func)(AnyClass*)) {
		list<Function*>::iterator i = FuncList.begin();
		for (int I = 0; I<FuncList.size(); I++) {
			if ((*i)->Type == 1)
				if (((FUNCTION<AnyClass,AnyClass>*)(*i))->simplef1==func) {
					delete (*i);
					removeList.push_back(i);
					return;
				}
			i++;
		}
	}

	template <class AnyClass,class FuncParam>
	//Remove a function from handler list
	void removeHandler(void (*func)(AnyClass*,FuncParam*)) {
		list<Function*>::iterator i = FuncList.begin();
		for (unsigned int I = 0; I<FuncList.size(); I++) {
			if ((*i)->Type == 1)
				if (((FUNCTION<AnyClass,AnyClass,FuncParam>*)(*i))->simplef2==func) {
					delete (*i);
					removeList.push_back(i);
					return;
				}
			i++;
		}
	}

	template <class Any>
	//Remove a function from handler list
	void removeHandler(void (Any::*func)()){
		list<Function*>::iterator i = FuncList.begin();
		for (int I = 0; I<FuncList.size(); I++) {
			if ((*i)->Type == 4)
				if (((FUNCTION<Any,SuperClass>*)(*i))->f0==func) {
					delete (*i);
					removeList.push_back(i);
					return;
				}
			i++;
		}
	}

	template <class Any,class AnyClass>
	//Remove a function from handler list
	void removeHandler(void (Any::*func)(AnyClass*)){
		list<Function*>::iterator i = FuncList.begin();
		for (int I = 0; I<FuncList.size(); I++) {
			if ((*i)->Type == 4)
				if (((FUNCTION<Any,AnyClass>*)(*i))->f1==func) {
					delete (*i);
					removeList.push_back(i);
					return;
				}
			i++;
		}
	}

	template <class Any,class AnyClass,class FuncParam>
	//Remove a function from handler list
	void removeHandler(void (Any::*func)(AnyClass*,FuncParam*)) {
		list<Function*>::iterator i = FuncList.begin();
		for (int I = 0; I<FuncList.size(); I++) {
			if ((*i)->Type == 5)
				if (((FUNCTION<Any,AnyClass,FuncParam>*)(*i))->f2==func) {
					delete (*i);
					list<Function*>::iterator j = i;
					i++;
					FuncList.erase(j);
					j = i;
					return;
				}
			i++;
		}
	}

	void updateList() {
		while (removeList.size()>0) {
			FuncList.erase(*removeList.begin());
			removeList.pop_front();
		}
	}

};

template <class SuperClass,class ParamType>
void raiseEvent(Event<SuperClass,ParamType>& e,ParamType *p=0) {
	e.raise(p);
}


//Manages all the events that an object can have.
//EventType should be able to hold unique IDs to all the events possible, EventSuperClass is the type of object generating the events, and EventParamType is the basic Parameter all the event handling function would be passed
template <class EventType,class EventSuperClass,class EventParamType = void>
class EventHandler {
	map<EventType,Event<EventSuperClass,EventParamType>*> Handlers;
	EventSuperClass *Parent;
public:
	//ParentObject is a pointer to the object that contains the Handler
	EventHandler(EventSuperClass *ParentObject) {
		Parent = ParentObject;
	};

	//Used for adding handler-functions to events. Can also be used to raise events. 
	//Allocates internal handlers automatically if no handler exists. May cause memory wastage. If using for raising events, use with HandlerDefined()
	Event<EventSuperClass,EventParamType> *event(EventType eventName) {
		if (Handlers.find(eventName)==Handlers.end()) {
			Event<EventSuperClass,EventParamType> *e = new Event<EventSuperClass,EventParamType>(Parent);
			Handlers[eventName] = e;
			return e;
		} else
			return Handlers[eventName];
	}

	//Can be used to add handlers or raise events, passing only the parent object as argument.
	//Allocates internal handlers automatically if no handler exists. May cause memory wastage. If using for raising events, use with HandlerDefined()
	Event<EventSuperClass,EventParamType> *operator() (EventType eventName) {
		return event(eventName);
	}

	//Used to raise events with parent object and a pointer to the parameters to be passed
	void operator() (EventType eventName,EventParamType *params) {
		if (HandlerDefined(eventName)) 
			event(eventName)->raise(params);
	}

	//Used to raise events with parent object and a pointer to the parameters to be passed
	void raise(EventType eventName,EventParamType *params = 0) {
		if (HandlerDefined(eventName)) event(eventName)->raise(params);
	}

	//Check if any functions handle the event "eventName"
	bool HandlerDefined(EventType eventName) {
		return Handlers.find(eventName)!=Handlers.end();
	}

	//Returns true if no event has a handler.
	bool empty() {
		return Handlers.empty();
	}

	//Delete all the internal handlers and the handler function lists.
	~EventHandler() {
		map<EventType,Event<EventSuperClass,EventParamType>*>::iterator t = Handlers.begin();
		for (unsigned int i = 0; i<Handlers.size(); i++) {
			delete t->second;
			t++;
		}
		Handlers.clear();
	}
};