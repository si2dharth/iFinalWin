#include "Panel.h"

const int 
	GAME_HEIGHT		= 786,
	GAME_WIDTH		= 1386,
	BAT_WIDTH		= 200,
	BAT_HEIGHT		= 20,
	BAT_ROUND		= 10,
	BALL_DIA		= 20,
	BALL_BORDER		= 5,
	BRICK_VSPACING  = 50,
	BRICK_WIDTH		= 100,
	BRICK_HEIGHT	= 30,
	BRICK_RAD		= 0,
	BRICK_SPACING	= 20,
	BRICK_BORDER	= 2,
	BRICK_ROWS		= 5,
	INIT_LIVES		= 3,
	BAT_Y			= GAME_HEIGHT - 50,
	BAT_COLOR		= Color::Red,
	BRICK_COLOR		= Color::Orange,
	GAME_COLOR		= Color::White,
	BALL_COLOR		= Color::Blue,
	START_VX		= 0,
	START_VY		= -20,
	REFRESH_RATE    = 50,
	LAUNCH_TIME		= 2000;

Window *mainWindow = 0;

typedef bool Direction;
#define VERTICAL true
#define HORIZONTAL false

class Brick : public Rect {
	static list<Brick*> Bricks;
	static Direction lastHitDirection;

	void BreakBrick(Window *W) {
		FillBrush->SetColor(GAME_COLOR);
		W->GetGraphics()->FillRectangle(FillBrush,X,Y,Width,Height);
	}
public:
	static SolidBrush *FillBrush;

	Brick (Window *ContainerWindow, int Row, int Column) {
		X = BRICK_SPACING + Row*BRICK_WIDTH + BRICK_BORDER; 
		Y = Column*BRICK_HEIGHT + BRICK_BORDER + BRICK_VSPACING;
		Width = BRICK_WIDTH - 2*BRICK_BORDER;
		Height = BRICK_HEIGHT - 2*BRICK_BORDER;
		ContainerWindow->GetGraphics()->FillRectangle(FillBrush,X,Y,Width,Height);
		Bricks.push_back(this);
	}

	~Brick () {
		Bricks.remove(this);
	}

	static Direction getLastHitDir() {
		return lastHitDirection;
	}

	static void deleteBrick(Brick* Brk,Window *W) {
		if (!Brk) return;
		Brk->BreakBrick(W);
		delete Brk;
	}

	static Brick *checkHit(Rect *BallArea) {
		for (list<Brick*>::iterator I = Bricks.begin(); I != Bricks.end(); I++)
			if ((*I)->IntersectsWith(*BallArea)) {
				if (BallArea->X +  BallArea->Width - BALL_BORDER < (*I)->X || BallArea->X > (*I)->X + (*I)->Width - BRICK_BORDER*2)
					lastHitDirection = HORIZONTAL; else lastHitDirection = VERTICAL;
				return *I;
			}
		return 0;
	}

	static void RemoveAll() {
		Brick* last = 0;
		for (list<Brick*>::iterator I = Bricks.begin(); I != Bricks.end(); I++) {
			if (last) delete last;
			last = (*I);
		}
		delete last;
	}
};

class Bat : public Rect {
	static Bat *bt;
	SolidBrush *FillColor;
	int lastX;
public:
	Bat(Window *W) {
		if (bt) delete bt;
		bt = this;
		X = (GAME_WIDTH - BAT_WIDTH) >> 1;
		Y = BAT_Y;
		Width =  BAT_WIDTH;
		Height = BAT_HEIGHT;
		FillColor = new SolidBrush(BAT_COLOR);
		W->GetGraphics()->FillRectangle(FillColor,X,Y,Width,Height);
	}

	~Bat() { delete FillColor; }

	void MoveBat(Window *W,MouseEventParams *MP) {
		if (X == MP->x - BAT_WIDTH/2) return;
		FillColor->SetColor(GAME_COLOR);
		W->GetGraphics()->FillRectangle(FillColor,X,Y,Width,Height);
		
		X = MP->x - BAT_WIDTH/2;
		FillColor->SetColor(BAT_COLOR);
		W->GetGraphics()->FillRectangle(FillColor,X,Y,Width,Height);
	}

	static void Destroy() {
		delete bt;
	}

	static bool HitTest(Rect *Region) {
		return bt->IntersectsWith(*Region);
	}

	static int GetAngle(int x) {
		return x - (bt->X + BAT_WIDTH/2);
	}

	static int getX() {
		return bt->X;
	}
};

