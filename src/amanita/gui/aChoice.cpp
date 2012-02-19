
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#endif
#include <amanita/gui/aChoice.h>



#ifdef USE_GTK
static void choice_changed_callback(GtkWidget *widget,gpointer data) {
	aChoice *ch = (aChoice *)data;
	widget_event_handler weh = ch->getEventHandler();
	weh(ch,aCHOICE_CHANGED,0,0,0);
}
#endif


aObject_Inheritance(aChoice,aWidget)

aChoice::aChoice(widget_event_handler weh) : aWidget(weh,aWIDGET_COMBOBOX) {
	items = 0;
	item = 0;
	selected = -1;
}

aChoice::~aChoice() {
	if(items) delete items;
	items = 0;
	if(item) free(item);
	item = 0;
}

void aChoice::create(aWindow *wnd,uint32_t st) {
	int i,n,t = style&0xf;
	char *str;
	if(t==aCHOICE_COMBOBOX) type = aWIDGET_COMBOBOX;
	else if(t==aCHOICE_ENTRY) type = aWIDGET_COMBOBOX_ENTRY;
#ifdef USE_GTK
	if(type==aWIDGET_COMBOBOX) component = (aComponent)gtk_combo_box_text_new();
	else component = (aComponent)gtk_combo_box_entry_new_text();
	if(items && items->size()) {
		for(i=0,n=items->size(); i<n; ++i) {
			str = (char *)(*items)[i];
			if(type==aWIDGET_COMBOBOX) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(component),(gchar *)str);
			else gtk_combo_box_append_text(GTK_COMBO_BOX(component),(gchar *)str);
		}
		if(selected>=0) gtk_combo_box_set_active(GTK_COMBO_BOX(component),selected);
	}
	if(event_handler) {
		g_signal_connect(G_OBJECT(component),"changed",G_CALLBACK(choice_changed_callback),this);
	}
#endif

	aWidget::create(wnd,0);

#ifdef USE_WIN32
	if(items && items->size()) {
		for(i=0,n=items->size(); i<n; ++i) {
			str = (char *)(*items)[i];
#ifdef USE_WCHAR
			int len = strlen(str)+1;
			wchar_t wstr[len];
			char2w(wstr,str,len);
			SendMessage((HWND)component,CB_ADDSTRING,0,(LPARAM)wstr);
#else
			SendMessage((HWND)component,CB_ADDSTRING,0,(LPARAM)str);
#endif
		}
		if(selected>=0) SendMessage((HWND)component,CB_SETCURSEL,(WPARAM)selected,0);
	}
#endif
	if(items) delete items;
	items = 0;
}

void aChoice::setItems(const char *arr[]) {
	if(items) delete items;
	if(component) {
#ifdef USE_GTK
		gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(component))));
#endif
#ifdef USE_WIN32
		SendMessage((HWND)component,CB_RESETCONTENT,0,0);
#endif
	}
	for(; *arr; ++arr) addItem(*arr);
}

void aChoice::addItem(const char *str) {
	if(str) {
		if(component) {
#ifdef USE_GTK
			if(type==aWIDGET_COMBOBOX) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(component),(gchar *)str);
			else gtk_combo_box_append_text(GTK_COMBO_BOX(component),(gchar *)str);
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
			int len = strlen(str)+1;
			wchar_t wstr[len];
			char2w(wstr,str,len);
debug_output("aChoice::addItem(wstr: %ls)\n",wstr);
			SendMessage((HWND)component,CB_ADDSTRING,0,(LPARAM)wstr);
#else
			SendMessage((HWND)component,CB_ADDSTRING,0,(LPARAM)str);
#endif
#endif
		} else {
			if(!items) items = new aVector();
			*items += str;
		}
	}
}

void aChoice::select(int n) {
	int sz = 0;
	if(component) {
#ifdef USE_GTK
		gtk_combo_box_set_active(GTK_COMBO_BOX(component),selected);
#endif
#ifdef USE_WIN32
		SendMessage((HWND)component,CB_SETCURSEL,(WPARAM)selected,0);
#endif
	} else selected = n>=0 && items && n<items->size()? n : -1;
}

int aChoice::getSelected() {
	if(component) {
#ifdef USE_GTK
		selected = gtk_combo_box_get_active(GTK_COMBO_BOX(component));
#endif
#ifdef USE_WIN32
		selected = SendMessage((HWND)component,CB_GETCURSEL,0,0);
		if(selected==CB_ERR) selected = -1;
#endif
	}
	return selected;
}

const char *aChoice::getItem() {
	if(item) free(item);
	item = 0;
	if(component) {
#ifdef USE_GTK
		if(type==aWIDGET_COMBOBOX) item = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(component));
		else item = gtk_combo_box_get_active_text(GTK_COMBO_BOX(component));
#endif
#ifdef USE_WIN32
		selected = SendMessage((HWND)component,CB_GETCURSEL,0,0);
		if(selected==CB_ERR) selected = -1,item = 0;
		else {
			int len = SendMessage((HWND)component,CB_GETLBTEXTLEN,(WPARAM)selected,0)+1;
#ifdef USE_WCHAR
			wchar_t str[len];
			SendMessage((HWND)component,CB_GETLBTEXT,(WPARAM)selected,(LPARAM)str);
			item = w2char(str);
#else
			item = (char *)malloc(len);
			SendMessage((HWND)component,CB_GETLBTEXT,(WPARAM)selected,(LPARAM)item);
#endif
		}
#endif
		return item;
	}
	return 0;
}


