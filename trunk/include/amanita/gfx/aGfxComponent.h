#ifndef _AMANITA_GFX_COMPONENT_H
#define _AMANITA_GFX_COMPONENT_H

/**
 * @file amanita/gfx/aGfxComponent.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <amanita/Object.h>
#include <amanita/Vector.h>
#include <amanita/gui/aEvent.h>


class aGfxTooltip;
struct SDL_Rect;


/** Baseclass for all components.
 * @ingroup sdl
 */
class aGfxComponent : public Object {
Object_Instance(aGfxComponent)

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
		aGfxComponent *p;					//!< Parent.
		Vector *ch;							//!< Children.
		aGfxTooltip *tt;					//!< Tooltip message.
	};
	component com;

protected:
	static aGfxComponent *componentFocus;
	static aGfxComponent *keyFocus;
	static aGfxComponent *mouseDownFocus;
	static aGfxComponent *mouseOverFocus;
	static aGfxComponent *toolTipFocus;

	aGfxFocusListener *focusListener;
	aGfxKeyListener *keyListener;
	aGfxMouseListener *mouseListener;
	aGfxMouseMotionListener *mouseMotionListener;
	aGfxActionListener *actionListener;

	static bool setaComponentFocus(aGfxComponent *c);
	static bool setKeyFocus(aGfxComponent *c);
	static bool setMouseDownFocus(aGfxComponent *c) { mouseDownFocus = c;return true; }
	static bool setMouseOverFocus(aGfxComponent *c) { mouseOverFocus = c;return true; }
	static bool setToolTipFocus(aGfxComponent *c) { toolTipFocus = c;return true; }

public:
	aGfxComponent(int x=0,int y=0,int w=0,int h=0);
	aGfxComponent(aGfxComponent &c);
	virtual ~aGfxComponent();

	void setID(uint32_t i) { com.id = i; }
	uint32_t getID() { return com.id; }
	uint32_t getStatus() { return com.status; }

	virtual void add(aGfxComponent *c);
	virtual void remove(aGfxComponent *c);
	void moveToTop();
	aGfxComponent *getComponent(size_t i);
	aGfxComponent *getComponent(int x,int y);
	aGfxComponent *getParent() { return com.p; }
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
	bool contains(aGfxComponent *c);
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

	static aGfxComponent *getaComponentFocus() { return componentFocus; }
	bool hasaComponentFocus() { return this==componentFocus; }
	static aGfxComponent *getKeyFocus() { return keyFocus; }
	bool hasKeyFocus() { return this==keyFocus; }
	static aGfxComponent *getMouseDownFocus() { return mouseDownFocus; }
	bool hasMouseDownFocus() { return this==mouseDownFocus; }
	static aGfxComponent *getMouseOverFocus() { return mouseOverFocus; }
	bool hasMouseOverFocus() { return this==mouseOverFocus; }
	static aGfxComponent *getToolTipFocus() { return toolTipFocus; }
	bool hasToolTipFocus() { return this==toolTipFocus; }

	virtual void paint(time_t time);
	void paintAll(time_t time);

	void setFocusListener(aGfxFocusListener *fl) { focusListener = fl; }
	aGfxFocusListener *getFocusListener() { return focusListener; }
	void setKeyListener(aGfxKeyListener *kl) { keyListener = kl; }
	aGfxKeyListener *getKeyListener() { return keyListener; }
	void setMouseListener(aGfxMouseListener *ml) { mouseListener = ml; }
	aGfxMouseListener *getMouseListener() { return mouseListener; }
	void setMouseMotionListener(aGfxMouseMotionListener *mml) { mouseMotionListener = mml; }
	aGfxMouseMotionListener *getMouseMotionListener() { return mouseMotionListener; }
	void setActionListener(aGfxActionListener *al) { actionListener = al; }
	aGfxActionListener *getActionListener() { return actionListener; }

	bool handleKeyDown(aGfxKeyEvent &ke);
	bool handleKeyUp(aGfxKeyEvent &ke);
	bool handleMouseDown(aGfxMouseEvent &me);
	bool handleMouseUp(aGfxMouseEvent &me);
	bool handleMouseMove(aGfxMouseMotionEvent &me);
	bool handleMouseDrag(aGfxMouseMotionEvent &me);

	void setToolTip(aGfxTooltip *tt);
	aGfxTooltip *getToolTip() { return com.tt; }
	bool handleToolTip(aTooltipEvent &tte);
};



#endif /* _AMANITA_GFX_COMPONENT_H */
