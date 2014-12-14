#include "iBase.h"
#include "DCircle.h"
#include "DRect.h"

#define GAME_WIDTH 1600
#define GAME_HEIGHT 900

#define SKIN_COLOR D3DCOLOR_XRGB(241,179,141)
#define SKIN_BORDER D3DCOLOR_XRGB(241,179,141)
#define SHIRT_COLOR D3DCOLOR_XRGB(0,128,0)
#define SHIRT_BORDER D3DCOLOR_XRGB(0,64,0)
#define TROUSERS_COLOR D3DCOLOR_XRGB(0,0,128)
#define TROUSERS_BORDER D3DCOLOR_XRGB(0,0,64)
#define CAP_COLOR D3DCOLOR_XRGB(0,0,255)
#define EYE_COLOR D3DCOLOR_XRGB(0,0,0)
#define SHOE_COLOR D3DCOLOR_XRGB(240,0,0)
#define SHOE_BORDER D3DCOLOR_XRGB(120,0,0)
#define WALL_COLOR D3DCOLOR_XRGB(128,64,0)
#define SMALLEST_X 40
#define SMALLEST_Y 40
#define CHECK_RADIUS 5
#define HORIZONTAL_SPEED 5
#define VERTICAL_SPEED 4
#define GRAVITY 5
#define JUMP_VELOCITY 50
#define JUMP_DIST 200

#define LOWER_BOUNDARY 800
#define RIGHT_BOUNDARY 1200

struct EnvType {
	short int Type;
	bool KillOnContact;
	bool Obstacle;
	bool Anim;
	unsigned int TmrID;
	DDrawing *drawing1;
	DDrawing *drawing2;
};

class EnvironmentCreator;

class Environment {
	DGraphics *container;
	friend class EnvironmentCreator;

	list <EnvType*> Level;
public:
	Environment(DGraphics *Container) {
		container = Container;
		Container->eHandler(DGraphicsEvents::StartedDraw)->addHandler(this,&Environment::Draw);
	}

	DGraphics *getGraphics() {
		return container;
	}

	void Draw() {
		container->ClearScreen();
		for (list<EnvType*>::iterator I = Level.begin(); I != Level.end(); I++) {
			(*I)->drawing1->Draw();
			DRect::Draw((*I)->drawing1->getX(),(*I)->drawing1->getY(),(*I)->drawing1->width(),(*I)->drawing1->height(),D3DCOLOR_XRGB(0,255,0));
		}
	}

	bool checkHit(DDrawing *D) {
		for (list<EnvType*>::iterator I = Level.begin(); I!=Level.end(); I++)
			if ((*I)->KillOnContact)
				if ((*I)->drawing1->Intersects(D,0,0,0,CHECK_RADIUS))
					return true;
		return false;
	}

	bool isDownEmpty(DDrawing *D,int Padding = CHECK_RADIUS) {
		DDrawing *D1,*D2;
		for (list<EnvType*>::iterator I = Level.begin(); I!=Level.end(); I++)
			if ((*I)->Obstacle) 
				if (D->getY() + D->height() + Padding >= (*I)->drawing1->getY())
					if (D->getY() + D->height() - Padding <= (*I)->drawing1->getY())
				{
					if ((*I)->drawing1->width() > D->width()) {
						D1 = (*I)->drawing1;
						D2 = D;
					} else {
						D1 = D;
						D2 = (*I)->drawing1;
					}
					if (D2->getX() >= D1->getX() && D2-> getX() <= D1->getX() + D1->width())return false;
					if (D2->getX() + D2->width() >= D1->getX() && D2-> getX() + D2->width() <= D1->getX() + D1->width())return false;
				}
		return D->getY() + D->height() + CHECK_RADIUS + Padding< LOWER_BOUNDARY;
	}

	bool isLeftEmpty(DDrawing *D,int Padding = 0) {
		for (list<EnvType*>::iterator I = Level.begin(); I!=Level.end(); I++)
			if ((*I)->Obstacle)
				if ((*I)->drawing1->Intersects(D,CHECK_RADIUS + Padding,0,0,0))
					return false;
		return D->getX() - CHECK_RADIUS - Padding> 0;
	}

	bool isRightEmpty(DDrawing *D,int Padding = 0) {
		for (list<EnvType*>::iterator I = Level.begin(); I!=Level.end(); I++)
			if ((*I)->Obstacle)
				if ((*I)->drawing1->Intersects(D,0,CHECK_RADIUS + Padding,0,0))
					return false;
		return D->getX() + D->width() + Padding< RIGHT_BOUNDARY;
	}

