#ifndef _AMANITA_GUI_WIDGET_H
#define _AMANITA_GUI_WIDGET_H

/**
 * @file amanita/gui/Widget.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <amanita/Geometry.h>
#include <amanita/Object.h>
#include <amanita/Vector.h>
#include <amanita/gui/Event.h>



/** Amanita Namespace */
namespace a {
/** Amanita GUI Namespace */
namespace gui {


/** @cond */
class Tooltip;
/** @endcond */


/** Baseclass for all components.
 * 
 * @ingroup gui */
class Widget : public Object {
/** @cond */
Object_Instance(Widget)
/** @endcond */

private:
	enum {
		COM_VALID		= 0x00000010,
		COM_ENABLED		= 0x00000020,
		COM_VISIBLE		= 0x00000040,
		COM_LOCKED		= 0x00000080,
		COM_OPAQUE		= 0x00000100,
		COM_CLIP			= 0x00000200,
		COM_MODAL		= 0x00001000,
	};
	struct component {
		size_t cnt;							//!< Counter for number of bindings to component. When 0 component is deleted.
		uint32_t id;						//!< ID.
		uint32_t status;					//!< Status.
		short x;								//!< X-coordinate.
		short y;								//!< Y-coordinate.
		short w;								//!< Width.
		short h;								//!< Height.
		Widget *p;							//!< Parent.
		Vector *ch;							//!< Children.
		Tooltip *tt;						//!< Tooltip message.
	};
	component com;

protected:
	static Widget *componentFocus;
	static Widget *keyFocus;
	static Widget *mouseDownFocus;
	static Widget *mouseOverFocus;
	static Widget *toolTipFocus;

	FocusListener *focusListener;
	KeyListener *keyListener;
	MouseListener *mouseListener;
	MouseMotionListener *mouseMotionListener;
	ActionListener *actionListener;

	static bool setComponentFocus(Widget *c);
	static bool setKeyFocus(Widget *c);
	static bool setMouseDownFocus(Widget *c) { mouseDownFocus = c;return true; }
	static bool setMouseOverFocus(Widget *c) { mouseOverFocus = c;return true; }
	static bool setToolTipFocus(Widget *c) { toolTipFocus = c;return true; }

public:
	Widget(int x=0,int y=0,int w=0,int h=0);
	Widget(Widget &c);
	virtual ~Widget();

	void setID(uint32_t i) { com.id = i; }
	uint32_t getID() { return com.id; }
	uint32_t getStatus() { return com.status; }

	virtual void add(Widget *c);
	virtual void remove(Widget *c);
	void moveToTop();
	Widget *getComponent(size_t i);
	Widget *getComponent(int x,int y);
	Widget *getParent() { return com.p; }
	void setBounds(int x,int y,int w,int h) { setLocation(x,y);com.w = w,com.h = h; }
	void setLocation(int x,int y) { moveLocation(x-com.x,y-com.y); }
	void moveLocation(int x,int y);
	void setSize(int w,int h) { com.w = w,com.h = h; }
	void getBounds(int &x,int &y,int &w,int &h) { x = com.x,y = com.y,w = com.w,h = com.h; }
	void getSize(int &w,int &h) { w = com.w,h = com.h; }
	int getX() { return com.x; }
	int getY() { return com.y; }
	int getWidth() { return com.w; }
	int getHeight() { return com.h; }
	bool contains(Widget *c);
	bool contains(int x,int y) { return x>=com.x && y>=com.y && x<com.x+com.w && y<com.y+com.h; }
	bool contains(const rect16_t &r,int x,int y);
	void invalidate() { if(com.status&COM_VALID) com.status ^= COM_VALID; }
	virtual void validate() { com.status |= COM_VALID; }
	bool isValid() { return com.status&COM_VALID; }
	void setEnabled(bool e) { com.status |= COM_ENABLED;if(!e) com.status ^= COM_ENABLED; }
	bool isEnabled() { return com.status&COM_ENABLED; }
	void setVisible(bool v) { com.status |= COM_VISIBLE;if(!v) com.status ^= COM_VISIBLE;setKeyFocus(this); }
	void show() { setVisible(true); }
	void hide() { setVisible(false); }
	bool isVisible() { return com.status&COM_VISIBLE; }
	void setLocked(bool l) { com.status |= COM_LOCKED;if(!l) com.status ^= COM_LOCKED; }
	bool isLocked() { return com.status&COM_LOCKED; }
	void setOpaque(bool o) { com.status |= COM_OPAQUE;if(!o) com.status ^= COM_OPAQUE; }
	bool isOpaque() { return com.status&COM_OPAQUE; }
	void setClipBounds(bool c) { com.status |= COM_CLIP;if(!c) com.status ^= COM_CLIP; }
	bool clipBounds() { return com.status&COM_CLIP; }
	void setModal(bool m) { com.status |= COM_MODAL;if(m) setVisible(false);else com.status ^= COM_MODAL; }
	bool isModal() { return com.status&COM_MODAL; }
	bool isShowing();
	bool isMouseDown() { return this==mouseDownFocus; }
	bool isMouseOver() { return this==mouseOverFocus; }

	bool requestFocus() { return setKeyFocus(this); }

	static Widget *getComponentFocus() { return componentFocus; }
	bool hasComponentFocus() { return this==componentFocus; }
	static Widget *getKeyFocus() { return keyFocus; }
	bool hasKeyFocus() { return this==keyFocus; }
	static Widget *getMouseDownFocus() { return mouseDownFocus; }
	bool hasMouseDownFocus() { return this==mouseDownFocus; }
	static Widget *getMouseOverFocus() { return mouseOverFocus; }
	bool hasMouseOverFocus() { return this==mouseOverFocus; }
	static Widget *getToolTipFocus() { return toolTipFocus; }
	bool hasToolTipFocus() { return this==toolTipFocus; }

	virtual void paint(time_t time);
	void paintAll(time_t time);

	void setFocusListener(FocusListener *fl) { focusListener = fl; }
	FocusListener *getFocusListener() { return focusListener; }
	void setKeyListener(KeyListener *kl) { keyListener = kl; }
	KeyListener *getKeyListener() { return keyListener; }
	void setMouseListener(MouseListener *ml) { mouseListener = ml; }
	MouseListener *getMouseListener() { return mouseListener; }
	void setMouseMotionListener(MouseMotionListener *mml) { mouseMotionListener = mml; }
	MouseMotionListener *getMouseMotionListener() { return mouseMotionListener; }
	void setActionListener(ActionListener *al) { actionListener = al; }
	ActionListener *getActionListener() { return actionListener; }

	bool handleKeyDown(KeyEvent &ke);
	bool handleKeyUp(KeyEvent &ke);
	bool handleMouseDown(MouseEvent &me);
	bool handleMouseUp(MouseEvent &me);
	bool handleMouseMove(MouseMotionEvent &me);
	bool handleMouseDrag(MouseMotionEvent &me);

	void setToolTip(Tooltip *tt);
	Tooltip *getToolTip() { return com.tt; }
	bool handleToolTip(TooltipEvent &tte);
};


}; /* namespace gui */
}; /* namespace a */


#endif /* _AMANITA_GUI_WIDGET_H */
