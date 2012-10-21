
#include "../_config.h"
#include <stdlib.h>
#include <string.h>
#include <amanita/tk/Menu.h>
#include <amanita/tk/Window.h>


namespace a {
namespace tk {


#ifdef USE_GTK
static gboolean menuitem_callback(GtkWidget *widget,gpointer data) {
	MenuItem *mi = (MenuItem *)data;
	widget_event_handler weh = mi->menu->getEventHandler();
debug_output("Menu::menuitem_callback(%p, id: %d)\n",mi,mi->id);
	weh(mi->menu,MENU_EVENT_ACTION,mi->id,(intptr_t)mi->data,0);
	return FALSE;
}
#endif


Object_Inheritance(Menu,Widget)

Menu::Menu(widget_event_handler weh,const MenuItem *mi) : Widget(weh,WIDGET_MENU) {
	int i,n;
	MenuItem *m,*mp;

	parent = 0;
	nitems = 0;

	for(const MenuItem *m1=mi; m1->pid!=-1 || m1->id!=-1; ++nitems,++m1);
debug_output("nitems=%d\n",nitems);

	items = (MenuItem *)malloc(nitems*sizeof(MenuItem));
	memcpy(items,mi,nitems*sizeof(MenuItem));
	items_index = (MenuItem **)malloc(nitems*sizeof(MenuItem *));
	memset(items_index,0,nitems*sizeof(MenuItem *));
	for(i=0; i<nitems; ++i) {
		m = &items[i];
		m->index = i,m->lvl = 0;
		m->menu = this;
		if(m->id==-1) m->name = "---";
		else {
			if(!m->name) m->name = "?";
			items_index[m->index] = m;
		}
		if(m->pid!=-1) {
			for(n=0; n<i; n++) if(items[n].id==m->pid) {
				m->parent = &items[n];
				if(!items[n].child) items[n].child = m;
				break;
			}
		}
	}
	for(i=0; i<nitems; ++i) {
		m = &items[i];
		if(m->parent) for(mp=m->parent,m->lvl=0; mp; mp=mp->parent) ++m->lvl;
	}
}


Menu::~Menu() {
	if(items) free(items);
	items = 0;
	if(items_index) free(items_index);
	items_index = 0;
	nitems = 0;
	parent = 0;
	component = 0;
}

void Menu::create(Window *wnd,uint32_t st) {
	int i,n;
	char str[257];
#ifdef USE_WIN32
	char buf[5];
#endif
#ifdef USE_WCHAR
	wchar_t wstr[257];
#endif
	MenuItem *m;
#ifdef USE_GTK
	GtkAccelGroup *accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW((GtkWindow *)((Window *)parent)->wnd),accel_group);

	component = (Component)gtk_menu_bar_new();
#endif

#ifdef USE_WIN32
	component = (Component)CreateMenu();
#endif

	window = wnd;

	for(i=0,m=items; m && i<nitems; ++i) {
*str = '\0';
for(n=0; n<m->lvl; ++n) strcat(str,"--> ");
strcat(str,m->name);
debug_output("Menu::create(menu: index=%d,id='%d',parent='%d',child='%d',menuitem='%s')\n",m->index,m->id,m->parent? m->parent->id : -1,m->child? m->child->id : -1,str);

#ifdef USE_GTK
		if(m->id==-1) {
			if(m->parent) {
				m->item = gtk_separator_menu_item_new();
				gtk_menu_shell_append(GTK_MENU_SHELL((GtkWidget *)m->parent->submenu),m->item);
			}
		} else {
			m->item = gtk_image_menu_item_new_with_label(m->name);
			if(m->icon)
				gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(m->item),GTK_WIDGET(gtk_image_new_from_stock(m->icon,GTK_ICON_SIZE_MENU)));
			if(!m->sensitive) gtk_widget_set_sensitive(m->item,false);
			if(m->acc!=-1) {
				gtk_image_menu_item_set_accel_group(GTK_IMAGE_MENU_ITEM(m->item),accel_group);
				gtk_widget_add_accelerator(m->item,"activate",accel_group,m->acc,(GdkModifierType)m->acc_mod,GTK_ACCEL_VISIBLE);
			}
			if(m->child) {
				m->submenu = (Component)gtk_menu_new();
				gtk_menu_item_set_submenu(GTK_MENU_ITEM(m->item),m->submenu);
			}
			if(m->parent) {
				gtk_menu_shell_append(GTK_MENU_SHELL(m->parent->submenu),m->item);
				if(!m->child) {
debug_output("Menu::create(g_signal_connect: %s - %p)\n",m->name,m);
					if(event_handler) g_signal_connect(G_OBJECT(m->item),"activate",G_CALLBACK(menuitem_callback),(gpointer)m);
				}
			} else {
				gtk_menu_shell_append(GTK_MENU_SHELL((GtkWidget *)component),m->item);
			}
		}
#endif

#ifdef USE_WIN32
		if(m->id==-1) {
			if(m->parent)
				AppendMenu((HMENU)m->parent->submenu,MF_SEPARATOR,0,0);
		} else {
			strcpy(str,m->name);
			if(m->acc!=-1) {
				strcat(str,"\t");
				if((m->acc_mod&KEY_CONTROL)) strcat(str,"Ctrl-");
				if((m->acc_mod&KEY_ALT)) strcat(str,"Alt-");
				if((m->acc_mod&KEY_SHIFT)) strcat(str,"Shift-");
				if(m->acc>=KEY_a && m->acc<=KEY_z) sprintf(buf,"%c",m->acc);
				else if(m->acc>=KEY_F1 && m->acc<=KEY_F12) sprintf(buf,"F%d",m->acc+1-KEY_F1);
				strcat(str,buf);
			}
			if(m->child) {
				m->submenu = (Component)CreatePopupMenu();
#ifdef USE_WCHAR
				char2w(wstr,str,256);
debug_output("Menu::create(m: %p, wstr: %ls)\n",m,wstr);
				AppendMenu((HMENU)(m->parent? m->parent->submenu : component),MF_STRING|MF_POPUP,(UINT)m->submenu,wstr);
#else
				AppendMenu((HMENU)(m->parent? m->parent->submenu : component),MF_STRING|MF_POPUP,(UINT)m->submenu,str);
#endif
			} else if(m->parent) {
#ifdef USE_WCHAR
				char2w(wstr,str,256);
debug_output("Menu::create(m: %p, parent: %p, submenu: %p, wstr: %ls)\n",m,m->parent,m->parent? m->parent->submenu : 0,wstr);
				AppendMenu((HMENU)m->parent->submenu,MF_STRING,WIDGET_MAKE_ID(WIDGET_MENU,m->index),wstr);
#else
				AppendMenu((HMENU)m->parent->submenu,MF_STRING,WIDGET_MAKE_ID(WIDGET_MENU,m->index),str);
#endif
			}
//			if(!m->sensitive) gtk_widget_set_sensitive(m->item,false);
//			if(m->acc!=-1) {
//				gtk_image_menu_item_set_accel_group(GTK_IMAGE_MENU_ITEM(m->item),accel_group);
//				gtk_widget_add_accelerator(m->item,"activate",accel_group,m->acc,(GdkModifierType)m->acc_mod,GTK_ACCEL_VISIBLE);
//			}
		}
#endif

		if(i==nitems-1) break;
		if(m->child) m = m->child;
		else if(items[m->index+1].parent==m->parent || m->parent) m = &items[m->index+1];
		else break;
	}

#ifdef USE_WIN32
	SetMenu((HWND)((Window *)parent)->component,(HMENU)component);
#endif
}


}; /* namespace tk */
}; /* namespace a */



