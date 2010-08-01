#ifndef _AMANITA_SDL_COMPONENT_H
#define _AMANITA_SDL_COMPONENT_H

/**
 * @file amanita/sdl/aComponent.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <amanita/aObject.h>
#include <amanita/aVector.h>
#include <amanita/gui/aEvent.h>


class aTooltip;
struct SDL_Rect;


/** Baseclass for all components.
 * @ingroup sdl
 */
class aComponent : public aObject {
aObject_Instance(aComponent)

private:
	enum {
		VALID				= 0x00000010,
		ENABLED			= 0x00000020,
		VISIBLE			= 0x00000040,
		LOCKED			= 0x00000080,
		OPAQUE			= 0x00000100,
		CLIP				= 0x00000200,
		MODAL				= 0x00001000,
	};
	struct component {
		size_t cnt;							//!< Counter for number of bindings to component. When 0 component is deleted.
		uint32_t id;						//!< ID.
		uint32_t status;					//!< Status.
		short x;								//!< X-coordinate.
		short y;								//!< Y-coordinate.
		short w;								//!< Width.
		short h;								//!< Height.
		aComponent *p;						//!< Parent.
		aVector *ch;						//!< Children.
		aTooltip *tt;						//!< Tooltip message.
	};
	component com;

protected:
	static aComponent *componentFocus;
	static aComponent *keyFocus;
	static aComponent *mouseDownFocus;
	static aComponent *mouseOverFocus;
	static aComponent *toolTipFocus;

	aFocusListener *focusListener;
	aKeyListener *keyListener;
	aMouseListener *mouseListener;
	aMouseMotionListener *mouseMotionListener;
	aActionListener *actionListener;

	static bool setaComponentFocus(aComponent *c);
	static bool setKeyFocus(aComponent *c);
	static bool setMouseDownFocus(aComponent *c) { mouseDownFocus = c;return true; }
	static bool setMouseOverFocus(aComponent *c) { mouseOverFocus = c;return true; }
	static bool setToolTipFocus(aComponent *c) { toolTipFocus = c;return true; }

public:
	aComponent(int x=0,int y=0,int w=0,int h=0);
	aComponent(aComponent &c);
	virtual ~aComponent();

	void setID(uint32_t i) { com.id = i; }
	uint32_t getID() { return com.id; }
	uint32_t getStatus() { return com.status; }

	virtual void add(aComponent *c);
	virtual void remove(aComponent *c);
	void moveToTop();
	aComponent *getComponent(size_t i);
	aComponent *getComponent(int x,int y);
	aComponent *getParent() { return com.p; }
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
	bool contains(aComponent *c);
	bool contains(int x,int y) { return x>=com.x && y>=com.y && x<com.x+com.w && y<com.y+com.h; }
	bool contains(const SDL_Rect &r,int x,int y);
	void invalidate() { if(com.status&VALID) com.status ^= VALID; }
	virtual void validate() { com.status |= VALID; }
	bool isValid() { return com.status&VALID; }
	void setEnabled(bool e) { com.status |= ENABLED;if(!e) com.status ^= ENABLED; }
	bool isEnabled() { return com.status&ENABLED; }
	void setVisible(bool v) { com.status |= VISIBLE;if(!v) com.status ^= VISIBLE;setKeyFocus(this); }
	void show() { setVisible(true); }
	void hide() { setVisible(false); }
	bool isVisible() { return com.status&VISIBLE; }
	void setLocked(bool l) { com.status |= LOCKED;if(!l) com.status ^= LOCKED; }
	bool isLocked() { return com.status&LOCKED; }
	void setOpaque(bool o) { com.status |= OPAQUE;if(!o) com.status ^= OPAQUE; }
	bool isOpaque() { return com.status&OPAQUE; }
	void setClipBounds(bool c) { com.status |= CLIP;if(!c) com.status ^= CLIP; }
	bool clipBounds() { return com.status&CLIP; }
	void setModal(bool m) { com.status |= MODAL;if(m) setVisible(false);else com.status ^= MODAL; }
	bool isModal() { return com.status&MODAL; }
	bool isShowing();
	bool isMouseDown() { return this==mouseDownFocus; }
	bool isMouseOver() { return this==mouseOverFocus; }

	bool requestFocus() { return setKeyFocus(this); }

	static aComponent *getaComponentFocus() { return componentFocus; }
	bool hasaComponentFocus() { return this==componentFocus; }
	static aComponent *getKeyFocus() { return keyFocus; }
	bool hasKeyFocus() { return this==keyFocus; }
	static aComponent *getMouseDownFocus() { return mouseDownFocus; }
	bool hasMouseDownFocus() { return this==mouseDownFocus; }
	static aComponent *getMouseOverFocus() { return mouseOverFocus; }
	bool hasMouseOverFocus() { return this==mouseOverFocus; }
	static aComponent *getToolTipFocus() { return toolTipFocus; }
	bool hasToolTipFocus() { return this==toolTipFocus; }

	virtual void paint(time_t time);
	void paintAll(time_t time);

	void setFocusListener(aFocusListener *fl) { focusListener = fl; }
	aFocusListener *getFocusListener() { return focusListener; }
	void setKeyListener(aKeyListener *kl) { keyListener = kl; }
	aKeyListener *getKeyListener() { return keyListener; }
	void setMouseListener(aMouseListener *ml) { mouseListener = ml; }
	aMouseListener *getMouseListener() { return mouseListener; }
	void setMouseMotionListener(aMouseMotionListener *mml) { mouseMotionListener = mml; }
	aMouseMotionListener *getMouseMotionListener() { return mouseMotionListener; }
	void setActionListener(aActionListener *al) { actionListener = al; }
	aActionListener *getActionListener() { return actionListener; }

	bool handleKeyDown(aKeyEvent &ke);
	bool handleKeyUp(aKeyEvent &ke);
	bool handleMouseDown(aMouseEvent &me);
	bool handleMouseUp(aMouseEvent &me);
	bool handleMouseMove(aMouseMotionEvent &me);
	bool handleMouseDrag(aMouseMotionEvent &me);

	void setToolTip(aTooltip *tt);
	aTooltip *getToolTip() { return com.tt; }
	bool handleToolTip(aTooltipEvent &tte);
};



#endif /* _AMANITA_SDL_COMPONENT_H */
