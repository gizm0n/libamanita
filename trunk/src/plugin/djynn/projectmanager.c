

#include <stdlib.h>
#include <string.h>
#include <ipomoea/string_t.h>
#include "djynn.h"



enum {
	DOCS_LIST_FILENAME,
	DOCS_LIST_COLS,
};

enum {
	PROJECT_TREE_ICON,
	PROJECT_TREE_FILENAME,
	PROJECT_TREE_PATH,
	PROJECT_TREE_DATA,
	PROJECT_TREE_COLS,
};

enum {
	STATUS_EXPANDED		= 0x0001,
};

static gchar *stocks[] = {
	GTK_STOCK_EXECUTE,
	GTK_STOCK_DIRECTORY,
	GTK_STOCK_FILE,
};


static GtkWidget *popup_menu = NULL;
static GtkWidget *project_vbox = NULL;
static GtkWidget *open_documents_list = NULL;
static GtkListStore *open_documents_store = NULL;
static GtkWidget *workspaces_list = NULL;
static GtkWidget *project_scroll = NULL;
static GtkWidget *project_tree_view = NULL;
static GtkTreeStore *project_tree_store = NULL;
static guint project_tree_view_button = 0;
static guint32 project_tree_view_time = 0;


static void free_open_document(djynn_open_document *d) {
	if(d->name!=NULL) free(d->name);
	if(d->directory!=NULL) free(d->directory);
	free(d);
}

static void free_open_documents() {
	djynn_open_document *d1 = djynn.pm.docs,*d2;
	for(; d1!=NULL; d1=d2) {
		d2 = d1->next;
		free_open_document(d1);
	}
	djynn.pm.docs = NULL;
	if(djynn.pm.docs_table!=NULL) ht_delete(djynn.pm.docs_table);
	djynn.pm.docs_table = NULL;
}

static void free_project_files(djynn_project_file *f) {
	djynn_project_file *f1,*f2;
	if(f->name!=NULL) free(f->name);
	if(f->directory!=NULL) free(f->directory);
	if(f->path!=NULL) gtk_tree_path_free(f->path);
	for(f1=f->files; f1!=NULL; f1=f2) {
		f2 = f1->next;
		free_project_files(f1);
	}
	free(f);
}

static void remove_project_file(djynn_project_file *f) {
	djynn_project_file *p1 = f->parent,*f1;
	if(p1==NULL) {
		f1 = (djynn_project_file *)djynn.pm.prj;
		if(f==f1) {
			int i;
			djynn.pm.prj = (djynn_project *)f->next,djynn.pm.nprj--;
			for(i=1,p1=f->next; p1!=NULL; i++,p1=p1->next) ((djynn_project *)p1)->index = i;
			return;
		}
	} else {
		f1 = p1->files;
		if(f1==f) { p1->files = f->next;return; }
	}
	for(; f1->next!=f; f1=f1->next);
	f1->next = f->next;
}

static void free_projects() {
	djynn_project *p1,*p2;
	for(p1=djynn.pm.prj; p1!=NULL; p1=p2) {
		p2 = p1->next;
		if(p1->config_filename!=NULL) g_free(p1->config_filename);
		if(p1->geany_project_filename!=NULL) free(p1->geany_project_filename);
		free_project_files((djynn_project_file *)p1);
	}
	ht_delete(djynn.pm.prj_files_table);
	djynn.pm.nprj = 0;
	djynn.pm.prj = NULL;
	djynn.pm.prj_files_table = NULL;
}


static void open_documents_list_changed_cb(GtkComboBox *combo,gpointer data) {
	int i;
	gint n = gtk_combo_box_get_active(combo);
	djynn_open_document *d = djynn.pm.docs;
	if(n==0) return;
	gtk_combo_box_set_active(combo,0);
	for(i=1; d!=NULL; i++,d=d->next) if(i==n) break;
	if(d!=NULL) {
		char filename[1024];
		sprintf(filename,"%s" G_DIR_SEPARATOR_S "%s",d->directory,d->name);
//fprintf(djynn.log,"open_documents_list_changed_cb(%s)\n",filename);
//fflush(djynn.log);
		document_open_file(filename,FALSE,NULL,NULL);
	}
}

static void workspaces_list_changed_cb(GtkComboBox *combo,gpointer data) {
	gint n = gtk_combo_box_get_active(combo);
	if(n==0) return;
	else {
		char str[257],*p1;
		djynn_projectmanager_save_workspace();
		djynn_open_config();
		djynn_config_set_int(djynn.str.workspace,djynn.str.workspace,n);
		sprintf(str,djynn.str.workspace_d,n);
fprintf(djynn.log,"workspaces_list_changed_cb(n=%d,key=%s)\n",n,str);
fflush(djynn.log);
		p1 = djynn_config_get_str(djynn.str.workspace,str);
		sprintf(str,"pm_%s",p1);
		g_free(p1);
		free(djynn.workspace);
		djynn.workspace = strdup(str);
fprintf(djynn.log,"workspaces_list_changed_cb(workspace=%s)\n",djynn.workspace);
fflush(djynn.log);
		djynn_projectmanager_load_workspace();
		gtk_combo_box_set_active(combo,0);
		djynn_save_config();
		djynn_close_config();
	}
}

