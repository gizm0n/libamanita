#ifndef _LIBAMANITA_SDL_AEVENT_H
#define _LIBAMANITA_SDL_AEVENT_H


#include <SDL/SDL_stdinc.h>


class aComponent;


struct aFocusEvent {
	aComponent *source;
	aComponent *prev;
};
struct aKeyEvent {
	aComponent *source;
	Uint32 sym;
	Uint32 mod;
	Uint16 unicode;
};
struct aMouseEvent {
	aComponent *source;
	Uint16 x,y;
	Uint8 button;
	Uint16 clicks;
};
struct aMouseMotionEvent {
	aComponent *source;
	Uint16 x,y;
	Uint16 dx,dy;
};
struct aActionEvent {
	aComponent *source;
	Uint32 params;
};
struct aTooltipEvent {
	aComponent *source;
	Uint16 x,y;
	Uint16 showTimer,hideTimer;
};

class aFocusListener {
public:
	virtual bool focusGained(aFocusEvent &fe) { return false; }
	virtual bool focusLost(aFocusEvent &fe) { return false; }
};
class aKeyListener {
public:
	virtual bool keyDown(aKeyEvent &ke) { return false; }
	virtual bool keyUp(aKeyEvent &ke) { return false; }
};
class aMouseListener {
public:
	virtual bool mouseDown(aMouseEvent &me) { return false; }
	virtual bool mouseUp(aMouseEvent &me) { return false; }
};
class aMouseMotionListener {
public:
	virtual bool mouseMove(aMouseMotionEvent &mme) { return false; }
	virtual bool mouseDrag(aMouseMotionEvent &mme) { return false; }
};
class aActionListener {
public:
	virtual bool actionPerformed(aActionEvent &ae) { return false; }
};


#endif /* _LIBAMANITA_SDL_AEVENT_H */
