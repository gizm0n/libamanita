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
	DJYNN_PM_OPEN_EXTERNALLY,
	DJYNN_PM_OPEN_TERMINAL,
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
	gchar *path;
	gchar *name;
	GtkTreeIter iter;
	GeanyDocument *doc;
	struct djynn_open_document *next;
} djynn_open_document;
*/

typedef struct djynn_project_file {
	gchar *path;
	gchar *name;
	gint type;
	gint depth;
	gint status;
	GtkTreePath *tree_path;
	struct djynn_project_file *parent;
	struct djynn_project_file *files;
	struct djynn_project_file *next;
} djynn_project_file;

typedef struct djynn_project {
	gchar *path;
	gchar *name;
	gint type;
	gint depth;
	gint status;
	GtkTreePath *tree_path;
	struct djynn_project_file *parent;
	struct djynn_project_file *files;
	struct djynn_project *next;
	gint index;
	gchar *config_filename;
	gchar *geany_project_filename;
} djynn_project;


typedef struct djynn_projectmanager {
	gint page_number;
/* Use built in "Show Document List" instead.
	djynn_open_document *docs;
	hashtable_t *docs_table;
*/
	gint nprj;
	djynn_project *prj;
	hashtable_t *prj_files_table;
} djynn_projectmanager;



typedef struct djynn_menu_item {
	gchar *label;
	const gchar *menu_stock;
	GtkWidget *menu_item;
	void (*activate)(gint);
} djynn_menu_item;

typedef struct djynn_keybind {
	gchar *name;
	gint menu_item_index;
} djynn_keybind;

typedef struct djynn_data {
	FILE *log;
	gchar *config_dir;
	gchar *config_filename;
	GKeyFile *config_file;
	gint config_file_ref;
	djynn_menu_item *menu_items;
	djynn_keybind *keybinds;
	GtkWidget *tools_menu_item;
	GtkWidget *tools_menu;
	gchar *workspace;
	gchar *workspace_key;
	djynn_projectmanager pm;

	struct {
		gchar *ext_open_cmd;
		gint ws_list;
		gint sess_list;
		gint show_icons;
		gboolean show_tree_lines;
		gboolean single_click_open;
		gboolean double_click_ext;
		gboolean activate_prj;
		gboolean activate_sidebar;
	} conf;

	struct {
		const gchar *djynn;
		const gchar *workspace;
		const gchar *workspace_id;
		const gchar *workspace_n;
		const gchar *workspace_d;
		const gchar *session;
		const gchar *session_id;
		const gchar *session_n;
		const gchar *session_d;
		const gchar *projectmanager;
		const gchar *project;
		const gchar *project_n;
		const gchar *project_d;

		const gchar *ext_open_cmd;
		const gchar *ws_list;
		const gchar *sess_list;
		const gchar *show_icons;
		const gchar *show_tree_lines;
		const gchar *single_click_open;
		const gchar *double_click_ext;
		const gchar *activate_prj;
		const gchar *activate_sidebar;
	} str;
} djynn_data;


extern djynn_data djynn;

extern GeanyPlugin *geany_plugin;
extern GeanyData *geany_data;
extern GeanyFunctions *geany_functions;

void djynn_cfg_open();
void djynn_cfg_save();
void djynn_cfg_close();
gboolean djynn_cfg_has_group(const gchar *group);
void djynn_cfg_remove_group(const gchar *group);
void djynn_cfg_remove(const gchar *group,const gchar *key);
void djynn_cfg_remove_from_list(const gchar *group,const gchar *key,gint index);
gint djynn_cfg_get_int(const gchar *group,const gchar *key);
void djynn_cfg_set_int(const gchar *group,const gchar *key,gint n);
gchar *djynn_cfg_get_str(const gchar *group,const gchar *key);
void djynn_cfg_set_str(const gchar *group,const gchar *key,const gchar *str);
gchar *djynn_cfg_get_workspace_key(const gchar *name);

gint djynn_msgbox_ask(const gchar *title,const gchar *msg,const gchar *item);
void djynn_msgbox_warn(const gchar *title,const gchar *msg,const gchar *item);

void djynn_pm_configure();
void djynn_pm_init(GeanyData *data,gint *menu_index);
void djynn_pm_cleanup();
void djynn_pm_action(gint id);
void djynn_pm_ws_dlg(gboolean create);
void djynn_pm_ws_save();
void djynn_pm_ws_load();
void djynn_pm_ws_set(gint index);
void djynn_pm_ws_list_select(gint index);
void djynn_pm_ws_list_remove(gint index);
void djynn_pm_ws_list_add(const gchar *name);
void djynn_pm_ws_list_set(gint index,const gchar *name);
void djynn_pm_sess_dlg(gboolean create);
void djynn_pm_sess_set(gint index);
void djynn_pm_sess_list_select(gint index);
void djynn_pm_sess_list_remove(gint index);
void djynn_pm_sess_list_add(const gchar *name);
void djynn_pm_sess_list_set(gint index,const gchar *name);
void djynn_pm_prj_dlg();
void djynn_pm_folder_dlg();
void djynn_pm_folder_add(djynn_project_file *f,const gchar *path,const gchar *name,gboolean files,gboolean folders);

void djynn_base64_init(GeanyData *data,gint *menu_index);
void djynn_base64_cleanup();
void djynn_base64_action(gint id);

void djynn_comment_init(GeanyData *data,gint *menu_index);
void djynn_comment_cleanup();
void djynn_comment_action(gint id);

void djynn_sort_init(GeanyData *data,gint *menu_index);
void djynn_sort_cleanup();
void djynn_sort_action(gint id);


#endif /* _DJYNN_H */


