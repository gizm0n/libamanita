
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <amanita/gui/aBrowser.h>
#include <amanita/gui/aWidget.h>
#include <amanita/gui/aWindow.h>


#ifdef WIN32
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
//#include <ctype.h>
#include <commctrl.h>
#include <richedit.h>


uint32_t class_registers = 0;

enum {
	WIN32_CONTROL_BUTTON,
	WIN32_CONTROL_COMBOBOX,
	WIN32_CONTROL_EDIT,
	WIN32_CONTROL_LISTBOX,
	WIN32_CONTROL_LISTVIEW,
	WIN32_CONTROL_STATIC,
	WIN32_CONTROL_RICHEDIT,
	WIN32_CONTROL_BROWSER,
	WIN32_CONTROL_WINDOW,
};
static const char *win32_classes[] = {
	"Button",
	"ComboBox",
	"Edit",
	"ListBox",
	"SysListView32",
	"Static",
	RICHEDIT_CLASS,
	BROWSER_CLASS,
	AMANITA_MAIN_WINDOW_CLASS,
};

struct widget_control {
	int style;
	int style_x;
	const char *win32_control;
};

static const widget_control controls[] = {
	/* WIDGET_CONTROL */
	{ 0,0,0 },
	/* WIDGET_BOX */
	{	WS_CHILD|WS_VISIBLE|BS_GROUPBOX,0,"Button" },
	/* WIDGET_BROWSER */
	{ WS_CHILD|WS_VISIBLE,0,win32_classes[WIN32_CONTROL_BROWSER], },
	/* WIDGET_BUTTON */
	{ WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,0,"Button", },
	/* WIDGET_CANVAS */
	{ WS_CHILD|WS_VISIBLE|SS_CENTER,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_CLABEL */
	{ WS_CHILD|WS_VISIBLE|SS_CENTER,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_CHOICE */
	{ CBS_DROPDOWNLIST|CBS_AUTOHSCROLL|WS_BORDER|WS_VSCROLL,0,win32_classes[WIN32_CONTROL_COMBOBOX], },
	/* WIDGET_CHECKBOX */
	{ BS_AUTOCHECKBOX|WS_GROUP,0,"Button", },
	/* WIDGET_DEFAULT_BUTTON */
	{ BS_DEFPUSHBUTTON,0,win32_classes[WIN32_CONTROL_BUTTON], },
	/* WIDGET_ENTRY */
	{ WS_CHILD|ES_AUTOHSCROLL,WS_EX_CLIENTEDGE,win32_classes[WIN32_CONTROL_EDIT], },
	/* WIDGET_IMAGE */
	{ WS_CHILD|SS_CENTER,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_LABEL */
	{ WS_CHILD|SS_LEFT,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_LISTBOX */
	{ WS_CHILD|LBS_USETABSTOPS|LBS_EXTENDEDSEL|WS_VSCROLL|WS_HSCROLL|WS_TABSTOP,WS_EX_CLIENTEDGE,win32_classes[WIN32_CONTROL_LISTBOX], },
	/* WIDGET_MENU */
	{ 0,0,0, },
	/* WIDGET_NOTEBOOK */
	{ 0,0,0, },
	/* WIDGET_PROGRESS */
	{ WS_CHILD|SS_CENTER,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_RLABEL */
	{ WS_CHILD|SS_RIGHT,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_SEPARATOR */
	{ WS_CHILD|SS_CENTER,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_STATUSBAR */
	{ 0,0,0, },
	/* WIDGET_TABLE */
	{ WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|LVS_REPORT|LVS_SINGLESEL,
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP,
		win32_classes[WIN32_CONTROL_LISTVIEW], },
	/* WIDGET_TEXT */
	{ WS_VISIBLE|ES_READONLY|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN,WS_EX_CLIENTEDGE,win32_classes[WIN32_CONTROL_RICHEDIT], },
	/* WIDGET_TREE */
	{ WS_CHILD|SS_CENTER,0,win32_classes[WIN32_CONTROL_STATIC], },
	/* WIDGET_WINDOW */
	{ 0,0,0, },
};
#endif

aWidget::aWidget(widget_event_handler weh,widget_type t) : event_handler(weh),type(t) {
	id = 0;
	parent = 0;
	child = 0;
	next = 0;
	container = 0;
	component = 0;
	text = 0;
	format = 0;
	x = 0,y = 0,width = 0,height = 0,min_width = 0,min_height = 0,pad_x = 0,pad_y = 0;
}

aWidget::~aWidget() {
	if(text) free(text);
	text = 0;
	if(child) delete child;
	child = 0;
	if(next) delete next;
	next = 0;
}


void aWidget::add(aWidget *w) {
	w->parent = this;
	if(!child) child = w;
	else {
		aWidget *c;
		for(c=child; c->next; c=c->next);
		c->next = w;
	}
}

aWidget *aWidget::get(uint32_t id) {
	if(id==this->id) return this;
	else {
		aWidget *w = child;
		while(w && w!=this) {
			if(id==w->id) return w;
			if(w->child) w = w->child;
			else if(w->next) w = w->next;
			else if(w->parent) {
				while(w->parent && !w->parent->next) w = w->parent;
				if(w) w = w->next;
			}
		}
	}
}

aWidget *aWidget::iterate(int &lvl) {
	if(child) {
		++lvl;
		return child;
	}
	else if(next) return next;
	else if(parent) {
		aWidget *w = this;
		while(w->parent && !w->parent->next) w = w->parent,--lvl;
		if(w) w = w->next;
		return w;
	}
	return 0;
}

aWidget *aWidget::remove(aWidget *w) {
}

aWidget *aWidget::remove(uint32_t id) {
}


aComponent aWidget::create() {
#ifdef __linux__
	switch(type) {
		case WIDGET_CONTROL:return 0;
		case WIDGET_BROWSER:break; /* See: aBrowser::create(). */
		case WIDGET_MENU:break; /* See: aMenu::create(). */
		case WIDGET_WINDOW:break; /* See: aWindow::create(). */
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
#endif
#ifdef WIN32
	{
		const widget_control *c = &controls[type-WIDGET_CONTROL];
debug_output("aWidget::create(widget=%p)\n",(aWidget *)this);
		component = (aComponent)CreateWindowEx(c->style_x,c->win32_control,
				text? text : "",c->style,0,0,0,0,(HWND)parent->container,(HMENU)id,hinst,this);
		container = component;
	}
#endif
	return container;
}

void aWidget::setID(unsigned long i) {
	id = i;
}

void aWidget::setText(const char *str) {
	if(text) free(text);
	text = strdup(text);
}

void aWidget::setFont(aComponent font) {
	if(component) {
#ifdef WIN32
		SendMessage((HWND)component,WM_SETFONT,(WPARAM)font,0);
#endif
	}
}

int aWidget::getWidth() {
	if(container) {
#ifdef __linux__
		return 0;
#endif
#ifdef WIN32
/*		if(sizer) return sizer->width;
		else {*/
			RECT r;
			GetClientRect((HWND)container,&r);
			return r.right;
//		}
#endif
	} else return 0;
}

int aWidget::getHeight() {
	if(container) {
#ifdef __linux__
		return 0;
#endif
#ifdef WIN32
//		if(sizer) return sizer->height;
//		else {
			RECT r;
			GetClientRect((HWND)container,&r);
			return r.bottom;
//		}
#endif
	} else return 0;
}


void aWidget::setFormat(uint32_t f,int w,int h,int px,int py) {
	format = f;
	x = 0;
	y = 0;
	width = w;
	height = h;
	min_width = w;
	min_height = h;
	pad_x = px;
	pad_y = py;
}

#ifdef WIN32
void aWidget::makeLayout(int w,int h) {
}
#endif

void aWidget::show() {
debug_output("aWidget::show(1)\n");
	if(!container) return;
#ifdef __linux__
debug_output("aWidget::show(2)\n");
	gtk_widget_show_all((GtkWidget *)container);
debug_output("aWidget::show(3)\n");
#endif
#ifdef WIN32
	ShowWindow((HWND)container,SW_SHOW);
	UpdateWindow((HWND)container);
#endif
}

void aWidget::hide() {
}


