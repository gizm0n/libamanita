#ifndef _AMANITA_TK_WIDGET_H
#define _AMANITA_TK_WIDGET_H

/**
 * @file amanita/tk/Widget.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-01-28
 */ 

#include <amanita/Config.h>
#ifdef USE_GTK
#ifdef USE_WIN32
#undef USE_WIN32
#endif
#endif

#include <stdint.h>
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
#endif /* USE_WIN32 */
#include <amanita/Hashtable.h>


/** Amanita Namespace */
namespace a {
/** Amanita Tool Kit Namespace */
namespace tk {


/** @cond */
#ifdef USE_WIN32
enum {
	BROWSER_CLASS_REGISTER		= 0x00000010,
	SCINTILLA_CLASS_REGISTER	= 0x00000020,
	TEXT_CLASS_REGISTER			= 0x00000040,
	WINDOW_CLASS_REGISTER		= 0x00000001,
};

#define BROWSER_CLASS _T("WebControl32")
#define WINDOW_CLASS _T("AmanitaMainWndClass")

extern uint32_t class_registers;

extern int dbu_x;
extern int dbu_y;
#endif /* USE_WIN32 */
/** @endcond */


#define WIDGET_MAKE_ID(type,index) (((type)<<9)|(index))
#define WIDGET_ID_TYPE(id) ((id)>>9)
#define WIDGET_ID_INDEX(id) ((id)&0x01ff)


enum {
	HORIZONTAL			= 0x00010000,	//!< Style used to order widgets horizontally.
	VERTICAL				= 0x00020000,	//!< Style used to order widgets vertically.
	TABLE					= 0x00040000,	//!< Style used to order widgets in a table.
	HOMOGENOUS			= 0x00080000,	//!< All widgets are generated with a homogenous size, width or height or both depending on style.

	LEFT					= 0x00100000,	//!< Align widgets to left.
	RIGHT					= 0x00200000,	//!< Align widgets to right.
	CENTER				= 0x00300000,	//!< Align widgets to center.
	TOP					= 0x00400000,	//!< Align widgets to top.
	BOTTOM				= 0x00800000,	//!< Align widgets to bottom.
	MIDDLE				= 0x00c00000,	//!< Align widgets to middle.
	ALIGNED				= 0x00f00000,	//!< Mask for widgets that have an alignment set.

