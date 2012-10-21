
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#endif
#include <amanita/tk/Select.h>



namespace a {
namespace tk {


#ifdef USE_GTK
static void choice_changed_callback(GtkWidget *widget,gpointer data) {
	Select *ch = (Select *)data;
	widget_event_handler weh = ch->getEventHandler();
	weh(ch,SELECT_CHANGED,0,0,0);
}
#endif


Object_Inheritance(Select,Widget)

Select::Select(widget_event_handler weh) : Widget(weh,WIDGET_COMBOBOX) {
	items = 0;
	item = 0;
	selected = -1;
}

Select::~Select() {
	if(items) delete items;
	items = 0;
	if(item) free(item);
	item = 0;
}

void Select::create(Window *wnd,uint32_t st) {
	int i,n,t = style&0xf;
	if(t==SELECT_COMBOBOX) type = WIDGET_COMBOBOX;
	else if(t==SELECT_ENTRY) type = WIDGET_COMBOBOX_ENTRY;
#ifdef USE_GTK
	if(type==WIDGET_COMBOBOX) component = (Component)gtk_combo_box_text_new();
	else component = (Component)gtk_combo_box_entry_new_text();
	if(items && items->size()) {
		char *str;
		for(i=0,n=items->size(); i<n; ++i) {
			str = (char *)(*items)[i];
			if(type==WIDGET_COMBOBOX) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(component),(gchar *)str);
			else gtk_combo_box_append_text(GTK_COMBO_BOX(component),(gchar *)str);
		}
		if(selected>=0) gtk_combo_box_set_active(GTK_COMBO_BOX(component),selected);
	}
	if(event_handler) {
		g_signal_connect(G_OBJECT(component),"changed",G_CALLBACK(choice_changed_callback),this);
	}
#endif

	Widget::create(wnd,0);

#ifdef USE_WIN32
	if(items && items->size()) {
#ifdef USE_WCHAR
		wchar_t wstr[257];
#endif
		for(i=0,n=items->size(); i<n; ++i) {
#ifdef USE_WCHAR
			char2w(wstr,(char *)(*items)[i]);
			SendMessage((HWND)component,CB_ADDSTRING,0,(LPARAM)wstr);
#else
			str = (char *)(*items)[i];
			SendMessage((HWND)component,CB_ADDSTRING,0,(LPARAM)str);
#endif
		}
		if(selected>=0) SendMessage((HWND)component,CB_SETCURSEL,(WPARAM)selected,0);
	}
#endif
	if(items) delete items;
	items = 0;
}

void Select::setItems(const char *arr[]) {
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

void Select::addItem(const char *str) {
	if(str) {
		if(component) {
#ifdef USE_GTK
			if(type==WIDGET_COMBOBOX) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(component),(gchar *)str);
			else gtk_combo_box_append_text(GTK_COMBO_BOX(component),(gchar *)str);
#endif
#ifdef USE_WIN32
			tchar_t *t = tstrdup(str);
			SendMessage((HWND)component,CB_ADDSTRING,0,(LPARAM)t);
			tfree(t);
#endif
		} else {
			if(!items) items = new Vector();
			*items << str;
		}
	}
}

void Select::select(int n) {
	if(component) {
#ifdef USE_GTK
		gtk_combo_box_set_active(GTK_COMBO_BOX(component),selected);
#endif
#ifdef USE_WIN32
		SendMessage((HWND)component,CB_SETCURSEL,(WPARAM)selected,0);
#endif
	} else selected = n>=0 && items && n<(int)items->size()? n : -1;
}

int Select::getSelected() {
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

const char *Select::getItem() {
	if(item) free(item);
	item = 0;
	if(component) {
#ifdef USE_GTK
		if(type==WIDGET_COMBOBOX) item = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(component));
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


}; /* namespace tk */
}; /* namespace a */


