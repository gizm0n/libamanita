

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipomoea/base64.h>
#include "djynn.h"


static char *eol;
static int eol_len = 0;

static void item_activate_cb(GtkMenuItem *menuitem, gpointer gdata) {
	djynn_base64_action((int)((intptr_t)gdata));
}

void djynn_base64_action(int id) {
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
			int l = strlen(text);
			char *s;
			if(id==DJYNN_BASE64_DECODE) {
				i = base64_decoded_size(l);
				s = malloc(i+1);
				base64_decode(s,text);
			} else {
				i = base64_encoded_size(l);
				s = malloc(i+1);
				base64_encode(s,text,l);
			}
			if(sel) sci_replace_sel(sci,(const gchar *)s);
			else sci_set_text(sci,(const gchar *)s);
			free(s);
			g_free(text);
		}
	}
}

void djynn_base64_init(GeanyData *data,int *menu_index) {
	int i;
	djynn_menu_item *m;

	if(*menu_index>0) {
		gtk_container_add(GTK_CONTAINER(djynn.tools_menu),gtk_separator_menu_item_new());
		*menu_index += 1;
	}

	for(i=DJYNN_BASE64_ENCODE; i<=DJYNN_BASE64_DECODE; ++i) {
		m = &djynn.menu_items[i];
		if(m->menu_stock==NULL) m->menu_item = gtk_menu_item_new_with_label(m->label);
		else m->menu_item = ui_image_menu_item_new(m->menu_stock,m->label);
		g_signal_connect(m->menu_item,"activate",G_CALLBACK(item_activate_cb),(gpointer)((intptr_t)i));
		gtk_menu_append(GTK_MENU(djynn.tools_menu),m->menu_item);
		*menu_index += 1;
	}
}


void djynn_base64_cleanup() {
}

