

#include <stdlib.h>
#include <string.h>
#include <ipomoea/string_t.h>
#include "djynn.h"




static gboolean delete_event_cb(GtkWidget *widget,GtkWidget *event,gpointer data) {
	GtkWidget *window = (GtkWidget *)data;
	gtk_widget_destroy(window);
	return FALSE;
}

static void cancel_clicked_cb(GtkWidget *widget,gpointer data) {
	GtkWidget *window = (GtkWidget *)data;
	gtk_widget_destroy(window);
}


struct {
	GtkWidget *window;
	GtkWidget *name;
	GtkWidget *ok;
	GtkWidget *cancel;
} workspace_dlg;


static void create_workspace_cb(GtkWidget *widget,gpointer data) {
	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
	gtk_widget_destroy(workspace_dlg.window);
}


void djynn_projectmanager_open_new_workspace_dlg() {
	GtkWidget *table;
	GtkWidget *hbox;
	GtkWidget *label;

	workspace_dlg.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(workspace_dlg.window),"Project Manager");
	gtk_window_set_modal(GTK_WINDOW(workspace_dlg.window),TRUE);
	gtk_window_set_resizable(GTK_WINDOW(workspace_dlg.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(workspace_dlg.window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(workspace_dlg.window),5);

	table = gtk_table_new(1,4,FALSE);

	label = gtk_label_new("Workspace Name:");
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	workspace_dlg.name = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),workspace_dlg.name,0,1,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	hbox = gtk_hbox_new(FALSE,5);
	workspace_dlg.ok = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(workspace_dlg.ok),"clicked",G_CALLBACK(create_workspace_cb),NULL);
	gtk_widget_set_size_request(workspace_dlg.ok,120,-1);
	gtk_box_pack_start(GTK_BOX(hbox),workspace_dlg.ok,FALSE,FALSE,0);
	workspace_dlg.cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(workspace_dlg.cancel),"clicked",G_CALLBACK(cancel_clicked_cb),workspace_dlg.window);
	gtk_widget_set_size_request(workspace_dlg.cancel,120,-1);
	gtk_box_pack_start(GTK_BOX(hbox),workspace_dlg.cancel,FALSE,FALSE,0);
	gtk_table_attach(GTK_TABLE(table),hbox,0,1,2,3,
		(GtkAttachOptions)(GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);

	gtk_container_add(GTK_CONTAINER(workspace_dlg.window),table);

	gtk_widget_show_all(workspace_dlg.window);
	g_signal_connect(G_OBJECT(workspace_dlg.window),"delete_event",G_CALLBACK(delete_event_cb),workspace_dlg.window);
}


struct {
	GtkWidget *window;
	GtkWidget *name;
	GtkWidget *directory;
	GtkWidget *description;
	GtkWidget *create;
	GtkWidget *cancel;
} project_dlg;


static void browse_clicked_cb(GtkWidget *widget,gpointer data) {
	const gchar *dir;
	GtkWidget *dlg;
	gint r;

	dlg = gtk_file_chooser_dialog_new("Select Folder",GTK_WINDOW(project_dlg.window),
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);
	dir = gtk_entry_get_text(GTK_ENTRY(project_dlg.directory));
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dlg),dir);
	r = gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_hide(dlg);
	if(r==GTK_RESPONSE_OK) {
		dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
		gtk_entry_set_text(GTK_ENTRY(project_dlg.directory),dir);
	}
	gtk_widget_destroy(dlg);
}

static void create_project_clicked_cb(GtkWidget *widget,gpointer data) {
	int n;
	char key[64],prj1[257],prj2[257];
	const gchar *name,*dir,*descr;
	FILE *fp;

	name = gtk_entry_get_text(GTK_ENTRY(project_dlg.name));
	dir = gtk_entry_get_text(GTK_ENTRY(project_dlg.directory));
	descr = gtk_entry_get_text(GTK_ENTRY(project_dlg.description));

	djynn_open_config();
	n = djynn_config_get_int(djynn.workspace,djynn.str.project_n);
	n++;
	djynn_config_set_int(djynn.workspace,djynn.str.project_n,n);
	sprintf(key,djynn.str.project_d,n);
	sprintf(prj1,"%s%s.project",djynn.config_dir,name);
	sprintf(prj2,"%s%s.geany",djynn.config_dir,name);
fprintf(djynn.log,"create_project_clicked_cb(workspace=%s,key=%s,prj1=%s)\n",djynn.workspace,key,prj1);
fflush(djynn.log);
	djynn_config_set_str(djynn.workspace,key,prj1);

	djynn_save_config();
	djynn_close_config();

	fp = fopen(prj1,"w");
	fprintf(fp,"+%s\n"
		"%s\n"
		"%s\n"
		"-src\n",
		name,dir,prj2);
	fclose(fp);
	fp = fopen(prj2,"w");
	fprintf(fp,"\n"
			"[indentation]\n"
			"indent_width=3\n"
			"indent_type=1\n"
			"indent_hard_tab_width=8\n"
			"detect_indent=false\n"
			"indent_mode=2\n"
			"\n[project]\n"
			"name=%s\n"
			"base_path=%s\n"
			"make_in_base_path=false\n"
			"description=%s\n"
			"run_cmd=%s\n",
			name,dir,descr,name);
	fclose(fp);

	djynn_projectmanager_save_workspace();
	djynn_projectmanager_load_workspace();
	gtk_widget_destroy(project_dlg.window);
}


