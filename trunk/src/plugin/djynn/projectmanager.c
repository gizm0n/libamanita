

#include <stdlib.h>
#include <string.h>
#include <glib/gstdio.h>
#ifdef HAVE_GIO
#include <gio/gio.h>
#endif
#include <ipomoea/string_t.h>
#include "djynn.h"



enum {
	WS_LIST_ICON,
	WS_LIST_WORKSPACE,
	WS_LIST_COLS,
};

enum {
	SESS_LIST_ICON,
	SESS_LIST_SESSION,
	SESS_LIST_COLS,
};

enum {
	DOCS_LIST_ICON,
	DOCS_LIST_FILENAME,
	DOCS_LIST_COLS,
};

enum {
	PRJ_TREE_ICON,
	PRJ_TREE_FILENAME,
	PRJ_TREE_PATH,
	PRJ_TREE_DATA,
	PRJ_TREE_COLS,
};

enum {
	STATUS_EXPANDED		= 0x0001,
};

static GdkPixbuf *icons[DJYNN_PM_TYPES];

static const gchar *file_types = ""
"asm adb ads c h cpp cxx c++ cc hpp hxx h++ hh cs d di f for ftn f77 f90 "
"f95 f03 bas bi glsl frag vert hs lhs hx as java jsp pas pp inc dpr dpk "
"vala vapi vhd vhdl fe js lua m mq4 pl perl pm agi pod php php3 php4 php5 "
"phtml py pyw R r rb rhtml ruby sh ksh zsh ash bash tcl tk wish css htm "
"html shtml hta htd htt cfm xml sgml xsl xslt xsd xhtml txt cmake ctest "
"conf ini config rc cfg diff patch rej debdiff dpatch nsi nsh po pot tex "
"sty idx ltx rest reST rst sql yaml yml";
static const gchar *exclude_dirs = ""
"CMakeFiles "
".svn";

static GtkWidget *popup_menu = NULL;
static GtkWidget *workspace_list = NULL;
static GtkListStore *workspace_store = NULL;
static GtkWidget *sess_list = NULL;
static GtkListStore *sess_store = NULL;
/* Use built in "Show Document List" instead.
static GtkWidget *doc_list = NULL;
static GtkListStore *doc_store = NULL;
*/
static GtkWidget *prj_vbox = NULL;
static GtkWidget *prj_scroll = NULL;
static GtkWidget *prj_tree_view = NULL;
static GtkTreeStore *prj_tree_store = NULL;
static guint prj_tree_view_button = 0;
static guint32 prj_tree_view_time = 0;


/* Use built in "Show Document List" instead.
static void doc_free(djynn_open_document *d) {
	if(d->path!=NULL) g_free(d->path);
	if(d->name!=NULL) g_free(d->name);
	g_free(d);
}

static void sess_free() {
	djynn_open_document *d1 = djynn.pm.docs,*d2;
	for(; d1!=NULL; d1=d2) {
		d2 = d1->next;
		doc_free(d1);
	}
	djynn.pm.docs = NULL;
	if(djynn.pm.docs_table!=NULL) ht_delete(djynn.pm.docs_table);
	djynn.pm.docs_table = NULL;
}
*/

static void prj_free_files(djynn_project_file *f) {
	djynn_project_file *f1,*f2;
//fprintf(djynn.log,"prj_free_files(path=%s,name=%s)\n",f->path,f->name);
//fflush(djynn.log);
	if(f->path!=NULL) g_free(f->path);
	if(f->name!=NULL) g_free(f->name);
	if(f->tree_path!=NULL) gtk_tree_path_free(f->tree_path);
	for(f1=f->files; f1!=NULL; f1=f2) {
		f2 = f1->next;
		prj_free_files(f1);
	}
	g_free(f);
}

