#ifndef _AMANITA_GUI_WIDGET_H
#define _AMANITA_GUI_WIDGET_H

/**
 * @file amanita/gui/aWidget.h  
 * @author Per Löwgren
 * @date Modified: 2012-01-28
 * @date Created: 2012-01-28
 */ 

#if defined(__linux__)
#include <gtk/gtk.h>



#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN
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
typedef void *aHandle;
#elif defined(WIN32)
typedef HANDLE aHandle;
#endif

enum {
	WIDGET_CONTROL = 0x1000,						//!< 
	WIDGET_BOX,											//!< 
	WIDGET_BROWSER,									//!< 
	WIDGET_BUTTON,										//!< 
	WIDGET_CANVAS,										//!< 
	WIDGET_CLABEL,										//!< 
	WIDGET_CHOICE,										//!< 
	WIDGET_CHECKBOX,									//!< 
	WIDGET_DEFAULT_BUTTON,							//!< 
	WIDGET_ENTRY,										//!< 
	WIDGET_IMAGE,										//!< 
	WIDGET_LABEL,										//!< 
	WIDGET_LISTBOX,									//!< 
	WIDGET_MENU,										//!< 
	WIDGET_NOTEBOOK,									//!< 
	WIDGET_PROGRESS,									//!< 
	WIDGET_RLABEL,										//!< 
	WIDGET_SEPARATOR,									//!< 
	WIDGET_TABLE,										//!< 
	WIDGET_TEXT,										//!< 
	WIDGET_TREE,										//!< 
};

class aSizer;

class aWidget {
friend class aSizer;
protected:
	unsigned long id;
	int type;
	aHandle parent;
	aHandle handle;
	char *text;
	aSizer *sizer;

public:	
	aWidget();
	aWidget(int t,aHandle h=0);
	virtual ~aWidget();

	virtual aHandle create(aHandle p,int s);

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

