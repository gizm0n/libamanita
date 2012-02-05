
#include "../_config.h"
#include <stdlib.h>
#include <string.h>
#include <amanita/gui/aMenu.h>
//#include "..\util\String.h"
//#include "..\util\Array.h"


gboolean aMenu::menuitem_callback(GtkWidget *widget,GdkEventCrossing *ev,gpointer data) {
	menu_item *mi = (menu_item *)data;
	if(mi->menu && mi->menu->event_handler) mi->menu->event_handler(mi->menu,mi->action,(intptr_t)mi,0,0);
//	app.handleAction(ACTION_SHOW_STATUS,0,(intptr_t)g_object_get_data(G_OBJECT(widget),"StatusbarText"));
	return FALSE;
}

aMenu::aMenu(widget_event_handler weh,const menu_item *mi) : aWidget(weh,WIDGET_MENU) {
	int i,n;
	menu_item *m,*mp;

	nitems = 0;

	for(const menu_item *m1=mi; m1->pid!=-1 || m1->id!=-1; ++nitems,++m1);
debug_output("nitems=%d\n",nitems);

	items = (menu_item *)malloc(nitems*sizeof(menu_item));
	memcpy(items,mi,nitems*sizeof(menu_item));
	items_index = (menu_item **)malloc(nitems*sizeof(menu_item *));
	memset(items_index,0,nitems*sizeof(menu_item *));
	for(i=0; i<nitems; ++i) {
		m = &items[i];
		m->index = i,m->lvl = 0;
		m->menu = this;
		if(m->id==-1) m->name = "---",m->status = 0;
		else {
			if(!m->name) m->name = "?";
			items_index[m->id] = m;
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


aMenu::~aMenu() {
	if(items) free(items);
	items = 0;
	if(items_index) free(items_index);
	items_index = 0;
	nitems = 0;
	parent = 0;
	handle = 0;
}

aHandle aMenu::create(aHandle p,int s) {
	int i,n;
	char str[256];
	menu_item *m;
	GtkAccelGroup *accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(p),accel_group);

	handle = (aHandle)gtk_menu_bar_new();

	for(i=0,m=items; m && i<nitems; ++i) {
*str = '\0';
for(n=0; n<m->lvl; ++n) strcat(str,"--> ");
strcat(str,m->name);
debug_output("init(menu: index=%d,id='%d',parent='%d',child='%d',menuitem='%s')\n",m->index,m->id,m->parent? m->parent->id : -1,m->child? m->child->id : -1,str);

		if(m->id==-1) {
			if(m->parent) {
				m->item = gtk_separator_menu_item_new();
				gtk_menu_shell_append(GTK_MENU_SHELL((GtkWidget *)m->parent->submenu),m->item);
			}
		} else {
			m->item = gtk_image_menu_item_new_with_label(m->name);
//			menu.put(m->action,m);
			if(!m->sensitive) gtk_widget_set_sensitive(m->item,false);
			if(m->acc!=-1) {
				gtk_image_menu_item_set_accel_group(GTK_IMAGE_MENU_ITEM(m->item),accel_group);
				gtk_widget_add_accelerator(m->item,"activate",accel_group,m->acc,(GdkModifierType)m->acc_mod,GTK_ACCEL_VISIBLE);
			}
			if(m->status) {
				g_object_set_data(G_OBJECT(m->item),"StatusbarText",(gpointer)m->status);
//				g_signal_connect(m->item,"enter-notify-event",G_CALLBACK(statusbar_push),(gpointer)gui.statusbar);
//				g_signal_connect(m->item,"leave-notify-event",G_CALLBACK(statusbar_pop),(gpointer)gui.statusbar);
			}
			if(m->child) {
				m->submenu = (aHandle)gtk_menu_new();
				gtk_menu_item_set_submenu(GTK_MENU_ITEM(m->item),m->submenu);
			}
			if(m->parent) {
				gtk_menu_shell_append(GTK_MENU_SHELL(m->parent->submenu),m->item);
				if(m->action) g_signal_connect(m->item,"activate",G_CALLBACK(menuitem_callback),(gpointer)m);
			} else {
				gtk_menu_shell_append(GTK_MENU_SHELL((GtkWidget *)handle),m->item);
			}
		}

		if(i==nitems-1) break;
		if(m->child) m = m->child;
		else if(items[m->index+1].parent==m->parent || m->parent) m = &items[m->index+1];
		/*else if(m->parent) {
debug_output("init(menu: next <--)\n");
			for(n=m->index+1,m=m->parent; m && items[n].parent!=m->parent; m=m->parent);
			if(m) {
				if(!m->parent && items[m->index+1].parent) break;
				m = &items[m->index+1];
			}
		}*/ else break;
	}
	return handle;
}


/*
aMenu::aMenu(unsigned int f,unsigned int id,char *c,char *t,aMenu **m) : id(id) {
	flags = f;
	if(c && *c) {
		if(!(flags&MF_BITMAP) && !(flags&MF_OWNERDRAW)) {
			cont = new char[strlen(c)+1];
			strcpy(cont,c);
		} else cont = c;
	} else cont = 0;
	if(t && *t) {
		tit = new char[strlen(t)+1];
		strcpy(tit,t);
	} else tit = 0;
	if(m) {
		for(sz=0; m[sz]; sz++);
		if(sz) {
			items = new aMenu*[sz];
			for(int i=0; i<sz; i++) items[i] = m[i];
		}
	} else items = 0,sz = 0;
}

aMenu::~aMenu() {
	if(cont && !(flags&MF_BITMAP) && !(flags&MF_OWNERDRAW)) { delete[] cont;cont = 0; }
	if(tit) { delete[] tit;tit = 0; }
	if(items) {
		for(int i=0; i<sz; i++) delete items[i];
		delete[] items;
		items = 0;
	}
}

HMENU aMenu::createMenu() {
	menu = CreateMenu();
	for(int i=0; i<sz; i++) items[i]->create(menu);
	return menu;
}
void aMenu::create(HMENU m) {
	menu = m;
	if(sz) {
		m = CreatePopupMenu();
		for(int i=0; i<sz; i++) items[i]->create(m);
		AppendMenu(menu,flags|MF_POPUP,(UINT)m,cont);
	} else AppendMenu(menu,flags,id,cont);
}

aMenu *aMenu::read(FILE *fp) {
	if(feof(fp)) return 0;
	String str;
	str.readWord(fp);
	aMenu *m = 0;
	if(str.equals("MENU") || str.equals("POPUP")) {
		if(str.equals("POPUP")) {
			str.clear().readWord(fp).unquote().unescape();
			m = new aMenu(MF_STRING,0,str.array());
		} else m = new aMenu(0,0);
		str.clear().readWord(fp);
		if(str.equals("{") || str.equals("BEGIN")) {
			aMenu *l[32],*m1;
			int i,n = 0;
			for(i=0; i<32; i++) {
				m1 = read(fp);
				if(!m1) break;
				l[n++] = m1;
			}
			if(n) {
				m->items = new aMenu*[n],m->sz = n;
				for(i=0; i<n; i++) m->items[i] = l[i];
			}
		}
	} else if(str.equals("MENUITEM")) {
		str.clear().readLine(fp,false).trim();
		if(str.equals("---")) m = new aMenu(MF_SEPARATOR,0);
		else {
			Array arr;
			arr.split(",",str);
			m = new aMenu(
				MF_STRING,
				arr.getString(2).toInt(),
				arr.getString(0).unquote().unescape().array(),
				arr.getString(1).unquote().unescape().array()
			);
		}
	}
	return m;
}

char *aMenu::getTitle(unsigned int n) {
	if(n==id) return tit? tit : String::blank;
	if(sz) {
		char *t;
		for(int i=0; i<sz; i++) if(t=items[i]->getTitle(n)) return t;
	}
	return 0;
}
*/


