#ifndef _AMANITA_TK_CAIRO_H
#define _AMANITA_TK_CAIRO_H

/**
 * @file amanita/tk/Cairo.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-11
 */ 

#include <amanita/tk/Widget.h>

/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


enum {
	CAIRO_BUFFERED			= 0x00000001,
};

enum {
	CAIRO_PAINT				= 0x00000001,
};


/** Cairo widget.
 * 
 * Use this widget to paint with Cairo.
 * 
 * @ingroup tk */
class Cairo : public Widget {
friend class Window;
/** @cond */
Object_Instance(Cairo)
/** @endcond */

private:
/** @cond */
#ifdef USE_WIN32
//	cairo_surface_t *surface;
	HBITMAP old;
	HBITMAP bitmap;
	HDC buffer;
	HDC dc;
#endif
/** @endcond */

public:
	Cairo(widget_event_handler weh);
	virtual ~Cairo();

	virtual void create(Window *wnd,uint32_t st);

#ifdef USE_GTK
	void createBuffer(GtkWidget *widget,GdkEventConfigure *event);
	void beginPaint(GtkWidget *widget,GdkEventExpose *event);
#endif
#ifdef USE_WIN32
	virtual bool drawItem(LPDRAWITEMSTRUCT dis);
	virtual void makeLayout(int x,int y,int w,int h);
#endif
};

}; /* namespace tk */
}; /* namespace a */



#endif /* _AMANITA_TK_CAIRO_H */

