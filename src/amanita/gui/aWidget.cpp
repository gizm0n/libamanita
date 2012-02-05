
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <amanita/gui/aBrowser.h>
#include <amanita/gui/aSizer.h>
#include <amanita/gui/aWidget.h>


#if defined(__linux__)

#elif defined(WIN32)
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
//#include <ctype.h>
#include <commctrl.h>
#include <richedit.h>


enum {
	WIN32_CONTROL_BUTTON,
	WIN32_CONTROL_COMBOBOX,
	WIN32_CONTROL_EDIT,
	WIN32_CONTROL_LISTBOX,
	WIN32_CONTROL_LISTVIEW,
	WIN32_CONTROL_STATIC,
	WIN32_CONTROL_RICHEDIT,
	WIN32_CONTROL_BROWSER,
};

#ifdef UNICODE 
static const WCHAR *win32_classes[] = {
	L"BUTTON",
	L"COMBOBOX",
	L"EDIT",
	L"LISTBOX",
	L"SysListView32",
	L"STATIC",
#else
static const char *win32_classes[] = {
	"BUTTON",
	"COMBOBOX",
	"EDIT",
	"LISTBOX",
	"SysListView32",
	"STATIC",
#endif
	RICHEDIT_CLASS,
	BROWSER_CLASS,
};

struct widget_control {
	int style;
	int style_x;
	const char *win32_control;
};

static const widget_control controls[] = {
	{ /* WIDGET_CONTROL */
		0,0,0
	},
	{ /* WIDGET_BOX */
		WS_CHILD|WS_VISIBLE|BS_GROUPBOX,0,
		win32_classes[WIN32_CONTROL_BUTTON]
	},
	{ /* WIDGET_BROWSER */
		WS_CHILD|WS_VISIBLE,0,
		win32_classes[WIN32_CONTROL_BROWSER],
	},
	{ /* WIDGET_BUTTON */
		WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,0,
		win32_classes[WIN32_CONTROL_BUTTON],
	},
	{ /* WIDGET_CANVAS */
		WS_CHILD|WS_VISIBLE|SS_CENTER,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
	{ /* WIDGET_CLABEL */
		WS_CHILD|WS_VISIBLE|SS_CENTER,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
	{ /* WIDGET_CHOICE */
		CBS_DROPDOWNLIST|CBS_AUTOHSCROLL|WS_BORDER|WS_VSCROLL,0,
		win32_classes[WIN32_CONTROL_COMBOBOX],
	},
	{ /* WIDGET_CHECKBOX */
		BS_AUTOCHECKBOX|WS_GROUP,0,
		win32_classes[WIN32_CONTROL_BUTTON],
	},
	{ /* WIDGET_DEFAULT_BUTTON */
		BS_DEFPUSHBUTTON,0,
		win32_classes[WIN32_CONTROL_BUTTON],
	},
	{ /* WIDGET_ENTRY */
		WS_CHILD|ES_AUTOHSCROLL,WS_EX_CLIENTEDGE,
		win32_classes[WIN32_CONTROL_EDIT],
	},
	{ /* WIDGET_IMAGE */
		WS_CHILD|SS_CENTER,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
	{ /* WIDGET_LABEL */
		WS_CHILD|SS_LEFT,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
	{ /* WIDGET_LISTBOX */
		WS_CHILD|LBS_USETABSTOPS|LBS_EXTENDEDSEL|WS_VSCROLL|WS_HSCROLL|WS_TABSTOP,
		WS_EX_CLIENTEDGE,
		win32_classes[WIN32_CONTROL_LISTBOX],
	},
	{ /* WIDGET_MENU */
		0,0,0,
	},
	{ /* WIDGET_NOTEBOOK */
		0,0,0,
	},
	{ /* WIDGET_PROGRESS */
		WS_CHILD|SS_CENTER,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
	{ /* WIDGET_RLABEL */
		WS_CHILD|SS_RIGHT,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
	{ /* WIDGET_SEPARATOR */
		WS_CHILD|SS_CENTER,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
	{ /* WIDGET_TABLE */
		WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|LVS_REPORT|LVS_SINGLESEL,
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP,
		win32_classes[WIN32_CONTROL_LISTVIEW],
	},
	{ /* WIDGET_TEXT */
		WS_VISIBLE|ES_READONLY|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN,
		WS_EX_CLIENTEDGE,
		win32_classes[WIN32_CONTROL_RICHEDIT],
	},
	{ /* WIDGET_TREE */
		WS_CHILD|SS_CENTER,0,
		win32_classes[WIN32_CONTROL_STATIC],
	},
};
#endif

aWidget::aWidget(widget_event_handler weh,widget_type t) : event_handler(0),id(0),type(t),parent(0),handle(0),text(0)/*,sizer(0)*/ {
}

aWidget::~aWidget() {
	if(text) { free(text);text = 0; }
}


aHandle aWidget::create(aHandle p,int s) {
	parent = p;
#if defined(__linux__)
	switch(type) {
		case WIDGET_CONTROL:return 0;
		case WIDGET_BROWSER:break; /* See: aBrowser::create(). */
		case WIDGET_MENU:break; /* See: aMenu::create(). */
		case WIDGET_BOX:
		case WIDGET_BUTTON:
		case WIDGET_CANVAS:
		case WIDGET_CLABEL:
		case WIDGET_CHOICE:
		case WIDGET_CHECKBOX:
		case WIDGET_DEFAULT_BUTTON:
		case WIDGET_ENTRY:
		case WIDGET_IMAGE:
		case WIDGET_LABEL:
		case WIDGET_LISTBOX:
		case WIDGET_NOTEBOOK:
		case WIDGET_PROGRESS:
		case WIDGET_RLABEL:
		case WIDGET_SEPARATOR:
		case WIDGET_TABLE:
		case WIDGET_TEXT:
		case WIDGET_TREE:break;
	}
#elif defined(WIN32)
	{
		const widget_control *c = &controls[type-WIDGET_CONTROL];
fprintf(stderr,"aWidget::create(widget=%p)\n",(aWidget *)this);
fflush(stderr);
		handle = (aHandle)CreateWindowEx(c->style_x,c->win32_control,
				text? text : "",c->style,0,0,0,0,(HWND)parent,(HMENU)id,hinst,this);
	}
#endif
	return handle;
}


void aWidget::setID(unsigned long i) {
	id = i;
}

void aWidget::setText(const char *str) {
	if(text) free(text);
	text = strdup(text);
#if defined(__linux__)
#elif defined(WIN32)
#endif
}

void aWidget::setFont(aHandle font) {
	if(handle) {
#if defined(__linux__)
#elif defined(WIN32)
		SendMessage((HWND)handle,WM_SETFONT,(WPARAM)font,0);
#endif
	}
}

int aWidget::getWidth() {
	if(handle) {
#if defined(__linux__)
		return 0;
#elif defined(WIN32)
/*		if(sizer) return sizer->width;
		else {*/
			RECT r;
			GetClientRect((HWND)handle,&r);
			return r.right;
//		}
#endif
	} else return 0;
}

int aWidget::getHeight() {
	if(handle) {
#if defined(__linux__)
		return 0;
#elif defined(WIN32)
//		if(sizer) return sizer->height;
//		else {
			RECT r;
			GetClientRect((HWND)handle,&r);
			return r.bottom;
//		}
#endif
	} else return 0;
}

