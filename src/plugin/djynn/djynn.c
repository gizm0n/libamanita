

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <ipomoea/string_t.h>
#include "djynn.h"

GeanyPlugin *geany_plugin;
GeanyData *geany_data;
GeanyFunctions *geany_functions;

PLUGIN_VERSION_CHECK(GEANY_API_VERSION)

PLUGIN_SET_INFO(_("Djynn"),
	_("A small project manager, with some additional functionality; encoding/decoding, commenting and sorting."),
	"0.2",
	"Per Löwgren <per.lowgren@gmail.com>");


PLUGIN_KEY_GROUP(djynn,DJYNN_KB_COUNT)


static djynn_menu_item djynn_menu_items[] = {
	{ _("New Workspace"),					GTK_STOCK_NEW,					NULL,		djynn_pm_action },
	{ _("Rename Workspace"),				NULL,								NULL,		djynn_pm_action },
	{ _("Delete Workspace"),				GTK_STOCK_DELETE,				NULL,		djynn_pm_action },
	{ _("Reload Workspace"),				GTK_STOCK_REFRESH,			NULL,		djynn_pm_action },
	{ _("New Session"),						GTK_STOCK_NEW,					NULL,		djynn_pm_action },
	{ _("Rename Session"),					NULL,								NULL,		djynn_pm_action },
	{ _("Delete Session"),					GTK_STOCK_DELETE,				NULL,		djynn_pm_action },
	{ _("Open Directory"),					GTK_STOCK_OPEN,				NULL,		djynn_pm_action },
	{ _("Open Terminal"),					GTK_STOCK_OPEN,				NULL,		djynn_pm_action },
	{ _("New Project"),						GTK_STOCK_NEW,					NULL,		djynn_pm_action },
	{ _("New Folder"),						GTK_STOCK_NEW,					NULL,		djynn_pm_action },
	{ _("Add Active File"),					GTK_STOCK_ADD,					NULL,		djynn_pm_action },
	{ _("Add All Open Files"),				GTK_STOCK_ADD,					NULL,		djynn_pm_action },
	{ _("Delete"),								GTK_STOCK_DELETE,				NULL,		djynn_pm_action },
	{ _("Sort Files"),						GTK_STOCK_SORT_ASCENDING,	NULL,		djynn_pm_action },
	{ _("Move Up"),							GTK_STOCK_GO_UP,				NULL,		djynn_pm_action },
	{ _("Move Down"),							GTK_STOCK_GO_DOWN,			NULL,		djynn_pm_action },
	{ _("Encode Base64"),					NULL,								NULL,		djynn_base64_action },
	{ _("Decode Base64"),					NULL,								NULL,		djynn_base64_action },
	{ _("Toggle Comment"),					NULL,								NULL,		djynn_comment_action },
	{ _("Toggle Block Comment"),			NULL,								NULL,		djynn_comment_action },
	{ _("Insert Doxygen Comment"),		NULL,								NULL,		djynn_comment_action },
	{ _("Sort Asc. [aA-zZ]"),				GTK_STOCK_SORT_ASCENDING,	NULL,		djynn_sort_action },
	{ _("Sort Asc. Case [a-z-A-Z]"),		GTK_STOCK_SORT_ASCENDING,	NULL,		djynn_sort_action },
	{ _("Sort Desc. [Zz-Aa]"),				GTK_STOCK_SORT_DESCENDING,	NULL,		djynn_sort_action },
	{ _("Sort Desc. Case [Z-A-z-a]"),	GTK_STOCK_SORT_DESCENDING,	NULL,		djynn_sort_action },
	{ _("Reverse Lines"),					NULL,								NULL,		djynn_sort_action },
};

static djynn_keybind djynn_keybinds[] = {
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
	NULL,
	{
		0,
/* Use built in "Show Document List" instead.
		NULL,
		NULL,
*/
		0,
		NULL,
		NULL,
	},
	{
		NULL,
		0,
		0,
		0,
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		FALSE,
	},
	{
		"djynn",
		"workspace",
		"workspace_id",
		"workspace_n",
		"workspace_%d",
		"session",
		"session_id",
		"session_n",
		"session_%d",
		"projectmanager",
		"project",
		"project_n",
		"project_%d",

		"ext_open_cmd",
		"ws_list",
		"sess_list",
		"show_icons",
		"show_tree_lines",
		"single_click_open",
		"double_click_ext",
		"activate_prj",
		"activate_sidebar",
	}
};