static void open_documents_open(GeanyDocument *doc) {
	djynn_open_document *d = NULL;
//fprintf(djynn.log,"open_documents_open(%s)\n",doc->file_name);
//fflush(djynn.log);

	if(doc->file_name==NULL || !g_path_is_absolute(doc->file_name)) return;

	if(djynn.pm.docs_table==NULL) djynn.pm.docs_table = ht_new(0);
	else d = (djynn_open_document *)ht_get(djynn.pm.docs_table,doc->file_name);
	if(d==NULL) {
		djynn_open_document *d1;

		char str[1024],*p;
		strcpy(str,doc->file_name);

		d = (djynn_open_document *)malloc(sizeof(djynn_open_document));
		ht_put(djynn.pm.docs_table,str,d);

		p = strrchr(str,G_DIR_SEPARATOR);
		if(p!=NULL) {
			*p++ = '\0';
			d->name = strdup(p);
			d->directory = strdup(str);
		} else {
			d->name = strdup(str);
			d->directory = NULL;
		}

		if(djynn.pm.docs==NULL) djynn.pm.docs = d,d1 = NULL,d->next = NULL;
		else {
			if(stricmp(d->name,djynn.pm.docs->name)<0) d1 = NULL,d->next = djynn.pm.docs,djynn.pm.docs = d;
			else {
				for(d1=djynn.pm.docs; 1; d1=d1->next) {
					if(d1->next==NULL || stricmp(d1->next->name,d->name)>0) {
						d->next = d1->next,d1->next = d;
						break;
					}
				}
			}
		}
		d->doc = doc;

		if(d->next==NULL) gtk_list_store_append(open_documents_store,&d->iter);
		else gtk_list_store_insert_before(open_documents_store,&d->iter,&d->next->iter);
		gtk_list_store_set(open_documents_store,&d->iter,DOCS_LIST_FILENAME,d->name,-1);
	}
}

static void open_documents_close(GeanyDocument *doc) {
	djynn_open_document *d0 = NULL,*d1 = djynn.pm.docs;
	for(; d1!=NULL; d0=d1,d1=d1->next) if(d1->doc==doc) break;
	if(d1!=NULL) {
		if(d0==NULL) djynn.pm.docs = d1->next;
		else d0->next = d1->next;
		gtk_list_store_remove(open_documents_store,&d1->iter);
		free_open_document(d1);
	}
}

static djynn_project_file *add_file(djynn_project_file *f,GeanyDocument *doc) {
	djynn_project_file *f1;
fprintf(djynn.log,"add_file(doc->file_name=%s)\n",doc->file_name);
fflush(djynn.log);
	if(doc->file_name==NULL || !g_path_is_absolute(doc->file_name)) return f;
	else {
		char dir[256],*file;
		strcpy(dir,doc->file_name);
		file = strrchr(dir,G_DIR_SEPARATOR),*file++ = '\0';
		for(f1=(f->type==DJYNN_PM_FILE? f->parent->files : f->files); f1!=NULL; f1=f1->next)
			if(f1->type==DJYNN_PM_FILE && strcmp(f1->name,file)==0 && strcmp(f1->directory,dir)==0) break;
		if(f1!=NULL) return f;

		f1 = (djynn_project_file *)malloc(sizeof(djynn_project_file));
		f1->status = 0;
		f1->type = DJYNN_PM_FILE;
		f1->name = strdup(file);
		f1->directory = strdup(dir);
		f1->depth = f->depth+(f->type==DJYNN_PM_FILE? 0 : 1);
		f1->path = NULL;
		f1->files = NULL;
		f1->next = NULL;
fprintf(djynn.log,"add_file(f->name=%s,f->type=%d,f1->name=%s)\n",f->name,f->type,f1->name);
fflush(djynn.log);
		if(f->type==DJYNN_PM_FILE) f1->next = f->next,f->next = f1,f1->parent = f->parent;
		else f1->next = f->files,f->files = f1,f1->parent = f;
		return f1;
	}
}

static void add_folder(djynn_project_file *f,const gchar *name) {
	djynn_project_file *f1;
fprintf(djynn.log,"add_folder(name=%s)\n",name);
fflush(djynn.log);
	for(f1=(f->type==DJYNN_PM_FILE? f->parent->files : f->files); f1!=NULL; f1=f1->next)
		if(f1->type==DJYNN_PM_FOLDER && strcmp(f1->name,name)==0) break;
	if(f1!=NULL) return;

	f1 = (djynn_project_file *)malloc(sizeof(djynn_project_file));
	f1->status = 0;
	f1->type = DJYNN_PM_FOLDER;
	f1->name = strdup(name);
	f1->directory = NULL;
	f1->depth = f->depth+(f->type==DJYNN_PM_FILE? 0 : 1);
	f1->path = NULL;
	f1->files = NULL;
	f1->next = NULL;
fprintf(djynn.log,"add_file(f->name=%s,f->type=%d,f1->name=%s)\n",f->name,f->type,f1->name);
fflush(djynn.log);
	if(f->type==DJYNN_PM_FILE) f = f->parent;
	f1->next = f->files,f->files = f1,f1->parent = f;
}