	bool isUpEmpty(DDrawing *D,int Padding = 0) {
		for (list<EnvType*>::iterator I = Level.begin(); I!=Level.end(); I++)
			if ((*I)->Obstacle)
				if ((*I)->drawing1->Intersects(D,0,0,CHECK_RADIUS + Padding,0))
					return false;
		return D->getY() - CHECK_RADIUS - Padding > 0;
	}
};

class EnvironmentCreator {
	Environment *e;
public:
	EnvironmentCreator(Environment *E) {
		e = E;
	}

	EnvType &CreateType() {
		EnvType *E = new EnvType;
		E->drawing1 = 0;
		E->drawing2 = 0;
		return *E;
	}

	void AddObject(int x,int y,EnvType &E) {
		EnvType *ET = new EnvType(E);
		ET->drawing1 = new DDrawing(*E.drawing1);
		if (ET->drawing1) ET->drawing1->Translate(x,y);
		if (ET->drawing2) ET->drawing2->Translate(x,y);
		e->Level.push_back(ET);
	}

	void RemoveObject(int x,int y) {
		for (list<EnvType*>::iterator I = e->Level.begin(); I != e->Level.end(); I++)
			if ((*I)->drawing1->ContainsPoint(x,y)) {
				delete (*I);
				e->Level.erase(I);
			}
	}

	void RemoveAllObjects() {
		for (list<EnvType*>::iterator I = e->Level.begin(); I != e->Level.end(); I++)
			delete (*I);
		e->Level.clear();
	}
};

class Player {
	int x,y;
	Window *PWindow;
	DGraphics *PGraph;
	Environment *Env;
	DDrawing *Dave;
	DDrawing *BentLegs;
	DRect *NormalLegs;
	DDrawing *Cap;
	bool UpdateRequired;
	unsigned int PlrTimer;

	bool facingRight;
	int fallingVelocity;
public:
	Player(Environment *env) {
		if (!env) throw("Invalid container!");
		DGraphics *container = env->getGraphics();
		Env = env;
		
		PGraph = container;
		PWindow = container->GetWindow();
		DCircle::defContainer	= container;
		DEllipse::defContainer	= container;
			DArc::defContainer	= container;
		   DRect::defContainer	= container;
		DDrawing::defContainer	= container;
		  DShape::defContainer  = container;

		x = 390;
		y = 480;
		facingRight = true;
		fallingVelocity = 0;

		Dave = new DDrawing();
		DCircle *Head = new DCircle(400,400,10,SKIN_COLOR,0,true);
		Head->setBorderColor(SKIN_BORDER);

		DShape *FaceDetails = new DShape;
		FaceDetails->curColor = EYE_COLOR;
		FaceDetails->addVertex(407,400);
		FaceDetails->addVertex(408,400);
		FaceDetails->addVertex(407,401);
		FaceDetails->addVertex(408,401);
		FaceDetails->addVertex(407,406);
		FaceDetails->addVertex(405,406);
		FaceDetails->drawType = ptLines;
		Head->addShape(FaceDetails);
		
		DRect *Body = new DRect(390,410,20,30,SHIRT_COLOR,0,true);
		Body->SetBorderColor(SHIRT_BORDER);
		
		Cap = new DDrawing;
		Cap->addDrawing(new DArc(400,400,11,11,180,360,CAP_COLOR,0,true));
		Cap->addDrawing(new DRect(400,398,15,1,CAP_COLOR,0,true));

		NormalLegs = new DRect(395,441,10,30,TROUSERS_COLOR,0,true);
		NormalLegs->SetBorderColor(TROUSERS_BORDER);
		DRect *Shoe = new DRect(395,466,15,5,SHOE_COLOR,0,true);
		Shoe->SetBorderColor(SHOE_BORDER);
		NormalLegs->addDrawing(Shoe);
		
		BentLegs = new DDrawing;
		
		DRect *Leg1 = new DRect(392,441,10,30,TROUSERS_COLOR,0,true);
		Leg1->SetBorderColor(TROUSERS_BORDER);
		Shoe = new DRect(392,466,15,5,SHOE_COLOR,0,true);
		Shoe->SetBorderColor(SHOE_BORDER);
		Leg1->addDrawing(Shoe);
		DRect *Leg2 = new DRect(398,441,10,30,TROUSERS_COLOR,0,true);
		Leg2->SetBorderColor(TROUSERS_BORDER);
		Shoe = new DRect(398,466,15,5,SHOE_COLOR,0,true);
		Shoe->SetBorderColor(SHOE_BORDER);
		Leg2->addDrawing(Shoe);
		BentLegs->addDrawing(Leg1);
		BentLegs->addDrawing(Leg2);
		BentLegs->visible = false;

		Dave->addDrawing(Head);
		Dave->addDrawing(Cap);
		Dave->addDrawing(Body);
		Dave->addDrawing(NormalLegs);
		Dave->addDrawing(BentLegs);
		
		PWindow->handler(EventName::KeyDown)->addHandler(this,&Player::keyHandler);
		PWindow->handler(EventName::Timer)->addHandler(this,&Player::Update);
		PlrTimer = PWindow->CreateTimer(50);
		PGraph->eHandler(DGraphicsEvents::EndingDraw)->addHandler(this,&Player::draw);

		UpdateRequired = true;
	}