void djynn_cfg_open() {
	if(djynn.config_file==NULL) {
		djynn.config_file = g_key_file_new();
		g_key_file_load_from_file(djynn.config_file,djynn.config_filename,G_KEY_FILE_NONE,NULL);
	}
	++djynn.config_file_ref;
}

void djynn_cfg_save() {
	gchar *data;
	gsize size;
	gboolean ret;
	data = g_key_file_to_data(djynn.config_file,&size,NULL);
//fprintf(djynn.log,"djynn_cfg_save(\n%s\n)\n",data);
	ret = g_file_set_contents(djynn.config_filename,data,size,NULL);
	g_free(data);
	(void)ret;
}

void djynn_cfg_close() {
	--djynn.config_file_ref;
	if(djynn.config_file_ref==0) {
		g_key_file_free(djynn.config_file);
		djynn.config_file = NULL;
	}
}

gboolean djynn_cfg_has_group(const gchar *group) {
	if(djynn.config_file==NULL) return FALSE;
	return g_key_file_has_group(djynn.config_file,group);
}

void djynn_cfg_remove_group(const gchar *group) {
	if(djynn.config_file==NULL) return;
//fprintf(djynn.log,"djynn_cfg_remove_group(group=%s)\n",group);
//fflush(djynn.log);
	g_key_file_remove_group(djynn.config_file,group,NULL);
}

void djynn_cfg_remove(const gchar *group,const gchar *key) {
	if(djynn.config_file==NULL) return;
	g_key_file_remove_key(djynn.config_file,group,key,NULL);
}

void djynn_cfg_remove_from_list(const gchar *group,const gchar *key,gint index) {
	gint i,n;
	gchar key_n[64];
	sprintf(key_n,"%s_n",key);
	n = djynn_cfg_get_int(group,key_n);
	if(index>=1 && index<=n) {
		gchar key_d[64];
		gchar k[64],*s;
		sprintf(key_d,"%s_%%d",key);
		for(i=index; i<n; ++i) {
			sprintf(k,key_d,i+1);
			s = djynn_cfg_get_str(group,k);
			sprintf(k,key_d,i);
			djynn_cfg_set_str(group,k,s);
			g_free(s);
		}
		sprintf(k,key_d,n);
		djynn_cfg_remove(group,k);
		djynn_cfg_set_int(group,key_n,n-1);
	}
}

gint djynn_cfg_get_int(const gchar *group,const gchar *key) {
	if(djynn.config_file==NULL) return 0;
	gint n = g_key_file_get_integer(djynn.config_file,group,key,NULL);
	return n;
}

void djynn_cfg_set_int(const gchar *group,const gchar *key,gint n) {
	if(djynn.config_file==NULL) return;
	g_key_file_set_integer(djynn.config_file,group,key,n);
}

gchar *djynn_cfg_get_str(const gchar *group,const gchar *key) {
	if(djynn.config_file==NULL) return NULL;
	gchar *str = g_key_file_get_string(djynn.config_file,group,key,NULL);
	return str;
}

void djynn_cfg_set_str(const gchar *group,const gchar *key,const gchar *str) {
	if(djynn.config_file==NULL) return;
	g_key_file_set_string(djynn.config_file,group,key,str);
}

gint djynn_msgbox_ask(const gchar *title,const gchar *msg,const gchar *item) {
	gint r;
	GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(geany->main_widgets->window),GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK_CANCEL,msg,item);
	gtk_window_set_title(GTK_WINDOW(dlg),title);
	r = (gint)gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(dlg);
	return r;
}

void djynn_msgbox_warn(const gchar *title,const gchar *msg,const gchar *item) {
	GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(geany->main_widgets->window),GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,msg,item);
	gtk_window_set_title(GTK_WINDOW(dlg),title);
	gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(dlg);
}