static djynn_project_file *get_selected_file() {
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(project_tree_view));
	GtkTreeModel *model;
	GtkTreeIter iter;
	djynn_project_file *f = NULL;
	if(gtk_tree_selection_get_selected(selection,&model,&iter)) {
		gtk_tree_model_get(model,&iter,PROJECT_TREE_DATA,&f,-1);
	}
	return f;
}

static void open_selected_document() {
	djynn_project_file *f = get_selected_file();
	if(f!=NULL && f->type==DJYNN_PM_FILE) {
		char filename[1024];
		sprintf(filename,"%s" G_DIR_SEPARATOR_S "%s",f->directory,f->name);
//fprintf(djynn.log,"project_tree_view_selection_changed_cb(%s)\n",filename);
//fflush(djynn.log);
		document_open_file(filename,FALSE,NULL,NULL);
	}
}

static void popup_open_clicked(GtkMenuItem *menuitem,gpointer data) {
	open_selected_document();
}

static void popup_clicked(GtkMenuItem *menuitem,gpointer data) {
	djynn_projectmanager_action((int)((intptr_t)data));
}

static void popup_hide_sidebar() {
	keybindings_send_command(GEANY_KEY_GROUP_VIEW,GEANY_KEYS_VIEW_SIDEBAR);
}


static GtkWidget *create_popup_menu() {
	int menu_items[] = {
		-1,
		DJYNN_PM_NEW_PROJECT,
		DJYNN_PM_NEW_FOLDER,
		DJYNN_PM_ADD_FILE,
		DJYNN_PM_ADD_OPEN_FILES,
		DJYNN_PM_DELETE,
		-1,
		DJYNN_PM_MOVE_UP,
		DJYNN_PM_MOVE_DOWN,
		-1,
		DJYNN_PM_RELOAD_WORKSPACE,
		-1,
		-2
	};
	int i,n;
	GtkWidget *menu;
	GtkWidget *item;
	djynn_menu_item *m;

	menu = gtk_menu_new();

	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,NULL);
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu),item);
	g_signal_connect(item,"activate",G_CALLBACK(popup_open_clicked),NULL);

	for(i=0,n=menu_items[i]; n!=-2; n=menu_items[++i]) {
		if(n==-1) {
			item = gtk_separator_menu_item_new();
			gtk_widget_show(item);
			gtk_container_add(GTK_CONTAINER(menu),item);
		} else {
			m = &djynn.menu_items[n];
			if(m->menu_stock==NULL) item = gtk_menu_item_new_with_label(m->label);
			else item = ui_image_menu_item_new(m->menu_stock,m->label);
			gtk_widget_show(item);
			gtk_container_add(GTK_CONTAINER(menu),item);
			g_signal_connect(item,"activate",G_CALLBACK(popup_clicked),(gpointer)((intptr_t)n));
		}
	}

	item = ui_image_menu_item_new(GTK_STOCK_CLOSE,_("Hide Sidebar"));
	gtk_widget_show(item);
	gtk_container_add(GTK_CONTAINER(menu),item);
	g_signal_connect(item,"activate",G_CALLBACK(popup_hide_sidebar),NULL);

	return menu;
}


static gboolean project_tree_view_clicked_cb(GtkWidget *widget,GdkEventButton *event,gpointer data) {
	project_tree_view_button = event->button;
	project_tree_view_time = event->time;
	if(project_tree_view_button==1 && event->type==GDK_2BUTTON_PRESS) {
		djynn_project_file *f = get_selected_file();
		if(f->type==DJYNN_PM_PROJECT) {
			document_open_file(((djynn_project *)f)->config_filename,FALSE,NULL,NULL);
			//project_load_file(((djynn_project *)f)->geany_project_filename);
		}
	} else if(project_tree_view_button==3) {
		if(popup_menu==NULL) popup_menu = create_popup_menu();
		gtk_menu_popup(GTK_MENU(popup_menu),NULL,NULL,NULL,NULL,project_tree_view_button,project_tree_view_time);
	}
	return FALSE;
}

static void project_tree_view_selection_changed_cb(GtkWidget *widget,gpointer data) {
	if(project_tree_view_button==1) {
		open_selected_document();
	} else if(project_tree_view_button==3) {
	}
}


