#include "ControlManager.h"

map<Window*, ControlManager*> ControlManager::ManagerList;

void ControlManager::eventRaised(Window *W, EventParams *E)  {
	for (list<Object*>::iterator i = handlerList[E->en].begin(); i != handlerList[E->en].end(); i++) {
		if ((*i)->eventApplicable(E))
			(*i)->handler.raise(E->en,E);
	}
}

ControlManager *ControlManager::CManager(Window *W) {
	if (ControlManager::ManagerList.find(W)==ControlManager::ManagerList.end()) {
		ControlManager* C = new ControlManager(W);
		return C;
	} else
		return (ControlManager::ManagerList[W]);
}