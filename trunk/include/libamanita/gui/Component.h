#ifndef _LIBAMANITA_GUI_COMPONENT_H
#define _LIBAMANITA_GUI_COMPONENT_H

#include <libamanita/Object.h>
#include <libamanita/Vector.h>
#include <libamanita/sdl/Thread.h>
#include <libamanita/gui/Event.h>


class ToolTip;
struct SDL_Rect;


class Component : public Object {
RttiObjectInstance(Component)

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
		size_t cnt;
		unsigned long id,status;
		short x,y,w,h;
		Component *p;					// Parent
		Vector *ch;						// Children
		ToolTip *tt;
	};
	component com;

protected:
	static Component *componentFocus,*keyFocus,*mouseDownFocus,*mouseOverFocus,*toolTipFocus;
	FocusListener *focusListener;
	KeyListener *keyListener;
	MouseListener *mouseListener;
	MouseMotionListener *mouseMotionListener;
	ActionListener *actionListener;

	static bool setComponentFocus(Component *c);
	static bool setKeyFocus(Component *c);
	static bool setMouseDownFocus(Component *c) { mouseDownFocus = c;return true; }
	static bool setMouseOverFocus(Component *c) { mouseOverFocus = c;return true; }
	static bool setToolTipFocus(Component *c) { toolTipFocus = c;return true; }

public:
	Component(int x=0,int y=0,int w=0,int h=0);
	Component(Component &c);
	virtual ~Component();

	void setID(unsigned long i) { com.id = i; }
	unsigned long getID() { return com.id; }
	unsigned long getStatus() { return com.status; }

	virtual void add(Component *c);
	virtual void remove(Component *c);
	void moveToTop();
	Component *getComponent(size_t i);
	Component *getComponent(int x,int y);
	Component *getParent() { return com.p; }
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
	bool contains(Component *c);
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

	static Component *getComponentFocus() { return componentFocus; }
	bool hasComponentFocus() { return this==componentFocus; }
	static Component *getKeyFocus() { return keyFocus; }
	bool hasKeyFocus() { return this==keyFocus; }
	static Component *getMouseDownFocus() { return mouseDownFocus; }
	bool hasMouseDownFocus() { return this==mouseDownFocus; }
	static Component *getMouseOverFocus() { return mouseOverFocus; }
	bool hasMouseOverFocus() { return this==mouseOverFocus; }
	static Component *getToolTipFocus() { return toolTipFocus; }
	bool hasToolTipFocus() { return this==toolTipFocus; }

	virtual void paint(int time);
	void paintAll(int time);

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

	void setToolTip(ToolTip *tt);
	ToolTip *getToolTip() { return com.tt; }
	bool handleToolTip(ToolTipEvent &tte);

	operator unsigned long() const { return com.id; }
};



#endif /* _LIBAMANITA_GUI_COMPONENT_H */
