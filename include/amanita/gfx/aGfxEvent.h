#ifndef _AMANITA_GFX_EVENT_H
#define _AMANITA_GFX_EVENT_H

/**
 * @file amanita/gui/aGfxEvent.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 


#include <stdint.h>


class aGfxComponent;


struct aGfxFocusEvent {
	aGfxComponent *source;
	aGfxComponent *prev;
};

struct aGfxKeyEvent {
	aGfxComponent *source;
	uint32_t sym;
	uint32_t mod;
	uint16_t unicode;
};

struct aGfxMouseEvent {
	aGfxComponent *source;
	uint16_t x,y;
	Uint8 button;
	uint16_t clicks;
};

struct aGfxMouseMotionEvent {
	aGfxComponent *source;
	uint16_t x,y;
	uint16_t dx,dy;
};

struct aGfxActionEvent {
	aGfxComponent *source;
	uint32_t params;
};

struct aGfxTooltipEvent {
	aGfxComponent *source;
	uint16_t x,y;
	uint16_t showTimer,hideTimer;
};

class aGfxFocusListener {
public:
	virtual bool focusGained(aGfxFocusEvent &fe) { return false; }
	virtual bool focusLost(aGfxFocusEvent &fe) { return false; }
};

class aGfxKeyListener {
public:
	virtual bool keyDown(aGfxKeyEvent &ke) { return false; }
	virtual bool keyUp(aGfxKeyEvent &ke) { return false; }
};

class aGfxMouseListener {
public:
	virtual bool mouseDown(aGfxMouseEvent &me) { return false; }
	virtual bool mouseUp(aGfxMouseEvent &me) { return false; }
};

class aGfxMouseMotionListener {
public:
	virtual bool mouseMove(aGfxMouseMotionEvent &mme) { return false; }
	virtual bool mouseDrag(aGfxMouseMotionEvent &mme) { return false; }
};

class aGfxActionListener {
public:
	virtual bool actionPerformed(aGfxActionEvent &ae) { return false; }
};


#endif /* _AMANITA_GFX_EVENT_H */