	~Player() {
		delete Dave;
	}

	void draw() {
		//		wchar_t str[10];
		//_itow(fallingVelocity,str,10);
		//PWindow->Write(10,10,str);
		Dave->Draw();
		DRect::Draw(Dave->getX(),Dave->getY(),Dave->width(),Dave->height(),D3DCOLOR_XRGB(0,0,0));

	}

	void keyHandler(Window *W,KeyEventParams *K) {
		switch (K->key) {
		case 38:
			jump();
			break;
		case 40:
			duck();
			break;
		case 37:
			left();
			break;
		case 39:
			right();
			break;
		}
	}

	void jump() {
		if (fallingVelocity != 0) return;
		if (Env->isDownEmpty(Dave)) return;
		fallingVelocity = -JUMP_VELOCITY;
		UpdateRequired = true;
	}

	void duck() {

	}

	void left() {
		if (facingRight) {
			Dave->FlipX(x+10);
			UpdateRequired = true;
			facingRight = false;
		}
		else if (Env->isLeftEmpty(NormalLegs)) {
			Dave->Translate(-HORIZONTAL_SPEED,0);
			x -= HORIZONTAL_SPEED;
			if (Env->checkHit(Dave)) Die();
			BentLegs->visible = !BentLegs->visible;
			NormalLegs->visible = !NormalLegs->visible;
			UpdateRequired = true;
		}
	}

	void right() {
		if (!facingRight) {
			Dave->FlipX(x+10);
			UpdateRequired = true;
			facingRight = true;
		}
		else if (Env->isRightEmpty(NormalLegs)) {
			Dave->Translate(HORIZONTAL_SPEED,0);
			x += HORIZONTAL_SPEED;
			if (Env->checkHit(Dave)) Die();
			BentLegs->visible = !BentLegs->visible;
			NormalLegs->visible = !NormalLegs->visible;
			UpdateRequired = true;
		}
	}

	void Die() {
	}

	void Update(Window *W,EventParams *E) {
		if (E->Param1 != PlrTimer) return;

		if (Env->isDownEmpty(NormalLegs,fallingVelocity)) { 
			Dave->Translate(0,fallingVelocity + 1);
			y += fallingVelocity +1;
			if (Env->checkHit(NormalLegs)) Die();
			UpdateRequired = true;
			fallingVelocity += GRAVITY;

			while (!Env->isDownEmpty(NormalLegs,fallingVelocity+1) && fallingVelocity > 0)
				fallingVelocity /= 2;

		} else if (fallingVelocity!=0) fallingVelocity = fallingVelocity / 2;

		if (UpdateRequired) PGraph->PaintScreen();
		UpdateRequired = false;
	}
};

DGraphics *graph;
Player *plr;
Environment *e;
EnvironmentCreator *EC;

void init(Window *W) {
	graph = new DGraphics(W,false);
	e = new Environment(graph);
	EC = new EnvironmentCreator(e);
	EnvType &et = EC->CreateType();
	et.drawing1 = new DRect(0,0,SMALLEST_X,SMALLEST_Y,WALL_COLOR,graph,true);
	et.Obstacle = true;
	EC->AddObject(370,600,et);
	delete &et;
	plr = new Player(e);
}

void destroy(Window *W) {
	delete graph;
	delete plr;
	W->CloseWin();
}

void KeyPressed(Window *W,KeyEventParams *K) {
	if (K->key == 27) destroy(W);
}

int Dave(char args[]) {
	Window main(init,0,(LPSTR)"Dave",GAME_WIDTH,GAME_HEIGHT,WS_POPUPWINDOW,(LPSTR)"WindowClass",0,0);
	main.handler(EventName::Close)->addHandler(destroy);
	main.handler(EventName::KeyDown)->addHandler(KeyPressed);

	main.Run();
	return 0;
}