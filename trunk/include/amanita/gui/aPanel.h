#ifndef _AMANITA_GUI_PANEL_H
#define _AMANITA_GUI_PANEL_H

/**
 * @file amanita/gui/aPanel.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-13
 * @date Created: 2012-03-13
 */ 


#include <amanita/gui/aWidget.h>

enum {
	aPANEL_CLICKED				= 0x00000001,
};

enum {
	aPANEL_BUTTON = 1,
	aPANEL_CHECK,
	aPANEL_SEPARATOR,
};

enum {
	aPANEL_DISABLED			= 0x00000001,
	aPANEL_PRESSED				= 0x00000002,
	aPANEL_CHECKED				= 0x00000004,
	aPANEL_HIDDEN				= 0x00000008,
};

class aPanel;

struct aPanelButton {
	int style;
#ifdef USE_GTK
	const char *icon;
#endif
#ifdef USE_WIN32
	int icon;
#endif
	int action;
	int state;
	const char *label;
	const char *tooltip;
	void *data;
	aPanel *panel;
};


class aPanel : public aWidget {
friend class aContainer;
friend class aWindow;

/** @cond */
Object_Instance(aPanel)
/** @endcond */

private:
	aPanelButton *buttons;
	uint32_t image_id;

	void deleteButtons();

public:	
	aPanel(widget_event_handler weh,aPanelButton *b);
	virtual ~aPanel();

	void setButtons(aPanelButton *b);

	virtual void create(aWindow *wnd,uint32_t st);
#ifdef USE_WIN32
	virtual void move();
#endif
};