static void read_project_files(char *data,djynn_project_file *f) {
//fprintf(djynn.log,"read_project_files()\n");
//fflush(djynn.log);
	int i,depth;
	char *dir,*file,*endl;
	djynn_project_file *f1,*f2;
	char str[1024];

	while(data!=NULL && *data!='\0') {
//fprintf(djynn.log,"read_project_files(1)\n");
//fflush(djynn.log);

		for(; *data!='\0' && *data=='\n'; data++);
		if(*data=='\0') break;

		for(depth=1; *data!='\0' && *data=='\t'; depth++,data++);
		if(*data=='\0') break;

		endl = strchr(data,'\n');
		if(endl!=NULL) *endl++ = '\0';

//fprintf(djynn.log,"read_project_files(depth=%d,data=%s)\n",depth,data);
//fflush(djynn.log);

		dir = data;
		file = strrchr(data,G_DIR_SEPARATOR);
		if(file!=NULL) *file++ = '\0';

		f1 = (djynn_project_file *)malloc(sizeof(djynn_project_file));
		f1->status = 0;
		if(*dir=='+') dir++,f1->type = DJYNN_PM_FOLDER,f1->status |= STATUS_EXPANDED;
		else if(*dir=='-') dir++,f1->type = DJYNN_PM_FOLDER;
		else f1->type = DJYNN_PM_FILE;
		if(file!=NULL) {
			f1->name = strdup(file),f1->directory = strdup(dir);
			sprintf(str,"%s" G_DIR_SEPARATOR_S "%s",dir,file);
			ht_put(djynn.pm.prj_files_table,str,f1);
		} else f1->name = strdup(dir),f1->directory = NULL;

		if(depth>f->depth+1) depth = f->depth+1;
		f1->depth = depth;
		f1->path = NULL;
		f1->files = NULL;
		f1->next = NULL;

//fprintf(djynn.log,"read_project_files(depth=%d,dir=%s,name=%s)\n",depth,f1->directory,f1->name);
//fflush(djynn.log);


		if(depth==f->depth) f->next = f1,f1->parent = f->parent;
		else if(depth>f->depth) f1->parent = f,f->files = f1;
		else if(depth<f->depth) {
			for(i=depth; i<f->depth+1; f=f->parent);
			f1->parent = f;
			for(f2=f->files; f2->next!=NULL; f2=f2->next);
			f2->next = f1;
		}
		f = f1;

		data = endl;
//fprintf(djynn.log,"read_project_files(2)\n");
//fflush(djynn.log);
	}
//fprintf(djynn.log,"read_project_files(done)\n");
//fflush(djynn.log);
}

static void write_project(djynn_project *p) {
	int i;
	gboolean e;
	FILE *fp = fopen(p->config_filename,"w");
	djynn_project_file *f;
	e = gtk_tree_view_row_expanded(GTK_TREE_VIEW(project_tree_view),p->path);
	fprintf(fp,"%c%s\n%s\n%s\n",e? '+' : '-',p->name,p->directory,p->geany_project_filename);
fflush(fp);
fprintf(djynn.log,"write_project(config_filename=%s,name=%c%s,dir=%s)\n",p->config_filename,e? '+' : '-',p->name,p->directory);
fflush(djynn.log);
	for(f=p->files; f!=NULL; ) {
		for(i=1; i<f->depth; i++) fputc('\t',fp);
		if(f->type==DJYNN_PM_FOLDER) {
			e = gtk_tree_view_row_expanded(GTK_TREE_VIEW(project_tree_view),f->path);
			fprintf(fp,"%c%s\n",e? '+' : '-',f->name);
fflush(fp);
fprintf(djynn.log,"write_project(%c%s)\n",e? '+' : '-',f->name);
fflush(djynn.log);
		} else {
			fprintf(fp,"%s" G_DIR_SEPARATOR_S "%s\n",f->directory,f->name);
fflush(fp);
fprintf(djynn.log,"write_project(%s" G_DIR_SEPARATOR_S "%s)\n",f->directory,f->name);
fflush(djynn.log);
		}
		if(f->files!=NULL) f = f->files;
		else if(f->next!=NULL) f = f->next;
		else {
			for(f=f->parent; f!=NULL && f->next==NULL; f=f->parent);
			if(f==NULL || f->depth==0) break;
			f = f->next;
		}
	}
	fclose(fp);
}

static void expand_project_files() {
	djynn_project_file *f1 = (djynn_project_file *)djynn.pm.prj,*f2;

//fprintf(djynn.log,"expand_project_files(1)\n");
//fflush(djynn.log);
	while(f1!=NULL) {
//fprintf(djynn.log,"expand_project_files(name=%s)\n",f1->name);
//fflush(djynn.log);
		if((f1->status&STATUS_EXPANDED) && f1->path!=NULL)
			gtk_tree_view_expand_row(GTK_TREE_VIEW(project_tree_view),f1->path,FALSE);

		if(f1->files!=NULL) f1 = f1->files;
		else if(f1->next!=NULL) f1 = f1->next;
		else if(f1->parent!=NULL) {
			for(f2=f1->parent; f2->next==NULL && f2->parent!=NULL; f2=f2->parent);
			f1 = f2->next;
		} else f1 = NULL;
	}
//fprintf(djynn.log,"expand_project_files(done)\n");
//fflush(djynn.log);
}