static void prj_file_remove(djynn_project_file *f) {
	djynn_project_file *p1 = f->parent,*f1;
	if(p1==NULL) {
		f1 = (djynn_project_file *)djynn.pm.prj;
		if(f==f1) {
			gint i;
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

static void prj_free_all() {
	djynn_project *p1,*p2;
	for(p1=djynn.pm.prj; p1!=NULL; p1=p2) {
		p2 = p1->next;
		if(p1->config_filename!=NULL) g_free(p1->config_filename);
		if(p1->geany_project_filename!=NULL) g_free(p1->geany_project_filename);
		prj_free_files((djynn_project_file *)p1);
	}
	ht_delete(djynn.pm.prj_files_table);
	djynn.pm.nprj = 0;
	djynn.pm.prj = NULL;
	djynn.pm.prj_files_table = NULL;
}

/*static void sess_write_from_geany_cfg() {
	gint i,n;
	gchar key[64],*s,*p,*fn;
	gchar **arr;
	gsize len;
	FILE *fp;
	GKeyFile *conf = g_key_file_new();;

	fn = g_strconcat(geany->app->configdir,G_DIR_SEPARATOR_S,"geany.conf",NULL);
	g_key_file_load_from_file(conf,fn,G_KEY_FILE_NONE,NULL);
	g_free(fn);

	djynn_cfg_open();
	n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.session);
	sprintf(key,djynn.str.session_d,n);
	s = djynn_cfg_get_str(djynn.str.session,key);
	p = strchr(s,':'),*p = '\0',++p;
	fn = g_strconcat(geany->app->configdir,G_DIR_SEPARATOR_S,"plugins",G_DIR_SEPARATOR_S,
			djynn.str.djynn,G_DIR_SEPARATOR_S,"session",s,".txt",NULL);
fprintf(djynn.log,"sess_write_from_geany_cfg(s=%s,p=%s,fn=%s)\n",s,p,fn);
fflush(djynn.log);
	g_free(s);
	fp = fopen(fn,"w");

	if(fp!=NULL) {
		gchar str[1025];
		gint c = g_key_file_get_integer(conf,"files","current_page",NULL);
		for(n=0; 1; ++n) {
			sprintf(key,"FILE_NAME_%d",n);
			arr = g_key_file_get_string_list(conf,"files",key,&len,NULL);
			if(arr==NULL) break;
			else {
				for(i=0,p=arr[7]; *p!='\0'; ++i,++p)
					if(*p=='%') str[i] = (xtoi(p[1])<<4)|xtoi(p[2]),p += 2;
					else str[i] = *p;
				str[i] = '\0';
				fprintf(fp,"%s;%d;%s\n",arr[0],n==c,str);
fprintf(djynn.log,"sess_write_from_geany_cfg(file=%s)\n",str);
fflush(djynn.log);
			}
		}
		fclose(fp);
	} else perror(fn);
	g_free(fn);
	djynn_cfg_close();

	g_key_file_free(conf);
}*/

static void sess_write() {
	gint n;
	gchar key[64],*s,*p,*fn;
	FILE *fp;
	djynn_cfg_open();
	n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.session);
	sprintf(key,djynn.str.session_d,n);
	s = djynn_cfg_get_str(djynn.str.session,key);
	p = strchr(s,':'),*p = '\0',++p;
	fn = g_strconcat(geany->app->configdir,G_DIR_SEPARATOR_S,"plugins",G_DIR_SEPARATOR_S,
			djynn.str.djynn,G_DIR_SEPARATOR_S,"session",s,".txt",NULL);
fprintf(djynn.log,"sess_write(s=%s,p=%s,fn=%s)\n",s,p,fn);
fflush(djynn.log);
	g_free(s);
	fp = fopen(fn,"w");
	if(fp!=NULL) {
		GeanyDocument *doc = document_get_current();
		gint c = doc->index;
		for(n=0; 1; ++n) {
			doc = document_get_from_page(n);
			if(doc==NULL) break;
			if(doc->file_name!=NULL && g_path_is_absolute(doc->file_name)) {
				fprintf(fp,"%d;%d;%s\n",sci_get_current_position(doc->editor->sci),doc->index==c,doc->file_name);
fprintf(djynn.log,"sess_write(file=%s)\n",doc->file_name);
fflush(djynn.log);
			}
		}
		fclose(fp);
	} else perror(fn);
	g_free(fn);
	djynn_cfg_close();
}

void sess_open(const gchar *fn) {
	FILE *fp = fopen(fn,"r");
	if(fp!=NULL) {
		gint pos,sel;
		gchar str[1024],*p1,*p2,open[1024] = "";
		GeanyDocument *doc;
		document_close_all();
		while(!feof(fp)) {
			p1 = fgets(str,1024,fp);
			if(p1==NULL) break;
			p2 = strchr(p1,';'),*p2 = '\0',++p2;
			pos = atoi(p1),p1 = p2;
			p2 = strchr(p1,';'),*p2 = '\0',++p2;
			sel = atoi(p1),p1 = p2;
			if((p2=strchr(p1,'\n'))!=NULL) *p2 = '\0';
			if((p2=strchr(p1,'\r'))!=NULL) *p2 = '\0';
fprintf(djynn.log,"sess_open(pos=%d,sel=%d,path=%s)\n",pos,sel,p1);
fflush(djynn.log);
			doc = document_open_file(p1,FALSE,NULL,NULL);
			sci_set_current_position(doc->editor->sci,pos,TRUE);
			if(sel) strcpy(open,p1);
		}
		fclose(fp);
		if(*open!='\0') document_open_file(open,FALSE,NULL,NULL);
	} else perror(fn);
}

/* Use built in "Show Document List" instead.
static void sess_open_doc(GeanyDocument *doc) {
	djynn_open_document *d = NULL;
//fprintf(djynn.log,"sess_open_doc(%s)\n",doc->file_name);
//fflush(djynn.log);

	if(doc->file_name==NULL || !g_path_is_absolute(doc->file_name)) return;

	if(djynn.pm.docs_table==NULL) djynn.pm.docs_table = ht_new(0);
	else d = (djynn_open_document *)ht_get(djynn.pm.docs_table,doc->file_name);
	if(d==NULL) {
		djynn_open_document *d1;

		gchar str[1024],*p;
		strcpy(str,doc->file_name);

		d = (djynn_open_document *)g_malloc(sizeof(djynn_open_document));
		ht_put(djynn.pm.docs_table,str,d);

		d->path = g_strdup(str);
		p = strrchr(str,G_DIR_SEPARATOR);
		if(p!=NULL) *p++ = '\0',d->name = g_strdup(p);
		else d->name = g_strdup(str);

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

		if(d->next==NULL) gtk_list_store_append(doc_store,&d->iter);
		else gtk_list_store_insert_before(doc_store,&d->iter,&d->next->iter);
		gtk_list_store_set(doc_store,&d->iter,DOCS_LIST_ICON,icons[DJYNN_PM_FILE],DOCS_LIST_FILENAME,d->name,-1);
	}
}

static void sess_close_doc(GeanyDocument *doc) {
	djynn_open_document *d0 = NULL,*d1 = djynn.pm.docs;
	for(; d1!=NULL; d0=d1,d1=d1->next) if(d1->doc==doc) break;
	if(d1!=NULL) {
		if(d0==NULL) djynn.pm.docs = d1->next;
		else d0->next = d1->next;
		gtk_list_store_remove(doc_store,&d1->iter);
		doc_free(d1);
	}
}
*/

static gint sort_files_compare(const void *a,const void * b) {
	djynn_project_file *f1 = *(djynn_project_file **)a;
	djynn_project_file *f2 = *(djynn_project_file **)b;
//fprintf(djynn.log,"sort_files_compare(name1=%s,type1=%d,name2=%s,type2=%d)\n",f1->name,f1->type,f2->name,f2->type);
//fflush(djynn.log);
	if(f1->type==DJYNN_PM_FILE && f2->type!=DJYNN_PM_FILE) return 1;
	if(f1->type!=DJYNN_PM_FILE && f2->type==DJYNN_PM_FILE) return -1;
	return strcmp(f1->name,f2->name);
}

static void sort_files(djynn_project_file *f,gboolean r) {
	if(f!=NULL && f->files!=NULL) {
		gint i,n;
		djynn_project_file *f0,*f1;
		for(n=0,f1=f->files; f1!=NULL; ++n) f1=f1->next;
		if(n>1) {
			djynn_project_file *files[n];
			for(i=0,f1=f->files; i<n; ++i,f1=f1->next) files[i] = f1;
			qsort(files,n,sizeof(djynn_project_file *),sort_files_compare);
			for(i=0,f0=NULL; i<n; ++i,f0=f1) {
				f1 = files[i];
//fprintf(djynn.log,"sort_files(name=%s)\n",f1->name);
//fflush(djynn.log);
				if(f0==NULL) f->files = f1;
				else f0->next = f1;
				f1->next = NULL;
				if(f1->type!=DJYNN_PM_FILE && f1->files!=NULL && r) sort_files(f1,r);
			}
		}
	}
}

static djynn_project_file *add_file(djynn_project_file *f,const gchar *path,const gchar *name) {
	djynn_project_file *f1;
//fprintf(djynn.log,"add_file(path=%s)\n",path);
//fflush(djynn.log);
	for(f1=(f->type==DJYNN_PM_FILE? f->parent->files : f->files); f1!=NULL; f1=f1->next)
		if(f1->type==DJYNN_PM_FILE && strcmp(f1->path,path)==0) break;
	if(f1!=NULL) return f;

	f1 = (djynn_project_file *)g_malloc(sizeof(djynn_project_file));
	f1->status = 0;
	f1->type = DJYNN_PM_FILE;
	f1->path = g_strdup(path);
	f1->name = g_strdup(name);
	f1->depth = f->depth+(f->type==DJYNN_PM_FILE? 0 : 1);
	f1->tree_path = NULL;
	f1->files = NULL;
	f1->next = NULL;
//fprintf(djynn.log,"add_file(f->name=%s,f->type=%d,f1->name=%s)\n",f->name,f->type,f1->name);
//fflush(djynn.log);
	if(f->type==DJYNN_PM_FILE) f1->next = f->next,f->next = f1,f1->parent = f->parent;
	else f1->next = f->files,f->files = f1,f1->parent = f;
	return f1;
}

static djynn_project_file *add_document(djynn_project_file *f,GeanyDocument *doc) {
//fprintf(djynn.log,"add_file(doc->file_name=%s)\n",doc->file_name);
//fflush(djynn.log);
	if(doc->file_name==NULL || !g_path_is_absolute(doc->file_name)) return f;
	return add_file(f,doc->file_name,strrchr(doc->file_name,G_DIR_SEPARATOR));
}

static void add_folder(djynn_project_file *f,const gchar *path,const gchar *name,gboolean files,gboolean folders) {
	djynn_project_file *f1;
//fprintf(djynn.log,"add_folder(path=%s,name=%s,files=%s,folders=%s)\n",path,name,files? "true" : "false",folders? "true" : "false");
//fflush(djynn.log);
	for(f1=(f->type==DJYNN_PM_FILE? f->parent->files : f->files); f1!=NULL; f1=f1->next)
		if(f1->type==DJYNN_PM_FOLDER && strcmp(f1->name,name)==0) break;
	if(f1!=NULL) return;

	f1 = (djynn_project_file *)g_malloc(sizeof(djynn_project_file));
	f1->status = 0;
	f1->type = DJYNN_PM_FOLDER;
	f1->path = NULL;
	f1->name = g_strdup(name);
	f1->depth = f->depth+(f->type==DJYNN_PM_FILE? 0 : 1);
	f1->tree_path = NULL;
	f1->files = NULL;
	f1->next = NULL;
//fprintf(djynn.log,"add_file(f->name=%s,f->type=%d,f1->name=%s)\n",f->name,f->type,f1->name);
//fflush(djynn.log);
	if(f->type==DJYNN_PM_FILE) f = f->parent;
	f1->next = f->files,f->files = f1,f1->parent = f;

	if(files || folders) {
		GDir *d = g_dir_open(path,0,NULL);
		const gchar *nm;
		gchar str[1024],*ext;
		if(d!=NULL) while((nm=g_dir_read_name(d))!=NULL) {
			sprintf(str,"%s%c%s",path,G_DIR_SEPARATOR,nm);
			if(g_file_test(str,G_FILE_TEST_IS_DIR)) {
//fprintf(djynn.log,"add_folder(path=%s,name=%s)\n",str,nm);
//fflush(djynn.log);
				if(folders && strstr(exclude_dirs,nm)==NULL)
					add_folder(f1,str,nm,files,folders);
			} else if(files && (ext=strrchr(nm,'.'))!=NULL && strstr(file_types,++ext)!=NULL) {
//fprintf(djynn.log,"add_file(path=%s,name=%s)\n",str,nm);
//fflush(djynn.log);
				add_file(f1,str,nm);
			}
		}
		if(f1->files!=NULL) sort_files(f1,TRUE);
	}
}


static djynn_project_file *get_selected_file() {
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(prj_tree_view));
	GtkTreeModel *model;
	GtkTreeIter iter;
	djynn_project_file *f = NULL;
	if(gtk_tree_selection_get_selected(selection,&model,&iter)) {
		gtk_tree_model_get(model,&iter,PRJ_TREE_DATA,&f,-1);
	}
	return f;
}

static void open_selected_document() {
	djynn_project_file *f = get_selected_file();
	if(f!=NULL && f->type==DJYNN_PM_FILE) {
//fprintf(djynn.log,"project_tree_view_selection_changed_cb(%s)\n",filename);
//fflush(djynn.log);
		document_open_file(f->path,FALSE,NULL,NULL);
	}
}

static void popup_open_clicked(GtkMenuItem *menuitem,gpointer data) {
	open_selected_document();
}

static void popup_clicked(GtkMenuItem *menuitem,gpointer data) {
	djynn_pm_action((gint)((intptr_t)data));
}

static void popup_hide_sidebar() {
	keybindings_send_command(GEANY_KEY_GROUP_VIEW,GEANY_KEYS_VIEW_SIDEBAR);
}


static GtkWidget *create_popup_menu() {
	gint menu_items[] = {
		DJYNN_PM_OPEN_EXTERNALLY,
		DJYNN_PM_OPEN_TERMINAL,
		-1,
		DJYNN_PM_NEW_PROJECT,
		DJYNN_PM_NEW_FOLDER,
		DJYNN_PM_ADD_FILE,
		DJYNN_PM_ADD_OPEN_FILES,
		DJYNN_PM_DELETE,
		-1,
		DJYNN_PM_SORT_FILES,
		DJYNN_PM_MOVE_UP,
		DJYNN_PM_MOVE_DOWN,
		-1,
		DJYNN_PM_RELOAD_WORKSPACE,
		-1,
		-2
	};
	gint i,n;
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

static void prj_read(djynn_project *p,gchar *data) {
//fprintf(djynn.log,"prj_read()\n");
//fflush(djynn.log);
	gint i,depth;
	gchar *path,*name,*endl;
	djynn_project_file *f = (djynn_project_file *)p,*f1,*f2;
	gchar str[1024];

	while(data!=NULL && *data!='\0') {
//fprintf(djynn.log,"prj_read(1)\n");
//fflush(djynn.log);

		for(; *data!='\0' && *data=='\n'; data++);
		if(*data=='\0') break;

		for(depth=1; *data!='\0' && *data=='\t'; depth++,data++);
		if(*data=='\0') break;

		endl = strchr(data,'\n');
		if(endl!=NULL) *endl++ = '\0';

//fprintf(djynn.log,"prj_read(depth=%d,data=%s)\n",depth,data);
//fflush(djynn.log);

		f1 = (djynn_project_file *)g_malloc(sizeof(djynn_project_file));
		f1->status = 0;

		path = data;
		if(*path=='+') ++path,f1->type = DJYNN_PM_FOLDER,f1->status |= STATUS_EXPANDED;
		else if(*path=='-') ++path,f1->type = DJYNN_PM_FOLDER;
		else f1->type = DJYNN_PM_FILE;
		if(f1->type==DJYNN_PM_FILE) {
			if(*path==G_DIR_SEPARATOR) f1->path = g_strdup(path);
			else {
				sprintf(str,"%s%c%s",p->path,G_DIR_SEPARATOR,path);
				f1->path = g_strdup(str);
			}
			name = strrchr(path,G_DIR_SEPARATOR);
			f1->name = g_strdup(name==NULL? path : name+1);
			ht_put(djynn.pm.prj_files_table,f1->path,f1);
		} else {
			f1->path = NULL;
			f1->name = g_strdup(path);
		}
		if(depth>f->depth+1) depth = f->depth+1;
		f1->depth = depth;
		f1->tree_path = NULL;
		f1->files = NULL;
		f1->next = NULL;

//fprintf(djynn.log,"prj_read(depth=%d,path=%s,name=%s)\n",depth,f1->path,f1->name);
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
//fprintf(djynn.log,"prj_read(2)\n");
//fflush(djynn.log);
	}
//fprintf(djynn.log,"prj_read(done)\n");
//fflush(djynn.log);
}

static void prj_write(djynn_project *p) {
	FILE *fp = fopen(p->config_filename,"w");
	if(fp!=NULL) {
		gint i,n = strlen(p->path);
		gboolean e = gtk_tree_view_row_expanded(GTK_TREE_VIEW(prj_tree_view),p->tree_path);
		djynn_project_file *f;
		gchar *s;
		fprintf(fp,"%c%s\n%s\n%s\n",e? '+' : '-',p->name,p->path,p->geany_project_filename);
//fflush(fp);
//fprintf(djynn.log,"prj_write(config_filename=%s,name=%c%s,dir=%s)\n",p->config_filename,e? '+' : '-',p->name,p->path);
//fflush(djynn.log);
		for(f=p->files; f!=NULL; ) {
			for(i=1; i<f->depth; ++i) fputc('\t',fp);
			if(f->type==DJYNN_PM_FOLDER) {
				e = gtk_tree_view_row_expanded(GTK_TREE_VIEW(prj_tree_view),f->tree_path);
				fprintf(fp,"%c%s\n",e? '+' : '-',f->name);
//fflush(fp);
//fprintf(djynn.log,"prj_write(%c%s)\n",e? '+' : '-',f->name);
//fflush(djynn.log);
			} else {
				if(strncmp(f->path,p->path,n)==0 && f->path[n]==G_DIR_SEPARATOR) s = &f->path[n+1];
				else s = f->path;
				fprintf(fp,"%s\n",s);
//fflush(fp);
//fprintf(djynn.log,"prj_write(%s)\n",f->path);
//fflush(djynn.log);
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
	} else perror(p->config_filename);
}

static void prj_expand_tree() {
	djynn_project_file *f1 = (djynn_project_file *)djynn.pm.prj,*f2;
//fprintf(djynn.log,"prj_expand_tree(1)\n");
//fflush(djynn.log);
	while(f1!=NULL) {
//fprintf(djynn.log,"prj_expand_tree(name=%s)\n",f1->name);
//fflush(djynn.log);
		if((f1->status&STATUS_EXPANDED) && f1->tree_path!=NULL)
			gtk_tree_view_expand_row(GTK_TREE_VIEW(prj_tree_view),f1->tree_path,FALSE);

		if(f1->files!=NULL) f1 = f1->files;
		else if(f1->next!=NULL) f1 = f1->next;
		else if(f1->parent!=NULL) {
			for(f2=f1->parent; f2->next==NULL && f2->parent!=NULL; f2=f2->parent);
			f1 = f2->next;
		} else f1 = NULL;
	}
//fprintf(djynn.log,"prj_expand_tree(done)\n");
//fflush(djynn.log);
}

static GdkPixbuf *utils_pixbuf_from_path(const gchar *path) {
#if defined(HAVE_GIO) && GTK_CHECK_VERSION(2,14,0)
	GIcon *icon;
	GdkPixbuf *ret = NULL;
	GtkIconInfo *info;
	gchar *ctype;
	gint width;
	ctype = g_content_type_guess(path,NULL,0,NULL);
	icon = g_content_type_get_icon(ctype);
	g_free(ctype);
	if(icon!=NULL) {
		gtk_icon_size_lookup(GTK_ICON_SIZE_MENU,&width,NULL);
		info = gtk_icon_theme_lookup_by_gicon(gtk_icon_theme_get_default(),icon,width,GTK_ICON_LOOKUP_USE_BUILTIN);
		g_object_unref(icon);
		if(!info) return icons[DJYNN_PM_FILE];
		ret = gtk_icon_info_load_icon(info,NULL);
		gtk_icon_info_free(info);
	}
	return ret;
#else
	return icons[DJYNN_PM_FILE];
#endif
}

static void create_tree(GtkTreeStore *store,GtkWidget *cellview,GtkTreeIter *iter_parent,djynn_project_file *f) {
	GtkTreeIter iter;
	GdkPixbuf *icon;
	gchar str[1025];
	for(; f!=NULL; f=f->next) {
//fprintf(djynn.log,"create_tree(name=%s,%p)\n",f->name,f);
//fflush(djynn.log);
		icon = icons[f->type];
		if(f->type==DJYNN_PM_PROJECT) sprintf(str,"%s: %s\n%s: %s",_("Project"),f->name,_("Config File"),((djynn_project *)f)->config_filename);
		else if(f->type==DJYNN_PM_FOLDER) strcpy(str,f->name);
		else {
			strcpy(str,f->path);
			if(djynn.conf.show_icons==0) icon = NULL;
			else if(djynn.conf.show_icons==2) icon = utils_pixbuf_from_path(f->path);
		}
		gtk_tree_store_append(store,&iter,iter_parent);
		gtk_tree_store_set(store,&iter,
				PRJ_TREE_ICON,icon,
				PRJ_TREE_FILENAME,f->name,
				PRJ_TREE_PATH,str,
				PRJ_TREE_DATA,(gpointer)f,-1);
		if(f->tree_path!=NULL) gtk_tree_path_free(f->tree_path);
		f->tree_path = gtk_tree_model_get_path(GTK_TREE_MODEL(store),&iter);
		if(f->files!=NULL) create_tree(store,cellview,&iter,f->files);
		if(icon!=NULL && icon!=icons[f->type]) g_object_unref(icon);
	}
}

static GtkTreeModel* create_tree_model() {
	GtkWidget *cellview = gtk_cell_view_new();
//fprintf(djynn.log,"create_tree_model(1)\n");
//fflush(djynn.log);
	if(prj_tree_store==NULL) {
		prj_tree_store = gtk_tree_store_new(PRJ_TREE_COLS,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_POINTER);
	} else gtk_tree_store_clear(prj_tree_store);
	create_tree(prj_tree_store,cellview,NULL,(djynn_project_file *)djynn.pm.prj);
//fprintf(djynn.log,"create_tree_model(done)\n");
//fflush(djynn.log);
	return GTK_TREE_MODEL(prj_tree_store);
}

static void prj_read_all() {
	gint i,n;
	gchar key[64];
	gchar *value,*d,*d1,*d2,c;
	FILE *fp;
	djynn_project *p1 = NULL,*p2 = NULL;

//fprintf(djynn.log,"prj_read_all(workspace=%s,key=%s)\n",djynn.workspace,djynn.workspace_key);
//fflush(djynn.log);
	djynn_cfg_open();

	if(djynn.pm.prj!=NULL) prj_free_all();

	djynn.pm.nprj = djynn_cfg_get_int(djynn.workspace_key,djynn.str.project_n);
	djynn.pm.prj_files_table = ht_new(0);
//fprintf(djynn.log,"project_n=%d\n",djynn.pm.nprj);
//fflush(djynn.log);
	if(djynn.pm.nprj>0) {
		for(i=1; i<=djynn.pm.nprj; i++) {
			sprintf(key,djynn.str.project_d,i);
			value = djynn_cfg_get_str(djynn.workspace_key,key);
//fprintf(djynn.log,"%s=%s\n",key,value);
//fflush(djynn.log);
			if(!(fp=fopen(value,"r"))) {
				perror(value);
				continue;
			}
			fseek(fp,0,SEEK_END);
			n = ftell(fp);
			fseek(fp,0,SEEK_SET);
			d = (gchar *)g_malloc(n+1);
			for(d1=d; (c=fgetc(fp))!=EOF; *d1++=c);
			*d1 = '\0';
			fclose(fp);

			p2 = (djynn_project *)g_malloc(sizeof(djynn_project));
			memset(p2,0,sizeof(djynn_project));
			d1 = d,d2 = strchr(d1,'\n'),*d2++ = '\0';
			p2->status = 0;
			if(*d1=='+') p2->status |= STATUS_EXPANDED,d1++;
			else if(*d1=='-') d1++;
			p2->name = g_strdup(d1);
			d1 = d2,d2 = strchr(d1,'\n'),*d2++ = '\0';
			p2->path = g_strdup(d1);
			p2->type = DJYNN_PM_PROJECT;
			p2->depth = 0;
			p2->tree_path = NULL;
			p2->parent = NULL;
			p2->files = NULL;
			p2->next = NULL;
			p2->index = i;
			p2->config_filename = value;
			d1 = d2,d2 = strchr(d1,'\n'),*d2++ = '\0';
			p2->geany_project_filename = g_strdup(d1);
//fprintf(djynn.log,"name=%s, path=%s, index=%d\n",p2->name,p2->path,p2->index);
//fflush(djynn.log);

			prj_read(p2,d2);
			if(djynn.pm.prj==NULL) djynn.pm.prj = p1 = p2;
			else p1->next = p2,p1 = p2;
			g_free(d);
		}
	}
	djynn_cfg_close();
//fprintf(djynn.log,"prj_read_all(done)\n");
//fflush(djynn.log);
}

void djynn_pm_ws_set(gint index) {
	gint n;
	djynn_cfg_open();
	n = djynn_cfg_get_int(djynn.str.workspace,djynn.str.workspace_n);
	if(index>=1 && index<=n) {
		n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.workspace);
		if(index!=n) {
			gchar key[257],*p1,*p2;
			djynn_pm_ws_save();
			n = djynn_cfg_get_int(djynn.str.workspace,djynn.str.workspace_n);
			djynn_cfg_set_int(djynn.str.djynn,djynn.str.workspace,index);
			sprintf(key,djynn.str.workspace_d,index);
//fprintf(djynn.log,"djynn_pm_ws_set(n=%d,key=%s)\n",index,key);
//fflush(djynn.log);
			g_free(djynn.workspace);
			g_free(djynn.workspace_key);
			p1 = djynn_cfg_get_str(djynn.str.workspace,key);
			p2 = strchr(p1,':'),*p2 = '\0',++p2;
			sprintf(key,"%s_%s",djynn.str.workspace,p1);
			djynn.workspace = g_strdup(p2);
			djynn.workspace_key = g_strdup(key);
			g_free(p1);
//fprintf(djynn.log,"djynn_pm_ws_set(workspace=%s)\n",djynn.workspace);
//fflush(djynn.log);
			djynn_pm_ws_load();
			djynn_cfg_save();
		}
	}
	djynn_cfg_close();
}

void djynn_pm_ws_list_select(gint index) {
	gtk_combo_box_set_active(GTK_COMBO_BOX(workspace_list),index-1);
}

void djynn_pm_ws_list_remove(gint index) {
	GtkTreeIter iter;
	if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(workspace_store),&iter,NULL,index-1))
		gtk_list_store_remove(workspace_store,&iter);
}

void djynn_pm_ws_list_add(const gchar *name) {
	GtkTreeIter iter;
	gtk_list_store_append(workspace_store,&iter);
	gtk_list_store_set(workspace_store,&iter,WS_LIST_ICON,icons[DJYNN_PM_WORKSPACE],WS_LIST_WORKSPACE,name,-1);
}

void djynn_pm_ws_list_set(gint index,const gchar *name) {
	GtkTreeIter iter;
	if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(workspace_store),&iter,NULL,index-1))
		gtk_list_store_set(workspace_store,&iter,WS_LIST_ICON,icons[DJYNN_PM_WORKSPACE],WS_LIST_WORKSPACE,name,-1);
}