	FIXED					= 0x01000000,	//!< Fixed size of widget, in GTK+ this means minimum size, in win32 exact size in pixels.
	EXPAND				= 0x02000000,	//!< Expand space for this widget in relations to siblings, other siblings without this flag will shrink to minimum size.
	FILL					= 0x04000000,	//!< Fill the expanded space with the widget, if not set widget will have its minimum size.
	HIDE					= 0x80000000,	//!< Hide component.
};

enum {
	FONT_BOLD			= 0x00000001,	//!< Flag used by <tt>setFont()</tt> to set font to bold.
	FONT_ITALIC			= 0x00000002,	//!< Flag used by <tt>setFont()</tt> to set font to italic.
	FONT_UNDERLINE		= 0x00000004,	//!< Flag used by <tt>setFont()</tt> to set font to underline.
	FONT_STRIKE			= 0x00000008,	//!< Flag used by <tt>setFont()</tt> to set font to strike through.
};


/** Component. */
#ifdef USE_GTK
typedef GtkWidget *Component;
#endif
#ifdef USE_WIN32
typedef HANDLE Component;
#endif

/** @cond */
class Widget;
class Window;
/** @endcond */


/** Widget event handler. */
typedef uint32_t (*widget_event_handler)(Widget *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);


#ifdef USE_WIN32
/** Data used when capturing event. */
struct widget_event_data {
	WNDPROC proc;
	Widget *widget;
	Window *window;
	bool (*event_handler)(Window *,UINT,WPARAM,LPARAM);
};
#endif


enum widget_type {
	WIDGET_VOID,						//!< An empty component, the widget using it is purely virtual, e.g. used for layout or similar.
	WIDGET_BROWSER,					//!< The browser component, on GTK+ use the WebKitGtk, on win32 use the Internet Explorer WebControl.
	WIDGET_BUTTON,						//!< An ordinary Button.
	WIDGET_CAIRO,						//!< A Cairo canvas, for drawing with Cairo.
	WIDGET_CANVAS,						//!< A Canvas for drawing using GDK with GTK+, and GDI with win32.
	WIDGET_CHECKBOX,					//!< An ordinary Checkbox.
	WIDGET_COMBOBOX,					//!< An ordinary Combobox.
	WIDGET_COMBOBOX_ENTRY,			//!< An ordinary Combobox with an Entry.
	WIDGET_CONTAINER,					//!< A container class, on GTK+ use the VBox, HBox and Table classes, on win32 a Static.
	WIDGET_ENTRY,						//!< An ordinary Text Entry box.
	WIDGET_FRAME,						//!< A Frame with a label.
	WIDGET_IMAGE,						//!< 
	WIDGET_LABEL,						//!< An ordinary Label.
	WIDGET_LISTBOX,					//!< An ordinary Listbox, not used in GTK+.
	WIDGET_LISTVIEW,					//!< An ordinary Listview.
	WIDGET_MENU,						//!< An ordinary Menu.
	WIDGET_NOTEBOOK,					//!< A Notebook, this is a set of tabs to switch between pages.
	WIDGET_PANEL,						//!< 
	WIDGET_PROGRESS,					//!< 
	WIDGET_RADIOBUTTON,				//!< An ordinary Radiobutton
	WIDGET_SCINTILLA,					//!< Using the Scintilla component, need to be built separately.
	WIDGET_SEPARATOR,					//!< 
	WIDGET_STATUSBAR,					//!< An ordinary Statusbar.
	WIDGET_TABLE,						//!< 
	WIDGET_TEXT,						//!< A Text component, on GTK+ it's a GtkTextView, or GtkSourceView depending on settings, on win32 it's a RichEdit control.
	WIDGET_TREE,						//!< 
	WIDGET_WINDOW,						//!< A Window that can be either a Dialog window or an ordinary window.
};


/** The Amanita Library GUI-interface base class from which all other widgets inherit.
 * 
 * Inherits the Object class, and so is part of the Object-rtti-interface, and all
 * classes that inherit Widget is too.
 * 
 * Look at examples/gui.cpp to see how to use the GUI-classes.
 * 
 * @ingroup tk */
class Widget : public Object {
friend class Container;
friend class Window;
friend class Notebook;
/** @cond */
Object_Instance(Widget)
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
	Window *window;								//!< Window this widget is created by.
	Widget *parent;								//!< Parent of this widget.
	Widget *next;									//!< Next component in a linked chain of sibling components that share the same parent.
	Component component;						//!< Handle to native widget, on GTK+ a GtkWidget, on win32 a HANDLE, usually a HWND.
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
	void addComponent(uint16_t id,Component c);

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
	 * @param h Height of widget. */
	virtual void makeLayout(int x,int y,int w,int h);

	/** Get minimum width of widget. Only used in win32.
	 * @return The minimum width. */
	virtual int getMinimumWidth();

	/** Get minimum height of widget. Only used in win32.
	 * @return The minimum height. */
	virtual int getMinimumHeight();

	/** Move widget to the dimensions set by makeLayout. Only used in win32.
	 * @see makeLayout()
	 * @return The minimum width. */
	virtual void move();
#endif

	/** Protected constructor, this class can not be instantiated directly, but must be inherited.
	 * @param weh Event handler callback function.
	 * @param t Type of widget. */
	Widget(widget_event_handler weh,widget_type t);

public:
	/** Destructor for class Widget. */
	virtual ~Widget();