static void create_tree(GtkTreeStore *store,GtkWidget *cellview,GtkTreeIter *iter_parent,djynn_project_file *f) {
	GdkPixbuf *pixbuf;
	GtkTreeIter iter;
	char str[1025];
	for(; f!=NULL; f=f->next) {
//fprintf(djynn.log,"create_tree(name=%s,%p)\n",f->name,f);
//fflush(djynn.log);
		pixbuf = gtk_widget_render_icon(cellview,stocks[f->type],GTK_ICON_SIZE_MENU,NULL);
		if(f->type==DJYNN_PM_PROJECT) sprintf(str,"Project: %s\nConfig File: %s",f->name,((djynn_project *)f)->config_filename);
		else if(f->type==DJYNN_PM_FOLDER) sprintf(str,"%s",f->name);
		else sprintf(str,"%s" G_DIR_SEPARATOR_S "%s",f->directory,f->name);
		gtk_tree_store_append(store,&iter,iter_parent);
		gtk_tree_store_set(store,&iter,
				PROJECT_TREE_ICON,pixbuf,
				PROJECT_TREE_FILENAME,f->name,
				PROJECT_TREE_PATH,str,
				PROJECT_TREE_DATA,(gpointer)f,-1);
		if(f->path!=NULL) gtk_tree_path_free(f->path);
		f->path = gtk_tree_model_get_path(GTK_TREE_MODEL(store),&iter);
		gdk_pixbuf_unref(pixbuf);
		if(f->files!=NULL) create_tree(store,cellview,&iter,f->files);
	}
}

static GtkTreeModel* create_tree_model() {
	GtkWidget *cellview = gtk_cell_view_new();
//fprintf(djynn.log,"create_tree_model(1)\n");
//fflush(djynn.log);
	if(project_tree_store==NULL) {
		project_tree_store = gtk_tree_store_new(PROJECT_TREE_COLS,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_POINTER);
	} else gtk_tree_store_clear(project_tree_store);
	create_tree(project_tree_store,cellview,NULL,(djynn_project_file *)djynn.pm.prj);
//fprintf(djynn.log,"create_tree_model(done)\n");
//fflush(djynn.log);
	return GTK_TREE_MODEL(project_tree_store);
}


static void document_activate_cb(GObject *obj,GeanyDocument *doc,gpointer user_data) {
	djynn_project_file *f;

//fprintf(djynn.log,"document_activate_cb(file_name=%s)\n",doc->file_name);
//fflush(djynn.log);
	if(doc->file_name==NULL || !g_path_is_absolute(doc->file_name)) return;

	f = ht_get(djynn.pm.prj_files_table,doc->file_name);
	if(f) {
//fprintf(djynn.log,"document_activate_cb(dir=%s,name=%s)\n",f->directory,f->name);
//fflush(djynn.log);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(geany->main_widgets->sidebar_notebook),djynn.pm.page_number);
		gtk_tree_view_expand_to_path(GTK_TREE_VIEW(project_tree_view),f->path);
		gtk_tree_view_set_cursor(GTK_TREE_VIEW(project_tree_view),f->path,NULL,FALSE);
	}/* else {
		GtkTreePath *path = gtk_tree_path_new_from_string("-1");
		gtk_tree_view_set_cursor(GTK_TREE_VIEW(project_tree_view),path,NULL,FALSE);
		gtk_tree_path_free(path);
	}*/
}

static void document_open_cb(GObject *obj,GeanyDocument *doc,gpointer user_data) {
	open_documents_open(doc);
	document_activate_cb(obj,doc,user_data);
}

static void document_close_cb(GObject *obj,GeanyDocument *doc,gpointer user_data) {
//fprintf(djynn.log,"document_close_cb()\n");
//fflush(djynn.log);
	open_documents_close(doc);
}

