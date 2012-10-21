
#include "../_config.h"
#include <stdlib.h>
#include <string.h>
#ifdef USE_WIN32
#include <windows.h>
#include <commctrl.h>
#endif
#include <amanita/gui/aStatus.h>



Object_Inheritance(aStatus,aWidget)

aStatus::aStatus(const int c[]) : aWidget(0,aWIDGET_STATUSBAR),cells(0),ncells(0) {
	if(c) {
		int n = 0;
		while(c[n++]!=-1);
		if(n>0) {
			cells = (int *)malloc(n*sizeof(int));
			memcpy(cells,c,n*sizeof(int));
			ncells = n;
		}
	}
#ifdef USE_GTK
	_cells = 0;
#endif
}


aStatus::~aStatus() {
	if(cells) free(cells);
	cells = 0;
	ncells = 0;
#ifdef USE_GTK
	if(_cells) free(_cells);
	_cells = 0;
#endif
}

void aStatus::create(aWindow *wnd,uint32_t st) {
#ifdef USE_GTK
	int i;
	GtkWidget *message_area;
	component = (aComponent)gtk_statusbar_new();
	message_area = (aComponent)gtk_statusbar_get_message_area(GTK_STATUSBAR(component));
	if(cells) {
		GList *children = gtk_container_get_children(GTK_CONTAINER(message_area)),*t;
		_cells = (GtkWidget **)malloc(ncells*sizeof(GtkWidget *));
		for(i=0,t=children; i<ncells && cells[i]!=-1; ++i) {
			if(t) _cells[i] = GTK_WIDGET(t->data),t = t->next;
			else {
				_cells[i] = gtk_label_new("");
				gtk_box_pack_start(GTK_BOX(message_area),_cells[i],TRUE,FALSE,0);
			}
//			gtk_misc_set_alignment(GTK_MISC(_cells[i]),0.0f,0.5f);
			if(cells[i]!=-1)
				gtk_widget_set_size_request(_cells[i],cells[i],-1);
		}
		g_list_free (children);
	}
#endif

	aWidget::create(wnd,0);

#ifdef USE_WIN32
	int i,n,x;
	for(i=1,x=0; i<ncells; ++i)
		n = cells[i-1],cells[i-1] += x,x += n;
	SendMessage((HWND)component,SB_SETPARTS,(WPARAM)ncells,(LPARAM)cells);
#endif
}


void aStatus::update(int n,const char *str) {
#ifdef USE_GTK
	gtk_label_set_text(GTK_LABEL(_cells[n]),str);
//	gtk_statusbar_push(GTK_STATUSBAR(component),
//		gtk_statusbar_get_context_id(GTK_STATUSBAR(component),str),str);
#endif

#ifdef USE_WIN32
	tchar_t *t = tstrdup(str);
	SendMessage((HWND)component,SB_SETTEXT,n,(LPARAM)t);
	tfree(t);
#endif
}