	/** Creates the native component from values set in Widget.
	 * @param wnd Window widget is created by.
	 * @param st Style to define the creation of widget. */
	virtual void create(Window *wnd,uint32_t st);

	/** Create all children and sibling widgets.
	 * @param p Parent component.
	 * @param n Boolean value, if set to true will create siblings, otherwise only children. */
	virtual void createAll(Component p,bool n);

	/** Get the widget associated to an ID.
	 * @param id ID of component to look up.
	 * @return The widget associatd with the ID. */
	Widget *getWidget(uint16_t id);

	/** Get the widget associated to a component.
	 * @param c Component to look up.
	 * @return The widget associatd with the component. */
	Widget *getWidget(Component c);

	/** Get the window of this widget.
	 * @return A Window object. */
	Window *getWindow() { return window; }

	/** Get the parent widget of this widget.
	 * @return A Widget. */
	Widget *getParent() { return parent; }

	/** Get component the widget class contains. On GTK+ this is a GtkWidget, on win32 it's a HANDLE, usually a HWND.
	 * @return The component associated with this widget. */
	Component getComponent() { return component; }

	/** Get event handle callback function used by this widget to send events to client application.
	 * @return Event handle callback function. */
	widget_event_handler getEventHandler() { return event_handler; }

	/** Get the unique ID for the widget.
	 * @return This widgets ID. */
	uint16_t getID() { return id; }

	/** Store any kind of data.
	 * @param d A void * value containing a pointer to some user defined data. */
	void setData(void *d) { data = d; }

	/** Get data associated with this widget by user.
	 * @see setData()
	 * @return A void *. */
	void *getData() { return data; }

	/** Set text shown on or in the widget. 
	 * @param str Text for widget. */
	virtual void setText(const char *str);

	/** Get the text for the widget.
	 * @return A string. */
	const char *getText();

	/** Set the style for the widget, used when creating widget, and for layout management.
	 * @param st Style flags, see each widget for which flags can be set.
	 * @param minw Minimum width.
	 * @param minh Minimum height.
	 * @param b Border, free space in pixels around widget.
	 * @param sp Spacing, space in pixels between sibling widgets. */
	void setStyle(uint32_t st,uint16_t minw=0,uint16_t minh=0,uint8_t b=0,uint8_t sp=0);

	/** Get the widget's style.
	 * @see setStyle()
	 * @return An int value of the style. */
	uint32_t getStyle() { return style; }

	/** Get the width of the widget.
	 * @return Width of widget. */
	int getWidth() { return width; }

	/** Get the height of the widget.
	 * @return Height of widget. */
	int getHeight() { return height; }

	/** Show widget. */
	void show();

	/** Hide widget. */
	void hide();