static void load_projects() {
	int i,n;
	char key[64];
	char *value,*d,*d1,*d2,c;
	FILE *fp;
	djynn_project *p1 = NULL,*p2 = NULL;

fprintf(djynn.log,"load_projects(1)\n");
fflush(djynn.log);
	djynn_open_config();

	if(djynn.pm.prj!=NULL) free_projects();

	djynn.pm.nprj = djynn_config_get_int(djynn.workspace,djynn.str.project_n);
	djynn.pm.prj_files_table = ht_new(0);
fprintf(djynn.log,"project_n=%d\n",djynn.pm.nprj);
fflush(djynn.log);
	if(djynn.pm.nprj>0) {
		for(i=1; i<=djynn.pm.nprj; i++) {
			sprintf(key,djynn.str.project_d,i);
			value = djynn_config_get_str(djynn.workspace,key);
fprintf(djynn.log,"%s=%s\n",key,value);
fflush(djynn.log);
			fp = fopen(value,"r");
			if(!fp) continue;
			fseek(fp,0,SEEK_END);
			n = ftell(fp);
			fseek(fp,0,SEEK_SET);
			d = (char *)malloc(n+1);
			for(d1=d; (c=fgetc(fp))!=EOF; *d1++=c);
			*d1 = '\0';
			fclose(fp);

			p2 = (djynn_project *)malloc(sizeof(djynn_project));
			memset(p2,0,sizeof(djynn_project));
			d1 = d,d2 = strchr(d1,'\n'),*d2++ = '\0';
			p2->status = 0;
			if(*d1=='+') p2->status |= STATUS_EXPANDED,d1++;
			else if(*d1=='-') d1++;
			p2->name = strdup(d1);
			d1 = d2,d2 = strchr(d1,'\n'),*d2++ = '\0';
			p2->directory = strdup(d1);
			p2->type = 0;
			p2->depth = 0;
			p2->path = NULL;
			p2->parent = NULL;
			p2->files = NULL;
			p2->next = NULL;
			p2->index = i;
			p2->config_filename = value;
			d1 = d2,d2 = strchr(d1,'\n'),*d2++ = '\0';
			p2->geany_project_filename = strdup(d1);
fprintf(djynn.log,"name=%s, directory=%s, index=%d\n",p2->name,p2->directory,p2->index);
fflush(djynn.log);

			read_project_files(d2,(djynn_project_file *)p2);
			if(djynn.pm.prj==NULL) djynn.pm.prj = p1 = p2;
			else p1->next = p2,p1 = p2;
			free(d);
		}
	}

	djynn_close_config();
fprintf(djynn.log,"load_projects(done)\n");
fflush(djynn.log);
}

void djynn_projectmanager_save_workspace() {
	djynn_project *p = djynn.pm.prj;
	for(; p!=NULL; p=p->next) write_project(p);
}

void djynn_projectmanager_load_workspace() {
	load_projects();
	create_tree_model();
	expand_project_files();
}

static void djynn_projectmanager_add_file() {
	add_file(get_selected_file(),document_get_current());
	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
}

void djynn_projectmanager_add_folder(const gchar *name) {
	add_folder(get_selected_file(),name);
	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
}

static void djynn_projectmanager_add_open_files() {
	int i;
	GeanyDocument *doc;
	djynn_project_file *f = get_selected_file();
	for(i=0; 1; i++) {
		doc = document_get_from_page(i);
		if(doc==NULL) break;
		f = add_file(f,doc);
	}
	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
}

static void djynn_projectmanager_delete() {
	djynn_project_file *f = get_selected_file();
	if(f==NULL) return;
	else {
		GtkWidget *dlg;
		gchar *title;
		gchar *message;
		gint r = 0;
		if(f->type==DJYNN_PM_PROJECT) {
			title = "Delete Project";
			message = "Are you sure you want to remove the project \"%s\" from this workspace?";
		} else if(f->type==DJYNN_PM_FOLDER) {
			title = "Delete Folder";
			message = "Are you sure you want to delete this folder \"%s\" and all its files and subfolders?";
		} else {
			title = "Delete File";
			message = "Are you sure you want to delete this file \"%s\" from the project?";
		}
		dlg = gtk_message_dialog_new(GTK_WINDOW(geany->main_widgets->window),GTK_DIALOG_MODAL,
				GTK_MESSAGE_INFO,GTK_BUTTONS_OK_CANCEL,message,f->name);
		gtk_window_set_title(GTK_WINDOW(dlg),title);
		r = gtk_dialog_run(GTK_DIALOG(dlg));
		gtk_widget_destroy(dlg);
		if(r!=GTK_RESPONSE_OK) return;
	}
	if(f->type==DJYNN_PM_PROJECT) {
		int i = ((djynn_project *)f)->index,n;
		char key[64],*p;
		djynn_open_config();
		n = djynn_config_get_int(djynn.workspace,djynn.str.project_n);
//		sprintf(key,djynn.str.project_d,i);
//		p = djynn_config_get_str(djynn.workspace,key);
//		remove(p);
		for(; i<n-1; i++) {
			sprintf(key,djynn.str.project_d,i+1);
			p = djynn_config_get_str(djynn.workspace,key);
			sprintf(key,djynn.str.project_d,i);
			djynn_config_set_str(djynn.workspace,key,p);
		}
		sprintf(key,djynn.str.project_d,n);
		djynn_config_remove(djynn.workspace,key);
		djynn_config_set_int(djynn.workspace,djynn.str.project_n,n-1);
		djynn_save_config();
		djynn_close_config();
	}
	remove_project_file(f);
	free_project_files(f);
	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
}

