
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
#include <amanita/tk/Notebook.h>
#include <amanita/tk/Window.h>


namespace a {
namespace tk {


Object_Inheritance(NotebookPage,Object)


NotebookPage::NotebookPage(Notebook *nb) : Object() {
	tab.notebook = nb;
#ifdef USE_GTK
	tab.button = 0;
	tab.label = 0;
#endif
	tab.index = 0;
	tab.name = 0;
	page = 0;
	status = 0;
}

NotebookPage::~NotebookPage() {
	if(tab.name) free(tab.name);
	tab.name = 0;
	if(page) delete(page);
	page = 0;
}

void NotebookPage::open() {
	tab.notebook->openPage(this);
}

void NotebookPage::close() {
	tab.notebook->closePage(tab.index);
}


void NotebookPage::setTabLabel(const char *str) {
	if(tab.name) free(tab.name);
	tab.name = strdup(str);
#ifdef USE_GTK
	gtk_label_set_text(GTK_LABEL(tab.label),str);
#endif
#ifdef USE_WIN32
	TCITEM ti = {TCIF_TEXT};
	ti.pszText = (tchar_t *)tstrdup(str);
	TabCtrl_SetItem((HWND)tab.notebook->getComponent(),tab.index,&ti);
	tfree(ti.pszText);
#endif
}


#ifdef USE_GTK
static void switch_page_event_callback(GtkNotebook *notebook,GtkNotebookPage *page,guint num,gpointer data) {
debug_output("switch_page_event_callback(page: %d)\n",num);
	int *selected = (int *)data;
	*selected = num;
}

static void close_tab_callback(GtkWidget *widget,gpointer data) {
	NotebookTab *nt = (NotebookTab *)data;
	if(nt) nt->notebook->closePage(nt->index);
}
#endif


Object_Inheritance(Notebook,Widget)

Notebook::Notebook(widget_event_handler weh) : Widget(weh,WIDGET_NOTEBOOK),pages() {
	selected = -1;
}

Notebook::~Notebook() {
debug_output("Notebook::~Notebook()\n");
	int i;
	for(i=pages.size()-1; i>=0; --i)
		delete (NotebookPage *)pages[i];
}


void Notebook::create(Window *wnd,uint32_t st) {
	if(style&NOTEBOOK_HIDE_TABS) {
		style &= ~NOTEBOOK_CLOSE_BUTTON;
		type = WIDGET_CONTAINER;
	}

#ifdef USE_GTK
	component = gtk_notebook_new();

	if(style&NOTEBOOK_HIDE_TABS) {
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(component),FALSE);
	} else {
		gtk_notebook_set_show_border(GTK_NOTEBOOK(component),FALSE);
		gtk_notebook_set_scrollable(GTK_NOTEBOOK(component),TRUE);
		gtk_notebook_set_tab_border(GTK_NOTEBOOK(component),3);
		gtk_notebook_set_tab_pos(GTK_NOTEBOOK(component),GTK_POS_TOP);

		g_signal_connect(G_OBJECT(component),"switch-page",G_CALLBACK(switch_page_event_callback),&selected);
	}
#endif
debug_output("Notebook::create()\n");
	Widget::create(wnd,0);

#ifdef USE_WIN32
	captureEvents();
#endif
}


int Notebook::openPage(const char *tab,Widget *page,bool sel) {
	NotebookPage *np = new NotebookPage(this);
	np->tab.name = strdup(tab);
	np->page = page;
	return openPage(np,sel);
}

int Notebook::openPage(NotebookPage *np,bool sel) {
	np->tab.index = pages.size();
	pages << np;
	np->page->parent = this;
debug_output("Notebook::openPage(page->create(%s))\n",np->page->getInstance().getName());
	np->page->create(window,0);
	np->page->createAll(0,false);
#ifdef USE_GTK
	GtkWidget *t;
	if((style&NOTEBOOK_CLOSE_BUTTON)) {
		GValue val = { 0 };
		int w,h;
		GtkWidget *bt,*im,*l;

		t = gtk_hbox_new(FALSE,3);
		l = gtk_label_new(np->tab.name);

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
		g_signal_connect(G_OBJECT(bt),"clicked",G_CALLBACK(close_tab_callback),(gpointer)&np->tab);
		gtk_box_pack_start(GTK_BOX(t),l,TRUE,TRUE,0);
		gtk_box_pack_start(GTK_BOX(t),bt,FALSE,FALSE,0);

		np->tab.button = bt;
		np->tab.label = l;

		gtk_widget_show_all(t);
	} else {
		t = gtk_label_new(np->tab.name);
	}
	gtk_widget_show_all(t);
	np->page->show();
	gtk_notebook_append_page(GTK_NOTEBOOK(component),(GtkWidget *)np->page->getComponent(),t);
#endif
#ifdef USE_WIN32
	np->status = 1;
//	SetParent((HWND)page->getComponent(),(HWND)parent->getComponent());
debug_output("Notebook::openPage(%s)\n",np->tab.name);
	if(type==WIDGET_NOTEBOOK) {
		TCITEM ti = {TCIF_TEXT};
		ti.pszText = tstrdup(np->tab.name);
		TabCtrl_InsertItem((HWND)component,np->tab.index,&ti);
		tfree(ti.pszText);
	}
#endif
	if(sel) selectPage(np->tab.index);
	return selected;
}

void Notebook::selectPage(int n) {
	if(pages.size()>0 && n>=0 && n<(int)pages.size() && n!=selected) {
		NotebookPage *np;
		if(selected>=0) {
			np = (NotebookPage *)pages[selected];
			if(event_handler) event_handler(this,NOTEBOOK_PAGE_HIDE,(intptr_t)np->page,np->tab.index,(intptr_t)np->tab.name);
#ifdef USE_WIN32
			np->page->hide();
#endif
		}
#ifdef USE_GTK
debug_output("Notebook::selectPage((index: %d)\n",n);
		np = (NotebookPage *)pages[n];
		if(event_handler) event_handler(this,NOTEBOOK_PAGE_SHOW,(intptr_t)np->page,np->tab.index,(intptr_t)np->tab.name);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(component),n);
		selected = n;
#endif
#ifdef USE_WIN32
		if(type==WIDGET_NOTEBOOK) TabCtrl_SetCurSel((HWND)component,n);
		np = (NotebookPage *)pages[n];
debug_output("Notebook::selectPage(action: %d, tab: %s, width: %ld, height: %ld)\n",NOTEBOOK_PAGE_SHOW,np->tab.name,client.right-client.left,client.bottom-client.top);
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
		if(event_handler) event_handler(this,NOTEBOOK_PAGE_SHOW,(intptr_t)np->page,np->tab.index,(intptr_t)np->tab.name);
#endif
	}
}

