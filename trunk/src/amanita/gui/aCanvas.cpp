
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#ifdef USE_WIN32
#include <windows.h>
#include <cairo-win32.h>
#endif
#include <amanita/gui/aCanvas.h>


#ifdef USE_GTK
static gboolean configure_event_callback(GtkWidget *widget,GdkEventConfigure *event,gpointer data) {
	((aCanvas *)data)->createBuffer(widget,event);
	return TRUE;
}

static gboolean expose_event_callback(GtkWidget *widget,GdkEventExpose *event,gpointer data) {
	((aCanvas *)data)->beginPaint(widget,event);
	return TRUE;
}

/*static gboolean mouse_move_event_callback(GtkWidget *widget,GdkEventMotion *event,gpointer data) {
	((aCanvas *)data)->canvasEvent();
	return TRUE;
}

static gboolean mouse_press_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data) {
	((aCanvas *)data)->canvasEvent();
	return TRUE;
}

static gboolean mouse_release_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data) {
	((aCanvas *)data)->canvasEvent();
	return TRUE;
}*/

#endif

Object_Inheritance(aCanvas,aWidget)

aCanvas::aCanvas(widget_event_handler weh) : aWidget(weh,aWIDGET_CANVAS) {
#ifdef USE_GTK
	drawable = 0;
	pixmap = 0;
	gc = 0;
#endif
#ifdef USE_WIN32
	old = 0;
	bitmap = 0;
	buffer = 0;
	dc = 0;
#endif
}

aCanvas::~aCanvas() {
#ifdef USE_GTK
	if(pixmap) g_object_unref(pixmap);
	pixmap = 0;
#endif
#ifdef USE_WIN32
	if(bitmap) {
		SelectObject(buffer,old);
		DeleteObject(bitmap);
		bitmap = 0;
		DeleteDC(buffer);
		buffer = 0;
	}
#endif
}

void aCanvas::create(aWindow *wnd,uint32_t st) {
#ifdef USE_GTK
	component = (aComponent)gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(component),"expose_event",G_CALLBACK(expose_event_callback),(gpointer)this);
	g_signal_connect(G_OBJECT(component),"configure_event",G_CALLBACK(configure_event_callback),(gpointer)this);
/*	g_signal_connect(G_OBJECT(component),"motion_notify_event",G_CALLBACK(mouse_move_event_callback),(gpointer)this);
	g_signal_connect(G_OBJECT(component),"button_press_event",G_CALLBACK(mouse_press_event_callback),(gpointer)this);
	g_signal_connect(G_OBJECT(component),"button_release_event",G_CALLBACK(mouse_release_event_callback),(gpointer)this);*/
	gtk_widget_set_events((GtkWidget *)component,GDK_EXPOSURE_MASK
/*			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK*/);
//	pixmap = gdk_pixmap_new(((GtkWidget *)component)->window,width,height,-1);
#endif
	aWidget::create(wnd,0);
}

#ifdef USE_GTK
void aCanvas::createBuffer(GtkWidget *widget,GdkEventConfigure *event) {
//debug_output("aCanvas::createBuffer(widget: %p, drawable: %p)\n",widget,widget->window);
	if(width!=event->width || height!=event->height) {
		if(pixmap) g_object_unref(pixmap);
		if((style&aCANVAS_BUFFERED))
			drawable = pixmap = gdk_pixmap_new(gtk_widget_get_window(widget),event->width,event->height,-1);
		else pixmap = 0,drawable = gtk_widget_get_window(widget);
		width = event->width;
		height = event->height;
	}
}
#endif

#ifdef USE_GTK
static GdkColor color;
#endif
#ifdef USE_WIN32
static uint32_t fgcol = 0x80000000;
static uint32_t bgcol = 0x80000000;

