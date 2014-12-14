#include "Button.h"

namespace Learning_Try {

	Window *MainWindow;

	class MazeBtn : public Button {
		Color color1, color2;
		
	public:
		bool state;
		MazeBtn() {
			state = true;
			Width = 60;
			Height = 60;
			SetText("");
			handler(ButtonEvent::BtnLeftUp)->addHandler(this, &MazeBtn::OnClick);

			color1 = Color(150,50,50);
			color2 = Color(50, 150, 50);
			clr = state ? color2 : color1;
		}

		void OnClick(Button *b) {
			state = !state;
			clr = state ? color2 : color1;
			SetText("");
			Draw();
		}

		void SetColor(int x, int y, int z) {
			clr = Color(x, y, z);
			Draw();
		}
	};


	MazeBtn *MB[10][10];


	float reward[10][10];
	float alpha = 0.9;
	list<PointType> curPath;

	bool addToList(int x, int y) {
		for each (PointType l in curPath) {
			if ((l.X == x) && (l.Y == y)) return false;
		}
		curPath.push_front(PointType(x, y));
		//MB[x][y]->SetColor(50, 50, 150);
		return true;
	}

	void pop() {
		//MB[curPath.begin()->X][curPath.begin()->Y]->SetColor(50, 150, 50);
		curPath.pop_front();
	}

	float findRewardFor(int x, int y) {
		if (reward[x][y] > 0) return reward[x][y];
		if (!MB[x][y]->state) { //reward[x][y] = -1; 
			return -1; }

		float max = -1;
		char dir[2];
		dir[1] = 0;

		if (x > 0) {
			if (addToList(x - 1, y)) {
				float a = findRewardFor(x - 1, y);
				if (a > max) {
					max = a;
					dir[0] = '<';
				}
				pop();
			}

		}

		if (y > 0) {
			if (addToList(x, y - 1)) {
				float a = findRewardFor(x, y - 1);
				if (a > max) {
					max = a;
					dir[0] = '^';
				}
				pop();
			}
		}
		
		if (x < 9) {
			if (addToList(x + 1, y)) {
				float a = findRewardFor(x + 1, y);
				if (a > max) {
					max = a;
					dir[0] = '>';
				}
				pop();
			}
		}
		
		if (y < 9) {
			if (addToList(x, y + 1)) {
				float a = findRewardFor(x, y + 1);
				if (a > max) {
					max = a;
					dir[0] = 'v';
				}
				pop();
			}
		}

		max *= alpha;
		reward[x][y] = max;
		char num[20];
		strcpy(num,to_string(int(max*100)).c_str());
		MB[x][y]->SetText(num);
		MB[x][y]->Draw();
		return max;		
	}

	void learn() {
		

		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				reward[i][j] = -1;				//Unknown reward
			}
		}
		
		reward[9][9] = 1;
		findRewardFor(0,0);			
	}
	

	void initWin(Window *W) {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				MB[i][j] = new MazeBtn();
				MB[i][j]->Location = PointType(0 + i * 60, 0 + j * 60);
				MB[i][j]->AddToWindow(W);
				MB[i][j]->Draw();
			}
		}
		W->handler(EventName::KeyUp)->addHandler(learn);
	}

	int Run(char args[]) {
		MainWindow = new Window(initWin, 0, "Maze", 1200, 700);
		Window::Run();
		return 0;
	}
}