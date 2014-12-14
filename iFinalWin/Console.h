#include "iBase.h"
#include"TextBox.h"
#include <string>
using namespace std;

#define Ratio 1.5
#define endl wchar_t(13)

enum ConsoleEvents
{
	CommandEntered,
	CharEntered,
};

class ConsoleWindow : public Window {
	int x, y;
	char pressedKey;

	SolidBrush B,B2;

	void write(string str);
	void write(wchar_t ch, bool saveToMemory = true);
	void writeln(wchar_t str[]);

	void drawCursor();

	const int fontSize, twidth, theight;
	wchar_t read();
	wchar_t *readln();

	void CharPressed(Window*, KeyEventParams*);

	void next();
	void prev(bool drawCursor = true);
	void scroll();

	wchar_t memory[81][100];

	list<string> _memory;
	string curLine;
	string curInput;
public:
	ConsoleWindow(int FontSize = 10, int WidthInChar = 80, int HeightInChar = 40);
	virtual ~ConsoleWindow();

	void clear();
	void flush();
	void gotoXY(int x, int y);

	ConsoleWindow &operator <<(char c) { write(c); return (*this);  }
	ConsoleWindow &operator <<(wchar_t c) { write(c); return (*this); }
	ConsoleWindow &operator <<(signed long long i) { return (*this); }
	ConsoleWindow &operator <<(unsigned long long i) { return (*this); }
	ConsoleWindow &operator <<(long double f);
	ConsoleWindow &operator <<(bool b) { if (b) write("TRUE"); else write("FALSE"); return (*this); }
	ConsoleWindow &operator <<(string str) { write(str); return (*this); }
	ConsoleWindow &operator <<(void *p) { operator<<((long long)p); return (*this); }

	char &operator >>(char &c);
	char16_t &operator >>(char16_t &c);
	char32_t &operator >>(char32_t &c);
	wchar_t &operator >>(wchar_t &c);

	signed char &operator >>(signed char &c);
	signed short &operator >>(signed short &i);
	signed int &operator >>(signed int &i);
	signed long &operator >>(signed long &i);
	signed long long &operator >>(signed long long &i);

	unsigned char &operator >>(unsigned char &c);
	unsigned long &operator >>(unsigned long &i);
	unsigned long long &operator >>(unsigned long long &i);

	float &operator >>(float &f);
	double &operator >>(double &f);
	long double &operator >>(long double &f);

	bool &operator >>(bool &b);
	char* &operator >>(char * str);
	wchar_t* &operator >>(wchar_t * &str);
};