void djynn_pm_ws_save() {
	djynn_project *p = djynn.pm.prj;
	for(; p!=NULL; p=p->next) prj_write(p);
}

void djynn_pm_ws_load() {
	prj_read_all();
	create_tree_model();
	prj_expand_tree();
}

void djynn_pm_sess_set(gint index) {
	gint n;
	gchar key[64],*s,*p,*fn;
	djynn_cfg_open();
	n = djynn_cfg_get_int(djynn.str.session,djynn.str.session_n);
	if(index>=1 && index<=n) {
		n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.session);
		if(index!=n) {
fprintf(djynn.log,"djynn_pm_sess_set(n=%d)\n",index);
fflush(djynn.log);
			djynn_cfg_set_int(djynn.str.djynn,djynn.str.session,index);
			djynn_cfg_save();

			sprintf(key,djynn.str.session_d,index);
			s = djynn_cfg_get_str(djynn.str.session,key);
			p = strchr(s,':'),*p = '\0',++p;
			fn = g_strconcat(geany->app->configdir,G_DIR_SEPARATOR_S,"plugins",G_DIR_SEPARATOR_S,
					djynn.str.djynn,G_DIR_SEPARATOR_S,"session",s,".txt",NULL);
fprintf(djynn.log,"djynn_pm_sess_set(s=%s,p=%s,fn=%s)\n",s,p,fn);
fflush(djynn.log);
			g_free(s);
			sess_open(fn);
			g_free(fn);
		}
	}
	djynn_cfg_close();
}

