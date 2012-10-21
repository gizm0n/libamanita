
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#include <commctrl.h>
#endif
#include <amanita/Application.h>
#include <amanita/Resource.h>
#include <amanita/tk/Panel.h>
#include <amanita/tk/Window.h>


namespace a {
namespace tk {


#ifdef USE_GTK
void button_clicked_callback(GtkWidget *widget,gpointer data) {
	PanelButton *pb = (PanelButton *)data;
	Panel *p = pb->panel;
	widget_event_handler weh = p->getEventHandler();
	weh(p,PANEL_CLICKED,pb->action,(intptr_t)pb->data,0);
}
#endif

Object_Inheritance(Panel,Widget)

Panel::Panel(widget_event_handler weh,PanelButton *b) : Widget(weh,WIDGET_PANEL) {
	buttons = 0;
	setButtons(b);
}

Panel::~Panel() {
	deleteButtons();
}

void Panel::setButtons(PanelButton *b) {
	deleteButtons();
	if(b) {
		int i,n;
		for(n=0; b[n].style; ++n);
		buttons = (PanelButton *)malloc((n+1)*sizeof(PanelButton));
		memcpy(buttons,b,(n+1)*sizeof(PanelButton));
		for(i=0; i<n; ++i) {
			if(buttons[i].label) buttons[i].label = strdup(buttons[i].label);
			buttons[i].panel = this;
		}
	}
}

void Panel::deleteButtons() {
	if(buttons) {
		for(int i=0; buttons[i].style; ++i)
			if(buttons[i].label) free((void *)buttons[i].label);
		free(buttons);
		buttons = 0;
	}
}

void Panel::create(Window *wnd,uint32_t st) {
	int n;
#ifdef USE_WIN32
	int i;
#endif
	PanelButton *b1;
#ifdef USE_GTK
	GtkToolItem *item;
	component = (Component)gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(component),GTK_TOOLBAR_ICONS);
	gtk_container_set_border_width(GTK_CONTAINER(component),2);
	for(n=0,b1=buttons; b1->style; ++n,++b1) {
		if(b1->style==PANEL_BUTTON || b1->style==PANEL_CHECK) {
			item = gtk_tool_button_new_from_stock(b1->icon);
			if(b1->label) gtk_tool_button_set_label(GTK_TOOL_BUTTON(item),b1->label);
			g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(button_clicked_callback),b1);
		} else if(b1->style==PANEL_SEPARATOR)
			item = gtk_separator_tool_item_new();
		gtk_toolbar_insert(GTK_TOOLBAR(component),item,-1);
	}
#endif

	Widget::create(wnd,st);

#ifdef USE_WIN32
	SendMessage((HWND)component,CCM_SETVERSION,(WPARAM)5,0);
	SendMessage((HWND)component,TB_SETEXTENDEDSTYLE,0,(LPARAM)TBSTYLE_EX_HIDECLIPPEDBUTTONS);

	for(n=0; buttons[n].style; ++n);

	HIMAGELIST il = ImageList_LoadBitmap(hMainInstance,MAKEINTRESOURCE(aSTOCK_ICONS),24,24,0xff00ff);
//	HIMAGELIST il = ImageList_LoadBitmap(hMainInstance,MAKEINTRESOURCE(1001),24,24,0xff00ff);
//	HIMAGELIST il = ImageList_LoadImage(hMainInstance,MAKEINTRESOURCE(WIN32_STOCK_ICONS),24,24,0xff00ff,IMAGE_BITMAP,LR_SHARED);
	SendMessage((HWND)component,TB_SETIMAGELIST,0,(LPARAM)il);
	SendMessage((HWND)component,TB_SETIMAGELIST,1,(LPARAM)il);
	SendMessage((HWND)component,TB_SETIMAGELIST,2,(LPARAM)il);
//	SendMessage((HWND)component,TB_LOADIMAGES,(WPARAM)IDB_STD_SMALL_COLOR,(LPARAM)HINST_COMMCTRL);


	TBBUTTON tbb[n];
	ZeroMemory(tbb,sizeof(tbb));

	TBBUTTON *b2;
	tchar_t *t;
	for(i=0,b1=buttons,b2=tbb; i<n; ++i,++b1,++b2) {
		b2->iBitmap = b1->icon;
		b2->idCommand = makeID();
		if(!(b1->state&PANEL_DISABLED)) b2->fsState |= TBSTATE_ENABLED;
		if(b1->state&PANEL_PRESSED) b2->fsState |= TBSTATE_PRESSED;
		if(b1->state&PANEL_HIDDEN) b2->fsState |= TBSTATE_HIDDEN;
		if(b1->style==PANEL_BUTTON) b2->fsStyle = BTNS_BUTTON|BTNS_AUTOSIZE;
		else if(b1->style==PANEL_CHECK) b2->fsStyle = BTNS_CHECK|BTNS_AUTOSIZE;
		else if(b1->style==PANEL_SEPARATOR) b2->fsStyle = BTNS_SEP;
		b2->dwData = 0;
debug_output("Panel::create(button[%d]: %p)\n",i,b1);
		if(b1->label) {
			t = tstrdup(b1->label);
			b2->iString = (INT_PTR)t;
		} else b2->iString = 0;
		addComponent((uint16_t)b2->idCommand,(Component)b1);
	}

	SendMessage((HWND)component,TB_BUTTONSTRUCTSIZE,(WPARAM)sizeof(TBBUTTON),0);
	SendMessage((HWND)component,TB_ADDBUTTONS,n,(LPARAM)&tbb);
//	SendMessage((HWND)component,TB_AUTOSIZE,0,0);

	for(i=0,b2=tbb; i<n; ++i,++b2)
		if(b2->iString) tfree((tchar_t *)b2->iString);

	min_height = 24+12;
#endif
}

#ifdef USE_WIN32
void Panel::move() {
debug_output("Panel::move(component: %p)\n",component);
	if(component) SendMessage((HWND)component,TB_AUTOSIZE,0,0);
}
#endif


}; /* namespace tk */
}; /* namespace a */


