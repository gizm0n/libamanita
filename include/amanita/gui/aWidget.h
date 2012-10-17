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


#define aWIDGET_MAKE_ID(type,index) (((type)<<9)|(index))
#define aWIDGET_ID_TYPE(id) ((id)>>9)
#define aWIDGET_ID_INDEX(id) ((id)&0x01ff)


#ifdef USE_GTK
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#endif

#ifdef USE_WIN32
#ifndef WINVER
#define WINVER 0x0501
#endif
#ifdef _WIN32_IE
#undef _WIN32_IE
#endif
#define _WIN32_IE	0x0501
#include <windows.h>

enum {
	aBROWSER_CLASS_REGISTER		= 0x00000010,
	aSCINTILLA_CLASS_REGISTER	= 0x00000020,
	aTEXT_CLASS_REGISTER			= 0x00000040,
	aWINDOW_CLASS_REGISTER		= 0x00000001,
};

#define aBROWSER_CLASS _T("WebControl32")
#define aWINDOW_CLASS _T("AmanitaMainWndClass")

extern uint32_t class_registers;

extern int dbu_x;
extern int dbu_y;
#endif /* USE_WIN32 */



enum {
	aHORIZONTAL			= 0x00010000,	//!< Style used to order widgets horizontally.
	aVERTICAL			= 0x00020000,	//!< Style used to order widgets vertically.
	aTABLE				= 0x00040000,	//!< Style used to order widgets in a table.
	aHOMOGENOUS			= 0x00080000,	//!< All widgets are generated with a homogenous size, width or height or both depending on style.

	aLEFT					= 0x00100000,	//!< Align widgets to left.
	aRIGHT				= 0x00200000,	//!< Align widgets to right.
	aCENTER				= 0x00300000,	//!< Align widgets to center.
	aTOP					= 0x00400000,	//!< Align widgets to top.
	aBOTTOM				= 0x00800000,	//!< Align widgets to bottom.
	aMIDDLE				= 0x00c00000,	//!< Align widgets to middle.
	aALIGNED				= 0x00f00000,	//!< Mask for widgets that have and alignment set.

	aFIXED				= 0x01000000,	//!< Fixed size of widget, in GTK+ this means minimum size, in win32 exact size in pixels.
	aEXPAND				= 0x02000000,	//!< Expand space for this widget in relations to siblings, other siblings without this flag will shrink to minimum size.
	aFILL					= 0x04000000,	//!< Fill the expanded space with the widget, if not set widget will have its minimum size.
	aHIDE					= 0x80000000,	//!< Hide component.
};

