#ifndef _AMANITA_GUI_WIDGET_H
#define _AMANITA_GUI_WIDGET_H

/**
 * @file amanita/gui/aWidget.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-04
 * @date Created: 2012-01-28
 */ 

#include <stdint.h>

#ifdef __linux__
#include <gtk/gtk.h>
#endif

#ifdef WIN32
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#include <windows.h>

enum {
	WINDOW_CLASS_WINDOW		= 0x00000001,
	WINDOW_CLASS_BROWSER		= 0x00000010,
};

extern uint32_t class_registers;
#endif



enum {
	FORMAT_NONE					= 0x00000000,
	FORMAT_HORIZONTAL			= 0x00000001,
	FORMAT_VERTICAL			= 0x00000002,
	FORMAT_TABLE				= 0x00000004,
	FORMAT_LEFT					= 0x00000010,
	FORMAT_RIGHT				= 0x00000020,
	FORMAT_CENTER				= 0x00000030,
	FORMAT_TOP					= 0x00000040,
	FORMAT_BOTTOM				= 0x00000080,
	FORMAT_MIDDLE				= 0x000000c0,
	FORMAT_FIXED				= 0x00000100,
	FORMAT_SHRINK				= 0x00000200,
	STRETCH_LEFT				= 0x00001000,
	STRETCH_TOP					= 0x00002000,
	STRETCH_RIGHT				= 0x00004000,
	STRETCH_BOTTOM				= 0x00008000,
	STRETCH_HORIZONTAL		= 0x00005000,
	STRETCH_VERTICAL			= 0x0000a000,
	FORMAT_FILL					= 0x0000f000,
	FORMAT_X_PERCENT			= 0x00010000,
	FORMAT_Y_PERCENT			= 0x00020000,
	FORMAT_PERCENT				= 0x00030000,
	WINDOW_DIALOG				= 0x00100000, // aWindow specific
	WINDOW_RESIZABLE			= 0x00200000, // aWindow specific
	WINDOW_MODAL				= 0x00400000, // aWindow specific
	WINDOW_FULLSCREEN			= 0x00800000, // aWindow specific
	WINDOW_CENTER				= 0x01000000, // aWindow specific
};

enum {
	HANDLE_EMPTY,										//!<
#ifdef __linux__
	HANDLE_BITMAP, 									//!< GdkPixbuf
	HANDLE_FONT,										//!< 
#endif
#ifdef WIN32
	HANDLE_BITMAP,										//!< 
	HANDLE_BRUSH,										//!< 
	HANDLE_COLORSPACE,								//!< 
	HANDLE_CONV,										//!< 
	HANDLE_CONVLIST,									//!< 
	HANDLE_CURSOR,										//!< 
	HANDLE_DC,											//!< 
	HANDLE_DDEDATA,									//!< 
	HANDLE_DESK,										//!< 
	HANDLE_DROP,										//!< 
	HANDLE_DWP,											//!< 
	HANDLE_ENHMETAFILE,								//!< 
	HANDLE_FONT,										//!< 
	HANDLE_GDIOBJ,										//!< 
	HANDLE_GLOBAL,										//!< 
	HANDLE_HOOK,										//!< 
	HANDLE_ICON,										//!< 
	HANDLE_INSTANCE,									//!< 
	HANDLE_KEY,											//!< 
	HANDLE_KL,											//!< 
	HANDLE_LOCAL,										//!< 
	HANDLE_MENU,										//!< 
	HANDLE_METAFILE,									//!< 
	HANDLE_MODULE,										//!< 
	HANDLE_MONITOR,									//!< 
	HANDLE_PALETTE,									//!< 
	HANDLE_PEN,											//!< 
	HANDLE_RGN,											//!< 
	HANDLE_RSRC,										//!< 
	HANDLE_SZ,											//!< 
	HANDLE_WINSTA,										//!< 
	HANDLE_WND,											//!< HWND - All components of type HWND must have this bit set to be recognized as HWND.
#endif
};

#ifdef __linux__
typedef GtkWidget *aComponent;
#endif
#ifdef WIN32
typedef HANDLE aComponent;
#endif

class aWidget;

typedef uint32_t (*widget_event_handler)(aWidget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);

enum widget_type {
	WIDGET_CONTROL = 0x1000,					//!< 
	WIDGET_BOX,										//!< 
	WIDGET_BROWSER,								//!< 
	WIDGET_BUTTON,									//!< 
	WIDGET_CANVAS,									//!< 
	WIDGET_CLABEL,									//!< 
	WIDGET_CHOICE,									//!< 
	WIDGET_CHECKBOX,								//!< 
	WIDGET_DEFAULT_BUTTON,						//!< 
	WIDGET_ENTRY,									//!< 
	WIDGET_IMAGE,									//!< 
	WIDGET_LABEL,									//!< 
	WIDGET_LISTBOX,								//!< 
	WIDGET_MENU,									//!< 
	WIDGET_NOTEBOOK,								//!< 
	WIDGET_PROGRESS,								//!< 
	WIDGET_RLABEL,									//!< 
	WIDGET_SEPARATOR,								//!< 
	WIDGET_STATUSBAR,								//!< 
	WIDGET_TABLE,									//!< 
	WIDGET_TEXT,									//!< 
	WIDGET_TREE,									//!< 
	WIDGET_WINDOW,									//!< 
};

class aWidget {
protected:
	widget_event_handler event_handler;
	unsigned long id;
	widget_type type;								//!< Type of widget
	aWidget *parent;								//!< Handle to parent.
	aWidget *child;
	aWidget *next;
	aComponent container;								//!< Handle to container of native widget.
	aComponent component;							//!< Handle to native widget.
	char *text;
	uint32_t format;
	int16_t x;										//!< X-position.
	int16_t y;										//!< Y-position.
	int16_t width;									//!< Width.
	int16_t height;								//!< Height.
	int16_t min_width;
	int16_t min_height;
	int16_t pad_x;									//!< X-padding.
	int16_t pad_y;									//!< Y-padding.

public:	
	aWidget(widget_event_handler weh,widget_type t);
	virtual ~aWidget();

	void add(aWidget *w);
	aWidget *get(uint32_t id);
	aWidget *iterate(int &lvl);
	aWidget *remove(aWidget *w);
	aWidget *remove(uint32_t id);

	virtual aComponent create();

	aWidget *getParent() { return parent; }
	aComponent getContainer() { return container; }
	aComponent getComponent() { return component; }
	widget_event_handler getEventHandler() { return event_handler; }
	void setID(unsigned long i);
	void setText(const char *str);
	void setFont(aComponent font);
	int getWidth();
	int getHeight();

	void setFormat(uint32_t f,int w=0,int h=0,int px=0,int py=0);
#ifdef WIN32
	virtual void makeLayout(int w,int h);
#endif

	int getMinimumWidth() { return min_width; }
	int getMinimumHeight() { return min_height; }

	void show();
	void hide();

#ifdef WIN32
	HINSTANCE hinst;
#endif
};


#endif /* _AMANITA_GUI_WIDGET_H */

