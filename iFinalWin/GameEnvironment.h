#pragma once
#include "iGraphics.h"

class GameEnvironment : public DGraphics
{
public:
	GameEnvironment(Window *W, bool FullScreen = false);
	~GameEnvironment();
};

