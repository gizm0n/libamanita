#ifndef _AMANITA_GUI_EVENT_H
#define _AMANITA_GUI_EVENT_H

/**
 * @file amanita/gui/aEvent.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 


#include <stdint.h>


class aComponent;


struct aFocusEvent {
	aComponent *source;
	aComponent *prev;
};

struct aKeyEvent {
	aComponent *source;
	uint32_t sym;
	uint32_t mod;
	uint16_t unicode;
};

struct aMouseEvent {
	aComponent *source;
	uint16_t x,y;
	Uint8 button;
	uint16_t clicks;
};

struct aMouseMotionEvent {
	aComponent *source;
	uint16_t x,y;
	uint16_t dx,dy;
};

struct aActionEvent {
	aComponent *source;
	uint32_t params;
};

struct aTooltipEvent {
	aComponent *source;
	uint16_t x,y;
	uint16_t showTimer,hideTimer;
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


#endif /* _AMANITA_GUI_EVENT_H */
