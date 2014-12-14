//Contains all the events and their handling functions are classes.
#pragma once
#include <Windows.h>
#include <WindowsX.h>

#define NormalEvent 0
#define MouseEvent 1
#define KeyEvent 2

enum EventName {
	Create			=	WM_CREATE,
	Destroy			,
	Move			,
	Size			=	WM_SIZE,
	Activate		,
	MSetFocus		,
	LoseFocus		,
	Enable			=	WM_ENABLE,
	SetRedraw		,
	SetText			,
	GetText			,
	GetTextLength	,
	Paint			,
	Close			,
	QueryEndSession	,							//?
	Quit			,
	QueryOpen		,							//?
	EraseBackground	,
	SysColorChange	,
	EndSession		,							//?
	MShowWindow		=	WM_SHOWWINDOW,
	WinIniChange	=	WM_WININICHANGE,
	WinSettingChange=	WM_WININICHANGE,
	WinDevModeChange,							//?
	ActivateApp		,							//?
	FontChange		,
	TimeChange		,
	CancelMode		,							//?
	MSetCursor		,
	MouseActivate	,
	ChildActivate	,
	QueueSync		,							//?
	GetMinMaxInfo	,							//?
	PaintIcon		=	WM_PAINTICON,
	IconEraseBackGround,
	NextDialogCtl,								//?
	SpoolerStatus	=	WM_SPOOLERSTATUS,		//?
	DrawItem		,
	MeasureItem		,
	DeleteItem		,
	VKeyToItem		,
	CharToItem		,
	SetFont			=	WM_SETFONT,
	GetFont			,
	SetHotKey		,
	GetHotKey		,
	QueryDragIcon	=	WM_QUERYDRAGICON,
	CompareItem		=	WM_COMPAREITEM,
	MGetObject		=	WM_GETOBJECT,
	Compacting		=	WM_COMPACTING,
	WinPosChanging	=	WM_WINDOWPOSCHANGING,
	WinPosChanged	,
	Power			,
	CopyData		=	WM_COPYDATA,
	CancelJournal	,
	Notify			=	WM_NOTIFY,
	InputLangChangRequest	=	WM_INPUTLANGCHANGEREQUEST,
	InputLangChange ,
	TCard			,						//?
	Help			,
	UserChanged		,
	NotifyFormat	,
	ContextMenu		=	WM_CONTEXTMENU,
	StyleChanging	,
	StyleChanged	,
	DisplayChange	,
	GetIcon			,
	SetIcon			,
	NoClientCreate	,
	NoClientDestroy ,
	NoClientCalcSize,
	NoClientHitTest ,
	NoClientPaint	,
	NoClientActivate,
	GetDialogCode	,
	SyncPaint		,

	NCMouseMove		=	WM_NCMOUSEMOVE,
	NCLeftDown		,
	NCLeftUp		,
	NCLeftDblClick	,
	NCRightDown		,
	NCRightUp		,
	NVRightDblClick	,
	NCMiddleDown	,
	NCMiddleUp		,
	NCMiddleDblClick,
	NCXDown			=	WM_NCXBUTTONDOWN,
	NCXUp			,
	NCXDblClick		,
	InputDeviceChange=	WM_INPUT_DEVICE_CHANGE,
	Input			,

	KeyDown			=	WM_KEYFIRST,
	KeyUp			,
	Char			,
	DeadChar		,
	SysKeyDown		,
	SysKeyUp		,
	SysChar			,
	SysDeadChar		,
	UniChar			=	WM_UNICHAR,
	
	ImeStartComposition	=	WM_IME_STARTCOMPOSITION,		//?
	ImeEndComposition	=	WM_IME_ENDCOMPOSITION,
	ImeComposition		=	WM_IME_COMPOSITION,

	InitDialog		,
	Command			,
	SysCommand		,
	Timer			,
	HScroll			,
	VScroll			,
	InitMenu		,
	InitMenuPopup	,
	Gesture			=	WM_GESTURE,
	GestureNotify	,
	MenuSelect		=	WM_MENUSELECT,
	MenuChar		,
	EnterIdle		,
	MenuRUp			,
	MenuDrag		,
	MenuGetObject	,
	UnInitMenuPopup	,
	MenuCommand		,
	ChangeUIState	,
	UpdateUIState	,
	QueryUIState	,

	CtlColorMsgBox	=	WM_CTLCOLORMSGBOX,
	CtlColorEdit	,
	CtlColorListBox	,
	CtlColorBtn		,
	CtlColorDlg		,
	CtlColorScrollBar,
	CtlColorStatic	,

	MouseMove		=	WM_MOUSEFIRST,
	LeftDown		,
	LeftUp			,
	LeftDblClick	,
	RightDown		,
	RightUp			,
	RightDblClick	,
	MiddleDown		,
	MiddleUp		,
	MiddleDblClick	,
	MouseWheel		,
	XDown			,
	XUp				,
	XDblClick		,

