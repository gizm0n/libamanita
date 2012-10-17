

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <ipomoea/string_t.h>
#include "djynn.h"

GeanyPlugin *geany_plugin;
GeanyData *geany_data;
GeanyFunctions *geany_functions;

PLUGIN_VERSION_CHECK(147)

PLUGIN_SET_INFO(_("Djynn"),
	_("A small project manager, with some additional functionality, commenting and sorting."),
	_("0.1"),
	_("Per Löwgren <per.lowgren@gmail.com>"));


PLUGIN_KEY_GROUP(djynn,DJYNN_KB_COUNT)


static djynn_menu_item djynn_menu_items[] = {
	{ "New Workspace",					GTK_STOCK_NEW,					NULL,		djynn_projectmanager_action },
	{ "Delete Workspace",				GTK_STOCK_DELETE,				NULL,		djynn_projectmanager_action },
	{ "Reload Workspace",				GTK_STOCK_REFRESH,			NULL,		djynn_projectmanager_action },
	{ "New Project",						GTK_STOCK_NEW,					NULL,		djynn_projectmanager_action },
	{ "New Folder",						GTK_STOCK_NEW,					NULL,		djynn_projectmanager_action },
	{ "Add Active File",					GTK_STOCK_ADD,					NULL,		djynn_projectmanager_action },
	{ "Add All Open Files",				GTK_STOCK_ADD,					NULL,		djynn_projectmanager_action },
	{ "Delete",								GTK_STOCK_DELETE,				NULL,		djynn_projectmanager_action },
	{ "Move Up",							GTK_STOCK_GO_UP,				NULL,		djynn_projectmanager_action },
	{ "Move Down",							GTK_STOCK_GO_DOWN,			NULL,		djynn_projectmanager_action },
	{ "Encode Base64",					NULL,								NULL,		djynn_base64_action },
	{ "Decode Base64",					NULL,								NULL,		djynn_base64_action },
	{ "Toggle Comment",					NULL,								NULL,		djynn_comment_action },
	{ "Toggle Block Comment",			NULL,								NULL,		djynn_comment_action },
	{ "Insert Doxygen Comment",		NULL,								NULL,		djynn_comment_action },
	{ "Sort Asc. [aA-zZ]",				GTK_STOCK_SORT_ASCENDING,	NULL,		djynn_sort_action },
	{ "Sort Asc. Case [a-z-A-Z]",		GTK_STOCK_SORT_ASCENDING,	NULL,		djynn_sort_action },
	{ "Sort Desc. [Zz-Aa]",				GTK_STOCK_SORT_DESCENDING,	NULL,		djynn_sort_action },
	{ "Sort Desc. Case [Z-A-z-a]",	GTK_STOCK_SORT_DESCENDING,	NULL,		djynn_sort_action },
	{ "Reverse Lines",					NULL,								NULL,		djynn_sort_action },
};

static djynn_keybind djynn_keybinds[] = {
	{ "pm_new_project",			DJYNN_PM_NEW_PROJECT },
	{ "pm_new_folder",			DJYNN_PM_NEW_FOLDER },
	{ "pm_add_file",				DJYNN_PM_ADD_FILE },
	{ "pm_add_open_files",		DJYNN_PM_ADD_OPEN_FILES },
	{ "pm_delete",					DJYNN_PM_DELETE },
	{ "pm_move_up",				DJYNN_PM_MOVE_UP },
	{ "pm_move_down",				DJYNN_PM_MOVE_DOWN },
	{ "pm_reload_workspace",	DJYNN_PM_RELOAD_WORKSPACE },
	{ "b64encode",					DJYNN_BASE64_ENCODE },
	{ "b64decode",					DJYNN_BASE64_DECODE },
	{ "comment",					DJYNN_TOGGLE_COMMENT },
	{ "block_comment",			DJYNN_TOGGLE_BLOCK },
	{ "doxygen_comment",			DJYNN_INSERT_DOXYGEN_COMMENT },
	{ "sort_asc",					DJYNN_SORT_ASC },
	{ "sort_asc_case",			DJYNN_SORT_ASC_CASE },
	{ "sort_desc",					DJYNN_SORT_DESC },
	{ "sort_desc_case",			DJYNN_SORT_DESC_CASE },
	{ "reverse",					DJYNN_REVERSE },
};

djynn_data djynn = {
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	djynn_menu_items,
	djynn_keybinds,
	NULL,
	NULL,
	NULL,
	{
		0,
		NULL,
		NULL,
		0,
		NULL,
		NULL,
	},
	{
		"djynn",
		"workspace",
		"workspace_n",
		"workspace_%d",
		"projectmanager",
		"project_n",
		"project_%d"
	}
};


void djynn_open_config() {
	if(djynn.config_file==NULL) {
		djynn.config_file = g_key_file_new();
		g_key_file_load_from_file(djynn.config_file,djynn.config_filename,G_KEY_FILE_NONE,NULL);
	}
	++djynn.config_file_ref;
}

