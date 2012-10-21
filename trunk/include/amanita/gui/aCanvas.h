#ifndef _AMANITA_GUI_CANVAS_H
#define _AMANITA_GUI_CANVAS_H

/**
 * @file amanita/gui/aCanvas.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-10
 * @date Created: 2012-02-10
 */ 

#include <amanita/gui/aWidget.h>

enum {
	aCANVAS_BUFFERED		= 0x00000001,
	aCANVAS_ANIMATION		= 0x00000002,
};

enum {
	aCANVAS_PAINT			= 0x00000001,
};


class aCanvas : public aWidget {
friend class aWindow;

/** @cond */
Object_Instance(aCanvas)
/** @endcond */

private:
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

protected:
#ifdef USE_GTK
	GdkPixmap *getDrawable() { return drawable; }
	GdkGC *getGC() { return gc; }
#endif
#ifdef USE_WIN32
	HDC getDC() { return dc; }
#endif

public:
	aCanvas(widget_event_handler weh);
	virtual ~aCanvas();

	virtual void create(aWindow *wnd,uint32_t st);

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


#endif /* _AMANITA_GUI_CANVAS_H */