	ParentNotify	=	WM_PARENTNOTIFY,
	EnterMenuLoop	,
	ExitMenuLoop	,
	NextMenu		,
	Sizing			,
	CaptureChanged	,
	Moving			,
	PowerBroadcast	=	WM_POWERBROADCAST,			//?
	DeviceChange	,
	
	MDICreate		,
	MDIDestroy		,
	MDIActivate		,
	MDIRestore		,
	MDINext			,
	MDIMaximize		,
	MDITile			,
	MDICascade		,
	MDIIconArrange	,
	MDIGetActive	,
	MDISetMenu		,
	EnterSizeMove	,
	ExitSizeMove	,
	DropFiles		,
	MDIRefreshMenu	,
	Touch			=	WM_TOUCH,
	ImeSetContext	=	WM_IME_SETCONTEXT,
	ImeNotify		,
	ImeControl		,
	ImeCompositionFull,
	ImeSelect		,
	ImeChar			,
	ImeRequest		=	WM_IME_REQUEST,
	
	ImeKeyDown		=	WM_IME_KEYDOWN,
	ImeKeyUp		,

	NCMouseHover	=	WM_NCMOUSEHOVER,
	MouseHover		,
	NCMouseLeave	,
	MouseLeave		,

	WTSSessionChange=	WM_WTSSESSION_CHANGE,
	TabletFirst		=	WM_TABLET_FIRST,
	TabletLast		=	WM_TABLET_LAST,

	Cut				=	WM_CUT,
	Copy			,
	Paste			,
	Clear			,
	Undo			,
	RenderFormat	,
	RenderAllFormats,
	DestroyClipboard,
	DrawClipboard	,
	PaintClipboard	,
	VScrollClipboard,
	SizeClipboard	,
	AskCBFormatName	,
	ChangeCBChain	,
	HScrollClipboard,
	QueryNewPalette	,
	PaletteChanging	,
	PaletteChanged	,
	HotKey			,
	Print			=	WM_PRINT,
	PrintClient		,
	AppCommand		,
	ThemeChanged	=	WM_THEMECHANGED,
	ClipboardUpdate	=	WM_CLIPBOARDUPDATE,

	DWMCompositionChanged,
	DWMNCRenderingChanged,
	DWMColorizationColorChanged,
	DWMWindowMaximizedChange,
	DWMSendIconicThumbnail			=	WM_DWMSENDICONICTHUMBNAIL,
	DWMSendIconicLivePreviewBitmap	=	WM_DWMSENDICONICLIVEPREVIEWBITMAP,
	GetTitleBarInfoEx				=	WM_GETTITLEBARINFOEX,
	HandHeldFirst	=	WM_HANDHELDFIRST,
	HandHeldLast	=	WM_HANDHELDLAST,
	AFXFirst		=	WM_AFXFIRST,
	AFXLast			=	WM_AFXLAST,
	PenWinFirst		=	WM_PENWINFIRST,
	PenWinLast		=	WM_PENWINLAST,
	App				=	WM_APP,
	WindowExiting,
	LastEvent		=	WindowExiting
};

struct EventParams {
	EventName en;
	WPARAM Param1;
	LPARAM Param2;
	int EventType;

	EventParams() {
	}

	EventParams(WPARAM P1,LPARAM P2) {
		Param1 = P1;
		Param2 = P2;
	}
};

struct MouseEventParams : public EventParams {
	int x,y;
	bool ctrl, lButton, mButton, rButton, shift,x1Button,x2Button;

	MouseEventParams() {}

	MouseEventParams(WPARAM P1,LPARAM P2) : EventParams(P1,P2) {
		getAllParams();
	}

	void getAllParams() {
		x = GET_X_LPARAM(Param2);
		y = GET_Y_LPARAM(Param2);
		ctrl = (MK_CONTROL & Param1) != 0;
		lButton = (MK_LBUTTON & Param1) != 0;
		mButton = (MK_MBUTTON & Param1) != 0;
		rButton = (MK_RBUTTON & Param1) != 0;
		shift = (MK_SHIFT & Param1) != 0;
		x1Button = (MK_XBUTTON1 & Param1) != 0;
		x2Button = (MK_XBUTTON2 & Param1) != 0;
	};
};

struct KeyEventParams : public EventParams {
	unsigned int key;
	int repeatCount;
	int scanCode;
	bool extended,contextCode,previousState,transitionState;

	KeyEventParams() {}

	KeyEventParams(WPARAM P1,LPARAM P2): EventParams(P1,P2) {
		getAllParams();
	}
	
	void getAllParams() {
		key = Param1;
		repeatCount = Param2 & 424901760;
		scanCode = Param2 & 65280;
		extended = (Param2 & 128) != 0;
		contextCode = (Param2 & 4) != 0;
		previousState = (Param2 & 2) != 0;
		transitionState = (Param2 & 1) != 0;
	}
};

EventParams* GetEvent(EventName en, WPARAM P1, LPARAM P2, int &eventType);