void djynn_save_config() {
	char *data;
	gsize size;
	gboolean ret;
	data = g_key_file_to_data(djynn.config_file,&size,NULL);
fprintf(djynn.log,"djynn_save_config(\n%s\n)\n",data);
	ret = g_file_set_contents(djynn.config_filename,data,size,NULL);
	g_free(data);
	(void)ret;
}

void djynn_close_config() {
	djynn.config_file_ref--;
	if(djynn.config_file_ref==0) {
		g_key_file_free(djynn.config_file);
		djynn.config_file = NULL;
	}
}

void djynn_config_remove(const char *group,const char *key) {
	if(djynn.config_file==NULL) return;
	g_key_file_remove_key(djynn.config_file,group,key,NULL);
}

int djynn_config_get_int(const char *group,const char *key) {
	if(djynn.config_file==NULL) return 0;
	int n = g_key_file_get_integer(djynn.config_file,group,key,NULL);
	return n;
}

void djynn_config_set_int(const char *group,const char *key,int n) {
	if(djynn.config_file==NULL) return;
	g_key_file_set_integer(djynn.config_file,group,key,n);
}

char *djynn_config_get_str(const char *group,const char *key) {
	if(djynn.config_file==NULL) return NULL;
	char *str = g_key_file_get_string(djynn.config_file,group,key,NULL);
	return str;
}

void djynn_config_set_str(const char *group,const char *key,const char *str) {
	if(djynn.config_file==NULL) return;
	g_key_file_set_string(djynn.config_file,group,key,str);
}



static void kb_activate(guint key_id) {
	djynn_keybind *kb = &djynn.keybinds[key_id];
	djynn_menu_item *m = &djynn.menu_items[kb->menu_item_index];
	(*m->activate)(kb->menu_item_index);
}



void plugin_init(GeanyData *data) {
	int i;
	gchar str[257],*p1;
	djynn_keybind *kb;
	djynn_menu_item *m;

	djynn.config_dir = g_strconcat(geany->app->configdir,G_DIR_SEPARATOR_S,"plugins",G_DIR_SEPARATOR_S,
			djynn.str.djynn,G_DIR_SEPARATOR_S,NULL);
	if(g_file_test(djynn.config_dir,G_FILE_TEST_EXISTS)==FALSE) g_mkdir(djynn.config_dir,0700);

	p1 = g_strconcat(djynn.config_dir,"djynn.log",NULL);
	djynn.log = fopen(p1,"wb");
	if(!djynn.log) perror(p1);
	g_free(p1);

	djynn.config_filename = g_strconcat(djynn.config_dir,"djynn.conf",NULL);
	if(g_file_test(djynn.config_filename,G_FILE_TEST_EXISTS)==FALSE) {
		FILE *fp = fopen(djynn.config_filename,"wb");
		if(fp) {
			sprintf(str,djynn.str.workspace_d,1);
			fprintf(fp,
				"\n[%s]\n\n"
				"[%s]\n"
				"%s=1\n"
				"%s=1\n"
				"%s=default\n\n"
				"[pm_default]\n"
				"%s=0\n",
					djynn.str.djynn,
					djynn.str.workspace,
					djynn.str.workspace,
					djynn.str.workspace_n,
					str,
					djynn.str.project_n);
			fclose(fp);
		} else perror(djynn.config_filename);
	}

	djynn_open_config();
	i = djynn_config_get_int(djynn.str.workspace,djynn.str.workspace);
	sprintf(str,djynn.str.workspace_d,i);
	p1 = djynn_config_get_str(djynn.str.workspace,str);
	sprintf(str,"pm_%s",p1);
	g_free(p1);
	djynn.workspace = strdup(str);

	djynn.tools_menu = gtk_menu_new();

	i = 0;
	djynn_projectmanager_init(data,&i);
	djynn_base64_init(data,&i);
	djynn_comment_init(data,&i);
	djynn_sort_init(data,&i);

	djynn_close_config();

	djynn.tools_menu_item = gtk_menu_item_new_with_label("Djynn");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(djynn.tools_menu_item),djynn.tools_menu);
	gtk_widget_show_all(djynn.tools_menu_item);
	gtk_container_add(GTK_CONTAINER(geany->main_widgets->tools_menu),djynn.tools_menu_item);

	for(i=0; i<DJYNN_KB_COUNT; ++i) {
		kb = &djynn.keybinds[i];
		m = &djynn.menu_items[kb->menu_item_index];
		keybindings_set_item(plugin_key_group,i,kb_activate,0,0,kb->name,m->label,m->menu_item);
	}
}


void plugin_cleanup(void) {
	djynn_projectmanager_cleanup();
	djynn_base64_cleanup();
	djynn_comment_cleanup();
	djynn_sort_cleanup();
	gtk_widget_destroy(djynn.tools_menu_item);
	fclose(djynn.log);
	g_free(djynn.config_dir);
	g_free(djynn.config_filename);
	g_free(djynn.workspace);
}


