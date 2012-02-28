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


#ifdef USE_GTK
#define aICON_ABOUT									GTK_STOCK_ABOUT
#define aICON_ADD										GTK_STOCK_ADD
#define aICON_APPLY									GTK_STOCK_APPLY
#define aICON_BOLD									GTK_STOCK_BOLD
#define aICON_CANCEL									GTK_STOCK_CANCEL
#define aICON_CAPS_LOCK_WARNING					GTK_STOCK_CAPS_LOCK_WARNING
#define aICON_CDROM									GTK_STOCK_CDROM
#define aICON_CLEAR									GTK_STOCK_CLEAR
#define aICON_CLOSE									GTK_STOCK_CLOSE
#define aICON_COLOR_PICKER							GTK_STOCK_COLOR_PICKER
#define aICON_CONVERT								GTK_STOCK_CONVERT
#define aICON_CONNECT								GTK_STOCK_CONNECT
#define aICON_COPY									GTK_STOCK_COPY
#define aICON_CUT										GTK_STOCK_CUT
#define aICON_DELETE									GTK_STOCK_DELETE
#define aICON_AUTHENTICATION						GTK_STOCK_DIALOG_AUTHENTICATION
#define aICON_ERROR									GTK_STOCK_DIALOG_ERROR
#define aICON_DIALOG_INFO							GTK_STOCK_DIALOG_INFO
#define aICON_DIALOG_QUESTION						GTK_STOCK_DIALOG_QUESTION
#define aICON_DIALOG_WARNING						GTK_STOCK_DIALOG_WARNING
#define aICON_DIRECTORY								GTK_STOCK_DIRECTORY
#define aICON_DISCARD								GTK_STOCK_DISCARD
#define aICON_DISCONNECT							GTK_STOCK_DISCONNECT
#define aICON_DND										GTK_STOCK_DND
#define aICON_DND_MULTIPLE							GTK_STOCK_DND_MULTIPLE
#define aICON_EDIT									GTK_STOCK_EDIT
#define aICON_EXECUTE								GTK_STOCK_EXECUTE
#define aICON_FILE									GTK_STOCK_FILE
#define aICON_FIND									GTK_STOCK_FIND
#define aICON_RIND_AND_REPLACE					GTK_STOCK_FIND_AND_REPLACE
#define aICON_FLOPPY									GTK_STOCK_FLOPPY
#define aICON_FULLSCREEN							GTK_STOCK_FULLSCREEN
#define aICON_GOTO_BOTTOM							GTK_STOCK_GOTO_BOTTOM
#define aICON_GOTO_FIRST							GTK_STOCK_GOTO_FIRST
#define aICON_GOTO_LAST								GTK_STOCK_GOTO_LAST
#define aICON_GOTO_TOP								GTK_STOCK_GOTO_TOP
#define aICON_GO_BACK								GTK_STOCK_GO_BACK
#define aICON_GO_DOWN								GTK_STOCK_GO_DOWN
#define aICON_GO_FORWARD							GTK_STOCK_GO_FORWARD
#define aICON_GO_UP									GTK_STOCK_GO_UP
#define aICON_HARDDISK								GTK_STOCK_HARDDISK
#define aICON_HELP									GTK_STOCK_HELP
#define aICON_HOME									GTK_STOCK_HOME
#define aICON_INDENT									GTK_STOCK_INDENT
#define aICON_INDEX									GTK_STOCK_INDEX
#define aICON_INFO									GTK_STOCK_INFO
#define aICON_ITALIC									GTK_STOCK_ITALIC
#define aICON_JUMP_TO								GTK_STOCK_JUMP_TO
#define aICON_JUSTIFY_CENTER						GTK_STOCK_JUSTIFY_CENTER
#define aICON_JUSTIFY_FILL							GTK_STOCK_JUSTIFY_FILL
#define aICON_JUSTIFY_LEFT							GTK_STOCK_JUSTIFY_LEFT
#define aICON_JUSTIFY_RIGHT						GTK_STOCK_JUSTIFY_RIGHT
#define aICON_LEAVE_FULLSCREEN					GTK_STOCK_LEAVE_FULLSCREEN
#define aICON_MEDIA_FORWARD						GTK_STOCK_MEDIA_FORWARD
#define aICON_MEDIA_NEXT							GTK_STOCK_MEDIA_NEXT
#define aICON_MEDIA_PAUSE							GTK_STOCK_MEDIA_PAUSE
#define aICON_MEDIA_PLAY							GTK_STOCK_MEDIA_PLAY
#define aICON_MEDIA_PREVIOUS						GTK_STOCK_MEDIA_PREVIOUS
#define aICON_MEDIA_RECORD							GTK_STOCK_MEDIA_RECORD
#define aICON_MEDIA_REWIND							GTK_STOCK_MEDIA_REWIND
#define aICON_MEDIA_STOP							GTK_STOCK_MEDIA_STOP
#define aICON_MISSING_IMAGE						GTK_STOCK_MISSING_IMAGE
#define aICON_NETWORK								GTK_STOCK_NETWORK
#define aICON_NEW										GTK_STOCK_NEW
#define aICON_NO										GTK_STOCK_NO
#define aICON_OK										GTK_STOCK_OK
#define aICON_OPEN									GTK_STOCK_OPEN
#define aICON_ORIENTATION_LANDSCAPE				GTK_STOCK_ORIENTATION_LANDSCAPE
#define aICON_ORIENTATION_PORTRAIT				GTK_STOCK_ORIENTATION_PORTRAIT
#define aICON_ORIENTATION_REVERSE_LANDSCAPE	GTK_STOCK_ORIENTATION_REVERSE_LANDSCAPE
#define aICON_ORIENTATION_REVERSE_PORTRAIT	GTK_STOCK_ORIENTATION_REVERSE_PORTRAIT
#define aICON_PAGE_SETUP							GTK_STOCK_PAGE_SETUP
#define aICON_PASTE									GTK_STOCK_PASTE
#define aICON_PREFERENCES							GTK_STOCK_PREFERENCES
#define aICON_PRINT									GTK_STOCK_PRINT
#define aICON_PRINT_ERROR							GTK_STOCK_PRINT_ERROR
#define aICON_PRINT_PAUSED							GTK_STOCK_PRINT_PAUSED
#define aICON_PRINT_PREVIEW						GTK_STOCK_PRINT_PREVIEW
#define aICON_PRINT_REPORT							GTK_STOCK_PRINT_REPORT
#define aICON_PRINT_WARNING						GTK_STOCK_PRINT_WARNING
#define aICON_PROPERTIES							GTK_STOCK_PROPERTIES
#define aICON_QUIT									GTK_STOCK_QUIT
#define aICON_REDO									GTK_STOCK_REDO
#define aICON_REFRESH								GTK_STOCK_REFRESH
#define aICON_REMOVE									GTK_STOCK_REMOVE
#define aICON_REVERT_TO_SAVED						GTK_STOCK_REVERT_TO_SAVED
#define aICON_SAVE									GTK_STOCK_SAVE
#define aICON_SAVE_AS								GTK_STOCK_SAVE_AS
#define aICON_SELECT_ALL							GTK_STOCK_SELECT_ALL
#define aICON_SELECT_COLOR							GTK_STOCK_SELECT_COLOR
#define aICON_SELECT_FONT							GTK_STOCK_SELECT_FONT
#define aICON_SORT_ASCENDING						GTK_STOCK_SORT_ASCENDING
#define aICON_SORT_DESCENDING						GTK_STOCK_SORT_DESCENDING
#define aICON_SPELL_CHECK							GTK_STOCK_SPELL_CHECK
#define aICON_STOP									GTK_STOCK_STOP
#define aICON_STRIKETHROUGH						GTK_STOCK_STRIKETHROUGH
#define aICON_UNDELETE								GTK_STOCK_UNDELETE
#define aICON_UNDERLINE								GTK_STOCK_UNDERLINE
#define aICON_UNDO									GTK_STOCK_UNDO
#define aICON_UNINDENT								GTK_STOCK_UNINDENT
#define aICON_YES										GTK_STOCK_YES
#define aICON_ZOOM_100								GTK_STOCK_ZOOM_100
#define aICON_ZOOM_FIT								GTK_STOCK_ZOOM_FIT
#define aICON_ZOOM_IN								GTK_STOCK_ZOOM_IN
#define aICON_ZOOM_OUT								GTK_STOCK_ZOOM_OUT
#endif
#ifdef USE_WIN32
#define aICON_ABOUT									0
#define aICON_ADD										0
#define aICON_APPLY									0
#define aICON_BOLD									0
#define aICON_CANCEL									0
#define aICON_CAPS_LOCK_WARNING					0
#define aICON_CDROM									0
#define aICON_CLEAR									0
#define aICON_CLOSE									0
#define aICON_COLOR_PICKER							0
#define aICON_CONVERT								0
#define aICON_CONNECT								0
#define aICON_COPY									0
#define aICON_CUT										0
#define aICON_DELETE									0
#define aICON_AUTHENTICATION						0
#define aICON_ERROR									0
#define aICON_DIALOG_INFO							0
#define aICON_DIALOG_QUESTION						0
#define aICON_DIALOG_WARNING						0
#define aICON_DIRECTORY								0
#define aICON_DISCARD								0
#define aICON_DISCONNECT							0
#define aICON_DND										0
#define aICON_DND_MULTIPLE							0
#define aICON_EDIT									0
#define aICON_EXECUTE								0
#define aICON_FILE									0
#define aICON_FIND									0
#define aICON_RIND_AND_REPLACE					0
#define aICON_FLOPPY									0
#define aICON_FULLSCREEN							0
#define aICON_GOTO_BOTTOM							0
#define aICON_GOTO_FIRST							0
#define aICON_GOTO_LAST								0
#define aICON_GOTO_TOP								0
#define aICON_GO_BACK								0
#define aICON_GO_DOWN								0
#define aICON_GO_FORWARD							0
#define aICON_GO_UP									0
#define aICON_HARDDISK								0
#define aICON_HELP									0
#define aICON_HOME									0
#define aICON_INDENT									0
#define aICON_INDEX									0
#define aICON_INFO									0
#define aICON_ITALIC									0
#define aICON_JUMP_TO								0
#define aICON_JUSTIFY_CENTER						0
#define aICON_JUSTIFY_FILL							0
#define aICON_JUSTIFY_LEFT							0
#define aICON_JUSTIFY_RIGHT						0
#define aICON_LEAVE_FULLSCREEN					0
#define aICON_MEDIA_FORWARD						0
#define aICON_MEDIA_NEXT							0
#define aICON_MEDIA_PAUSE							0
#define aICON_MEDIA_PLAY							0
#define aICON_MEDIA_PREVIOUS						0
#define aICON_MEDIA_RECORD							0
#define aICON_MEDIA_REWIND							0
#define aICON_MEDIA_STOP							0
#define aICON_MISSING_IMAGE						0
#define aICON_NETWORK								0
#define aICON_NEW										0
#define aICON_NO										0
#define aICON_OK										0
#define aICON_OPEN									0
#define aICON_ORIENTATION_LANDSCAPE				0
#define aICON_ORIENTATION_PORTRAIT				0
#define aICON_ORIENTATION_REVERSE_LANDSCAPE	0
#define aICON_ORIENTATION_REVERSE_PORTRAIT	0
#define aICON_PAGE_SETUP							0
#define aICON_PASTE									0
#define aICON_PREFERENCES							0
#define aICON_PRINT									0
#define aICON_PRINT_ERROR							0
#define aICON_PRINT_PAUSED							0
#define aICON_PRINT_PREVIEW						0
#define aICON_PRINT_REPORT							0
#define aICON_PRINT_WARNING						0
#define aICON_PROPERTIES							0
#define aICON_QUIT									0
#define aICON_REDO									0
#define aICON_REFRESH								0
#define aICON_REMOVE									0
#define aICON_REVERT_TO_SAVED						0
#define aICON_SAVE									0
#define aICON_SAVE_AS								0
#define aICON_SELECT_ALL							0
#define aICON_SELECT_COLOR							0
#define aICON_SELECT_FONT							0
#define aICON_SORT_ASCENDING						0
#define aICON_SORT_DESCENDING						0
#define aICON_SPELL_CHECK							0
#define aICON_STOP									0
#define aICON_STRIKETHROUGH						0
#define aICON_UNDELETE								0
#define aICON_UNDERLINE								0
#define aICON_UNDO									0
#define aICON_UNINDENT								0
#define aICON_YES										0
#define aICON_ZOOM_100								0
#define aICON_ZOOM_FIT								0
#define aICON_ZOOM_IN								0
#define aICON_ZOOM_OUT								0
#endif



#endif /* _AMANITA_GUI_WIDGET_H */

