#ifndef _LIBAMANITA_GUI_EVENT_H
#define _LIBAMANITA_GUI_EVENT_H


#include <SDL/SDL_stdinc.h>


class Component;


struct FocusEvent {
	Component *source;
	Component *prev;
};
struct KeyEvent {
	Component *source;
	Uint32 sym;
	Uint32 mod;
	Uint16 unicode;
};
struct MouseEvent {
	Component *source;
	Uint16 x,y;
	Uint8 button;
	Uint16 clicks;
};
struct MouseMotionEvent {
	Component *source;
	Uint16 x,y;
	Uint16 dx,dy;
};
struct ActionEvent {
	Component *source;
	Uint32 params;
};
struct ToolTipEvent {
	Component *source;
	Uint16 x,y;
	Uint16 showTimer,hideTimer;
};

class FocusListener {
public:
	virtual bool focusGained(FocusEvent &fe) { return false; }
	virtual bool focusLost(FocusEvent &fe) { return false; }
};
class KeyListener {
public:
	virtual bool keyDown(KeyEvent &ke) { return false; }
	virtual bool keyUp(KeyEvent &ke) { return false; }
};
class MouseListener {
public:
	virtual bool mouseDown(MouseEvent &me) { return false; }
	virtual bool mouseUp(MouseEvent &me) { return false; }
};
class MouseMotionListener {
public:
	virtual bool mouseMove(MouseMotionEvent &mme) { return false; }
	virtual bool mouseDrag(MouseMotionEvent &mme) { return false; }
};
class ActionListener {
public:
	virtual bool actionPerformed(ActionEvent &ae) { return false; }
};


#endif /* _LIBAMANITA_GUI_EVENT_H */
