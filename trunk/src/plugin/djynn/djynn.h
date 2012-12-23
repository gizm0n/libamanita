#ifndef _DJYNN_H
#define _DJYNN_H

#include <stdio.h>
#include <ipomoea/hashtable_t.h>
#include "geanyplugin.h"


enum {
	DJYNN_PM_NEW_WORKSPACE,
	DJYNN_PM_RENAME_WORKSPACE,
	DJYNN_PM_DELETE_WORKSPACE,
	DJYNN_PM_RELOAD_WORKSPACE,
	DJYNN_PM_NEW_SESSION,
	DJYNN_PM_RENAME_SESSION,
	DJYNN_PM_DELETE_SESSION,
	DJYNN_PM_NEW_PROJECT,
	DJYNN_PM_NEW_FOLDER,
	DJYNN_PM_ADD_FILE,
	DJYNN_PM_ADD_OPEN_FILES,
	DJYNN_PM_DELETE,
	DJYNN_PM_SORT_FILES,
	DJYNN_PM_MOVE_UP,
	DJYNN_PM_MOVE_DOWN,
	DJYNN_BASE64_ENCODE,
	DJYNN_BASE64_DECODE,
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
	DJYNN_KB_PM_NEW_WORKSPACE,
	DJYNN_KB_PM_RENAME_WORKSPACE,
	DJYNN_KB_PM_DELETE_WORKSPACE,
	DJYNN_KB_PM_RELOAD_WORKSPACE,
	DJYNN_KB_PM_NEW_SESSION,
	DJYNN_KB_PM_RENAME_SESSION,
	DJYNN_KB_PM_DELETE_SESSION,
	DJYNN_KB_PM_NEW_PROJECT,
	DJYNN_KB_PM_NEW_FOLDER,
	DJYNN_KB_PM_ADD_FILE,
	DJYNN_KB_PM_ADD_OPEN_FILES,
	DJYNN_KB_PM_DELETE,
	DJYNN_KB_PM_SORT_FILES,
	DJYNN_KB_PM_MOVE_UP,
	DJYNN_KB_PM_MOVE_DOWN,
	DJYNN_KB_BASE64_ENCODE,
	DJYNN_KB_BASE64_DECODE,
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
	DJYNN_PM_WORKSPACE,
	DJYNN_PM_SESSION,
	DJYNN_PM_PROJECT,
	DJYNN_PM_FOLDER,
	DJYNN_PM_FILE,
	DJYNN_PM_TYPES
};

/* Use built in "Show Document List" instead.
typedef struct djynn_open_document {
	char *path;
	char *name;
	GtkTreeIter iter;
	GeanyDocument *doc;
	struct djynn_open_document *next;
} djynn_open_document;
*/

typedef struct djynn_project_file {
	char *path;
	char *name;
	int type;
	int depth;
	int status;
	GtkTreePath *tree_path;
	struct djynn_project_file *parent;
	struct djynn_project_file *files;
	struct djynn_project_file *next;
} djynn_project_file;

typedef struct djynn_project {
	char *path;
	char *name;
	int type;
	int depth;
	int status;
	GtkTreePath *tree_path;
	struct djynn_project_file *parent;
	struct djynn_project_file *files;
	struct djynn_project *next;
	int index;
	char *config_filename;
	char *geany_project_filename;
} djynn_project;


typedef struct djynn_projectmanager {
	int page_number;
/* Use built in "Show Document List" instead.
	djynn_open_document *docs;
	hashtable_t *docs_table;
*/
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
	char *workspace_key;
	djynn_projectmanager pm;

	struct {
		const char *djynn;
		const char *workspace;
		const char *workspace_id;
		const char *workspace_n;
		const char *workspace_d;
		const char *session;
		const char *session_id;
		const char *session_n;
		const char *session_d;
		const char *projectmanager;
		const char *project;
		const char *project_n;
		const char *project_d;
	} str;
} djynn_data;


extern djynn_data djynn;

extern GeanyPlugin *geany_plugin;
extern GeanyData *geany_data;
extern GeanyFunctions *geany_functions;

void djynn_cfg_open();
void djynn_cfg_save();
void djynn_cfg_close();
gboolean djynn_cfg_has_group(const char *group);
void djynn_cfg_remove_group(const char *group);
void djynn_cfg_remove(const char *group,const char *key);
void djynn_cfg_remove_from_list(const char *group,const char *key,int index);
int djynn_cfg_get_int(const char *group,const char *key);
void djynn_cfg_set_int(const char *group,const char *key,int n);
char *djynn_cfg_get_str(const char *group,const char *key);
void djynn_cfg_set_str(const char *group,const char *key,const char *str);
char *djynn_cfg_get_workspace_key(const char *name);

int djynn_msgbox_ask(const char *title,const char *msg,const char *item);
void djynn_msgbox_warn(const char *title,const char *msg,const char *item);

void djynn_pm_init(GeanyData *data,int *menu_index);
void djynn_pm_cleanup();
void djynn_pm_action(int id);
void djynn_pm_ws_dlg(gboolean create);
void djynn_pm_ws_save();
void djynn_pm_ws_load();
void djynn_pm_ws_set(int index);
void djynn_pm_ws_list_select(int index);
void djynn_pm_ws_list_remove(int index);
void djynn_pm_ws_list_add(const char *name);
void djynn_pm_ws_list_set(int index,const char *name);
void djynn_pm_sess_dlg(gboolean create);
void djynn_pm_sess_set(int index);
void djynn_pm_sess_list_select(int index);
void djynn_pm_sess_list_remove(int index);
void djynn_pm_sess_list_add(const char *name);
void djynn_pm_sess_list_set(int index,const char *name);
void djynn_pm_prj_dlg();
void djynn_pm_folder_dlg();
void djynn_pm_folder_add(djynn_project_file *f,const gchar *path,const gchar *name,gboolean files,gboolean folders);

void djynn_base64_init(GeanyData *data,int *menu_index);
void djynn_base64_cleanup();
void djynn_base64_action(int id);

void djynn_comment_init(GeanyData *data,int *menu_index);
void djynn_comment_cleanup();
void djynn_comment_action(int id);

void djynn_sort_init(GeanyData *data,int *menu_index);
void djynn_sort_cleanup();
void djynn_sort_action(int id);


#endif /* _DJYNN_H */


