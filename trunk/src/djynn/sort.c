

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipomoea/hashtable_t.h>
#include <ipomoea/vector_t.h>
#include "djynn.h"



enum {
	SORT		= 1,
	CASE		= 2,
	REVERSE	= 4
};

static char *eol;
static int eol_len = 0;

static void sort_text(int st) {
	GeanyDocument *doc = document_get_current();
	if(doc==NULL) return;
	else {
		ScintillaObject *sci = doc->editor->sci;
		gboolean sel = sci_has_selection(sci);
		gint start = sci_get_selection_start(sci);
		gint end = sci_get_selection_end(sci);
		gchar *text;
		int i = scintilla_send_message(sci,SCI_GETEOLMODE,0,0);
		switch(i) {
			case SC_EOL_CRLF:eol = "\r\n",eol_len = 2;break;
			case SC_EOL_CR:eol = "\r",eol_len = 1;break;
			default:eol = "\n",eol_len = 1;break;
		}
		if(sel) text = sci_get_contents_range(sci,start,end);
		else text = sci_get_contents(sci,sci_get_length(sci)+1);
		if(text!=NULL) {
			vector_t *vec = vec_new(0);
			char *s;
			vec_split_all(vec,text,eol);
			if(st&SORT) {
				if(st&CASE) vec_sort(vec);
				else vec_isort(vec);
			}
			if(st&REVERSE) vec_reverse(vec);
			//dialogs_show_msgbox(GTK_MESSAGE_INFO,"Vector[0][%s]",(char *)vec_get(vec,0));
			s = vec_join(vec,eol);
			//dialogs_show_msgbox(GTK_MESSAGE_INFO,"Sort[%s]",s);
			if(sel) sci_replace_sel(sci,(const gchar *)s);
			else sci_set_text(sci,(const gchar *)s);
			free(s);
			vec_delete(vec);
			g_free(text);
		}
	}
}

static void item_activate_cb(GtkMenuItem *menuitem, gpointer gdata) {
	djynn_sort_action((int)((intptr_t)gdata));
}

void djynn_sort_action(int id) {
	switch(id) {
		case DJYNN_SORT_ASC:sort_text(SORT);break;
		case DJYNN_SORT_ASC_CASE:sort_text(SORT|CASE);break;
		case DJYNN_SORT_DESC:sort_text(SORT|REVERSE);break;
		case DJYNN_SORT_DESC_CASE:sort_text(SORT|REVERSE|CASE);break;
		case DJYNN_REVERSE:sort_text(REVERSE);break;
	}
}

void djynn_sort_init(GeanyData *data,int *menu_index) {
	int i;
	djynn_menu_item *m;

	if(*menu_index>0) {
		gtk_container_add(GTK_CONTAINER(djynn.tools_menu),gtk_separator_menu_item_new());
		*menu_index += 1;
	}

	for(i=DJYNN_SORT_ASC; i<=DJYNN_REVERSE; ++i) {
		m = &djynn.menu_items[i];
		if(m->menu_stock==NULL) m->menu_item = gtk_menu_item_new_with_label(m->label);
		else m->menu_item = ui_image_menu_item_new(m->menu_stock,m->label);
		g_signal_connect(m->menu_item,"activate",G_CALLBACK(item_activate_cb),(gpointer)((intptr_t)i));
		gtk_menu_append(GTK_MENU(djynn.tools_menu),m->menu_item);
		*menu_index += 1;
	}
}


void djynn_sort_cleanup() {
}

