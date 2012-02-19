#ifndef _AMANITA_GUI_CAIRO_H
#define _AMANITA_GUI_CAIRO_H

/**
 * @file amanita/gui/aCairo.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-11
 * @date Created: 2012-02-11
 */ 

#include <amanita/gui/aWidget.h>

enum {
	aCAIRO_BUFFERED		= 0x00000001,
};

enum {
	aCAIRO_PAINT			= 0x00000001,
};


class aCairo : public aWidget {
friend class aWindow;

/** @cond */
aObject_Instance(aCairo)
/** @endcond */

private:
#ifdef USE_WIN32
//	cairo_surface_t *surface;
	HBITMAP old;
	HBITMAP bitmap;
	HDC buffer;
	HDC dc;
#endif

public:
	aCairo(widget_event_handler weh);
	virtual ~aCairo();

	virtual void create(aWindow *wnd,uint32_t st);

#ifdef USE_GTK
	void createBuffer(GtkWidget *widget,GdkEventConfigure *event);
	void beginPaint(GtkWidget *widget,GdkEventExpose *event);
#endif
#ifdef USE_WIN32
	virtual bool drawItem(LPDRAWITEMSTRUCT dis);
	virtual void makeLayout(int x,int y,int w,int h);
#endif
};


#endif /* _AMANITA_GUI_CAIRO_H */