static void kb_activate(guint key_id) {
	djynn_keybind *kb = &djynn.keybinds[key_id];
	djynn_menu_item *m = &djynn.menu_items[kb->menu_item_index];
	(*m->activate)(kb->menu_item_index);
}

static struct {
	GtkWidget *ext_open_cmd;
	GtkWidget *ws_list;
	GtkWidget *sess_list;
	GtkWidget *show_icons;
	GtkWidget *show_tree_lines;
	GtkWidget *single_click_open;
	GtkWidget *double_click_ext;
	GtkWidget *activate_prj;
	GtkWidget *activate_sidebar;
} configure;

static void configure_cb(GtkDialog *dlg,gint response,gpointer data) {
	djynn_cfg_open();
	g_free(djynn.conf.ext_open_cmd);
	djynn.conf.ext_open_cmd = gtk_editable_get_chars(GTK_EDITABLE(configure.ext_open_cmd),0,-1);
	djynn_cfg_set_str(djynn.str.djynn,djynn.str.ext_open_cmd,djynn.conf.ext_open_cmd);
	djynn.conf.ws_list = gtk_combo_box_get_active(GTK_COMBO_BOX(configure.ws_list));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.ws_list,djynn.conf.ws_list);
	djynn.conf.sess_list = gtk_combo_box_get_active(GTK_COMBO_BOX(configure.sess_list));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.sess_list,djynn.conf.sess_list);
	djynn.conf.show_icons = gtk_combo_box_get_active(GTK_COMBO_BOX(configure.show_icons));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.show_icons,djynn.conf.show_icons);
	djynn.conf.show_tree_lines = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure.show_tree_lines));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.show_tree_lines,djynn.conf.show_tree_lines);
	djynn.conf.single_click_open = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure.single_click_open));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.single_click_open,djynn.conf.single_click_open);
	djynn.conf.double_click_ext = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure.double_click_ext));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.double_click_ext,djynn.conf.double_click_ext);
	djynn.conf.activate_prj = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure.activate_prj));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.activate_prj,djynn.conf.activate_prj);
	djynn.conf.activate_sidebar = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure.activate_sidebar));
	djynn_cfg_set_int(djynn.str.djynn,djynn.str.activate_sidebar,djynn.conf.activate_sidebar);
	djynn_cfg_save();
	djynn_cfg_close();
	djynn_pm_ws_save();
	djynn_pm_ws_load();
	djynn_pm_configure();
}