static void djynn_projectmanager_move_up() {
	djynn_project_file *f = get_selected_file();
	if(f==NULL) return;
	if(f->type==DJYNN_PM_PROJECT) {
		int i = ((djynn_project *)f)->index;
		if(i<=1) return;
		else {
			char key1[64],key2[64],*p1,*p2;
			djynn_open_config();
			sprintf(key1,djynn.str.project_d,i-1);
			p1 = djynn_config_get_str(djynn.workspace,key1);
			sprintf(key2,djynn.str.project_d,i);
			p2 = djynn_config_get_str(djynn.workspace,key2);
			djynn_config_set_str(djynn.workspace,key1,p2);
			djynn_config_set_str(djynn.workspace,key2,p1);
			djynn_save_config();
			djynn_close_config();
		}
	} else {
		djynn_project_file *f1 = f->parent->files;
		if(f1==f) return;
		if(f1->next==f) f1->next = f->next,f->next = f1,f->parent->files = f;
		else {
			for(; f1->next->next!=f; f1=f1->next);
			f1->next->next = f->next,f->next = f1->next,f1->next = f;
		}
	}
	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
}

static void djynn_projectmanager_move_down() {
	djynn_project_file *f = get_selected_file();
	if(f==NULL) return;
	if(f->type==DJYNN_PM_PROJECT) {
		int i = ((djynn_project *)f)->index,n;
		djynn_open_config();
		n = djynn_config_get_int(djynn.workspace,djynn.str.project_n);
		if(i>=n) {
			djynn_close_config();
			return;
		} else {
			char key1[64],key2[64],*p1,*p2;
			sprintf(key1,djynn.str.project_d,i);
			p1 = djynn_config_get_str(djynn.workspace,key1);
			sprintf(key2,djynn.str.project_d,i+1);
			p2 = djynn_config_get_str(djynn.workspace,key2);
			djynn_config_set_str(djynn.workspace,key1,p2);
			djynn_config_set_str(djynn.workspace,key2,p1);
			djynn_save_config();
			djynn_close_config();
		}
	} else {
		djynn_project_file *f1 = f->parent->files;
		if(f->next==NULL) return;
		if(f1==f) f1 = f->next,f->parent->files = f1,f->next = f1->next,f1->next = f;
		else {
			for(; f1->next!=f; f1=f1->next);
			f1->next = f->next,f1 = f->next,f->next = f1->next,f1->next = f;
		}
	}
	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
}

static void item_activate_cb(GtkMenuItem *menuitem, gpointer gdata) {
	djynn_projectmanager_action((int)((intptr_t)gdata));
}


void djynn_projectmanager_action(int id) {
	switch(id) {
		case DJYNN_PM_NEW_WORKSPACE:break;
		case DJYNN_PM_DELETE_WORKSPACE:break;
		case DJYNN_PM_RELOAD_WORKSPACE:djynn_projectmanager_load_workspace();break;
		case DJYNN_PM_NEW_PROJECT:djynn_projectmanager_open_new_project_dlg();break;
		case DJYNN_PM_NEW_FOLDER:djynn_projectmanager_open_new_folder_dlg();break;
		case DJYNN_PM_ADD_FILE:djynn_projectmanager_add_file();break;
		case DJYNN_PM_ADD_OPEN_FILES:djynn_projectmanager_add_open_files();break;
		case DJYNN_PM_DELETE:djynn_projectmanager_delete();break;
		case DJYNN_PM_MOVE_UP:djynn_projectmanager_move_up();break;
		case DJYNN_PM_MOVE_DOWN:djynn_projectmanager_move_down();break;
	}
}

