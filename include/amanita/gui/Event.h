#ifndef _AMANITA_GUI_EVENT_H
#define _AMANITA_GUI_EVENT_H

/**
 * @file amanita/gui/Event.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 


#include <stdint.h>


/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


/** @cond */
class Widget;
/** @endcond */


struct FocusEvent {
	Widget *source;
	Widget *prev;
};

struct KeyEvent {
	Widget *source;
	uint32_t sym;
	uint32_t mod;
	uint16_t unicode;
};

struct MouseEvent {
	Widget *source;
	uint16_t x,y;
	uint8_t button;
	uint16_t clicks;
};

struct MouseMotionEvent {
	Widget *source;
	uint16_t x,y;
	uint16_t dx,dy;
};

struct ActionEvent {
	Widget *source;
	uint32_t params;
};

struct TooltipEvent {
	Widget *source;
	uint16_t x,y;
	uint16_t showTimer,hideTimer;
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

}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_EVENT_H */
