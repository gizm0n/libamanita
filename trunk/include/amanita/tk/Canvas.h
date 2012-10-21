#ifndef _AMANITA_TK_CANVAS_H
#define _AMANITA_TK_CANVAS_H

/**
 * @file amanita/tk/Canvas.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-02-10
 */ 

#include <amanita/tk/Widget.h>

/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


enum {
	CANVAS_BUFFERED		= 0x00000001,
	CANVAS_ANIMATION		= 0x00000002,
};

enum {
	CANVAS_PAINT			= 0x00000001,
};


/** Canvas widget.
 * 
 * Use this widget to paint on. It draws with the built in drawing functions
 * of the operating system. For Linux it is GDK, for windows it is GDI.
 * 
 * @ingroup tk */
class Canvas : public Widget {
friend class Window;
/** @cond */
Object_Instance(Canvas)
/** @endcond */

private:
/** @cond */
#ifdef USE_GTK
	GdkDrawable *drawable;
	GdkPixmap *pixmap;
	GdkGC *gc;
#endif
#ifdef USE_WIN32
	HBITMAP old;
	HBITMAP bitmap;
	HDC buffer;
	HDC dc;
#endif
/** @endcond */

protected:
/** @cond */
#ifdef USE_GTK
	GdkPixmap *getDrawable() { return drawable; }
	GdkGC *getGC() { return gc; }
#endif
#ifdef USE_WIN32
	HDC getDC() { return dc; }
#endif
/** @endcond */

public:
	Canvas(widget_event_handler weh);
	virtual ~Canvas();

	virtual void create(Window *wnd,uint32_t st);

	void setColor(uint32_t c);
//	void drawImage(aImage &i);
	void drawLine(int x1,int y1,int x2,int y2);
	void drawRectangle(int x,int y,int w,int h);
	void fillRectangle(int x,int y,int w,int h);

	virtual void paint();

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


#endif /* _AMANITA_TK_CANVAS_H */

