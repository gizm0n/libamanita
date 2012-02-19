
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
#include <amanita/gui/aList.h>



#ifdef USE_GTK
static void list_changed_callback(GtkWidget *widget,gpointer data) {
	aList *l = (aList *)data;
	widget_event_handler weh = l->getEventHandler();
	weh(l,aLIST_CHANGED,0,0,0);
}
#endif


aObject_Inheritance(aList,aWidget)

aList::aList(widget_event_handler weh) : aWidget(weh,aWIDGET_LISTVIEW) {
	columns = 0;
	column_widths = 0;
	ncolumns = 0;
	item = 0;
	selected = -1;

#ifdef USE_GTK
	list = 0;
	store = 0;
#endif
}

aList::~aList() {
	if(columns) {
		free(columns[0]);
		free(columns);
	}
	columns = 0;
	if(column_widths) free(column_widths);
	column_widths = 0;
	ncolumns = 0;
	if(item) free(item);
	item = 0;

#ifdef USE_GTK
	g_object_unref(store);
#endif
}

void aList::create(aWindow *wnd,uint32_t st) {
	int i,n;
#ifdef USE_GTK
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;

	component = (aComponent)gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(component),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(component),GTK_SHADOW_IN);
	list = gtk_tree_view_new();

	for(i=0; i<ncolumns; ++i) {
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes(columns[i],renderer,"text",i,NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(list),column);
	}

	GType types[ncolumns+1];
	for(i=0; i<ncolumns; ++i) types[i] = G_TYPE_STRING;
	types[ncolumns] = G_TYPE_POINTER;
	store = gtk_list_store_newv(ncolumns+1,types);
	gtk_tree_view_set_model(GTK_TREE_VIEW(list),GTK_TREE_MODEL(store));

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
	g_signal_connect(selection,"changed",G_CALLBACK(list_changed_callback),this);

	gtk_container_add(GTK_CONTAINER(component),list);
#endif

	aWidget::create(wnd,0);

#ifdef USE_WIN32
	ListView_SetExtendedListViewStyle((HWND)component,LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);
	if(ncolumns) {
		LVCOLUMN lvc = { 0 };
#ifdef USE_WCHAR
		wchar_t wstr[257];
#endif
		lvc.mask = LVCF_TEXT|LVCF_SUBITEM|LVCF_WIDTH|LVCF_FMT;
		lvc.fmt  = LVCFMT_LEFT;
		for(i=0; i<ncolumns; ++i) {
			lvc.iSubItem = i;
			lvc.cx       = column_widths[i];
#ifdef USE_WCHAR
			char2w(wstr,columns[i],strlen(columns[i])+1);
			lvc.pszText  = wstr;
#else
			lvc.pszText  = columns[i];
#endif
			ListView_InsertColumn((HWND)component,i,&lvc);
		}
	}
#endif
	if(columns) {
		free(columns[0]);
		free(columns);
	}
	columns = 0;
	if(column_widths) free(column_widths);
	column_widths = 0;
}

void aList::setColumns(const char *cols,const int widths[],char sep) {
	char *p1,*p2,*c = strdup(cols);
	int n;

	if(columns) {
		free(columns[0]);
		free(columns);
	}
	if(column_widths) free(column_widths);

	for(n=1,p1=c; (p1=strchr(p1,sep)); ++n,++p1);
	columns = (char **)malloc(n*sizeof(char *));
	column_widths = (int *)malloc(n*sizeof(int));
	ncolumns = n;
debug_output("ncolumns: %d\n",ncolumns);
	for(n=0,p1=c,p2=p1; n<ncolumns; ++n,++p2,p1=p2) {
		p2 = strchr(p2,sep);
		if(p2) *p2 = '\0';
		columns[n] = p1;
		column_widths[n] = widths[n];
debug_output("column[%d]: %s\n",n,p1);
	}
}

void aList::addRow(const char *cols,char sep) {
	if(cols) {
		if(component) {
			char *p1,*p2,*c = strdup(cols);
			int n;
#ifdef USE_GTK
			GValue val = G_VALUE_INIT;
			gtk_list_store_append(store,&iter);
#endif
#ifdef USE_WIN32
			LVITEM lv = { 0 };
#ifdef USE_WCHAR
			wchar_t wstr[256];
#endif
			lv.iItem = ListView_GetItemCount((HWND)component);
			ListView_InsertItem((HWND)component,&lv);
#endif
			for(n=0,p1=c,p2=p1; n<ncolumns; ++n,++p2,p1=p2) {
				p2 = strchr(p2,sep);
				if(p2) *p2 = '\0';
debug_output("column[%d]: %s\n",n,p1);
#ifdef USE_GTK
				g_value_init(&val,G_TYPE_STRING);
				g_value_set_string(&val,p1); 
				gtk_list_store_set_value(store,&iter,n,&val);
				g_value_unset(&val);
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
				char2w(wstr,p1,strlen(p1)+1);
				ListView_SetItemText((HWND)component,lv.iItem,n,wstr);
#else
				ListView_SetItemText((HWND)component,lv.iItem,n,p1);
#endif
#endif
			}
			free(c);
		}
	}
}

const char *aList::getColumn(int n) {
	if(!item) item = (char *)malloc(129);
	if(component) {
#ifdef USE_GTK
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
		GtkTreeModel *model;
		GtkTreeIter iter;
		*item = '\0';
		if(gtk_tree_selection_get_selected(selection,&model,&iter))
			gtk_tree_model_get(model,&iter,n,&item,-1);
#endif
#ifdef USE_WIN32
		LVITEM lv = { 0 };
		lv.mask = LVIF_TEXT;
		lv.iItem = ListView_GetSelectionMark((HWND)component);
		lv.iSubItem = n;
		if(lv.iItem==-1) *item = '\0';
		else {
#ifdef USE_WCHAR
			wchar_t wstr[129];
			lv.pszText = wstr;
#else
			lv.pszText = item;
#endif
			lv.cchTextMax = 128;
			ListView_GetItem((HWND)component,&lv);
#ifdef USE_WCHAR
			w2char(item,wstr,128);
#endif
		}
#endif
		return item;
	}
	return 0;
}