void djynn_pm_sess_list_select(gint index) {
	gtk_combo_box_set_active(GTK_COMBO_BOX(sess_list),index-1);
}

void djynn_pm_sess_list_remove(gint index) {
	GtkTreeIter iter;
	if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(sess_store),&iter,NULL,index-1))
		gtk_list_store_remove(sess_store,&iter);
}

void djynn_pm_sess_list_add(const gchar *name) {
	GtkTreeIter iter;
	gtk_list_store_append(sess_store,&iter);
	gtk_list_store_set(sess_store,&iter,SESS_LIST_ICON,icons[DJYNN_PM_SESSION],SESS_LIST_SESSION,name,-1);
}

void djynn_pm_sess_list_set(gint index,const gchar *name) {
	GtkTreeIter iter;
	if(gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(sess_store),&iter,NULL,index-1))
		gtk_list_store_set(sess_store,&iter,SESS_LIST_ICON,icons[DJYNN_PM_SESSION],SESS_LIST_SESSION,name,-1);
}

static void djynn_pm_add_file(djynn_project_file *f) {
	add_document(f,document_get_current());
	djynn_pm_ws_save();
	djynn_pm_ws_load();
}

void djynn_pm_folder_add(djynn_project_file *f,const gchar *path,const gchar *name,gboolean files,gboolean folders) {
	add_folder(f,path,name,files,folders);
	djynn_pm_ws_save();
	djynn_pm_ws_load();
}