enum {
	aFONT_BOLD			= 0x00000001,	//!< Flag used by <tt>setFont()</tt> to set font to bold.
	aFONT_ITALIC		= 0x00000002,	//!< Flag used by <tt>setFont()</tt> to set font to italic.
	aFONT_UNDERLINE	= 0x00000004,	//!< Flag used by <tt>setFont()</tt> to set font to underline.
	aFONT_STRIKE		= 0x00000008,	//!< Flag used by <tt>setFont()</tt> to set font to strike through.
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
/** Data used when capturing event. */
struct widget_event_data {
	WNDPROC proc;
	aWidget *widget;
	aWindow *window;
	bool (*event_handler)(aWindow *,UINT,WPARAM,LPARAM);
};
#endif

enum widget_type {
	aWIDGET_VOID,							//!< An empty component, the widget using it is purely virtual, e.g. used for layout or similar.
	aWIDGET_BROWSER,						//!< The browser component, on GTK+ use the WebKitGtk, on win32 use the Internet Explorer WebControl.
	aWIDGET_BUTTON,						//!< An ordinary Button.
	aWIDGET_CAIRO,							//!< A Cairo canvas, for drawing with Cairo.
	aWIDGET_CANVAS,						//!< A Canvas for drawing using GDK with GTK+, and GDI with win32.
	aWIDGET_CHECKBOX,						//!< An ordinary Checkbox.
	aWIDGET_COMBOBOX,						//!< An ordinary Combobox.
	aWIDGET_COMBOBOX_ENTRY,				//!< An ordinary Combobox with an Entry.
	aWIDGET_CONTAINER,					//!< A container class, on GTK+ use the VBox, HBox and Table classes, on win32 a Static.
	aWIDGET_ENTRY,							//!< An ordinary Text Entry box.
	aWIDGET_FRAME,							//!< A Frame with a label.
	aWIDGET_IMAGE,							//!< 
	aWIDGET_LABEL,							//!< An ordinary Label.
	aWIDGET_LISTBOX,						//!< An ordinary Listbox, not used in GTK+.
	aWIDGET_LISTVIEW,						//!< An ordinary Listview.
	aWIDGET_MENU,							//!< An ordinary Menu.
	aWIDGET_NOTEBOOK,						//!< A Notebook, this is a set of tabs to switch between pages.
	aWIDGET_PANEL,							//!< 
	aWIDGET_PROGRESS,						//!< 
	aWIDGET_RADIOBUTTON,					//!< An ordinary Radiobutton
	aWIDGET_SCINTILLA,					//!< Using the Scintilla component, need to be built separately.
	aWIDGET_SEPARATOR,					//!< 
	aWIDGET_STATUSBAR,					//!< An ordinary Statusbar.
	aWIDGET_TABLE,							//!< 
	aWIDGET_TEXT,							//!< A Text component, on GTK+ it's a GtkTextView, or GtkSourceView depending on settings, on win32 it's a RichEdit control.
	aWIDGET_TREE,							//!< 
	aWIDGET_WINDOW,						//!< A Window that can be either a Dialog window or an ordinary window.
};


/** The Amanita Library GUI-interface base class from which all other widgets inherit.
 * 
 * Inherits the aObject class, and so is part of the aObject-rtti-interface, and all
 * classes that inherit aWidget is too.
 * 
 * Look at examples/gui.cpp to see how to use the GUI-classes.
 * @ingroup gui
 */
class aWidget : public aObject {
friend class aContainer;
friend class aWindow;
friend class aNotebook;

/** @cond */
aObject_Instance(aWidget)
/** @endcond */

private:
#ifdef USE_WIN32
	/** A virtual method used by widgets that are user-drawn. Only used in win32.
	 * @param dis A LPDRAWITEMSTRUCT used for drawing.
	 * @return This method should return true if it has been used to draw, otherwise false. */
	virtual bool drawItem(LPDRAWITEMSTRUCT dis) { return true; }
#endif

protected:
	widget_event_handler event_handler;		//!< Event handler callback, used to send the client application event messages.
#ifdef USE_WIN32
	widget_event_data *event_data;			//!< Win32 specific, stores data used when capturing events.
#endif
	uint16_t id;									//!< Unique ID number for widget.
	widget_type type;								//!< Type of widget.
	aWindow *window;								//!< Window this widget is created by.
	aWidget *parent;								//!< Parent of this widget.
	aWidget *next;									//!< Next component in a linked chain of sibling components that share the same parent.
	aComponent component;						//!< Handle to native widget, on GTK+ a GtkWidget, on win32 a HANDLE, usually a HWND.
	void *data;										//!< Any user data associated with the widget.
	char *text;										//!< Text used when creating widget, deleted once component is created.
	uint32_t style;								//!< Style value, see each widget which styles can be set.
	int16_t x;										//!< X-position.
	int16_t y;										//!< Y-position.
	uint16_t width;								//!< Width.
	uint16_t height;								//!< Height.
	uint16_t min_width;							//!< Minimum width, the smallest width widget will accept, used for layout.
	uint16_t min_height;							//!< Minimum height, the smallest height widget will accept, used for layout.
	uint8_t border;								//!< Border, free space in pixels around widget, used for layout.
	uint8_t spacing;								//!< Spacing, space in pixels between sibling widgets, used for layout.

	/** Create a unique ID.
	 * @return A unique ID. */
	uint16_t makeID();

	/** Add a component to global storage. */
	void addComponent(uint16_t id,aComponent c);

	/** Add a widget to global storage. */
	void addWidget();

	/** Remove a widget from global storage. */
	void removeWidget();

#ifdef USE_WIN32
	/** Capture events for widget. Only used in win32. */
	void captureEvents();

