
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#endif
#include <amanita/gui/aNotebook.h>
#include <amanita/gui/aWindow.h>


struct notebook_page {
	uint8_t index;
	char *tab;
	aWidget *page;
	aNotebook *notebook;
#ifdef USE_WIN32
	int status;
#endif
};


#ifdef USE_GTK
void close_tab_callback(GtkWidget *widget,gpointer data) {
	notebook_page *np = (notebook_page *)data;
	if(np) np->notebook->closePage(np->index);
}
#endif


aObject_Inheritance(aNotebook,aWidget)

aNotebook::aNotebook(widget_event_handler weh) : aWidget(weh,aWIDGET_NOTEBOOK),pages() {
	selected = -1;
}

aNotebook::~aNotebook() {
debug_output("aNotebook::~aNotebook()\n");
	int i;
	notebook_page *np;
	for(i=pages.size()-1; i>=0; --i) {
		np = (notebook_page *)pages[i];
		free(np->tab);
		if(np->page) delete np->page;
		free(np);
	}
}


void aNotebook::create(aWindow *wnd,uint32_t st) {
#ifdef USE_GTK
	GValue val = G_VALUE_INIT;
	component = gtk_notebook_new();
	gtk_widget_set_name((GtkWidget *)component,"notebook");

	g_value_init(&val,G_TYPE_BOOLEAN);
	g_value_set_boolean(&val,TRUE); 
//	g_object_set_property(G_OBJECT(component),"homogeneous",&val);
	g_object_set_property(G_OBJECT(component),"scrollable",&val);
//	g_object_set_property(G_OBJECT(component),"reorderable",&val);
	g_value_unset(&val);

	g_value_init(&val,G_TYPE_BOOLEAN);
	g_value_set_boolean(&val,FALSE); 
	g_object_set_property(G_OBJECT(component),"show-border",&val);
	g_value_unset(&val);

	g_value_init(&val,G_TYPE_INT);
	g_value_set_int(&val,3); 
	g_object_set_property(G_OBJECT(component),"tab-border",&val);
	g_value_unset(&val);

	g_value_init(&val,G_TYPE_INT);
	g_value_set_int(&val,GTK_POS_TOP); 
	g_object_set_property(G_OBJECT(component),"tab-pos",&val);
	g_value_unset(&val);
//	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(component),GTK_POS_TOP);
#endif
debug_output("aNotebook::create()\n");
	aWidget::create(wnd,0);

#ifdef USE_WIN32
	captureEvents();
#endif
}


int aNotebook::openPage(const char *tab,aWidget *page) {
	notebook_page *np = (notebook_page *)malloc(sizeof(notebook_page));
	np->index = pages.size();
	np->tab = strdup(tab);
	np->page = page;
	np->notebook = this;
	pages += np;
	page->parent = this;
debug_output("aNotebook::openPage(page->create(%s))\n",page->getInstance().getName());
	page->create(window,0);
	page->createAll(0,false);
#ifdef USE_GTK
	GtkWidget *t;
	if((style&aNOTEBOOK_CLOSE_BUTTON)) {
		GValue val = G_VALUE_INIT;
		int w,h;
		GtkWidget *bt,*im,*l;

		t = gtk_hbox_new(FALSE,3);
		l = gtk_label_new(tab);

		bt = gtk_button_new();
		gtk_widget_set_name(bt,"tab-close-button");
		g_value_init(&val,G_TYPE_INT);
		g_value_set_int(&val,GTK_RELIEF_NONE); 
		g_object_set_property(G_OBJECT(bt),"relief",&val);
		g_value_unset(&val);
		im = gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
		gtk_icon_size_lookup(GTK_ICON_SIZE_MENU,&w,&h);
		gtk_widget_set_size_request(bt,w+2,h+2);
		gtk_container_add(GTK_CONTAINER(bt),im);
		g_signal_connect(G_OBJECT(bt),"clicked",G_CALLBACK(close_tab_callback),(gpointer)np);
		gtk_box_pack_start(GTK_BOX(t),l,TRUE,TRUE,0);
		gtk_box_pack_start(GTK_BOX(t),bt,FALSE,FALSE,0);
		gtk_widget_show_all(bt);
	} else {
		t = gtk_label_new(tab);
	}
	gtk_widget_show_all(t);
	page->show();
	gtk_notebook_append_page(GTK_NOTEBOOK(component),(GtkWidget *)page->getComponent(),t);
#endif
#ifdef USE_WIN32
	np->status = 1;
//	SetParent((HWND)page->getComponent(),(HWND)parent->getComponent());
debug_output("aNotebook::openPage(%s)\n",tab);
	TCITEM ti = {TCIF_TEXT};
#ifdef USE_WCHAR
	int len = strlen(tab)+1;
	wchar_t wtab[len];
	char2w(wtab,tab,len);
	ti.pszText = (wchar_t *)wtab;
#else
	ti.pszText = (char *)tab;
#endif
	TabCtrl_InsertItem((HWND)component,np->index,&ti);
#endif
	selectPage(np->index);
	return selected;
}