void djynn_projectmanager_open_new_project_dlg() {
	GtkWidget *table;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *button;

	project_dlg.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(project_dlg.window),"Project Manager");
	gtk_window_set_modal(GTK_WINDOW(project_dlg.window),TRUE);
	gtk_window_set_resizable(GTK_WINDOW(project_dlg.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(project_dlg.window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(project_dlg.window),0);

	table = gtk_table_new(1,7,FALSE);

	label = gtk_label_new("Name:");
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	project_dlg.name = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),project_dlg.name,0,1,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new("Base Path:");
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,2,3,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	hbox = gtk_hbox_new(FALSE,5);
	project_dlg.directory = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(project_dlg.directory),g_get_home_dir());
	gtk_box_pack_start(GTK_BOX(hbox),project_dlg.directory,TRUE,TRUE,0);
	button = gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(browse_clicked_cb),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	gtk_table_attach(GTK_TABLE(table),hbox,0,1,3,4,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	label = gtk_label_new("Description:");
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,4,5,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	project_dlg.description = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),project_dlg.description,0,1,5,6,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	hbox = gtk_hbox_new(FALSE,5);
	project_dlg.create = gtk_button_new_with_label("Create Project");
	g_signal_connect(G_OBJECT(project_dlg.create),"clicked",G_CALLBACK(create_project_clicked_cb),NULL);
	gtk_widget_set_size_request(project_dlg.create,120,-1);
	gtk_box_pack_start(GTK_BOX(hbox),project_dlg.create,FALSE,FALSE,0);
	project_dlg.cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(project_dlg.cancel),"clicked",G_CALLBACK(cancel_clicked_cb),project_dlg.window);
	gtk_widget_set_size_request(project_dlg.cancel,120,-1);
	gtk_box_pack_start(GTK_BOX(hbox),project_dlg.cancel,FALSE,FALSE,0);
	gtk_table_attach(GTK_TABLE(table),hbox,0,1,6,7,
		(GtkAttachOptions)(GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),7,7);

	gtk_container_add(GTK_CONTAINER(project_dlg.window),table);

	gtk_widget_show_all(project_dlg.window);
	g_signal_connect(G_OBJECT(project_dlg.window),"delete_event",G_CALLBACK(delete_event_cb),project_dlg.window);
}


struct {
	GtkWidget *window;
	GtkWidget *name;
	GtkWidget *ok;
	GtkWidget *cancel;
} folder_dlg;


static void create_project_folder_cb(GtkWidget *widget,gpointer data) {
	const gchar *name = gtk_entry_get_text(GTK_ENTRY(folder_dlg.name));
	djynn_projectmanager_add_folder(name);
	gtk_widget_destroy(folder_dlg.window);
}


void djynn_projectmanager_open_new_folder_dlg() {
	GtkWidget *table;
	GtkWidget *hbox;
	GtkWidget *label;

	folder_dlg.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(folder_dlg.window),"Project Manager");
	gtk_window_set_modal(GTK_WINDOW(folder_dlg.window),TRUE);
	gtk_window_set_resizable(GTK_WINDOW(folder_dlg.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(folder_dlg.window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(folder_dlg.window),5);

	table = gtk_table_new(1,4,FALSE);

	label = gtk_label_new("Folder Name:");
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_table_attach(GTK_TABLE(table),label,0,1,0,1,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);
	folder_dlg.name = gtk_entry_new();
	gtk_table_attach(GTK_TABLE(table),folder_dlg.name,0,1,1,2,
		(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),(GtkAttachOptions)(GTK_FILL|GTK_SHRINK),2,2);

	hbox = gtk_hbox_new(FALSE,5);
	folder_dlg.ok = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(folder_dlg.ok),"clicked",G_CALLBACK(create_project_folder_cb),NULL);
	gtk_widget_set_size_request(folder_dlg.ok,120,-1);
	gtk_box_pack_start(GTK_BOX(hbox),folder_dlg.ok,FALSE,FALSE,0);
	folder_dlg.cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(folder_dlg.cancel),"clicked",G_CALLBACK(cancel_clicked_cb),folder_dlg.window);
	gtk_widget_set_size_request(folder_dlg.cancel,120,-1);
	gtk_box_pack_start(GTK_BOX(hbox),folder_dlg.cancel,FALSE,FALSE,0);
	gtk_table_attach(GTK_TABLE(table),hbox,0,1,2,3,
		(GtkAttachOptions)(GTK_SHRINK),(GtkAttachOptions)(GTK_FILL|GTK_EXPAND),5,5);

	gtk_container_add(GTK_CONTAINER(folder_dlg.window),table);

	gtk_widget_show_all(folder_dlg.window);
	g_signal_connect(G_OBJECT(folder_dlg.window),"delete_event",G_CALLBACK(delete_event_cb),folder_dlg.window);
}