	/** Make layout for widget. Only used in win32.
	 * @param x X-position of widget.
	 * @param y Y-position of widget.
	 * @param w Width of widget.
	 * @param h Height of widget.
	 */
	virtual void makeLayout(int x,int y,int w,int h);

	/** Get minimum width of widget. Only used in win32.
	 * @return The minimum width.
	 */
	virtual int getMinimumWidth();

	/** Get minimum height of widget. Only used in win32.
	 * @return The minimum height.
	 */
	virtual int getMinimumHeight();

	/** Move widget to the dimensions set by makeLayout. Only used in win32.
	 * @see makeLayout()
	 * @return The minimum width.
	 */
	virtual void move();
#endif

	/** Protected constructor, this class can not be instantiated directly, but must be inherited.
	 * @param weh Event handler callback function.
	 * @param t Type of widget. */
	aWidget(widget_event_handler weh,widget_type t);

public:
	/** Destructor for class aWidget. */
	virtual ~aWidget();

	/** Creates the native component from values set in aWidget.
	 * @param wnd Window widget is created by.
	 * @param st Style to define the creation of widget. */
	virtual void create(aWindow *wnd,uint32_t st);

	/** Create all children and sibling widgets.
	 * @param p Parent component.
	 * @param n Boolean value, if set to true will create siblings, otherwise only children.
	 */
	virtual void createAll(aComponent p,bool n);

	/** Get the widget associated to an ID.
	 * @param id ID of component to look up.
	 * @return The widget associatd with the ID.
	 */
	aWidget *getWidget(uint16_t id);

	/** Get the widget associated to a component.
	 * @param c Component to look up.
	 * @return The widget associatd with the component.
	 */
	aWidget *getWidget(aComponent c);

	/** Get the window of this widget.
	 * @return A aWindow object.
	 */
	aWindow *getWindow() { return window; }

	/** Get the parent widget of this widget.
	 * @return A aWidget.
	 */
	aWidget *getParent() { return parent; }

	/** Get component the widget class contains. On GTK+ this is a GtkWidget, on win32 it's a HANDLE, usually a HWND.
	 * @return The component associated with this widget.
	 */
	aComponent getComponent() { return component; }

	/** Get event handle callback function used by this widget to send events to client application.
	 * @return Event handle callback function.
	 */
	widget_event_handler getEventHandler() { return event_handler; }

	/** Get the unique ID for the widget.
	 * @return This widgets ID. */
	uint16_t getID() { return id; }

	/** Store any kind of data.
	 * @param d A void * value containing a pointer to some user defined data. */
	void setData(void *d) { data = d; }

	/** Get data associated with this widget by user.
	 * @see setData()
	 * @return A void *.
	 */
	void *getData() { return data; }

	/** Set text shown on or in the widget. 
	 * @param str Text for widget.
	 */
	virtual void setText(const char *str);

	/** Get the text for the widget.
	 * @return A string. */
	const char *getText();

	/** Set the style for the widget, used when creating widget, and for layout management.
	 * @param st Style flags, see each widget for which flags can be set.
	 * @param minw Minimum width.
	 * @param minh Minimum height.
	 * @param b Border, free space in pixels around widget.
	 * @param sp Spacing, space in pixels between sibling widgets.
	 */
	void setStyle(uint32_t st,uint16_t minw=0,uint16_t minh=0,uint8_t b=0,uint8_t sp=0);

	/** Get the widget's style.
	 * @see setStyle()
	 * @return An int value of the style.
	 */
	uint32_t getStyle() { return style; }

	/** Get the width of the widget.
	 * @return Width of widget.
	 */
	int getWidth() { return width; }

	/** Get the height of the widget.
	 * @return Height of widget.
	 */
	int getHeight() { return height; }

	/** Show widget. */
	void show();

	/** Hide widget. */
	void hide();

	/** Set the font of this widget. This method must be called after component has been created.
	 * @param font String with the name of font face, e.g. "Sans Serif".
	 * @param sz Size in points of font.
	 * @param st Style of font, can be any of (aFONT_BOLD|aFONT_ITALIC|aFONT_UNDERLINE|aFONT_STRIKE).
	 */
	virtual void setFont(const char *font,int sz,int st=0);
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

