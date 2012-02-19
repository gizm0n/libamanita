#ifndef _AMANITA_GUI_WIDGET_H
#define _AMANITA_GUI_WIDGET_H

/**
 * @file amanita/gui/aWidget.h  
 * @author Per Löwgren
 * @date Modified: 2012-02-04
 * @date Created: 2012-01-28
 */ 

#include <stdint.h>
#include <amanita/aHashtable.h>

#ifdef USE_GTK
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#endif

#ifdef USE_WIN32
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#include <windows.h>

enum {
	aWINDOW_CLASS_REGISTER	= 0x00000001,
	aBROWSER_CLASS_REGISTER	= 0x00000010,
};

#define aBROWSER_CLASS _T("WebControl32")
#define aWINDOW_CLASS _T("AmanitaMainWndClass")

extern uint32_t class_registers;

extern int dbu_x;
extern int dbu_y;
#endif



enum {
	aHORIZONTAL			= 0x00010000,
	aVERTICAL			= 0x00020000,
	aTABLE				= 0x00040000,
	aHOMOGENOUS			= 0x00080000,

	aLEFT					= 0x00100000,
	aRIGHT				= 0x00200000,
	aCENTER				= 0x00300000,
	aTOP					= 0x00400000,
	aBOTTOM				= 0x00800000,
	aMIDDLE				= 0x00c00000,
	aALIGNED				= 0x00f00000,

	aFIXED				= 0x01000000,
	aEXPAND				= 0x02000000,
	aFILL					= 0x04000000,
	aHIDE					= 0x80000000,
};


#ifdef USE_GTK
typedef GtkWidget *aComponent;
#endif
#ifdef USE_WIN32
typedef HANDLE aComponent;
#endif

class aWidget;
class aWindow;

typedef uint32_t (*widget_event_handler)(aWidget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);

#ifdef USE_WIN32
struct widget_event_data {
	WNDPROC proc;
	aWidget *widget;
	aWindow *window;
	bool (*event_handler)(aWindow *,UINT,WPARAM,LPARAM);
};
#endif

enum widget_type {
	aWIDGET_CONTROL,								//!< 
	aWIDGET_BROWSER,								//!< 
	aWIDGET_BUTTON,								//!< 
	aWIDGET_CAIRO,									//!< 
	aWIDGET_CANVAS,								//!< 
	aWIDGET_CHECKBOX,								//!< 
	aWIDGET_COMBOBOX,								//!< 
	aWIDGET_COMBOBOX_ENTRY,						//!< 
	aWIDGET_CONTAINER,							//!< 
	aWIDGET_ENTRY,									//!< 
	aWIDGET_FRAME,									//!< 
	aWIDGET_IMAGE,									//!< 
	aWIDGET_LABEL,									//!< 
	aWIDGET_LISTBOX,								//!< 
	aWIDGET_LISTVIEW,								//!< 
	aWIDGET_MENU,									//!< 
	aWIDGET_NOTEBOOK,								//!< 
	aWIDGET_PROGRESS,								//!< 
	aWIDGET_RADIOBUTTON,							//!< 
	aWIDGET_SEPARATOR,							//!< 
	aWIDGET_STATUSBAR,							//!< 
	aWIDGET_TABLE,									//!< 
	aWIDGET_TEXT,									//!< 
	aWIDGET_TREE,									//!< 
	aWIDGET_WINDOW,								//!< 
};

class aWidget : public aObject {
friend class aContainer;
friend class aWindow;
friend class aNotebook;

/** @cond */
aObject_Instance(aWidget)
/** @endcond */

private:
	static aHashtable components;

#ifdef USE_WIN32
	virtual bool drawItem(LPDRAWITEMSTRUCT dis) { return true; }
#endif

protected:
	widget_event_handler event_handler;
#ifdef USE_WIN32
	widget_event_data *event_data;
#endif
	uint32_t id;
	widget_type type;								//!< Type of widget
	aWindow *window;
	aWidget *parent;								//!< Handle to parent.
	aWidget *next;
	aComponent component;						//!< Handle to native widget.
	char *text;
	uint32_t style;
	int16_t x;										//!< X-position.
	int16_t y;										//!< Y-position.
	uint16_t width;								//!< Width.
	uint16_t height;								//!< Height.
	uint16_t min_width;
	uint16_t min_height;
	uint8_t border;								//!< Border.
	uint8_t spacing;								//!< Spacing.

	void addComponent(aComponent c) { components.put(c,this); }
	void deleteComponent(aComponent c) { components.remove(c); }

#ifdef USE_WIN32
	void captureEvents();

	virtual void makeLayout(int x,int y,int w,int h);
	virtual int getMinimumWidth() { return min_width; }
	virtual int getMinimumHeight() { return min_height; }
	virtual void move();
#endif

