#ifndef _DJYNN_H
#define _DJYNN_H

#include <stdio.h>
#include <ipomoea/hashtable_t.h>
#include "geanyplugin.h"


enum {
	DJYNN_PM_NEW_WORKSPACE,
	DJYNN_PM_DELETE_WORKSPACE,
	DJYNN_PM_RELOAD_WORKSPACE,
	DJYNN_PM_NEW_PROJECT,
	DJYNN_PM_NEW_FOLDER,
	DJYNN_PM_ADD_FILE,
	DJYNN_PM_ADD_OPEN_FILES,
	DJYNN_PM_DELETE,
	DJYNN_PM_MOVE_UP,
	DJYNN_PM_MOVE_DOWN,
	DJYNN_TOGGLE_COMMENT,
	DJYNN_TOGGLE_BLOCK,
	DJYNN_INSERT_DOXYGEN_COMMENT,
	DJYNN_SORT_ASC,
	DJYNN_SORT_ASC_CASE,
	DJYNN_SORT_DESC,
	DJYNN_SORT_DESC_CASE,
	DJYNN_REVERSE,
	DJYNN_COUNT
};

enum {
	DJYNN_KB_PM_NEW_PROJECT,
	DJYNN_KB_PM_NEW_FOLDER,
	DJYNN_KB_PM_ADD_FILE,
	DJYNN_KB_PM_ADD_OPEN_FILES,
	DJYNN_KB_PM_DELETE,
	DJYNN_KB_PM_MOVE_UP,
	DJYNN_KB_PM_MOVE_DOWN,
	DJYNN_KB_PM_RELOAD_WORKSPACE,
	DJYNN_KB_TOGGLE_COMMENT,
	DJYNN_KB_TOGGLE_BLOCK,
	DJYNN_KB_INSERT_DOXYGEN_COMMENT,
	DJYNN_KB_SORT_ASC,
	DJYNN_KB_SORT_ASC_CASE,
	DJYNN_KB_SORT_DESC,
	DJYNN_KB_SORT_DESC_CASE,
	DJYNN_KB_REVERSE,
	DJYNN_KB_COUNT
};


enum {
	DJYNN_PM_PROJECT,
	DJYNN_PM_FOLDER,
	DJYNN_PM_FILE
};


typedef struct djynn_open_document {
	char *name;
	char *directory;
	GtkTreeIter iter;
	GeanyDocument *doc;
	struct djynn_open_document *next;
} djynn_open_document;

typedef struct djynn_project_file {
	char *name;
	char *directory;
	int type;
	int depth;
	int status;
	GtkTreePath *path;
	struct djynn_project_file *parent;
	struct djynn_project_file *files;
	struct djynn_project_file *next;
} djynn_project_file;

typedef struct djynn_project {
	char *name;
	char *directory;
	int type;
	int depth;
	int status;
	GtkTreePath *path;
	struct djynn_project_file *parent;
	struct djynn_project_file *files;
	struct djynn_project *next;
	int index;
	char *config_filename;
	char *geany_project_filename;
} djynn_project;


typedef struct djynn_projectmanager {
	int page_number;

	djynn_open_document *docs;
	hashtable_t *docs_table;

	int nprj;
	djynn_project *prj;
	hashtable_t *prj_files_table;
} djynn_projectmanager;



typedef struct djynn_menu_item {
	gchar *label;
	const gchar *menu_stock;
	GtkWidget *menu_item;
	void (*activate)(int);
} djynn_menu_item;

typedef struct djynn_keybind {
	gchar *name;
	int menu_item_index;
} djynn_keybind;

typedef struct djynn_data {
	FILE *log;
	char *config_dir;
	char *config_filename;
	GKeyFile *config_file;
	int config_file_ref;
	djynn_menu_item *menu_items;
	djynn_keybind *keybinds;
	GtkWidget *tools_menu_item;
	GtkWidget *tools_menu;
	char *workspace;
	djynn_projectmanager pm;

	struct {
		const char *djynn;
		const char *workspace;
		const char *workspace_n;
		const char *workspace_d;
		const char *projectmanager;
		const char *project_n;
		const char *project_d;
	} str;
} djynn_data;


extern djynn_data djynn;

extern GeanyPlugin *geany_plugin;
extern GeanyData *geany_data;
extern GeanyFunctions *geany_functions;


void djynn_open_config();
void djynn_save_config();
void djynn_close_config();
void djynn_config_remove(const char *group,const char *key);
int djynn_config_get_int(const char *group,const char *key);
void djynn_config_set_int(const char *group,const char *key,int n);
char *djynn_config_get_str(const char *group,const char *key);
void djynn_config_set_str(const char *group,const char *key,const char *str);

void djynn_projectmanager_init(GeanyData *data,int *menu_index);
void djynn_projectmanager_cleanup();
void djynn_projectmanager_action(int id);
void djynn_projectmanager_save_workspace();
void djynn_projectmanager_load_workspace();
void djynn_projectmanager_open_new_workspace_dlg();
void djynn_projectmanager_open_new_project_dlg();
void djynn_projectmanager_open_new_folder_dlg();
void djynn_projectmanager_add_folder(const gchar *name);

void djynn_comment_init(GeanyData *data,int *menu_index);
void djynn_comment_cleanup();
void djynn_comment_action(int id);

void djynn_sort_init(GeanyData *data,int *menu_index);
void djynn_sort_cleanup();
void djynn_sort_action(int id);


#endif /* _PROJECTMANAGER_H */


