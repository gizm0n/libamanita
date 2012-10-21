
#define LIBCAIRO_EXPORTS 

#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#ifdef USE_WIN32
#include <windows.h>
#include <cairo-win32.h>
#endif
#include <amanita/tk/Cairo.h>


namespace a {
namespace tk {



#ifdef USE_GTK
static gboolean configure_event_callback(GtkWidget *widget,GdkEventConfigure *event,gpointer data) {
	((Cairo *)data)->createBuffer(widget,event);
	return TRUE;
}

static gboolean expose_event_callback(GtkWidget *widget,GdkEventExpose *event,gpointer data) {
	((Cairo *)data)->beginPaint(widget,event);
	return TRUE;
}

/*static gboolean mouse_move_event_callback(GtkWidget *widget,GdkEventMotion *event,gpointer data) {
	((Cairo *)data)->canvasEvent();
	return TRUE;
}

static gboolean mouse_press_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data) {
	((Cairo *)data)->canvasEvent();
	return TRUE;
}

static gboolean mouse_release_event_callback(GtkWidget *widget,GdkEventButton *event,gpointer data) {
	((Cairo *)data)->canvasEvent();
	return TRUE;
}*/

#endif

Object_Inheritance(Cairo,Widget)

Cairo::Cairo(widget_event_handler weh) : Widget(weh,WIDGET_CANVAS) {
#ifdef USE_WIN32
	old = 0;
	bitmap = 0;
	buffer = 0;
	dc = 0;
#endif
}

Cairo::~Cairo() {
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

void Cairo::create(Window *wnd,uint32_t st) {
#ifdef USE_GTK
	component = (Component)gtk_drawing_area_new();
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
	Widget::create(wnd,0);
}

#ifdef USE_GTK
void Cairo::createBuffer(GtkWidget *widget,GdkEventConfigure *event) {
//debug_output("Cairo::createBuffer(widget: %p, drawable: %p)\n",widget,widget->window);
	if(width!=event->width || height!=event->height) {
//		if(cr_surface) cairo_surface_destroy(cr_surface);
//		cr_surface = cairo_surface_create(dis->hDC);
		width = event->width;
		height = event->height;
	}
}
#endif


#ifdef USE_GTK
void Cairo::beginPaint(GtkWidget *widget,GdkEventExpose *event) {
//debug_output("Cairo::beginPaint(widget: %p, drawable: %p)\n",widget,widget->window);
	if(event_handler) {
		cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
		event_handler(this,CAIRO_PAINT,(intptr_t)cr,0,0);
		cairo_destroy(cr);
	}
}
#endif

#ifdef USE_WIN32
bool Cairo::drawItem(LPDRAWITEMSTRUCT dis) {
	if(width>0 && height>0) {
		if(event_handler) {
			if((style&CAIRO_BUFFERED)) {
				if(!bitmap) {
					dc = buffer = CreateCompatibleDC(dis->hDC);
					bitmap = CreateCompatibleBitmap(dis->hDC,width,height);
					old = (HBITMAP)SelectObject(buffer,bitmap);
				}
			} else dc = dis->hDC;
debug_output("Cairo::drawItem()\n");
			cairo_surface_t *surface = cairo_win32_surface_create(dc);
			cairo_t *cr = cairo_create(surface);
			event_handler(this,CAIRO_PAINT,(intptr_t)cr,0,0);
			cairo_surface_flush(surface);
			cairo_surface_finish(surface);
			cairo_destroy(cr);
			cairo_surface_destroy(surface);
			if((style&CAIRO_BUFFERED))
				BitBlt(dis->hDC,0,0,width,height,buffer,0,0,SRCCOPY);
		}
	}
	return true;
}

void Cairo::makeLayout(int x,int y,int w,int h) {
	int w1 = width,h1 = height;
	Widget::makeLayout(x,y,w,h);
debug_output("Cairo::makeLayout(component: %p, w1: %d, h1: %d, width: %d, height: %d)\n",component,w1,h1,width,height);
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

}; /* namespace tk */
}; /* namespace a */