#ifdef USE_GTK
#define aPANEL_ABOUT					GTK_STOCK_ABOUT
#define aPANEL_ADD					GTK_STOCK_ADD
#define aPANEL_ALIGN_LEFT			GTK_STOCK_JUSTIFY_LEFT
#define aPANEL_ALIGN_RIGHT			GTK_STOCK_JUSTIFY_RIGHT
#define aPANEL_ALIGN_CENTER		GTK_STOCK_JUSTIFY_CENTER
#define aPANEL_ALIGN_JUSTIFY		GTK_STOCK_JUSTIFY_FILL
#define aPANEL_APPLY					GTK_STOCK_APPLY
#define aPANEL_CANCEL				GTK_STOCK_CANCEL
#define aPANEL_CDROM					GTK_STOCK_CDROM
#define aPANEL_CLEAR					GTK_STOCK_CLEAR
#define aPANEL_CLOSE					GTK_STOCK_CLOSE
#define aPANEL_COLOR_PICKER		GTK_STOCK_COLOR_PICKER
#define aPANEL_CONNECT				GTK_STOCK_CONNECT
#define aPANEL_CONVERT				GTK_STOCK_CONVERT
#define aPANEL_COPY					GTK_STOCK_COPY
#define aPANEL_CUT					GTK_STOCK_CUT
#define aPANEL_DELETE				GTK_STOCK_DELETE
#define aPANEL_DIRECTORY			GTK_STOCK_DIRECTORY
#define aPANEL_DISCONNECT			GTK_STOCK_DISCONNECT
#define aPANEL_EDIT					GTK_STOCK_EDIT
#define aPANEL_EXECUTE				GTK_STOCK_EXECUTE
#define aPANEL_FILE					GTK_STOCK_FILE
#define aPANEL_FIND					GTK_STOCK_FIND
#define aPANEL_FIND_AND_REPLACE	GTK_STOCK_FIND_AND_REPLACE
#define aPANEL_FULLSCREEN			GTK_STOCK_FULLSCREEN
#define aPANEL_LEAVE_FULLSCREEN	GTK_STOCK_LEAVE_FULLSCREEN
#define aPANEL_GOTO_FIRST			GTK_STOCK_GOTO_FIRST
#define aPANEL_GOTO_TOP				GTK_STOCK_GOTO_TOP
#define aPANEL_GOTO_LAST			GTK_STOCK_GOTO_LAST
#define aPANEL_GOTO_BOTTOM			GTK_STOCK_GOTO_BOTTOM
#define aPANEL_GO_BACK				GTK_STOCK_GO_BACK
#define aPANEL_GO_UP					GTK_STOCK_GO_UP
#define aPANEL_GO_FORWARD			GTK_STOCK_GO_FORWARD
#define aPANEL_GO_DOWN				GTK_STOCK_GO_DOWN
#define aPANEL_HARDDISK				GTK_STOCK_HARDDISK
#define aPANEL_HELP					GTK_STOCK_HELP
#define aPANEL_HOME					GTK_STOCK_HOME
#define aPANEL_INDEX					GTK_STOCK_INDEX
#define aPANEL_INFO					GTK_STOCK_INFO
#define aPANEL_JUMP_TO				GTK_STOCK_JUMP_TO
#define aPANEL_MEDIA_PREVIOUS		GTK_STOCK_MEDIA_PREVIOUS
#define aPANEL_MEDIA_REWIND		GTK_STOCK_MEDIA_REWIND
#define aPANEL_MEDIA_FORWARD		GTK_STOCK_MEDIA_FORWARD
#define aPANEL_MEDIA_NEXT			GTK_STOCK_MEDIA_NEXT
#define aPANEL_MEDIA_PLAY			GTK_STOCK_MEDIA_PLAY
#define aPANEL_MEDIA_PAUSE			GTK_STOCK_MEDIA_PAUSE
#define aPANEL_MEDIA_STOP			GTK_STOCK_MEDIA_STOP
#define aPANEL_MEDIA_RECORD		GTK_STOCK_MEDIA_RECORD
#define aPANEL_MISSING_IMAGE		GTK_STOCK_MISSING_IMAGE
#define aPANEL_NETWORK				GTK_STOCK_NETWORK
#define aPANEL_NEW					GTK_STOCK_NEW
#define aPANEL_NO						GTK_STOCK_NO
#define aPANEL_OK						GTK_STOCK_OK
#define aPANEL_OPEN					GTK_STOCK_OPEN
#define aPANEL_PAGE_SETUP			GTK_STOCK_PAGE_SETUP
#define aPANEL_PASTE					GTK_STOCK_PASTE
#define aPANEL_PREFERENCES			GTK_STOCK_PREFERENCES
#define aPANEL_PRINT					GTK_STOCK_PRINT
#define aPANEL_PRINT_ERROR			GTK_STOCK_PRINT_ERROR
#define aPANEL_PRINT_PAUSED		GTK_STOCK_PRINT_PAUSED
#define aPANEL_PRINT_PREVIEW		GTK_STOCK_PRINT_PREVIEW
#define aPANEL_PRINT_REPORT		GTK_STOCK_PRINT_REPORT
#define aPANEL_PRINT_WARNING		GTK_STOCK_PRINT_WARNING
#define aPANEL_PROPERTIES			GTK_STOCK_PROPERTIES
#define aPANEL_QUIT					GTK_STOCK_QUIT
#define aPANEL_REDO					GTK_STOCK_REDO
#define aPANEL_REFRESH				GTK_STOCK_REFRESH
#define aPANEL_REMOVE				GTK_STOCK_REMOVE
#define aPANEL_REVERT_TO_SAVED	GTK_STOCK_REVERT_TO_SAVED
#define aPANEL_SAVE					GTK_STOCK_SAVE
#define aPANEL_SAVE_AS				GTK_STOCK_SAVE_AS
#define aPANEL_SELECT_COLOR		GTK_STOCK_SELECT_COLOR
#define aPANEL_SELECT_FONT			GTK_STOCK_SELECT_FONT
#define aPANEL_SORT_ASCENDING		GTK_STOCK_SORT_ASCENDING
#define aPANEL_SORT_DESCENDING	GTK_STOCK_SORT_DESCENDING
#define aPANEL_SPELL_CHECK			GTK_STOCK_SPELL_CHECK
#define aPANEL_STOP					GTK_STOCK_STOP
#define aPANEL_BOLD					GTK_STOCK_BOLD
#define aPANEL_ITALIC				GTK_STOCK_ITALIC
#define aPANEL_UNDERLINE			GTK_STOCK_UNDERLINE
#define aPANEL_STRIKE				GTK_STOCK_STRIKETHROUGH
#define aPANEL_INDENT				GTK_STOCK_INDENT
#define aPANEL_UNINDENT				GTK_STOCK_UNINDENT
#define aPANEL_UNDELETE				GTK_STOCK_UNDELETE
#define aPANEL_UNDO					GTK_STOCK_UNDO
#define aPANEL_YES					GTK_STOCK_YES
#define aPANEL_ZOOM_IN				GTK_STOCK_ZOOM_IN
#define aPANEL_ZOOM_OUT				GTK_STOCK_ZOOM_OUT
#define aPANEL_ZOOM_100				GTK_STOCK_ZOOM_100
#define aPANEL_ZOOM_FIT				GTK_STOCK_ZOOM_FIT
#endif
#ifdef USE_WIN32
enum {
	aPANEL_ABOUT,
	aPANEL_ADD,
	aPANEL_ALIGN_LEFT,
	aPANEL_ALIGN_RIGHT,
	aPANEL_ALIGN_CENTER,
	aPANEL_ALIGN_JUSTIFY,
	aPANEL_APPLY,
	aPANEL_CANCEL,
	aPANEL_CDROM,
	aPANEL_CLEAR,
	aPANEL_CLOSE,
	aPANEL_COLOR_PICKER,
	aPANEL_CONNECT,
	aPANEL_CONVERT,
	aPANEL_COPY,
	aPANEL_CUT,
	aPANEL_DELETE,
	aPANEL_DIRECTORY,
	aPANEL_DISCONNECT,
	aPANEL_EDIT,
	aPANEL_EXECUTE,
	aPANEL_FILE,
	aPANEL_FIND,
	aPANEL_FIND_AND_REPLACE,
	aPANEL_FULLSCREEN,
	aPANEL_LEAVE_FULLSCREEN,
	aPANEL_GOTO_FIRST,
	aPANEL_GOTO_TOP,
	aPANEL_GOTO_LAST,
	aPANEL_GOTO_BOTTOM,
	aPANEL_GO_BACK,
	aPANEL_GO_UP,
	aPANEL_GO_FORWARD,
	aPANEL_GO_DOWN,
	aPANEL_HARDDISK,
	aPANEL_HELP,
	aPANEL_HOME,
	aPANEL_INDEX,
	aPANEL_INFO,
	aPANEL_JUMP_TO,
	aPANEL_MEDIA_PREVIOUS,
	aPANEL_MEDIA_REWIND,
	aPANEL_MEDIA_FORWARD,
	aPANEL_MEDIA_NEXT,
	aPANEL_MEDIA_PLAY,
	aPANEL_MEDIA_PAUSE,
	aPANEL_MEDIA_STOP,
	aPANEL_MEDIA_RECORD,
	aPANEL_MISSING_IMAGE,
	aPANEL_NETWORK,
	aPANEL_NEW,
	aPANEL_NO,
	aPANEL_OK,
	aPANEL_OPEN,
	aPANEL_PAGE_SETUP,
	aPANEL_PASTE,
	aPANEL_PREFERENCES,
	aPANEL_PRINT,
	aPANEL_PRINT_ERROR,
	aPANEL_PRINT_PAUSED,
	aPANEL_PRINT_PREVIEW,
	aPANEL_PRINT_REPORT,
	aPANEL_PRINT_WARNING,
	aPANEL_PROPERTIES,
	aPANEL_QUIT,
	aPANEL_REDO,
	aPANEL_REFRESH,
	aPANEL_REMOVE,
	aPANEL_REVERT_TO_SAVED,
	aPANEL_SAVE,
	aPANEL_SAVE_AS,
	aPANEL_SELECT_COLOR,
	aPANEL_SELECT_FONT,
	aPANEL_SORT_ASCENDING,
	aPANEL_SORT_DESCENDING,
	aPANEL_SPELL_CHECK,
	aPANEL_STOP,
	aPANEL_BOLD,
	aPANEL_ITALIC,
	aPANEL_UNDERLINE,
	aPANEL_STRIKE,
	aPANEL_INDENT,
	aPANEL_UNINDENT,
	aPANEL_UNDELETE,
	aPANEL_UNDO,
	aPANEL_YES,
	aPANEL_ZOOM_IN,
	aPANEL_ZOOM_OUT,
	aPANEL_ZOOM_100,
	aPANEL_ZOOM_FIT,
	aPANEL_ICONS,
};
#endif

#endif /* _AMANITA_GUI_PANEL_H */