static void djynn_pm_sort_files(djynn_project_file *f) {
	if(f!=NULL && f->files!=NULL) {
		sort_files(f,TRUE);
		djynn_pm_ws_save();
		djynn_pm_ws_load();
	}
}

static void djynn_pm_add_open_files(djynn_project_file *f) {
	gint i;
	GeanyDocument *doc;
	for(i=0; 1; ++i) {
		doc = document_get_from_page(i);
		if(doc==NULL) break;
		f = add_document(f,doc);
	}
	djynn_pm_ws_save();
	djynn_pm_ws_load();
}

static void djynn_pm_ws_delete() {
	gint index = 1,n;
	gchar *title = _("Delete Workspace");
	gint r = 0;
	r = djynn_msgbox_ask(title,_("Are you sure you want to delete the entire workspace \"%s\" and all its projects?"),djynn.workspace);
	if(r==GTK_RESPONSE_OK) {
		djynn_cfg_open();
		n = djynn_cfg_get_int(djynn.str.workspace,djynn.str.workspace_n);
		if(n==1) djynn_msgbox_warn(title,_("There must be at least one workspace!"),NULL);
		else {
			gchar key[257];
			strcpy(key,djynn.workspace_key);
//fprintf(djynn.log,"djynn_pm_ws_delete1(index=%d,workspace=%s)\n",index,djynn.workspace);
//fflush(djynn.log);
			index = djynn_cfg_get_int(djynn.str.djynn,djynn.str.workspace);
			djynn_pm_ws_list_select(index>1? index-1 : index+1);
			djynn_cfg_remove_group(key);
			djynn_cfg_remove_from_list(djynn.str.workspace,djynn.str.workspace,index);
			djynn_pm_ws_list_remove(index);
			djynn_cfg_save();
		}
		djynn_cfg_close();
	}
}