const char *Notebook::getTab(int n) {
	if(n<0 || n>=(int)pages.size()) n = selected;
	if(pages.size()>0)
		return (const char *)((NotebookPage *)pages[n])->tab.name;
	return 0;
}

NotebookPage *Notebook::getPage(int n) {
debug_output("Notebook::getPage(n: %d, pages: %d, selected: %d)\n",n,(int)pages.size(),selected);
	if(n<0 || n>=(int)pages.size()) n = selected;
	if(pages.size()>0)
		return (NotebookPage *)pages[n];
	return 0;
}

void Notebook::closePage(int n) {
	if(n<0 || n>=(int)pages.size()) n = selected;
	if(pages.size()>0) {
debug_output("Notebook::closePage(n: %d, pages: %d)\n",n,(int)pages.size());
		int i;
		NotebookPage *np = (NotebookPage *)pages[n];
		if(n==selected) selectPage(n==0? n+1 : n-1);
#ifdef USE_GTK
		gtk_notebook_remove_page(GTK_NOTEBOOK(component),n);
#endif
#ifdef USE_WIN32
		if(type==WIDGET_NOTEBOOK) TabCtrl_DeleteItem((HWND)component,n);
#endif
		pages.removeAt(n);
		delete np;

		for(i=pages.size()-1; i>=n; --i) {
			np = (NotebookPage *)pages[i];
debug_output("Notebook::closePage(index=%d -> i=%d)\n",np->tab.index,i);
			np->tab.index = i;
		}
	}
}



#ifdef USE_WIN32
void Notebook::tabEvent() {
	int i = TabCtrl_GetCurSel((HWND)component);
	selectPage(i);
}

void Notebook::makeLayout(int x,int y,int w,int h) {
	int i,w1 = width,h1 = height;
	client.left = x,client.top = y,client.right = x+w,client.bottom = y+h;
	NotebookPage *np = selected>=0? (NotebookPage *)pages[selected] : 0;
	Widget::makeLayout(x,y,w,h);
debug_output("Notebook::makeLayout(x: %d, y: %d, w: %d, h: %d)\n",this->x,this->y,width,height);
	RECT r = { 0,0,client.right-client.left,client.bottom-client.top };
	if(type==WIDGET_NOTEBOOK) TabCtrl_AdjustRect((HWND)component,FALSE,&r);
debug_output("Notebook::makeLayout(WIN32_CONTROL_TABS - TabCtrl_AdjustRect(l: %ld, t: %ld, r: %ld, b: %ld))\n",r.left,r.top,r.right,r.bottom);
	if(w1!=width || h1!=height)
		for(i=pages.size()-1; i>=0; --i)
			((NotebookPage *)pages[i])->status = 1;
	if(np && np->page) {
		np->page->makeLayout(x,r.top,width,height-r.top);
		np->status = 0;
	}
}

void Notebook::move() {
	NotebookPage *np = selected>=0? (NotebookPage *)pages[selected] : 0;
	MoveWindow((HWND)component,x,y,width,height,true);
//debug_output("Notebook::move(x: %d, y: %d, w: %d, h: %d)\n",x,y,width,height);
	if(np && np->page) np->page->move();
}
#endif


}; /* namespace tk */
}; /* namespace a */