	/** Set the font of this widget. This method must be called after component has been created.
	 * @param font String with the name of font face, e.g. "Sans Serif".
	 * @param sz Size in points of font.
	 * @param st Style of font, can be any of FONT_*. */
	virtual void setFont(const char *font,int sz,int st=0);
};


enum {
#ifdef USE_GTK
	KEY_CONTROL						= GDK_CONTROL_MASK,
	KEY_ALT							= GDK_MOD1_MASK,
	KEY_SHIFT						= GDK_SHIFT_MASK,
	KEY_ENTER						= GDK_Return,
	KEY_SPACE						= GDK_space,
	KEY_a								= GDK_a,
	KEY_b								= GDK_b,
	KEY_c								= GDK_c,
	KEY_d								= GDK_d,
	KEY_e								= GDK_e,
	KEY_f								= GDK_f,
	KEY_g								= GDK_g,
	KEY_h								= GDK_h,
	KEY_i								= GDK_i,
	KEY_j								= GDK_j,
	KEY_k								= GDK_k,
	KEY_l								= GDK_l,
	KEY_m								= GDK_m,
	KEY_n								= GDK_n,
	KEY_o								= GDK_o,
	KEY_p								= GDK_p,
	KEY_q								= GDK_q,
	KEY_r								= GDK_r,
	KEY_s								= GDK_s,
	KEY_t								= GDK_t,
	KEY_u								= GDK_u,
	KEY_v								= GDK_v,
	KEY_w								= GDK_w,
	KEY_x								= GDK_x,
	KEY_y								= GDK_y,
	KEY_z								= GDK_z,
	KEY_F1							= GDK_F1,
	KEY_F2							= GDK_F2,
	KEY_F3							= GDK_F3,
	KEY_F4							= GDK_F4,
	KEY_F5							= GDK_F5,
	KEY_F6							= GDK_F6,
	KEY_F7							= GDK_F7,
	KEY_F8							= GDK_F8,
	KEY_F9							= GDK_F9,
	KEY_F10							= GDK_F10,
	KEY_F11							= GDK_F11,
	KEY_F12							= GDK_F12,
#endif

#ifdef USE_WIN32
	KEY_CONTROL						= 0x0001,
	KEY_ALT							= 0x0002,
	KEY_SHIFT						= 0x0004,
	KEY_ENTER						= VK_RETURN,
	KEY_SPACE						= VK_SPACE,
	KEY_a								= 'A',
	KEY_b								= 'B',
	KEY_c								= 'C',
	KEY_d								= 'D',
	KEY_e								= 'E',
	KEY_f								= 'F',
	KEY_g								= 'G',
	KEY_h								= 'H',
	KEY_i								= 'I',
	KEY_j								= 'J',
	KEY_k								= 'K',
	KEY_l								= 'L',
	KEY_m								= 'M',
	KEY_n								= 'N',
	KEY_o								= 'O',
	KEY_p								= 'P',
	KEY_q								= 'Q',
	KEY_r								= 'R',
	KEY_s								= 'S',
	KEY_t								= 'T',
	KEY_u								= 'U',
	KEY_v								= 'V',
	KEY_w								= 'W',
	KEY_x								= 'X',
	KEY_y								= 'Y',
	KEY_z								= 'Z',
	KEY_F1							= VK_F1,
	KEY_F2							= VK_F2,
	KEY_F3							= VK_F3,
	KEY_F4							= VK_F4,
	KEY_F5							= VK_F5,
	KEY_F6							= VK_F6,
	KEY_F7							= VK_F7,
	KEY_F8							= VK_F8,
	KEY_F9							= VK_F9,
	KEY_F10							= VK_F10,
	KEY_F11							= VK_F11,
	KEY_F12							= VK_F12,
#endif
};


#ifdef USE_GTK
#define ICON_ABOUT									GTK_STOCK_ABOUT
#define ICON_ADD										GTK_STOCK_ADD
#define ICON_APPLY									GTK_STOCK_APPLY
#define ICON_BOLD										GTK_STOCK_BOLD
#define ICON_CANCEL									GTK_STOCK_CANCEL
#define ICON_CAPS_LOCK_WARNING					GTK_STOCK_CAPS_LOCK_WARNING
#define ICON_CDROM									GTK_STOCK_CDROM
#define ICON_CLEAR									GTK_STOCK_CLEAR
#define ICON_CLOSE									GTK_STOCK_CLOSE
#define ICON_COLOR_PICKER							GTK_STOCK_COLOR_PICKER
#define ICON_CONVERT									GTK_STOCK_CONVERT
#define ICON_CONNECT									GTK_STOCK_CONNECT
#define ICON_COPY										GTK_STOCK_COPY
#define ICON_CUT										GTK_STOCK_CUT
#define ICON_DELETE									GTK_STOCK_DELETE
#define ICON_AUTHENTICATION						GTK_STOCK_DIALOG_AUTHENTICATION
#define ICON_ERROR									GTK_STOCK_DIALOG_ERROR
#define ICON_DIALOG_INFO							GTK_STOCK_DIALOG_INFO
#define ICON_DIALOG_QUESTION						GTK_STOCK_DIALOG_QUESTION
#define ICON_DIALOG_WARNING						GTK_STOCK_DIALOG_WARNING
#define ICON_DIRECTORY								GTK_STOCK_DIRECTORY
#define ICON_DISCARD									GTK_STOCK_DISCARD
#define ICON_DISCONNECT								GTK_STOCK_DISCONNECT
#define ICON_DND										GTK_STOCK_DND
#define ICON_DND_MULTIPLE							GTK_STOCK_DND_MULTIPLE
#define ICON_EDIT										GTK_STOCK_EDIT
#define ICON_EXECUTE									GTK_STOCK_EXECUTE
#define ICON_FILE										GTK_STOCK_FILE
#define ICON_FIND										GTK_STOCK_FIND
#define ICON_RIND_AND_REPLACE						GTK_STOCK_FIND_AND_REPLACE
#define ICON_FLOPPY									GTK_STOCK_FLOPPY
#define ICON_FULLSCREEN								GTK_STOCK_FULLSCREEN
#define ICON_GOTO_BOTTOM							GTK_STOCK_GOTO_BOTTOM
#define ICON_GOTO_FIRST								GTK_STOCK_GOTO_FIRST
#define ICON_GOTO_LAST								GTK_STOCK_GOTO_LAST
#define ICON_GOTO_TOP								GTK_STOCK_GOTO_TOP
#define ICON_GO_BACK									GTK_STOCK_GO_BACK
#define ICON_GO_DOWN									GTK_STOCK_GO_DOWN
#define ICON_GO_FORWARD								GTK_STOCK_GO_FORWARD
#define ICON_GO_UP									GTK_STOCK_GO_UP
#define ICON_HARDDISK								GTK_STOCK_HARDDISK
#define ICON_HELP										GTK_STOCK_HELP
#define ICON_HOME										GTK_STOCK_HOME
#define ICON_INDENT									GTK_STOCK_INDENT
#define ICON_INDEX									GTK_STOCK_INDEX
#define ICON_INFO										GTK_STOCK_INFO
#define ICON_ITALIC									GTK_STOCK_ITALIC
#define ICON_JUMP_TO									GTK_STOCK_JUMP_TO
#define ICON_JUSTIFY_CENTER						GTK_STOCK_JUSTIFY_CENTER
#define ICON_JUSTIFY_FILL							GTK_STOCK_JUSTIFY_FILL
#define ICON_JUSTIFY_LEFT							GTK_STOCK_JUSTIFY_LEFT
#define ICON_JUSTIFY_RIGHT							GTK_STOCK_JUSTIFY_RIGHT
#define ICON_LEAVE_FULLSCREEN						GTK_STOCK_LEAVE_FULLSCREEN
#define ICON_MEDIA_FORWARD							GTK_STOCK_MEDIA_FORWARD
#define ICON_MEDIA_NEXT								GTK_STOCK_MEDIA_NEXT
#define ICON_MEDIA_PAUSE							GTK_STOCK_MEDIA_PAUSE
#define ICON_MEDIA_PLAY								GTK_STOCK_MEDIA_PLAY
#define ICON_MEDIA_PREVIOUS						GTK_STOCK_MEDIA_PREVIOUS
#define ICON_MEDIA_RECORD							GTK_STOCK_MEDIA_RECORD
#define ICON_MEDIA_REWIND							GTK_STOCK_MEDIA_REWIND
#define ICON_MEDIA_STOP								GTK_STOCK_MEDIA_STOP
#define ICON_MISSING_IMAGE							GTK_STOCK_MISSING_IMAGE
#define ICON_NETWORK									GTK_STOCK_NETWORK
#define ICON_NEW										GTK_STOCK_NEW
#define ICON_NO										GTK_STOCK_NO
#define ICON_OK										GTK_STOCK_OK
#define ICON_OPEN										GTK_STOCK_OPEN
#define ICON_ORIENTATION_LANDSCAPE				GTK_STOCK_ORIENTATION_LANDSCAPE
#define ICON_ORIENTATION_PORTRAIT				GTK_STOCK_ORIENTATION_PORTRAIT
#define ICON_ORIENTATION_REVERSE_LANDSCAPE	GTK_STOCK_ORIENTATION_REVERSE_LANDSCAPE
#define ICON_ORIENTATION_REVERSE_PORTRAIT		GTK_STOCK_ORIENTATION_REVERSE_PORTRAIT
#define ICON_PAGE_SETUP								GTK_STOCK_PAGE_SETUP
#define ICON_PASTE									GTK_STOCK_PASTE
#define ICON_PREFERENCES							GTK_STOCK_PREFERENCES
#define ICON_PRINT									GTK_STOCK_PRINT
#define ICON_PRINT_ERROR							GTK_STOCK_PRINT_ERROR
#define ICON_PRINT_PAUSED							GTK_STOCK_PRINT_PAUSED
#define ICON_PRINT_PREVIEW							GTK_STOCK_PRINT_PREVIEW
#define ICON_PRINT_REPORT							GTK_STOCK_PRINT_REPORT
#define ICON_PRINT_WARNING							GTK_STOCK_PRINT_WARNING
#define ICON_PROPERTIES								GTK_STOCK_PROPERTIES
#define ICON_QUIT										GTK_STOCK_QUIT
#define ICON_REDO										GTK_STOCK_REDO
#define ICON_REFRESH									GTK_STOCK_REFRESH
#define ICON_REMOVE									GTK_STOCK_REMOVE
#define ICON_REVERT_TO_SAVED						GTK_STOCK_REVERT_TO_SAVED
#define ICON_SAVE										GTK_STOCK_SAVE
#define ICON_SAVE_AS									GTK_STOCK_SAVE_AS
#define ICON_SELECT_ALL								GTK_STOCK_SELECT_ALL
#define ICON_SELECT_COLOR							GTK_STOCK_SELECT_COLOR
#define ICON_SELECT_FONT							GTK_STOCK_SELECT_FONT
#define ICON_SORT_ASCENDING						GTK_STOCK_SORT_ASCENDING
#define ICON_SORT_DESCENDING						GTK_STOCK_SORT_DESCENDING
#define ICON_SPELL_CHECK							GTK_STOCK_SPELL_CHECK
#define ICON_STOP										GTK_STOCK_STOP
#define ICON_STRIKETHROUGH							GTK_STOCK_STRIKETHROUGH
#define ICON_UNDELETE								GTK_STOCK_UNDELETE
#define ICON_UNDERLINE								GTK_STOCK_UNDERLINE
#define ICON_UNDO										GTK_STOCK_UNDO
#define ICON_UNINDENT								GTK_STOCK_UNINDENT
#define ICON_YES										GTK_STOCK_YES
#define ICON_ZOOM_100								GTK_STOCK_ZOOM_100
#define ICON_ZOOM_FIT								GTK_STOCK_ZOOM_FIT
#define ICON_ZOOM_IN									GTK_STOCK_ZOOM_IN
#define ICON_ZOOM_OUT								GTK_STOCK_ZOOM_OUT
#endif
#ifdef USE_WIN32
#define ICON_ABOUT									0
#define ICON_ADD										0
#define ICON_APPLY									0
#define ICON_BOLD										0
#define ICON_CANCEL									0
#define ICON_CAPS_LOCK_WARNING					0
#define ICON_CDROM									0
#define ICON_CLEAR									0
#define ICON_CLOSE									0
#define ICON_COLOR_PICKER							0
#define ICON_CONVERT									0
#define ICON_CONNECT									0
#define ICON_COPY										0
#define ICON_CUT										0
#define ICON_DELETE									0
#define ICON_AUTHENTICATION						0
#define ICON_ERROR									0
#define ICON_DIALOG_INFO							0
#define ICON_DIALOG_QUESTION						0
#define ICON_DIALOG_WARNING						0
#define ICON_DIRECTORY								0
#define ICON_DISCARD									0
#define ICON_DISCONNECT								0
#define ICON_DND										0
#define ICON_DND_MULTIPLE							0
#define ICON_EDIT										0
#define ICON_EXECUTE									0
#define ICON_FILE										0
#define ICON_FIND										0
#define ICON_RIND_AND_REPLACE						0
#define ICON_FLOPPY									0
#define ICON_FULLSCREEN								0
#define ICON_GOTO_BOTTOM							0
#define ICON_GOTO_FIRST								0
#define ICON_GOTO_LAST								0
#define ICON_GOTO_TOP								0
#define ICON_GO_BACK									0
#define ICON_GO_DOWN									0
#define ICON_GO_FORWARD								0
#define ICON_GO_UP									0
#define ICON_HARDDISK								0
#define ICON_HELP										0
#define ICON_HOME										0
#define ICON_INDENT									0
#define ICON_INDEX									0
#define ICON_INFO										0
#define ICON_ITALIC									0
#define ICON_JUMP_TO									0
#define ICON_JUSTIFY_CENTER						0
#define ICON_JUSTIFY_FILL							0
#define ICON_JUSTIFY_LEFT							0
#define ICON_JUSTIFY_RIGHT							0
#define ICON_LEAVE_FULLSCREEN						0
#define ICON_MEDIA_FORWARD							0
#define ICON_MEDIA_NEXT								0
#define ICON_MEDIA_PAUSE							0
#define ICON_MEDIA_PLAY								0
#define ICON_MEDIA_PREVIOUS						0
#define ICON_MEDIA_RECORD							0
#define ICON_MEDIA_REWIND							0
#define ICON_MEDIA_STOP								0
#define ICON_MISSING_IMAGE							0
#define ICON_NETWORK									0
#define ICON_NEW										0
#define ICON_NO										0
#define ICON_OK										0
#define ICON_OPEN										0
#define ICON_ORIENTATION_LANDSCAPE				0
#define ICON_ORIENTATION_PORTRAIT				0
#define ICON_ORIENTATION_REVERSE_LANDSCAPE	0
#define ICON_ORIENTATION_REVERSE_PORTRAIT		0
#define ICON_PAGE_SETUP								0
#define ICON_PASTE									0
#define ICON_PREFERENCES							0
#define ICON_PRINT									0
#define ICON_PRINT_ERROR							0
#define ICON_PRINT_PAUSED							0
#define ICON_PRINT_PREVIEW							0
#define ICON_PRINT_REPORT							0
#define ICON_PRINT_WARNING							0
#define ICON_PROPERTIES								0
#define ICON_QUIT										0
#define ICON_REDO										0
#define ICON_REFRESH									0
#define ICON_REMOVE									0
#define ICON_REVERT_TO_SAVED						0
#define ICON_SAVE										0
#define ICON_SAVE_AS									0
#define ICON_SELECT_ALL								0
#define ICON_SELECT_COLOR							0
#define ICON_SELECT_FONT							0
#define ICON_SORT_ASCENDING						0
#define ICON_SORT_DESCENDING						0
#define ICON_SPELL_CHECK							0
#define ICON_STOP										0
#define ICON_STRIKETHROUGH							0
#define ICON_UNDELETE								0
#define ICON_UNDERLINE								0
#define ICON_UNDO										0
#define ICON_UNINDENT								0
#define ICON_YES										0
#define ICON_ZOOM_100								0
#define ICON_ZOOM_FIT								0
#define ICON_ZOOM_IN									0
#define ICON_ZOOM_OUT								0
#endif


}; /* namespace tk */
}; /* namespace a */


#endif /* _AMANITA_TK_WIDGET_H */

