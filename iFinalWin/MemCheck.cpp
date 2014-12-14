#include "MemoryObjectManager.h"
#include "Console.h"

void ProcessCommand(ConsoleWindow *W, wchar_t *cmd)
{
	if (wcsicmp(cmd, L"Hello") == 0)
	{
		*W << L"Hello to you too";
		*W << endl;
	}
}

void ColorChanger(ConsoleWindow *W)
{
	W->SetBackColor(rand() % 255, rand() % 255, rand() % 255);
	W->clear();
}

void InitWindow(ConsoleWindow *W)
{
	W->handler((EventName)CommandEntered)->addHandler(ProcessCommand);
}

void InitWin2(Window *W)
{
	W->CreateTimer(1000);
	W->handler(EventName::Timer)->addHandler(ColorChanger);
}

int JustACheck(char args[])
{
	ConsoleWindow *W = new ConsoleWindow(15,80,20);
	Window *W2 = new Window(InitWin2);;
	InitWindow(W);
	Window::Run();
	return 0;
}