GtkWidget *plugin_configure(GtkDialog *dlg) {
	GtkWidget *label;
	GtkWidget *vbox,*hbox;

	vbox = gtk_vbox_new(FALSE,0);

	hbox = gtk_hbox_new(FALSE,0);
	label = gtk_label_new(_("External open command"));
	configure.ext_open_cmd = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(configure.ext_open_cmd),djynn.conf.ext_open_cmd);
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	ui_widget_set_tooltip_text(configure.ext_open_cmd,
		_("The command to execute when using \"Open externally\". You can use %f and %d wildcards.\n"
		  "%f will be replaced with the filename including full path\n"
		  "%d will be replaced with the path name of the selected file without the filename"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,6);
	gtk_box_pack_start(GTK_BOX(hbox),configure.ext_open_cmd,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,6);

	hbox = gtk_hbox_new(FALSE,0);
	label = gtk_label_new(_("Workspace List"));
	configure.ws_list = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.ws_list),_("Hidden"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.ws_list),_("Top"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.ws_list),_("Bottom"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,6);
	gtk_box_pack_start(GTK_BOX(hbox),configure.ws_list,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,6);
	ui_widget_set_tooltip_text(configure.ws_list,
		_("If position is changed, the option require plugin restart."));
	gtk_combo_box_set_active(GTK_COMBO_BOX(configure.ws_list),djynn.conf.ws_list);

	hbox = gtk_hbox_new(FALSE,0);
	label = gtk_label_new(_("Sessions List"));
	configure.sess_list = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.sess_list),_("Hidden"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.sess_list),_("Top"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.sess_list),_("Bottom"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,6);
	gtk_box_pack_start(GTK_BOX(hbox),configure.sess_list,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,6);
	ui_widget_set_tooltip_text(configure.sess_list,
		_("If position is changed, the option require plugin restart."));
	gtk_combo_box_set_active(GTK_COMBO_BOX(configure.sess_list),djynn.conf.sess_list);

	hbox = gtk_hbox_new(FALSE,0);
	label = gtk_label_new(_("Show icons"));
	configure.show_icons = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.show_icons),_("None"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.show_icons),_("Base"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(configure.show_icons),_("Content-type"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,6);
	gtk_box_pack_start(GTK_BOX(hbox),configure.show_icons,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,6);
	gtk_combo_box_set_active(GTK_COMBO_BOX(configure.show_icons),djynn.conf.show_icons);

	configure.show_tree_lines = gtk_check_button_new_with_label(_("Show tree lines"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure.show_tree_lines),djynn.conf.show_tree_lines);
#if GTK_CHECK_VERSION(2,10,0)
	gtk_box_pack_start(GTK_BOX(vbox),configure.show_tree_lines,FALSE,FALSE,0);
#endif

	configure.single_click_open = gtk_check_button_new_with_label(_("Single click, open document and focus it"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure.single_click_open),djynn.conf.single_click_open);
	gtk_box_pack_start(GTK_BOX(vbox),configure.single_click_open,FALSE,FALSE,0);

	configure.double_click_ext = gtk_check_button_new_with_label(_("Double click open directory"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure.double_click_ext),djynn.conf.double_click_ext);
	gtk_box_pack_start(GTK_BOX(vbox),configure.double_click_ext,FALSE,FALSE,0);

	configure.activate_prj = gtk_check_button_new_with_label(_("Activate project opens config-file instead of Geany project"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure.activate_prj),djynn.conf.activate_prj);
	gtk_box_pack_start(GTK_BOX(vbox),configure.activate_prj,FALSE,FALSE,0);
	ui_widget_set_tooltip_text(configure.activate_prj,
		_("When double clicking on the project, the project configuration file is opened in the editor "
		  "instead of opening the Geany project (see Project in the menu)."));

	configure.activate_sidebar = gtk_check_button_new_with_label(_("Activate document also activates sidebar page"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure.activate_sidebar),djynn.conf.activate_sidebar);
	gtk_box_pack_start(GTK_BOX(vbox),configure.activate_sidebar,FALSE,FALSE,0);
	ui_widget_set_tooltip_text(configure.activate_sidebar,
		_("When opening a document in the editor, if another page is visible in the sidebar the Project "
		  "tab is automatically activated."));

	gtk_widget_show_all(vbox);
	g_signal_connect(dlg,"response",G_CALLBACK(configure_cb),NULL);
	return vbox;
}