void djynn_projectmanager_init(GeanyData *data,int *menu_index) {
	int menu_items[] = {
		DJYNN_PM_NEW_WORKSPACE,
		DJYNN_PM_DELETE_WORKSPACE,
		-1,
		DJYNN_PM_NEW_PROJECT,
		DJYNN_PM_NEW_FOLDER,
		DJYNN_PM_ADD_FILE,
		DJYNN_PM_ADD_OPEN_FILES,
		-1,
		DJYNN_PM_DELETE,
		-2
	};
	djynn_menu_item *m;
	GtkCellRenderer *icon_renderer;
	GtkCellRenderer *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;
	GtkWidget *menu;
	GtkWidget *menu_item;
	GtkTreeIter iter;

	int i,n;
	gchar key[257],*value;
	GeanyDocument *doc;

	load_projects();

	project_vbox = gtk_vbox_new(FALSE,0);

	open_documents_store = gtk_list_store_new(DOCS_LIST_COLS,G_TYPE_STRING);
	gtk_list_store_append(open_documents_store,&iter);
	gtk_list_store_set(open_documents_store,&iter,DOCS_LIST_FILENAME,"Open Documents:",-1);
	for(i=0; 1; i++) {
		doc = document_get_from_page(i);
		if(doc==NULL) break;
		open_documents_open(doc);
	}
	open_documents_list = gtk_combo_box_new_with_model(GTK_TREE_MODEL(open_documents_store));
	g_object_unref(open_documents_store);
	text_renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(open_documents_list),text_renderer,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(open_documents_list),text_renderer,"text",0,NULL);

	g_signal_connect(open_documents_list,"changed",G_CALLBACK(open_documents_list_changed_cb),NULL);
	gtk_combo_box_set_active(GTK_COMBO_BOX(open_documents_list),0);
	ui_widget_set_tooltip_text(open_documents_list,
			_("A list of all open documents sorted alphabetically."));
	gtk_box_pack_start(GTK_BOX(project_vbox),open_documents_list,FALSE,TRUE,0);

	workspaces_list = gtk_combo_box_new_text();
	g_signal_connect(workspaces_list,"changed",G_CALLBACK(workspaces_list_changed_cb),NULL);
	gtk_combo_box_append_text(GTK_COMBO_BOX(workspaces_list),"Workspaces:");
	n = djynn_config_get_int(djynn.str.workspace,djynn.str.workspace_n);
	for(i=1; i<=n; i++) {
		sprintf(key,djynn.str.workspace_d,i);
		value = djynn_config_get_str(djynn.str.workspace,key);
		gtk_combo_box_append_text(GTK_COMBO_BOX(workspaces_list),value);
		g_free(value);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(workspaces_list),0);
	ui_widget_set_tooltip_text(workspaces_list,
			_("Switch between workspaces."));
	gtk_box_pack_start(GTK_BOX(project_vbox),workspaces_list,FALSE,TRUE,1);

	project_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(project_scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(project_scroll),GTK_SHADOW_IN);

	project_tree_view = gtk_tree_view_new_with_model(create_tree_model());
	g_object_unref(project_tree_store);
	column = gtk_tree_view_column_new();

	icon_renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column,icon_renderer,FALSE);
	gtk_tree_view_column_set_attributes(column,icon_renderer,"pixbuf",PROJECT_TREE_ICON,NULL);

	text_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column,text_renderer,TRUE);
	gtk_tree_view_column_add_attribute(column,text_renderer,"text",PROJECT_TREE_FILENAME);

	gtk_tree_view_append_column(GTK_TREE_VIEW(project_tree_view),column);

	ui_widget_modify_font_from_string(project_tree_view,geany->interface_prefs->tagbar_font);

	g_signal_connect(project_tree_view,"button-press-event",G_CALLBACK(project_tree_view_clicked_cb),NULL);
	g_object_set(project_tree_view,"has-tooltip",TRUE,"tooltip-column",PROJECT_TREE_PATH,NULL);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(project_tree_view));
	g_signal_connect(selection,"changed",G_CALLBACK(project_tree_view_selection_changed_cb),NULL);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(project_tree_view),FALSE);
	gtk_container_add(GTK_CONTAINER(project_scroll),project_tree_view);
	gtk_box_pack_start(GTK_BOX(project_vbox),project_scroll,TRUE,TRUE,0);

	gtk_widget_show_all(project_vbox);
	djynn.pm.page_number = gtk_notebook_append_page(GTK_NOTEBOOK(geany->main_widgets->sidebar_notebook),
			project_vbox,gtk_label_new(_("Projects")));

	expand_project_files();

	if(*menu_index>0) {
		gtk_container_add(GTK_CONTAINER(djynn.tools_menu),gtk_separator_menu_item_new());
		*menu_index += 1;
	}

	menu = gtk_menu_new();
	for(i=0,n=menu_items[i]; n!=-2; n=menu_items[++i]) {
		if(n==-1) gtk_container_add(GTK_CONTAINER(menu),gtk_separator_menu_item_new());
		else {
			m = &djynn.menu_items[n];
			if(m->menu_stock==NULL) m->menu_item = gtk_menu_item_new_with_label(m->label);
			else m->menu_item = ui_image_menu_item_new(m->menu_stock,m->label);
			g_signal_connect(m->menu_item,"activate",G_CALLBACK(item_activate_cb),(gpointer)((intptr_t)n));
			gtk_menu_append(GTK_MENU(menu),m->menu_item);
		}
	}
	menu_item = gtk_menu_item_new_with_label("Project Manager");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),menu);
	gtk_menu_append(GTK_MENU(djynn.tools_menu),menu_item);
	*menu_index += 1;


	plugin_signal_connect(geany_plugin,NULL,"document-activate",TRUE,
			(GCallback)&document_activate_cb,NULL);
	plugin_signal_connect(geany_plugin,NULL,"document-open",TRUE,
			(GCallback)&document_open_cb,NULL);
	plugin_signal_connect(geany_plugin,NULL,"document-close",TRUE,
			(GCallback)&document_close_cb,NULL);
}


void djynn_projectmanager_cleanup() {
	free_open_documents();
	djynn_projectmanager_save_workspace();
	free_projects();
	gtk_widget_destroy(project_vbox);
}
