
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <amanita/aApplication.h>
#include <amanita/gui/aBrowser.h>
#include <amanita/gui/aWidget.h>
#include <amanita/gui/aWindow.h>


#ifdef USE_WIN32
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>

uint32_t class_registers = 0;

int dbu_x = LOWORD(GetDialogBaseUnits());
int dbu_y = HIWORD(GetDialogBaseUnits());

enum {
	WIN32_CONTROL_BUTTON,
	WIN32_CONTROL_COMBOBOX,
	WIN32_CONTROL_EDIT,
	WIN32_CONTROL_LISTBOX,
	WIN32_CONTROL_LISTVIEW,
	WIN32_CONTROL_STATIC,
	WIN32_CONTROL_STATUSBAR,
	WIN32_CONTROL_TABS,
	WIN32_CONTROL_RICHEDIT,
	WIN32_CONTROL_BROWSER,
	WIN32_CONTROL_WINDOW,
};
static const tchar_t *win32_classes[] = {
	_T("Button"),
	_T("ComboBox"),
	_T("Edit"),
	_T("ListBox"),
	WC_LISTVIEW,
	_T("Static"),
	STATUSCLASSNAME,
	WC_TABCONTROL,
	RICHEDIT_CLASS,
	aBROWSER_CLASS,
	aWINDOW_CLASS,
};

struct widget_control {
	int style;
	int style_x;
	int control;
	int min_width;
	int min_height;
};