void plugin_init(GeanyData *data) {
	gint i;
	gchar str[257],key[257],*p1;
	djynn_keybind *kb;
	djynn_menu_item *m;

//printf("plugin_init()\n");
	djynn.config_dir = g_strconcat(geany->app->configdir,G_DIR_SEPARATOR_S,"plugins",G_DIR_SEPARATOR_S,
			djynn.str.djynn,G_DIR_SEPARATOR_S,NULL);
	if(g_file_test(djynn.config_dir,G_FILE_TEST_EXISTS)==FALSE) g_mkdir(djynn.config_dir,0700);

	p1 = g_strconcat(djynn.config_dir,"djynn.log",NULL);
	djynn.log = fopen(p1,"w");
	if(!djynn.log) perror(p1);
	g_free(p1);

	djynn.config_filename = g_strconcat(djynn.config_dir,"djynn.conf",NULL);
	if(g_file_test(djynn.config_filename,G_FILE_TEST_EXISTS)==FALSE) {
		FILE *fp = fopen(djynn.config_filename,"w");
		if(fp) {
			fprintf(fp,
				"\n[djynn]\n"
				"ext_open_cmd=%s\n"
				"ws_list=1\n"
				"sess_list=1\n"
				"show_icons=1\n"
				"show_tree_lines=0\n"
				"single_click_open=1\n"
				"double_click_ext=1\n"
				"activate_prj=0\n"
				"activate_sidebar=1\n"
				"workspace=1\n"
				"session=1\n"
				"\n"
				"[workspace]\n"
				"workspace_id=1\n"
				"workspace_n=1\n"
				"workspace_1=0001:%s\n"
				"\n"
				"[session]\n"
				"session_id=1\n"
				"session_n=1\n"
				"session_1=0001:%s\n"
				"\n"
				"[workspace_0001]\n"
				"project_n=0\n",
#ifdef G_OS_WIN32
				"explorer '%d'",
#else
				"nautilus '%d'",
#endif
				_("Workspace"),
				_("Session")
				);
			fclose(fp);
		} else perror(djynn.config_filename);
	}

	djynn_cfg_open();

	djynn.conf.ext_open_cmd = djynn_cfg_get_str(djynn.str.djynn,djynn.str.ext_open_cmd);
	if(djynn.conf.ext_open_cmd==NULL || *djynn.conf.ext_open_cmd=='\0') {
		if(djynn.conf.ext_open_cmd!=NULL) g_free(djynn.conf.ext_open_cmd);
		djynn_cfg_set_str(djynn.str.djynn,djynn.str.ext_open_cmd,
#ifdef G_OS_WIN32
				"explorer '%d'"
#else
				"nautilus '%d'"
#endif
		);
		djynn.conf.ext_open_cmd = djynn_cfg_get_str(djynn.str.djynn,djynn.str.ext_open_cmd);
	}
	djynn.conf.ws_list = djynn_cfg_get_int(djynn.str.djynn,djynn.str.ws_list);
	djynn.conf.sess_list = djynn_cfg_get_int(djynn.str.djynn,djynn.str.sess_list);
	djynn.conf.show_icons = djynn_cfg_get_int(djynn.str.djynn,djynn.str.show_icons);
	djynn.conf.show_tree_lines = djynn_cfg_get_int(djynn.str.djynn,djynn.str.show_tree_lines);
	djynn.conf.single_click_open = djynn_cfg_get_int(djynn.str.djynn,djynn.str.single_click_open);
	djynn.conf.double_click_ext = djynn_cfg_get_int(djynn.str.djynn,djynn.str.double_click_ext);
	djynn.conf.activate_prj = djynn_cfg_get_int(djynn.str.djynn,djynn.str.activate_prj);
	djynn.conf.activate_sidebar = djynn_cfg_get_int(djynn.str.djynn,djynn.str.activate_sidebar);

	i = djynn_cfg_get_int(djynn.str.djynn,djynn.str.workspace);
	sprintf(str,djynn.str.workspace_d,i);
	strcpy(str,djynn_cfg_get_str(djynn.str.workspace,str));
	p1 = strchr(str,':'),*p1 = '\0',++p1;
	djynn.workspace = g_strdup(p1);
	sprintf(key,"%s_%s",djynn.str.workspace,str);
	djynn.workspace_key = g_strdup(key);

	djynn.tools_menu = gtk_menu_new();

	i = 0;
	djynn_pm_init(data,&i);
	djynn_base64_init(data,&i);
	djynn_comment_init(data,&i);
	djynn_sort_init(data,&i);

	djynn_cfg_close();

	djynn.tools_menu_item = gtk_menu_item_new_with_label(_("Djynn"));
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
	djynn_pm_cleanup();
	djynn_base64_cleanup();
	djynn_comment_cleanup();
	djynn_sort_cleanup();
	gtk_widget_destroy(djynn.tools_menu_item);
	g_free(djynn.config_dir);
	g_free(djynn.config_filename);
	g_free(djynn.conf.ext_open_cmd);
	g_free(djynn.workspace);
	g_free(djynn.workspace_key);
//fprintf(djynn.log,"plugin_cleanup()\n");
	fclose(djynn.log);
//printf("plugin_cleanup()\n");
}