void aNotebook::selectPage(int n) {
	if(pages.size()>0 && n>=0 && n<pages.size() && n!=selected) {
		notebook_page *np;
		if(selected>=0) {
			np = (notebook_page *)pages[selected];
			if(event_handler) event_handler(this,aNOTEBOOK_PAGE_HIDE,(intptr_t)np->page,np->index,(intptr_t)np->tab);
		}
#ifdef USE_GTK
debug_output("aNotebook::selectPage((index: %d)\n",n);
		np = (notebook_page *)pages[n];
		if(event_handler) event_handler(this,aNOTEBOOK_PAGE_SHOW,(intptr_t)np->page,np->index,(intptr_t)np->tab);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(component),n);
#endif
#ifdef USE_WIN32
		TabCtrl_SetCurSel((HWND)component,n);
		if(selected>=0) np->page->hide();
		np = (notebook_page *)pages[n];
debug_output("aNotebook::selectPage(action: %d, tab: %s, width: %d, height: %d)\n",aNOTEBOOK_PAGE_SHOW,np->tab,client.right-client.left,client.bottom-client.top);
		selected = n;
		ShowWindow((HWND)np->page->getComponent(),SW_SHOW);
		if(np->status==1) {
			makeLayout(client.left,client.top,client.right-client.left,client.bottom-client.top);
			np->page->move();
			np->status = 0;
		}
//		BringWindowToTop((HWND)np->page->component);
//		InvalidateRect((HWND)np->page->component,0,false);
//		UpdateWindow((HWND)np->page->component);
		if(event_handler) event_handler(this,aNOTEBOOK_PAGE_SHOW,(intptr_t)np->page,np->index,(intptr_t)np->tab);
#endif
	}
}

const char *aNotebook::getTab(int n) {
	if(pages.size()>0 && n>=0 && n<pages.size())
		return (const char *)((notebook_page *)pages[n])->tab;
	return 0;
}

aWidget *aNotebook::getPage(int n) {
	if(pages.size()>0 && n>=0 && n<pages.size())
		return (aWidget *)((notebook_page *)pages[n])->page;
	return 0;
}

void aNotebook::closePage(int n) {
	if(pages.size()>0 && n>=0 && n<pages.size()) {
		int i;
		notebook_page *np = (notebook_page *)pages[n];
		if(n==selected) selectPage(n==0? n+1 : n-1);
#ifdef USE_GTK
		gtk_notebook_remove_page(GTK_NOTEBOOK(component),n);
#endif
#ifdef USE_WIN32
		TabCtrl_DeleteItem((HWND)component,n);
#endif
		pages.removeAt(n);
		free(np->tab);
		if(np->page) delete np->page;
		free(np);

		for(i=pages.size()-1; i>=n; --i) {
			np = (notebook_page *)pages[i];
debug_output("aNotebook::closePage(index=%d -> i=%d)\n",np->index,i);
			np->index = i;
		}
	}
}



#ifdef USE_WIN32

void aNotebook::tabEvent() {
	int i = TabCtrl_GetCurSel((HWND)component);
	selectPage(i);
}

void aNotebook::makeLayout(int x,int y,int w,int h) {
	int i,w1 = width,h1 = height;
	client.left = x,client.top = y,client.right = x+w,client.bottom = y+h;
	notebook_page *np = selected>=0? (notebook_page *)pages[selected] : 0;
	aWidget::makeLayout(x,y,w,h);
//debug_output("aNotebook::makeLayout(x: %d, y: %d, w: %d, h: %d)\n",this->x,this->y,width,height);
	RECT r = client;
	TabCtrl_AdjustRect((HWND)component,FALSE,&r);
debug_output("aNotebook::makeLayout(WIN32_CONTROL_TABS - TabCtrl_AdjustRect(l: %d, t: %d, r: %d, b: %d))\n",r.left,r.top,r.right,r.bottom);
	if(w1!=width || h1!=height)
		for(i=pages.size()-1; i>=0; --i)
			((notebook_page *)pages[i])->status = 1;
	if(np && np->page) {
		np->page->makeLayout(x,r.top,width,height-r.top);
		np->status = 0;
	}
}

void aNotebook::move() {
	notebook_page *np = selected>=0? (notebook_page *)pages[selected] : 0;
	MoveWindow((HWND)component,x,y,width,height,true);
//debug_output("aNotebook::move(x: %d, y: %d, w: %d, h: %d)\n",x,y,width,height);
	if(np && np->page) np->page->move();
}
#endif