static const widget_control controls[] = {
	/* aWIDGET_VOID */
	{ 0,0,0,0,0, },
	/* aWIDGET_BROWSER */
	{ WS_CHILD,0,WIN32_CONTROL_BROWSER,0,0, },
	/* aWIDGET_BUTTON */
	{ WS_CHILD|WS_TABSTOP|BS_NOTIFY,WS_EX_STATICEDGE,WIN32_CONTROL_BUTTON,MulDiv(dbu_x,40,4),MulDiv(dbu_y,14,8), },
	/* aWIDGET_CAIRO */
	{ WS_CHILD|SS_OWNERDRAW,0,WIN32_CONTROL_STATIC,0,0, },
	/* aWIDGET_CANVAS */
	{ WS_CHILD|SS_OWNERDRAW,0,WIN32_CONTROL_STATIC,0,0, },
	/* aWIDGET_CHECKBOX */
	{ WS_CHILD|WS_TABSTOP|BS_NOTIFY|BS_AUTOCHECKBOX,0,WIN32_CONTROL_BUTTON,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* aWIDGET_COMBOBOX */
	{ WS_CHILD|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,0,WIN32_CONTROL_COMBOBOX,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* aWIDGET_COMBOBOX_ENTRY */
	{ WS_CHILD|WS_TABSTOP|CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_VSCROLL,0,WIN32_CONTROL_COMBOBOX,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* aWIDGET_CONTAINER */
	{ WS_CHILD,0,WIN32_CONTROL_STATIC,0,0, },
	/* aWIDGET_ENTRY */
	{ WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL,WS_EX_CLIENTEDGE,WIN32_CONTROL_EDIT,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* aWIDGET_FRAME */
	{ WS_CHILD|BS_GROUPBOX,0,WIN32_CONTROL_BUTTON,MulDiv(dbu_x,7+7,4),MulDiv(dbu_y,11+7,8), },
	/* aWIDGET_IMAGE */
	{ WS_CHILD|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* aWIDGET_LABEL */
	{ WS_CHILD,0,WIN32_CONTROL_STATIC,MulDiv(dbu_x,40,4),MulDiv(dbu_y,8,8), },
	/* aWIDGET_LISTBOX */
	{ WS_CHILD|WS_TABSTOP|LBS_USETABSTOPS|LBS_NOTIFY|WS_VSCROLL|WS_HSCROLL,WS_EX_CLIENTEDGE,WIN32_CONTROL_LISTBOX,0,0, },
	/* aWIDGET_LISTVIEW */
	{ WS_CHILD|WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL|WS_VSCROLL|WS_HSCROLL,WS_EX_CLIENTEDGE,WIN32_CONTROL_LISTVIEW,0,0, },
	/* aWIDGET_MENU */
	{ 0,0,0,0,0, },
	/* aWIDGET_NOTEBOOK */
	{ WS_CHILD,0,WIN32_CONTROL_TABS,0,0, },
	/* aWIDGET_PROGRESS */
	{ WS_CHILD|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* aWIDGET_RADIOBUTTON */
	{ WS_CHILD|WS_TABSTOP|BS_NOTIFY|BS_AUTORADIOBUTTON,0,WIN32_CONTROL_BUTTON,0,MulDiv(dbu_y,10,8), },
	/* aWIDGET_SEPARATOR */
	{ WS_CHILD|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* aWIDGET_STATUSBAR */
	{ WS_CHILD,0,WIN32_CONTROL_STATUSBAR,0,0, },
	/* aWIDGET_TABLE */
	{ WS_CHILD|WS_TABSTOP|WS_HSCROLL|WS_VSCROLL|LVS_REPORT|LVS_SINGLESEL,
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP,
		WIN32_CONTROL_LISTVIEW,0,0, },
	/* aWIDGET_TEXT */
	{ WS_CHILD|WS_TABSTOP|ES_READONLY|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN,WS_EX_CLIENTEDGE,WIN32_CONTROL_RICHEDIT,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* aWIDGET_TREE */
	{ WS_CHILD|WS_TABSTOP|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* aWIDGET_WINDOW */
	{ 0,0,0,0,0, },
};

#endif

aHashtable aWidget::components = aHashtable();


aObject_Inheritance(aWidget,aObject)

aWidget::aWidget(widget_event_handler weh,widget_type t) : event_handler(weh),type(t) {
	id = 0;
	window = 0;
	parent = 0;
	next = 0;
	component = 0;
	text = 0;
	style = 0;
	x = 0,y = 0,width = 0,height = 0,min_width = 0,min_height = 0,border = 0,spacing = 0;
}

aWidget::~aWidget() {
debug_output("aWidget::~aWidget()\n");
	if(component) deleteComponent(component);
	if(text) free(text);
	text = 0;
	if(next) delete next;
	next = 0;
#ifdef USE_WIN32
	if(event_data) {
		SetWindowLongPtr((HWND)component,GWL_USERDATA,(LONG_PTR)0);
		free(event_data);
	}
	event_data = 0;
#endif
}

void aWidget::create(aWindow *wnd,uint32_t st) {
	static uint32_t id_index = 0;
	id = (type<<9)|(++id_index);
	window = wnd;
#ifdef USE_GTK
/*	switch(type) {
		case aWIDGET_BUTTON:component = (aComponent)gtk_button_new_with_label(text? text : "");break;
		case aWIDGET_CHOICE:component = (aComponent)gtk_label_new(text? text : "");
		case aWIDGET_LABEL:component = (aComponent)gtk_label_new(text? text : "");
		case aWIDGET_BOX:
		case aWIDGET_CHECKBOX:
		case aWIDGET_DEFAULT_BUTTON:
		case aWIDGET_ENTRY:
		case aWIDGET_IMAGE:
		case aWIDGET_LISTBOX:
		case aWIDGET_PROGRESS:
		case aWIDGET_SEPARATOR:
		case aWIDGET_TABLE:
		case aWIDGET_TEXT:
		case aWIDGET_TREE:break;
	}*/
	if(min_width || min_height)
		gtk_widget_set_size_request((GtkWidget *)component,min_width? min_width : -1,min_height? min_height : -1);
#endif
#ifdef USE_WIN32
	const widget_control *wc = &controls[type-aWIDGET_VOID];
	st |= wc->style;
	if(!(style&aHIDE)) st |= WS_VISIBLE;
#ifdef USE_WCHAR
	int len = text? strlen(text)+1 : 1;
	wchar_t t[len];
	if(text) char2w(t,text,len);
#else
	char *t = text;
#endif
debug_output("aWidget::create(window: %p, widget: %p, control: %s, component: %p, text: %" PRIts ")\n",window,this,win32_classes[wc->control],component,text? t : _T("-"));
	if(wc->style!=0) {
		component = (aComponent)CreateWindowEx(wc->style_x,win32_classes[wc->control],
			text? t : _T(""),st,0,0,0,0,(HWND)parent->component,(HMENU)id,hMainInstance,this);
	}
//	SetParent((HWND)component,(HWND)window->component);
	if(!min_width && wc->min_width) min_width = wc->min_width;
	if(!min_height && wc->min_height) min_height = wc->min_height;
	SendMessage((HWND)component,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),(LPARAM)1);
#ifdef USE_WCHAR
	if(text) free(t);
#endif
#endif
	if(component) addComponent(component);
}


void aWidget::createAll(aComponent p,bool n) {
	if(n && next) {
		aWidget *w;
		for(w=next; w; w=w->next) {
			w->create(window,0);
			w->createAll(component,false);
		}
	}
#ifdef USE_GTK
	if(p) gtk_container_add(GTK_CONTAINER(p),(GtkWidget *)component);
#endif
}

void aWidget::setText(const char *str) {
	if(text) free(text);
	text = strdup(str);
}

void aWidget::setFont(aComponent font) {
	if(component) {
#ifdef USE_WIN32
		SendMessage((HWND)component,WM_SETFONT,(WPARAM)font,0);
#endif
	}
}

void aWidget::setStyle(uint32_t st,uint16_t minw,uint16_t minh,uint8_t b,uint8_t sp) {
	style = st;
	x = 0;
	y = 0;
	width = minw;
	height = minh;
	min_width = minw;
	min_height = minh;
	border = b;
	spacing = sp;
}

#ifdef USE_WIN32
static LRESULT CALLBACK WidgetProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	widget_event_data *wed = (widget_event_data *)GetWindowLongPtr(hwnd,GWL_USERDATA);
	if(!wed || wed->event_handler(wed->window,msg,wparam,lparam)) return 0;
	return CallWindowProc(wed->proc,hwnd,msg,wparam,lparam);
}

void aWidget::captureEvents() {
	widget_event_data *wed = (widget_event_data *)malloc(sizeof(widget_event_data));
	event_data = wed;
	SetWindowLongPtr((HWND)component,GWL_USERDATA,(LONG_PTR)wed);
	wed->window = window;
	wed->event_handler = aWindow::handleEvent;
	wed->widget = this;
	wed->proc = (WNDPROC)SetWindowLongPtr((HWND)component,GWL_WNDPROC,(LONG_PTR)WidgetProc);
}

void aWidget::makeLayout(int x,int y,int w,int h) {
//debug_output("aWidget::makeLayout(x: %d, y: %d, w: %d, h: %d)\n",x,y,w,h);
	this->x = x,this->y = y;
	if((style&aFILL) || !min_width) width = w;
	else width = min_width;
	if((style&aFILL) || !min_height) height = h;
	else height = min_height;
}

void aWidget::move() {
debug_output("aWidget::move(component: %p, x: %d, y: %d, width: %d, heigth: %d)\n",component,x,y,width,height);
	if(component) MoveWindow((HWND)component,x,y,width,height,true);
}
#endif

void aWidget::show() {
debug_output("aWidget::show(1)\n");
	if(!component) return;
#ifdef USE_GTK
debug_output("aWidget::show(2)\n");
	gtk_widget_show_all((GtkWidget *)component);
debug_output("aWidget::show(3)\n");
#endif
#ifdef USE_WIN32
	ShowWindow((HWND)component,SW_SHOW);
	UpdateWindow((HWND)component);
#endif
}

void aWidget::hide() {
debug_output("aWidget::show(1)\n");
	if(!component) return;
#ifdef USE_GTK
debug_output("aWidget::show(2)\n");
	gtk_widget_hide((GtkWidget *)component);
debug_output("aWidget::show(3)\n");
#endif
#ifdef USE_WIN32
	ShowWindow((HWND)component,SW_HIDE);
#endif
}


