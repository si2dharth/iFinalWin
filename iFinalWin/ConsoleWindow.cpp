#include "Console.h"

ConsoleWindow::ConsoleWindow(int FontSize, int WidthInChar, int HeightInChar) :
Window(0, LPSTR("Console Window"), FontSize * (WidthInChar + 2), FontSize * Ratio * (HeightInChar + 5)),
fontSize(FontSize), twidth(WidthInChar), theight(HeightInChar), B(Color::White), B2(Color::Black)
{
	SetFontSize(FontSize);
	handler(Char)->addHandler(this, &ConsoleWindow::CharPressed);
	x = y = 1;
	SetFontStyle(L"Ubuntu Mono");
	//for (int i = 0; i < 81; i++)
	//for (int j = 0; j < 100; j++)
	//	memory[i][j] = 0;
	curLine = "";
	curInput = "";
}

ConsoleWindow::~ConsoleWindow() {}

void ConsoleWindow::drawCursor() {

}

wchar_t ConsoleWindow::read()
{
	return 0;
}

wchar_t* ConsoleWindow::readln()
{
	return 0;
}

void ConsoleWindow::write(string str)
{
	for (int i = 0; i < str.length(); i++)
		write(str[i]);
}

void ConsoleWindow::write(wchar_t ch, bool saveToMemory)
{
	if (saveToMemory) curLine += ch;

	if (ch != 13)
	{
		wchar_t c[2];
		c[0] = ch;
		c[1] = 0;
		if (y <= theight)
		{
			GetGraphics()->FillRectangle(&B, x*fontSize, (y)* fontSize * Ratio, fontSize, fontSize * Ratio);
			SetColor(255, 0, 0);
			Write(x*fontSize, y*fontSize*Ratio, c);
		}
		else
		{
			GetGraphics()->FillRectangle(&B, x*fontSize, (theight)* fontSize * Ratio, fontSize, fontSize*Ratio);
			Write(x * fontSize, theight * fontSize, c);
		}
		next();
	}
	else {
		//write('/', false);

		wchar_t finalCmd[100];
		if (handler.HandlerDefined((EventName)CommandEntered))
		{
			/*wchar_t cmd[100];
			for (int i = 0; i < 100; i++) cmd[i] = 0;
			int oX = x, oY = y;
			prev(false);
			int i = 0;
			while (memory[x][y] != 0)
			{
				cmd[i++] = memory[x][y];
				if (x == 1 && y == 1) break;
				prev(false);
			}
			x = oX;
			y = oY;
			wcscpy(finalCmd,wcsrev(cmd));*/
		}

		B.SetColor(GetBackColor());
		GetGraphics()->FillRectangle(&B, x*fontSize, (y + 0.8) * fontSize * Ratio, fontSize, fontSize * Ratio * 0.2);
		y++;
		x = 1;
		GetGraphics()->FillRectangle(&B2, x*fontSize, (y + 0.8) * fontSize * Ratio, fontSize, fontSize * Ratio * 0.2);
		if (y > theight) scroll();

		if (handler.HandlerDefined((EventName)CommandEntered))
			handler((EventName)CommandEntered, (EventParams*)&curInput);
		_memory.push_back(curInput);
		curInput = "";

	}

}

void ConsoleWindow::CharPressed(Window *W, KeyEventParams *KE)
{
	if (KE->key == 8)
	{
		if (x == 1 && y == 1) return;
		prev();
		//memory[x][y] = 0;
		write(' ', false);
		curLine = curLine.substr(0, curLine.length() - 1);
		prev();
	}
	else if ((KE->key >= 32 && KE->key <= 128) || KE->key == 13)
	{
		write(KE->key);
		if (KE->key != 13) RaiseEvent((EventName)CharEntered, (EventParams*)&KE->key);
	}
}

void ConsoleWindow::prev(bool draw)
{
	if (draw) GetGraphics()->FillRectangle(&B, x*fontSize, (y + 0.8) * fontSize * Ratio, fontSize, fontSize * Ratio * 0.2);
	if (x > 1) x--;
	else {
		if (y > 1)
		{
			x = twidth;
			y--;
		}
		else
		{
			if (draw) GetGraphics()->FillRectangle(&B2, x*fontSize, (y + 0.8) * fontSize * Ratio, fontSize, fontSize * Ratio * 0.2);
			return;
		}
		if (y > theight) scroll();
	}
	if (draw) GetGraphics()->FillRectangle(&B2, x*fontSize, (y + 0.8) * fontSize * Ratio, fontSize, fontSize * Ratio * 0.2);
	
}

void ConsoleWindow::next()
{
	//	GetGraphics()->FillRectangle(&B, x*fontSize, (y + 0.8) * fontSize * Ratio, fontSize, fontSize * Ratio * 0.2);
	if (x < twidth) x++;
	else {
		y++;
		x = 1;
		if (y > theight) scroll();
	}
	GetGraphics()->FillRectangle(&B2, x*fontSize, (y + 0.8) * fontSize * Ratio, fontSize, fontSize * Ratio * 0.2);
}

void ConsoleWindow::scroll()
{
	clear();
	//write(memory[x][y], false);
}

void ConsoleWindow::clear()
{
	Clear();
	x = 1; y = 1;
}