static void djynn_pm_sess_delete() {
	gint index = 1,n;
	gchar *title = _("Delete Session");
	gint r = 0;
	gchar key[64],str[257],*p1,*p2,*fn;

	djynn_cfg_open();
	n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.session);
	sprintf(key,djynn.str.session_d,n);
	p1 = djynn_cfg_get_str(djynn.str.session,key);
	p2 = strchr(p1,':'),*p2 = '\0',++p2;
	strcpy(key,p1);
	strcpy(str,p2);
	g_free(p1);

	r = djynn_msgbox_ask(title,_("Are you sure you want to delete the session \"%s\"?"),str);
	if(r==GTK_RESPONSE_OK) {
		n = djynn_cfg_get_int(djynn.str.session,djynn.str.session_n);
		if(n==1) djynn_msgbox_warn(title,_("There must be at least one session!"),NULL);
		else {
//fprintf(djynn.log,"djynn_pm_ws_delete1(index=%d,workspace=%s)\n",index,djynn.workspace);
//fflush(djynn.log);
			index = djynn_cfg_get_int(djynn.str.djynn,djynn.str.session);
			djynn_pm_sess_list_select(index>1? index-1 : index+1);
			djynn_cfg_remove_from_list(djynn.str.session,djynn.str.session,index);
			djynn_pm_sess_list_remove(index);
			djynn_cfg_save();

			fn = g_strconcat(geany->app->configdir,G_DIR_SEPARATOR_S,"plugins",G_DIR_SEPARATOR_S,
					djynn.str.djynn,G_DIR_SEPARATOR_S,"session",key,".txt",NULL);
			g_remove(fn);
			g_free(fn);
		}
	}
	djynn_cfg_close();
}

static void djynn_pm_delete(djynn_project_file *f) {
	if(f==NULL) return;
	else {
		gchar *title;
		gchar *message;
		gint r = 0;
		if(f->type==DJYNN_PM_PROJECT) {
			title = _("Delete Project");
			message = _("Are you sure you want to remove the project \"%s\" from this workspace?");
		} else if(f->type==DJYNN_PM_FOLDER) {
			title = _("Delete Folder");
			message = _("Are you sure you want to remove this folder \"%s\" and all its files and subfolders from the project?");
		} else {
			title = _("Delete File");
			message = _("Are you sure you want to remove this file \"%s\" from the project?");
		}
		r = djynn_msgbox_ask(title,message,f->name);
		if(r!=GTK_RESPONSE_OK) return;
	}
	if(f->type==DJYNN_PM_PROJECT) {
		djynn_cfg_open();
		djynn_cfg_remove_from_list(djynn.workspace_key,djynn.str.project,((djynn_project *)f)->index);
		djynn_cfg_save();
		djynn_cfg_close();
	}
	prj_file_remove(f);
	prj_free_files(f);
	djynn_pm_ws_save();
	djynn_pm_ws_load();
}

