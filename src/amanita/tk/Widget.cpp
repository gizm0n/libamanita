
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <amanita/Application.h>
#include <amanita/String.h>
#include <amanita/tk/Browser.h>
#include <amanita/tk/Widget.h>
#include <amanita/tk/Window.h>


static uint16_t id_index[27] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

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
#endif


namespace a {

static Hashtable widgets;

namespace tk {


#ifdef USE_WIN32
uint32_t class_registers = 0;

int dbu_x = LOWORD(GetDialogBaseUnits());
int dbu_y = HIWORD(GetDialogBaseUnits());

enum {
	WIN32_CONTROL_BROWSER,
	WIN32_CONTROL_BUTTON,
	WIN32_CONTROL_COMBOBOX,
	WIN32_CONTROL_EDIT,
	WIN32_CONTROL_LISTBOX,
	WIN32_CONTROL_LISTVIEW,
	WIN32_CONTROL_RICHEDIT,
	WIN32_CONTROL_SCINTILLA,
	WIN32_CONTROL_STATIC,
	WIN32_CONTROL_STATUSBAR,
	WIN32_CONTROL_TABS,
	WIN32_CONTROL_TOOLBAR,
	WIN32_CONTROL_WINDOW,
};
static const tchar_t *win32_classes[] = {
	BROWSER_CLASS,
	_T("Button"),
	_T("ComboBox"),
	_T("Edit"),
	_T("ListBox"),
	WC_LISTVIEW,
	RICHEDIT_CLASS,
	_T("Scintilla"),
	_T("Static"),
	STATUSCLASSNAME,
	WC_TABCONTROL,
	TOOLBARCLASSNAME,
	WINDOW_CLASS,
};

struct widget_control {
	int style;
	int style_x;
	int control;
	int min_width;
	int min_height;
};

static const widget_control controls[] = {
	/* WIDGET_VOID */
	{ 0,0,0,0,0, },
	/* WIDGET_BROWSER */
	{ WS_CHILD,0,WIN32_CONTROL_BROWSER,0,0, },
	/* WIDGET_BUTTON */
	{ WS_CHILD|WS_TABSTOP|BS_NOTIFY,WS_EX_STATICEDGE,WIN32_CONTROL_BUTTON,MulDiv(dbu_x,40,4),MulDiv(dbu_y,14,8), },
	/* WIDGET_CAIRO */
	{ WS_CHILD|SS_OWNERDRAW,0,WIN32_CONTROL_STATIC,0,0, },
	/* WIDGET_CANVAS */
	{ WS_CHILD|SS_OWNERDRAW,0,WIN32_CONTROL_STATIC,0,0, },
	/* WIDGET_CHECKBOX */
	{ WS_CHILD|WS_TABSTOP|BS_NOTIFY|BS_AUTOCHECKBOX,0,WIN32_CONTROL_BUTTON,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* WIDGET_COMBOBOX */
	{ WS_CHILD|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,0,WIN32_CONTROL_COMBOBOX,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* WIDGET_COMBOBOX_ENTRY */
	{ WS_CHILD|WS_TABSTOP|CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_VSCROLL,0,WIN32_CONTROL_COMBOBOX,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* WIDGET_CONTAINER */
	{ WS_CHILD|WS_CLIPCHILDREN,0,WIN32_CONTROL_STATIC,0,0, },
	/* WIDGET_ENTRY */
	{ WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL,WS_EX_CLIENTEDGE,WIN32_CONTROL_EDIT,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* WIDGET_FRAME */
	{ WS_CHILD|BS_GROUPBOX|WS_CLIPCHILDREN,0,WIN32_CONTROL_BUTTON,MulDiv(dbu_x,7+7,4),MulDiv(dbu_y,11+7,8), },
	/* WIDGET_IMAGE */
	{ WS_CHILD|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* WIDGET_LABEL */
	{ WS_CHILD,0,WIN32_CONTROL_STATIC,MulDiv(dbu_x,40,4),MulDiv(dbu_y,8,8), },
	/* WIDGET_LISTBOX */
	{ WS_CHILD|WS_TABSTOP|LBS_USETABSTOPS|LBS_NOTIFY|WS_VSCROLL|WS_HSCROLL,WS_EX_CLIENTEDGE,WIN32_CONTROL_LISTBOX,0,0, },
	/* WIDGET_LISTVIEW */
	{ WS_CHILD|WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL|WS_VSCROLL|WS_HSCROLL,WS_EX_CLIENTEDGE,WIN32_CONTROL_LISTVIEW,0,0, },
	/* WIDGET_MENU */
	{ 0,0,0,0,0, },
	/* WIDGET_NOTEBOOK */
	{ WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,0,WIN32_CONTROL_TABS,0,0, },
	/* WIDGET_PANEL */
	{ WS_CHILD|WS_TABSTOP|WS_CLIPSIBLINGS|TBSTYLE_LIST|TBSTYLE_TOOLTIPS|CCS_TOP,0,WIN32_CONTROL_TOOLBAR,0,28, },
	/* WIDGET_PROGRESS */
	{ WS_CHILD|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* WIDGET_RADIOBUTTON */
	{ WS_CHILD|WS_TABSTOP|BS_NOTIFY|BS_AUTORADIOBUTTON,0,WIN32_CONTROL_BUTTON,0,MulDiv(dbu_y,10,8), },
	/* WIDGET_SCINTILLA */
	{ WS_CHILD|WS_TABSTOP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,WS_EX_CLIENTEDGE,WIN32_CONTROL_SCINTILLA,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* WIDGET_SEPARATOR */
	{ WS_CHILD|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* WIDGET_STATUSBAR */
	{ WS_CHILD,0,WIN32_CONTROL_STATUSBAR,0,0, },
	/* WIDGET_TABLE */
	{ WS_CHILD|WS_TABSTOP|WS_HSCROLL|WS_VSCROLL|LVS_REPORT|LVS_SINGLESEL,
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP,
		WIN32_CONTROL_LISTVIEW,0,0, },
	/* WIDGET_TEXT */
	{ WS_CHILD|WS_TABSTOP|WS_VSCROLL|/*ES_DISABLENOSCROLL|*/ES_MULTILINE|ES_WANTRETURN,WS_EX_CLIENTEDGE,WIN32_CONTROL_RICHEDIT,MulDiv(dbu_x,40,4),MulDiv(dbu_y,10,8), },
	/* WIDGET_TREE */
	{ WS_CHILD|WS_TABSTOP|SS_CENTER,0,WIN32_CONTROL_STATIC,0,0, },
	/* WIDGET_WINDOW */
	{ 0,0,0,0,0, },
};

#endif


Object_Inheritance(Widget,Object)

Widget::Widget(widget_event_handler weh,widget_type t) : event_handler(weh),type(t) {
	id = 0;
	window = 0;
	parent = 0;
	next = 0;
	component = 0;
	text = 0;
	style = 0;
	x = 0,y = 0,width = 0,height = 0,min_width = 0,min_height = 0,border = 0,spacing = 0;
}

Widget::~Widget() {
debug_output("Widget::~Widget()\n");
	removeWidget();
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

void Widget::create(Window *wnd,uint32_t st) {
	id = makeID();
	window = wnd;
#ifdef USE_GTK
/*	switch(type) {
		case WIDGET_BUTTON:component = (Component)gtk_button_new_with_label(text? text : "");break;
		case WIDGET_CHOICE:component = (Component)gtk_label_new(text? text : "");
		case WIDGET_LABEL:component = (Component)gtk_label_new(text? text : "");
		case WIDGET_BOX:
		case WIDGET_CHECKBOX:
		case WIDGET_DEFAULT_BUTTON:
		case WIDGET_ENTRY:
		case WIDGET_IMAGE:
		case WIDGET_LISTBOX:
		case WIDGET_PROGRESS:
		case WIDGET_SEPARATOR:
		case WIDGET_TABLE:
		case WIDGET_TEXT:
		case WIDGET_TREE:break;
	}*/
	if(min_width || min_height)
		gtk_widget_set_size_request((GtkWidget *)component,min_width? min_width : -1,min_height? min_height : -1);
#endif
#ifdef USE_WIN32
	const widget_control *wc = &controls[type-WIDGET_VOID];
	tchar_t *t = tstrdup(text);
	st |= wc->style;
	if(!(style&HIDE)) st |= WS_VISIBLE;
	if(wc->style!=0) {
		component = (Component)CreateWindowEx(wc->style_x,win32_classes[wc->control],
			t? t : _T(""),st,0,0,0,0,(HWND)parent->component,(HMENU)(uint32_t)id,hMainInstance,this);
	}
debug_output("Widget::create(id: %d, window: %p, widget: %p, control: %" PRIts ", component: %p, text: %" PRIts ")\n",id,window,this,win32_classes[wc->control],component,text? t : _T("-"));
//	SetParent((HWND)component,(HWND)window->component);
	if(!min_width && wc->min_width) min_width = wc->min_width;
	if(!min_height && wc->min_height) min_height = wc->min_height;
	SendMessage((HWND)component,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),(LPARAM)1);
	if(t) tfree(t);
	if(text) {
		free(text);
		text = 0;
	}
#endif
	addWidget();
}


void Widget::createAll(Component p,bool n) {
	if(n && next) {
		Widget *w;
		for(w=next; w; w=w->next) {
			w->create(window,0);
			w->createAll(component,false);
		}
	}
#ifdef USE_GTK
	if(p) gtk_container_add(GTK_CONTAINER(p),(GtkWidget *)component);
#endif
}

void Widget::addComponent(uint16_t id,Component c) {
	if(c) widgets.put(id,c);
}

void Widget::addWidget() {
	widgets.put(id,this);
	if(component) widgets.put((void *)component,this);
}

void Widget::removeWidget() {
	widgets.remove(id);
	if(component) widgets.remove((void *)component);
}

Widget *Widget::getWidget(uint16_t id) {
	return (Widget *)widgets.get(id);
}

Widget *Widget::getWidget(Component c) {
	if(c) return (Widget *)widgets.get((void *)c);
	return 0;
}

uint16_t Widget::makeID() {
	return WIDGET_MAKE_ID(type,++id_index[type]);
}

void Widget::setText(const char *str) {
	if(component) {
#ifdef USE_WIN32
		tchar_t *t = tstrdup(str);
		SendMessage((HWND)component,WM_SETTEXT,0,(LPARAM)t);
		tfree(t);
#endif
		if(text) {
			free(text);
			text = 0;
		}
	} else if(str!=text) {
		if(text) free(text);
		text = strdup(str);
	}
}

const char *Widget::getText() {
	if(text) return text;
	if(component) {
	}
	return 0;
}

void Widget::setStyle(uint32_t st,uint16_t minw,uint16_t minh,uint8_t b,uint8_t sp) {
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

void Widget::show() {
debug_output("Widget::show(1)\n");
	if(component) {
#ifdef USE_GTK
debug_output("Widget::show(2)\n");
		gtk_widget_show_all((GtkWidget *)component);
debug_output("Widget::show(3)\n");
#endif
#ifdef USE_WIN32
		ShowWindow((HWND)component,SW_SHOW);
		UpdateWindow((HWND)component);
#endif
	} else style &= ~HIDE;
}

void Widget::hide() {
debug_output("Widget::show(1)\n");
	if(component) {
#ifdef USE_GTK
debug_output("Widget::show(2)\n");
		gtk_widget_hide((GtkWidget *)component);
debug_output("Widget::show(3)\n");
#endif
#ifdef USE_WIN32
		ShowWindow((HWND)component,SW_HIDE);
#endif
	} else style |= HIDE;
}

void Widget::setFont(const char *font,int sz,int st) {
	if(component) {
#ifdef USE_GTK
		GtkWidget *label = (GtkWidget *)component;
		PangoFontDescription *font_desc = pango_font_description_new();
		pango_font_description_set_family(font_desc,font);
		if((st&FONT_BOLD)) pango_font_description_set_weight(font_desc,PANGO_WEIGHT_BOLD);
		if((st&FONT_ITALIC)) pango_font_description_set_style(font_desc,PANGO_STYLE_ITALIC);
		pango_font_description_set_size(font_desc,sz*PANGO_SCALE);
		if(!GTK_IS_LABEL(label)) label = GTK_WIDGET(gtk_bin_get_child(GTK_BIN(label)));
		gtk_widget_modify_font(label,font_desc);
		pango_font_description_free(font_desc);
#endif
#ifdef USE_WIN32
		tchar_t *t = tstrdup(font);
		HDC hdc = GetDC((HWND)component);
		HFONT f;
		sz = -MulDiv(sz,GetDeviceCaps(hdc,LOGPIXELSY),72);
		f = CreateFont(sz,0,0,0,
				(st&FONT_BOLD)? FW_BOLD : FW_NORMAL,
				(st&FONT_ITALIC)? TRUE : FALSE,
				(st&FONT_UNDERLINE)? TRUE : FALSE,
				(st&FONT_STRIKE)? TRUE : FALSE,
				ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
				DEFAULT_PITCH|FF_SWISS,t);
		SendMessage((HWND)component,WM_SETFONT,(WPARAM)f,0);
		InvalidateRect((HWND)component,0,true);
		ReleaseDC((HWND)component,hdc);
		tfree(t);
#endif
	}
}

#ifdef USE_WIN32
static LRESULT CALLBACK WidgetProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	widget_event_data *wed = (widget_event_data *)GetWindowLongPtr(hwnd,GWL_USERDATA);
	if(!wed || wed->event_handler(wed->window,msg,wparam,lparam)) return 0;
	return CallWindowProc(wed->proc,hwnd,msg,wparam,lparam);
}

void Widget::captureEvents() {
	widget_event_data *wed = (widget_event_data *)malloc(sizeof(widget_event_data));
	event_data = wed;
	SetWindowLongPtr((HWND)component,GWL_USERDATA,(LONG_PTR)wed);
	wed->window = window;
	wed->event_handler = Window::handleEvent;
	wed->widget = this;
	wed->proc = (WNDPROC)SetWindowLongPtr((HWND)component,GWL_WNDPROC,(LONG_PTR)WidgetProc);
}

void Widget::makeLayout(int x,int y,int w,int h) {
//debug_output("Widget::makeLayout(x: %d, y: %d, w: %d, h: %d)\n",x,y,w,h);
	this->x = x+border,this->y = y+border;
	if((style&aFILL) || !min_width) width = w-border*2;
	else width = min_width-border*2;
	if((style&aFILL) || !min_height) height = h-border*2;
	else height = min_height-border*2;
}

int Widget::getMinimumWidth() {
	return min_width+border*2;
}

int Widget::getMinimumHeight() {
	return min_height+border*2;
}

void Widget::move() {
debug_output("Widget::move(component: %p, x: %d, y: %d, width: %d, heigth: %d)\n",component,x,y,width,height);
	if(component) MoveWindow((HWND)component,x,y,width,height,true);
}
#endif


}; /* namespace tk */
}; /* namespace a */


