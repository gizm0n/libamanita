#ifndef _AMANITA_GUI_WIDGET_H
#define _AMANITA_GUI_WIDGET_H

/**
 * @file amanita/gui/aWidget.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-04
 * @date Created: 2012-01-28
 */ 

#include <stdint.h>

#if defined(__linux__)
#include <gtk/gtk.h>

enum {
	ARRANGE_HORIZONTAL		= 0x00000001,
	ARRANGE_VERTICAL			= 0x00000002,
	ARRANGE_TABLE				= 0x00000004,
	ALIGN_CENTER				= 0x00000010,
	ALIGN_LEFT					= 0x00000020,
	ALIGN_TOP					= 0x00000030,
	ALIGN_RIGHT					= 0x00000040,
	ALIGN_BOTTOM				= 0x00000050,
	STRETCH_NONE				= 0x00000100,
	STRETCH_LEFT				= 0x00001000,
	STRETCH_TOP					= 0x00002000,
	STRETCH_RIGHT				= 0x00004000,
	STRETCH_BOTTOM				= 0x00008000,
	STRETCH_HORIZONTAL		= 0x00005000,
	STRETCH_VERTICAL			= 0x0000a000,
	STRETCH_FULL				= 0x0000f000,
};


#elif defined(WIN32)
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#include <windows.h>
#endif

enum {
	HANDLE_EMPTY,										//!<

#if defined(__linux__)
	HANDLE_BITMAP, 									//!< GdkPixbuf
	HANDLE_FONT,										//!< 

#elif defined(WIN32)
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

#if defined(__linux__)
typedef GtkWidget *aHandle;
#elif defined(WIN32)
typedef HANDLE aHandle;
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
	WIDGET_TABLE,									//!< 
	WIDGET_TEXT,									//!< 
	WIDGET_TREE,									//!< 
};

class aSizer;

class aWidget {
//friend class aSizer;
protected:
	widget_event_handler event_handler;
	unsigned long id;
	widget_type type;								//!< Type of widget
	aHandle parent;								//!< Handle to parent.
	aHandle handle;								//!< Handle to native widget (GtkWidget in GTK, HWND in Win32)
	char *text;
	int16_t x;										//!< X-position.
	int16_t y;										//!< Y-position.
	int16_t width;									//!< Width.
	int16_t height;								//!< Height.
	int16_t min_width;
	int16_t min_height;
	int px;											//!< X-padding.
	int py;											//!< Y-padding.

public:	
	aWidget(widget_event_handler weh,widget_type t);
	virtual ~aWidget();

	virtual aHandle create(aHandle p,int s); // <-- Move to private...

	aHandle getPArent() { return parent; }
	aHandle getHandle() { return handle; }
	void setID(unsigned long i);
	void setText(const char *str);
	void setFont(aHandle font);
	int getWidth();
	int getHeight();

#ifdef WIN32
	HINSTANCE hinst;
#endif
};


#endif /* _AMANITA_GUI_WIDGET_H */