static void djynn_pm_move_up(djynn_project_file *f) {
	if(f==NULL) return;
	if(f->type==DJYNN_PM_PROJECT) {
		gint i = ((djynn_project *)f)->index;
		if(i<=1) return;
		else {
			gchar key1[64],key2[64],*p1,*p2;
			djynn_cfg_open();
			sprintf(key1,djynn.str.project_d,i-1);
			p1 = djynn_cfg_get_str(djynn.workspace_key,key1);
			sprintf(key2,djynn.str.project_d,i);
			p2 = djynn_cfg_get_str(djynn.workspace_key,key2);
			djynn_cfg_set_str(djynn.workspace_key,key1,p2);
			djynn_cfg_set_str(djynn.workspace_key,key2,p1);
			djynn_cfg_save();
			djynn_cfg_close();
			g_free(p1);
			g_free(p2);
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
	djynn_pm_ws_save();
	djynn_pm_ws_load();
}

static void djynn_pm_move_down(djynn_project_file *f) {
	if(f==NULL) return;
	if(f->type==DJYNN_PM_PROJECT) {
		gint i = ((djynn_project *)f)->index,n;
		djynn_cfg_open();
		n = djynn_cfg_get_int(djynn.workspace_key,djynn.str.project_n);
		if(i>=n) {
			djynn_cfg_close();
			return;
		} else {
			gchar key1[64],key2[64],*p1,*p2;
			sprintf(key1,djynn.str.project_d,i);
			p1 = djynn_cfg_get_str(djynn.workspace_key,key1);
			sprintf(key2,djynn.str.project_d,i+1);
			p2 = djynn_cfg_get_str(djynn.workspace_key,key2);
			djynn_cfg_set_str(djynn.workspace_key,key1,p2);
			djynn_cfg_set_str(djynn.workspace_key,key2,p1);
			djynn_cfg_save();
			djynn_cfg_close();
			g_free(p1);
			g_free(p2);
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
	djynn_pm_ws_save();
	djynn_pm_ws_load();
}


static void ws_list_changed_cb(GtkComboBox *combo,gpointer data) {
	djynn_pm_ws_set(gtk_combo_box_get_active(combo)+1);
}

static void sess_list_changed_cb(GtkComboBox *combo,gpointer data) {
	sess_write();
	djynn_pm_sess_set(gtk_combo_box_get_active(combo)+1);
}

/* Use built in "Show Document List" instead.
static void doc_list_changed_cb(GtkComboBox *combo,gpointer data) {
	gint i;
	gint n = gtk_combo_box_get_active(combo);
	djynn_open_document *d = djynn.pm.docs;
	for(i=0; d!=NULL; ++i,d=d->next) if(i==n) break;
	if(d!=NULL) {
//fprintf(djynn.log,"doc_list_changed_cb(%s)\n",filename);
//fflush(djynn.log);
		document_open_file(d->path,FALSE,NULL,NULL);
	}
}
*/

static void open_externally(gchar *path) {
	gchar *cmd,*locale_cmd,*dir,*c;
	GString *cmd_str = g_string_new(djynn.conf.ext_open_cmd);
	GError *error = NULL;
	dir = g_file_test(path,G_FILE_TEST_IS_DIR)? g_strdup(path) : g_path_get_dirname(path);
	utils_string_replace_all(cmd_str,"%f",path);
	utils_string_replace_all(cmd_str,"%d",dir);
	cmd = g_string_free(cmd_str,FALSE);
	locale_cmd = utils_get_locale_from_utf8(cmd);
	if(!g_spawn_command_line_async(locale_cmd,&error)) {
		c = strchr(cmd,' ');
		if(c!=NULL) *c = '\0';
		ui_set_statusbar(TRUE,_("Could not execute configured external command '%s' (%s)."),cmd,error->message);
		g_error_free(error);
	}
	g_free(locale_cmd);
	g_free(cmd);
	g_free(dir);
}

static gchar *get_terminal() {
	gchar *terminal;
#ifdef G_OS_WIN32
	terminal = g_strdup("cmd");
#else
	const gchar *term = g_getenv("TERM");
	if(term!=NULL) terminal = g_strdup(term);
	else terminal = g_strdup("xterm");
#endif
	return terminal;
}

static void open_terminal(gchar *path) {
	if(g_file_test(path,G_FILE_TEST_EXISTS)) {
		gchar *argv[2] = { get_terminal(),NULL };
		path = g_file_test(path,G_FILE_TEST_IS_DIR)? g_strdup(path) : g_path_get_dirname(path);
		g_spawn_async(path,argv,NULL,G_SPAWN_SEARCH_PATH,NULL,NULL,NULL,NULL);
		g_free(path);
		g_free(argv[0]);
	}
}

static void djynn_pm_open_externally(djynn_project_file *f) {
	open_externally(f->path);
}

static void djynn_pm_open_terminal(djynn_project_file *f) {
	open_terminal(f->path);
}

static gboolean prj_tree_view_clicked_cb(GtkWidget *widget,GdkEventButton *event,gpointer data) {
	prj_tree_view_button = event->button;
	prj_tree_view_time = event->time;
	if(prj_tree_view_button==1 && event->type==GDK_2BUTTON_PRESS) {
		djynn_project_file *f = get_selected_file();
		if(f->type==DJYNN_PM_PROJECT) {
			if(djynn.conf.activate_prj) {
				document_open_file(((djynn_project *)f)->config_filename,FALSE,NULL,NULL);
			} else {
				project_close(FALSE);
				project_load_file(((djynn_project *)f)->geany_project_filename);
//				project_load_file_with_session(((djynn_project *)f)->geany_project_filename);
			}
		} else if(f->type==DJYNN_PM_FILE) {
			if(!djynn.conf.single_click_open) open_selected_document();
			if(djynn.conf.double_click_ext) open_externally(f->path);
		}
	} else if(prj_tree_view_button==3) {
		if(popup_menu==NULL) popup_menu = create_popup_menu();
		gtk_menu_popup(GTK_MENU(popup_menu),NULL,NULL,NULL,NULL,prj_tree_view_button,prj_tree_view_time);
	}
	return FALSE;
}

static void prj_tree_view_selection_changed_cb(GtkWidget *widget,gpointer data) {
	if(prj_tree_view_button==1) {
		if(djynn.conf.single_click_open) open_selected_document();
	} else if(prj_tree_view_button==3) {
	}
}

static void item_activate_cb(GtkMenuItem *menuitem, gpointer gdata) {
	djynn_pm_action((gint)((intptr_t)gdata));
}

static void doc_activate_cb(GObject *obj,GeanyDocument *doc,gpointer user_data) {
	if(doc->file_name!=NULL && g_path_is_absolute(doc->file_name)) {
/* Use built in "Show Document List" instead.
		gint i;
		djynn_open_document *d;
*/
		djynn_project_file *f;
//fprintf(djynn.log,"doc_activate_cb(file_name=%s)\n",doc->file_name);
//fflush(djynn.log);
		f = ht_get(djynn.pm.prj_files_table,doc->file_name);
		if(f!=NULL) {
/* Use built in "Show Document List" instead.
			for(i=0,d=djynn.pm.docs; d!=NULL; ++i,d=d->next)
				if(*d->name==*f->name && !strcmp(d->name,f->name) && !strcmp(d->path,f->path)) {
					gtk_combo_box_set_active(GTK_COMBO_BOX(doc_list),i);
					break;
				}
*/
//fprintf(djynn.log,"doc_activate_cb(dir=%s,name=%s)\n",f->path,f->name);
//fflush(djynn.log);
			if(djynn.conf.activate_sidebar)
				gtk_notebook_set_current_page(GTK_NOTEBOOK(geany->main_widgets->sidebar_notebook),djynn.pm.page_number);
			gtk_tree_view_expand_to_path(GTK_TREE_VIEW(prj_tree_view),f->tree_path);
			gtk_tree_view_set_cursor(GTK_TREE_VIEW(prj_tree_view),f->tree_path,NULL,FALSE);
/* Use built in "Show Document List" instead.
		} else {
			gchar *fn = strrchr(doc->file_name,G_DIR_SEPARATOR);
			if(fn!=NULL) ++fn;
//fprintf(djynn.log,"doc_activate_cb(doc->file_name=%s,fn=%s)\n",doc->file_name,fn);
//fflush(djynn.log);
			for(i=0,d=djynn.pm.docs; d!=NULL; ++i,d=d->next)
				if((fn==NULL || (*d->name==*fn && !strcmp(d->name,fn))) && !strcmp(d->path,doc->file_name)) {
					gtk_combo_box_set_active(GTK_COMBO_BOX(doc_list),i);
					break;
				}
			//GtkTreePath *path = gtk_tree_path_new_from_string("-1");
			//gtk_tree_view_set_cursor(GTK_TREE_VIEW(prj_tree_view),path,NULL,FALSE);
			//gtk_tree_path_free(path);
*/
		}
	}
}

static void doc_open_cb(GObject *obj,GeanyDocument *doc,gpointer user_data) {
/* Use built in "Show Document List" instead.
	sess_open_doc(doc);
*/
	doc_activate_cb(obj,doc,user_data);
}

static void doc_close_cb(GObject *obj,GeanyDocument *doc,gpointer user_data) {
//fprintf(djynn.log,"doc_close_cb()\n");
//fflush(djynn.log);
/* Use built in "Show Document List" instead.
	sess_close_doc(doc);
*/
}


void djynn_pm_action(gint id) {
	djynn_project_file *f = get_selected_file();
	switch(id) {
		case DJYNN_PM_NEW_WORKSPACE:djynn_pm_ws_dlg(TRUE);break;
		case DJYNN_PM_RENAME_WORKSPACE:djynn_pm_ws_dlg(FALSE);break;
		case DJYNN_PM_DELETE_WORKSPACE:djynn_pm_ws_delete();break;
		case DJYNN_PM_RELOAD_WORKSPACE:djynn_pm_ws_load();break;
		case DJYNN_PM_NEW_SESSION:djynn_pm_sess_dlg(TRUE);break;
		case DJYNN_PM_RENAME_SESSION:djynn_pm_sess_dlg(FALSE);break;
		case DJYNN_PM_DELETE_SESSION:djynn_pm_sess_delete();break;
		case DJYNN_PM_OPEN_EXTERNALLY:djynn_pm_open_externally(f);break;
		case DJYNN_PM_OPEN_TERMINAL:djynn_pm_open_terminal(f);break;
		case DJYNN_PM_NEW_PROJECT:djynn_pm_prj_dlg(f);break;
		case DJYNN_PM_NEW_FOLDER:djynn_pm_folder_dlg(f);break;
		case DJYNN_PM_ADD_FILE:djynn_pm_add_file(f);break;
		case DJYNN_PM_ADD_OPEN_FILES:djynn_pm_add_open_files(f);break;
		case DJYNN_PM_DELETE:djynn_pm_delete(f);break;
		case DJYNN_PM_SORT_FILES:djynn_pm_sort_files(f);break;
		case DJYNN_PM_MOVE_UP:djynn_pm_move_up(f);break;
		case DJYNN_PM_MOVE_DOWN:djynn_pm_move_down(f);break;
	}
}

void djynn_pm_configure() {
#if GTK_CHECK_VERSION(2,10,0)
	gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(prj_tree_view),djynn.conf.show_tree_lines);
#endif
}

void djynn_pm_init(GeanyData *data,gint *menu_index) {
	const gchar *stock_icons[] = {
		GTK_STOCK_CONVERT,		// DJYNN_PM_WORKSPACE
		GTK_STOCK_INDEX,			// DJYNN_PM_SESSION
		GTK_STOCK_EXECUTE,		// DJYNN_PM_PROJECT
		GTK_STOCK_DIRECTORY,		// DJYNN_PM_FOLDER
		GTK_STOCK_FILE,			// DJYNN_PM_FILE
	};
	const gint menu_items[] = {
		DJYNN_PM_NEW_WORKSPACE,
		DJYNN_PM_RENAME_WORKSPACE,
		DJYNN_PM_DELETE_WORKSPACE,
		-1,
		DJYNN_PM_NEW_SESSION,
		DJYNN_PM_RENAME_SESSION,
		DJYNN_PM_DELETE_SESSION,
		-1,
		DJYNN_PM_NEW_PROJECT,
		-2
	};
	djynn_menu_item *m;
	GtkCellRenderer *icon_renderer;
	GtkCellRenderer *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;
	GtkWidget *menu;
	GtkWidget *menu_item;

	gint i,n;
	gchar key[64],*value;
//	GeanyDocument *doc;

	prj_vbox = gtk_vbox_new(FALSE,0);

	for(i=0; i<DJYNN_PM_TYPES; ++i)
		icons[i] = gtk_widget_render_icon(prj_vbox,stock_icons[i],GTK_ICON_SIZE_MENU,NULL);

	workspace_store = gtk_list_store_new(WS_LIST_COLS,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	workspace_list = gtk_combo_box_new_with_model(GTK_TREE_MODEL(workspace_store));
	n = djynn_cfg_get_int(djynn.str.workspace,djynn.str.workspace_n);
	for(i=1; i<=n; ++i) {
		sprintf(key,djynn.str.workspace_d,i);
		value = djynn_cfg_get_str(djynn.str.workspace,key);
		djynn_pm_ws_list_add(strchr(value,':')+1);
		g_free(value);
	}
	g_object_unref(workspace_store);
	icon_renderer = gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(workspace_list),icon_renderer,FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(workspace_list),icon_renderer,"pixbuf",WS_LIST_ICON,NULL);
	text_renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(workspace_list),text_renderer,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(workspace_list),text_renderer,"text",WS_LIST_WORKSPACE,NULL);
	n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.workspace);
	gtk_combo_box_set_active(GTK_COMBO_BOX(workspace_list),n-1);
	g_signal_connect(workspace_list,"changed",G_CALLBACK(ws_list_changed_cb),NULL);
	ui_widget_set_tooltip_text(workspace_list,_("Switch between workspaces."));

	sess_store = gtk_list_store_new(SESS_LIST_COLS,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	sess_list = gtk_combo_box_new_with_model(GTK_TREE_MODEL(sess_store));
	n = djynn_cfg_get_int(djynn.str.session,djynn.str.session_n);
	for(i=1; i<=n; ++i) {
		sprintf(key,djynn.str.session_d,i);
		value = djynn_cfg_get_str(djynn.str.session,key);
		djynn_pm_sess_list_add(strchr(value,':')+1);
		g_free(value);
	}
	g_object_unref(sess_store);
	icon_renderer = gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(sess_list),icon_renderer,FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(sess_list),icon_renderer,"pixbuf",SESS_LIST_ICON,NULL);
	text_renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(sess_list),text_renderer,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(sess_list),text_renderer,"text",SESS_LIST_SESSION,NULL);
	n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.session);
	gtk_combo_box_set_active(GTK_COMBO_BOX(sess_list),n-1);
	g_signal_connect(sess_list,"changed",G_CALLBACK(sess_list_changed_cb),NULL);
	ui_widget_set_tooltip_text(sess_list,_("Switch between sessions."));