class Ball : public Rect {
	int vx,vy;
	int WaitTime;
	int Lives;
public:
	Ball(Window *W) {
		X = (GAME_WIDTH - BAT_WIDTH) >> 1;
		Y = BAT_Y - BALL_DIA;
		Height = Width = BALL_DIA;
		vx = START_VX;
		vy = START_VY;
		WaitTime = LAUNCH_TIME / REFRESH_RATE;
		Lives = INIT_LIVES;
	}

	void MoveBall(Window *W) {
		if (WaitTime > 0) {
			W->SetColor(GAME_COLOR);
			W->DrawEllipse(X,Y,Width,Height);
			X = Bat::getX() + BAT_WIDTH / 2 - BALL_DIA/2;
			W->SetColor(BALL_COLOR);
			W->DrawEllipse(X,Y,Width,Height);
			WaitTime--;
			return;
		}

		W->SetColor(GAME_COLOR);
		W->DrawEllipse(X,Y,Width,Height);
		X = X + vx;
		Y = Y + vy;
		
		if (Bat::HitTest(this)) {
			X -= vx;
			Y -= vy;

			vx += Bat::GetAngle(X + vx + BALL_DIA/2)/10;
			vy = -vy;

			X += vx;
			Y += vy;
		}

		Brick *b = Brick::checkHit(this);
		if (b) {
			Brick::deleteBrick(b,W);
			if (Brick::getLastHitDir() == VERTICAL) {
				Y -= vy;
				vy = -vy;
				Y += vy;
			} else {
				X -= vx;
				vx = -vx;
				X += vx;
			}
		}

		if (X < 0 || X > GAME_WIDTH) {
			X -= vx;
			vx = -vx;
			X += vx;
		}

		if (Y < BRICK_VSPACING*4/5 + 1) {
			Y -= vy;
			vy = -vy;
			Y += vy;
		}

		if (Y > GAME_HEIGHT) {
			Plr_Die();
		}

		W->SetColor(BALL_COLOR);
		W->DrawEllipse(X,Y, Width,Height);
	}

	void Plr_Die() {
		if (Lives == 1) {
			Game_Over();
			return;
		}
		X = Bat::getX() + BAT_WIDTH/2;
		Y = BAT_Y - BALL_DIA;
		Height = Width = BALL_DIA;
		vx = START_VX;
		vy = START_VY;
		WaitTime = LAUNCH_TIME / REFRESH_RATE;
		Lives -= 1;
	}

	void Game_Over() {
	}
};


list<Brick*> Brick :: Bricks;
Bat* Bat::bt = 0;
SolidBrush *Brick::FillBrush = 0;
Direction Brick::lastHitDirection = VERTICAL;

void DestroyAll() {
	Brick::RemoveAll();
	Bat::Destroy();
	delete Brick::FillBrush;
}

void InitEnv(Window *W) {
	int 
		nBricksHor = (GAME_WIDTH - 2*BRICK_SPACING) / BRICK_WIDTH,
		nBricksVer = BRICK_ROWS;

	Brick::FillBrush = new SolidBrush(BRICK_COLOR);
	for (int i = 0; i < nBricksHor; i++) {
		for (int j = 0; j < nBricksVer; j++)
			new Brick(W, i, j);
	}

	W->handler(EventName::MouseMove)->addHandler(new Bat(W),&Bat::MoveBat);
	W->handler(EventName::Timer)->addHandler(new Ball(W),&Ball::MoveBall);
	W->handler(EventName::Close)->addHandler(DestroyAll);
	W->handler(EventName::RightDown)->addHandler(DestroyAll);
	W->handler(EventName::RightUp)->addHandler(W,&Window::CloseWin);

	W->GetGraphics()->FillRectangle(new SolidBrush(Color::Black),0,0,GAME_WIDTH,BRICK_VSPACING*4/5);
	Button *B = new Button;
	B->AddToWindow(W);
	B->Location = PointType(GAME_WIDTH-20,0);
	B->Width = 20;
	B->Height = BRICK_VSPACING*4/5;

	B->handler(LeftUp)->addHandler(W,&Window::CloseWin);

	W->CreateTimer(REFRESH_RATE);
}

int Breakout(char* Params) {
	mainWindow = new Window(InitEnv,0,"Break-Out",GAME_WIDTH,GAME_HEIGHT,WS_POPUP,"WindowClass",0,0);
	mainWindow->handler(EventName::Create)->addHandler(InitEnv);
	mainWindow->Run();
	return 0;
};