	aWidget(widget_event_handler weh,widget_type t);

public:	
	virtual ~aWidget();

	virtual void create(aWindow *wnd,uint32_t st);
	virtual void createAll(aComponent p,bool n);

	aWidget *getWidget(aComponent c) { return (aWidget *)components.get(c); }

	aWindow *getWindow() { return window; }
	aWidget *getParent() { return parent; }
	aComponent getComponent() { return component; }
	widget_event_handler getEventHandler() { return event_handler; }
	uint32_t getID() { return id; }
	virtual void setText(const char *str);
	const char *getText() { return text; }
	void setFont(aComponent font);
	int getWidth() { return width; }
	int getHeight() { return height; }

	void setStyle(uint32_t st,uint16_t minw=0,uint16_t minh=0,uint8_t b=0,uint8_t sp=0);

	void show();
	void hide();
};


enum {
#ifdef USE_GTK
	aKEY_CONTROL					= GDK_CONTROL_MASK,
	aKEY_ALT							= GDK_MOD1_MASK,
	aKEY_SHIFT						= GDK_SHIFT_MASK,
	aKEY_ENTER						= GDK_Return,
	aKEY_SPACE						= GDK_space,
	aKEY_a							= GDK_a,
	aKEY_b							= GDK_b,
	aKEY_c							= GDK_c,
	aKEY_d							= GDK_d,
	aKEY_e							= GDK_e,
	aKEY_f							= GDK_f,
	aKEY_g							= GDK_g,
	aKEY_h							= GDK_h,
	aKEY_i							= GDK_i,
	aKEY_j							= GDK_j,
	aKEY_k							= GDK_k,
	aKEY_l							= GDK_l,
	aKEY_m							= GDK_m,
	aKEY_n							= GDK_n,
	aKEY_o							= GDK_o,
	aKEY_p							= GDK_p,
	aKEY_q							= GDK_q,
	aKEY_r							= GDK_r,
	aKEY_s							= GDK_s,
	aKEY_t							= GDK_t,
	aKEY_u							= GDK_u,
	aKEY_v							= GDK_v,
	aKEY_w							= GDK_w,
	aKEY_x							= GDK_x,
	aKEY_y							= GDK_y,
	aKEY_z							= GDK_z,
	aKEY_F1							= GDK_F1,
	aKEY_F2							= GDK_F2,
	aKEY_F3							= GDK_F3,
	aKEY_F4							= GDK_F4,
	aKEY_F5							= GDK_F5,
	aKEY_F6							= GDK_F6,
	aKEY_F7							= GDK_F7,
	aKEY_F8							= GDK_F8,
	aKEY_F9							= GDK_F9,
	aKEY_F10							= GDK_F10,
	aKEY_F11							= GDK_F11,
	aKEY_F12							= GDK_F12,
#endif

#ifdef USE_WIN32
	aKEY_CONTROL					= 0x0001,
	aKEY_ALT							= 0x0002,
	aKEY_SHIFT						= 0x0004,
	aKEY_ENTER						= VK_RETURN,
	aKEY_SPACE						= VK_SPACE,
	aKEY_a							= 'A',
	aKEY_b							= 'B',
	aKEY_c							= 'C',
	aKEY_d							= 'D',
	aKEY_e							= 'E',
	aKEY_f							= 'F',
	aKEY_g							= 'G',
	aKEY_h							= 'H',
	aKEY_i							= 'I',
	aKEY_j							= 'J',
	aKEY_k							= 'K',
	aKEY_l							= 'L',
	aKEY_m							= 'M',
	aKEY_n							= 'N',
	aKEY_o							= 'O',
	aKEY_p							= 'P',
	aKEY_q							= 'Q',
	aKEY_r							= 'R',
	aKEY_s							= 'S',
	aKEY_t							= 'T',
	aKEY_u							= 'U',
	aKEY_v							= 'V',
	aKEY_w							= 'W',
	aKEY_x							= 'X',
	aKEY_y							= 'Y',
	aKEY_z							= 'Z',
	aKEY_F1							= VK_F1,
	aKEY_F2							= VK_F2,
	aKEY_F3							= VK_F3,
	aKEY_F4							= VK_F4,
	aKEY_F5							= VK_F5,
	aKEY_F6							= VK_F6,
	aKEY_F7							= VK_F7,
	aKEY_F8							= VK_F8,
	aKEY_F9							= VK_F9,
	aKEY_F10							= VK_F10,
	aKEY_F11							= VK_F11,
	aKEY_F12							= VK_F12,
#endif
};


#endif /* _AMANITA_GUI_WIDGET_H */