/* Use built in "Show Document List" instead.
	doc_store = gtk_list_store_new(DOCS_LIST_COLS,GDK_TYPE_PIXBUF,G_TYPE_STRING);
	for(i=0; 1; ++i) {
		doc = document_get_from_page(i);
		if(doc==NULL) break;
		sess_open_doc(doc);
	}
	doc_list = gtk_combo_box_new_with_model(GTK_TREE_MODEL(doc_store));
	g_object_unref(doc_store);
	icon_renderer = gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(doc_list),icon_renderer,FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(doc_list),icon_renderer,"pixbuf",DOCS_LIST_ICON,NULL);
	text_renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(doc_list),text_renderer,TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(doc_list),text_renderer,"text",DOCS_LIST_FILENAME,NULL);
	g_signal_connect(doc_list,"changed",G_CALLBACK(doc_list_changed_cb),NULL);
	ui_widget_set_tooltip_text(doc_list,_("A list of all open documents sorted alphabetically."));
	gtk_box_pack_start(GTK_BOX(prj_vbox),doc_list,FALSE,TRUE,0);
*/

	prj_read_all();

	prj_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(prj_scroll),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(prj_scroll),GTK_SHADOW_IN);
	prj_tree_view = gtk_tree_view_new_with_model(create_tree_model());
	g_object_unref(prj_tree_store);
	column = gtk_tree_view_column_new();
	icon_renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column,icon_renderer,FALSE);
	gtk_tree_view_column_set_attributes(column,icon_renderer,"pixbuf",PRJ_TREE_ICON,NULL);
	text_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column,text_renderer,TRUE);
	gtk_tree_view_column_add_attribute(column,text_renderer,"text",PRJ_TREE_FILENAME);
	gtk_tree_view_append_column(GTK_TREE_VIEW(prj_tree_view),column);
	ui_widget_modify_font_from_string(prj_tree_view,geany->interface_prefs->tagbar_font);
	g_signal_connect(prj_tree_view,"button-press-event",G_CALLBACK(prj_tree_view_clicked_cb),NULL);
	g_object_set(prj_tree_view,"has-tooltip",TRUE,"tooltip-column",PRJ_TREE_PATH,NULL);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(prj_tree_view));
	g_signal_connect(selection,"changed",G_CALLBACK(prj_tree_view_selection_changed_cb),NULL);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(prj_tree_view),FALSE);
	gtk_container_add(GTK_CONTAINER(prj_scroll),prj_tree_view);

	if(djynn.conf.ws_list==1) gtk_box_pack_start(GTK_BOX(prj_vbox),workspace_list,FALSE,TRUE,0);
	if(djynn.conf.sess_list==1) gtk_box_pack_start(GTK_BOX(prj_vbox),sess_list,FALSE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(prj_vbox),prj_scroll,TRUE,TRUE,0);
	if(djynn.conf.ws_list==2) gtk_box_pack_start(GTK_BOX(prj_vbox),workspace_list,FALSE,TRUE,0);
	if(djynn.conf.sess_list==2) gtk_box_pack_start(GTK_BOX(prj_vbox),sess_list,FALSE,TRUE,0);

	gtk_widget_show_all(prj_vbox);
	djynn.pm.page_number = gtk_notebook_append_page(GTK_NOTEBOOK(geany->main_widgets->sidebar_notebook),
			prj_vbox,gtk_label_new(_("Project")));

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
	menu_item = gtk_menu_item_new_with_label(_("Project Manager"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),menu);
	gtk_menu_append(GTK_MENU(djynn.tools_menu),menu_item);
	*menu_index += 1;

	plugin_signal_connect(geany_plugin,NULL,"document-activate",TRUE,(GCallback)&doc_activate_cb,NULL);
	plugin_signal_connect(geany_plugin,NULL,"document-open",TRUE,(GCallback)&doc_open_cb,NULL);
	plugin_signal_connect(geany_plugin,NULL,"document-close",TRUE,(GCallback)&doc_close_cb,NULL);

	djynn_pm_configure();
	prj_expand_tree();
}


void djynn_pm_cleanup() {
	gint i;
/* Use built in "Show Document List" instead.
	sess_free();
*/
	djynn_pm_ws_save();
	prj_free_all();
	for(i=0; i<DJYNN_PM_TYPES; ++i) g_object_unref(icons[i]);
	gtk_widget_destroy(prj_vbox);
}