void set_background(HDC hdc,uint32_t c) {
	static LOGBRUSH brush = { BS_SOLID,0,0 };
	if(c==bgcol) return;
	bgcol = c;
	if((c&0x80000000)) brush.lbStyle = BS_HOLLOW;
	else brush.lbStyle = BS_SOLID,brush.lbColor = RGB((c>>16),((c>>8)&0xff),(c&0xff));
	DeleteObject(SelectObject(hdc,CreateBrushIndirect(&brush)));
}
#endif

void aCanvas::setColor(uint32_t c) {
#ifdef USE_GTK
	color.pixel = 0xff000000|c;
	gdk_gc_set_foreground(gc,&color);
#endif
#ifdef USE_WIN32
	if(c==fgcol) return;
	fgcol = c;
	DeleteObject(SelectObject(dc,CreatePen(PS_SOLID,1,RGB((c>>16),((c>>8)&0xff),(c&0xff)))));
#endif
}


/*void aCanvas::drawImage(aImage &i) {
}*/

void aCanvas::drawLine(int x1,int y1,int x2,int y2) {
#ifdef USE_GTK
	gdk_draw_line(drawable,gc,x1,y1,x2,y2);
#endif
}

void aCanvas::drawRectangle(int x,int y,int w,int h) {
#ifdef USE_GTK
	gdk_draw_rectangle(drawable,gc,false,x,y,w-1,h-1);
#endif
#ifdef USE_WIN32
	set_background(dc,0x80000000);
	Rectangle(dc,x,y,x+w,y+h);
#endif
}


void aCanvas::fillRectangle(int x,int y,int w,int h) {
#ifdef USE_GTK
	gdk_draw_rectangle(drawable,gc,true,x,y,w,h);
#endif
#ifdef USE_WIN32
	set_background(dc,fgcol);
	Rectangle(dc,x,y,x+w,y+h);
#endif
}

#ifdef USE_GTK
void aCanvas::beginPaint(GtkWidget *widget,GdkEventExpose *event) {
//debug_output("aCanvas::beginPaint(widget: %p, drawable: %p)\n",widget,widget->window);
	gc = gdk_gc_new(drawable);
	if(event_handler) event_handler(this,aCANVAS_PAINT,0,0,0);
	else paint();
	if((style&aCANVAS_BUFFERED))
		gdk_draw_pixmap(gtk_widget_get_window(widget),widget->style->fg_gc[GTK_WIDGET_STATE(widget)],pixmap,
				event->area.x,event->area.y,event->area.x,event->area.y,event->area.width,event->area.height);
	g_object_unref(gc);
}
#endif

#ifdef USE_WIN32
bool aCanvas::drawItem(LPDRAWITEMSTRUCT dis) {
//debug_output("aCanvas::drawItem(width: %d, height: %d)\n",width,height);
	if(width>0 && height>0) {
		if((style&aCANVAS_BUFFERED)) {
			if(!bitmap) {
				dc = buffer = CreateCompatibleDC(dis->hDC);
				bitmap = CreateCompatibleBitmap(dis->hDC,width,height);
				old = (HBITMAP)SelectObject(buffer,bitmap);
			}
		} else dc = dis->hDC;
		fgcol = 0x80000000;
		bgcol = 0x80000000;

		if(!event_handler || !event_handler(this,aCANVAS_PAINT,0,0,0)) paint();
		if((style&aCANVAS_BUFFERED))
			BitBlt(dis->hDC,0,0,width,height,buffer,0,0,SRCCOPY);
	}
	return true;
}

void aCanvas::makeLayout(int x,int y,int w,int h) {
	int w1 = width,h1 = height;
	aWidget::makeLayout(x,y,w,h);
//debug_output("aCanvas::makeLayout(width: %d, height: %d)\n",width,height);
	if(w1!=width || h1!=height) {
		if(bitmap) {
			SelectObject(buffer,old);
			DeleteObject(bitmap);
			bitmap = 0;
			DeleteDC(buffer);
			buffer = 0;
		}
	}
}
#endif

void aCanvas::paint() {
	setColor(0x00ffff);
	fillRectangle(0,0,width,height);
	setColor(0xff00ff);
	drawRectangle(2,2,width-4,height-4);
}


