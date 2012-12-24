

#include <stdlib.h>
#include <string.h>
#include <ipomoea/string_t.h>
#include "djynn.h"


static const gchar *project_manager_title = _("Project Manager");


static gboolean delete_event_cb(GtkWidget *widget,GtkWidget *event,gpointer data) {
	GtkWidget *dlg = (GtkWidget *)data;
	gtk_widget_destroy(dlg);
	return FALSE;
}

static void browse_select_folder_cb(GtkWidget *widget,gpointer data) {
	GtkWidget *file_chooser;
	GtkWidget *directory = (GtkWidget *)data;
	const gchar *dir;
	gint r;
	file_chooser = gtk_file_chooser_dialog_new(_("Select Folder"),GTK_WINDOW(geany_data->main_widgets->window),
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);
	dir = gtk_entry_get_text(GTK_ENTRY(directory));
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser),dir);
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(file_chooser),TRUE);
	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(file_chooser),TRUE);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser),FALSE);
	gtk_file_chooser_set_create_folders(GTK_FILE_CHOOSER(file_chooser),TRUE);
	r = gtk_dialog_run(GTK_DIALOG(file_chooser));
	gtk_widget_hide(file_chooser);
	if(r==GTK_RESPONSE_OK) {
		dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
		gtk_entry_set_text(GTK_ENTRY(directory),dir);
	}
	gtk_widget_destroy(file_chooser);
}

static GtkWidget *projectmanager_dlg(GtkWidget *vbox,gint w,gint h) {
	GtkWidget *dialog;
	GtkWidget *area;
	GtkWidget *hbox;
	dialog = gtk_dialog_new_with_buttons(project_manager_title,
			GTK_WINDOW(geany_data->main_widgets->window),
			GTK_DIALOG_MODAL|GTK_DIALOG_NO_SEPARATOR,
			GTK_STOCK_OK,GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_default_size(GTK_WINDOW(dialog),320,-1);
	gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(dialog),5);
	area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	hbox = gtk_hbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),vbox,TRUE,TRUE,5);
	gtk_box_pack_start(GTK_BOX(area),hbox,FALSE,FALSE,0);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
	g_signal_connect(G_OBJECT(dialog),"delete_event",G_CALLBACK(delete_event_cb),dialog);
	gtk_widget_show_all(dialog);
	return dialog;
}

void djynn_pm_ws_dlg(gboolean create) {
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *label;
	GtkWidget *name;
	gint r;

	vbox = gtk_vbox_new(FALSE,0);
	vbox2 = gtk_vbox_new(FALSE,2);
	label = gtk_label_new(_("Workspace Name:"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	name = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(name),TRUE);
	if(!create) gtk_entry_set_text(GTK_ENTRY(name),djynn.workspace);
	gtk_box_pack_start(GTK_BOX(vbox2),name,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),vbox2,FALSE,FALSE,7);

	dialog = projectmanager_dlg(vbox,320,-1);
	while(1) {
		r = gtk_dialog_run(GTK_DIALOG(dialog));
		if(r==GTK_RESPONSE_OK) {
			const gchar *nm = gtk_entry_get_text(GTK_ENTRY(name));
			if(nm==NULL || *nm=='\0') djynn_msgbox_warn(project_manager_title,_("Select a name for your workspace!"),NULL);
			else if(create) {
				gint id,n;
				gchar ws[5],key[64],str[257];
				djynn_pm_ws_save();
				djynn_cfg_open();
				id = djynn_cfg_get_int(djynn.str.workspace,djynn.str.workspace_id)+1;
				sprintf(ws,"%04d",id);
				djynn_cfg_set_int(djynn.str.workspace,djynn.str.workspace_id,id);
				n = djynn_cfg_get_int(djynn.str.workspace,djynn.str.workspace_n)+1;
				sprintf(key,djynn.str.workspace_d,n);
				sprintf(str,"%s:%s",ws,nm);
				djynn_cfg_set_str(djynn.str.workspace,key,str);
				djynn_cfg_set_int(djynn.str.workspace,djynn.str.workspace_n,n);
				sprintf(str,"%s_%s",djynn.str.workspace,ws);
				djynn_cfg_set_int(str,djynn.str.project_n,0);
				djynn_cfg_save();
				djynn_cfg_close();
				djynn_pm_ws_list_add(nm);
				djynn_pm_ws_list_select(n);
				break;
			} else {
				if(strcmp(djynn.workspace,nm)!=0) {
					gchar key[64],str[257];
					gint n;
					djynn_cfg_open();
					n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.workspace);
					sprintf(key,djynn.str.workspace_d,n);
					sprintf(str,"%s:%s",strchr(djynn.workspace_key,'_')+1,nm);
					djynn_cfg_set_str(djynn.str.workspace,key,str);
					djynn_cfg_save();
					djynn_cfg_close();
					djynn_pm_ws_list_set(n,nm);
				}
				break;
			}
		} else break;
	}
	gtk_widget_destroy(dialog);
}

void djynn_pm_sess_dlg(gboolean create) {
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *label;
	GtkWidget *name;
	gint r;
	gint n;
	gchar sess[5],key[64],str[257],*p1,*p2;

	djynn_cfg_open();
	n = djynn_cfg_get_int(djynn.str.djynn,djynn.str.session);
	sprintf(key,djynn.str.session_d,n);
	p1 = djynn_cfg_get_str(djynn.str.session,key);
	p2 = strchr(p1,':'),*p2 = '\0',++p2;
	strcpy(sess,p1);
	strcpy(str,p2);
	g_free(p1);

	vbox = gtk_vbox_new(FALSE,0);
	vbox2 = gtk_vbox_new(FALSE,2);
	label = gtk_label_new(_("Session Name:"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	name = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(name),TRUE);
	if(!create) gtk_entry_set_text(GTK_ENTRY(name),str);
	gtk_box_pack_start(GTK_BOX(vbox2),name,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),vbox2,FALSE,FALSE,7);

	dialog = projectmanager_dlg(vbox,320,-1);
	while(1) {
		r = gtk_dialog_run(GTK_DIALOG(dialog));
		if(r==GTK_RESPONSE_OK) {
			const gchar *nm = gtk_entry_get_text(GTK_ENTRY(name));
			if(nm==NULL || *nm=='\0') djynn_msgbox_warn(project_manager_title,_("Select a name for your session!"),NULL);
			else {
				if(create) {
					gint id = djynn_cfg_get_int(djynn.str.session,djynn.str.session_id)+1;
					sprintf(sess,"%04d",id);
					djynn_cfg_set_int(djynn.str.session,djynn.str.session_id,id);
					n = djynn_cfg_get_int(djynn.str.session,djynn.str.session_n)+1;
					sprintf(key,djynn.str.session_d,n);
					sprintf(str,"%s:%s",sess,nm);
					djynn_cfg_set_str(djynn.str.session,key,str);
					djynn_cfg_set_int(djynn.str.session,djynn.str.session_n,n);
					djynn_cfg_save();
					djynn_pm_sess_list_add(nm);
					djynn_pm_sess_list_select(n);
					break;
				} else if(strcmp(str,nm)!=0) {
					sprintf(str,"%s:%s",sess,nm);
					djynn_cfg_set_str(djynn.str.session,key,str);
					djynn_cfg_save();
					djynn_pm_sess_list_set(n,nm);
				}
				break;
			}
		} else break;
	}
	djynn_cfg_close();
	gtk_widget_destroy(dialog);
}

void djynn_pm_prj_dlg(djynn_project_file *f) {
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *name;
	GtkWidget *directory;
	GtkWidget *description;
	GtkWidget *button;
	gint r;

	vbox = gtk_vbox_new(FALSE,0);
	vbox2 = gtk_vbox_new(FALSE,2);
	label = gtk_label_new(_("Name:"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,FALSE,0);
	name = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(name),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox2),name,FALSE,FALSE,0);

	label = gtk_label_new(_("Base Path:"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,FALSE,0);
	hbox = gtk_hbox_new(FALSE,5);
	directory = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(directory),TRUE);
	gtk_entry_set_text(GTK_ENTRY(directory),g_get_home_dir());
	gtk_box_pack_start(GTK_BOX(hbox),directory,TRUE,TRUE,0);
	button = gtk_button_new_with_label(_("Browse..."));
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(browse_select_folder_cb),directory);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox2),hbox,FALSE,FALSE,0);

	label = gtk_label_new(_("Description:"));
	gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
	gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,FALSE,0);
	description = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(description),TRUE);
	gtk_box_pack_start(GTK_BOX(vbox2),description,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),vbox2,FALSE,FALSE,7);

	dialog = projectmanager_dlg(vbox,320,-1);
	while(1) {
		r = gtk_dialog_run(GTK_DIALOG(dialog));
		if(r==GTK_RESPONSE_OK) {
			const gchar *nm = gtk_entry_get_text(GTK_ENTRY(name));
			if(nm==NULL || *nm=='\0') djynn_msgbox_warn(project_manager_title,_("Select a name for your project!"),NULL);
			else {
				gint n;
				gchar key[64],prj1[257],prj2[257];
				sprintf(prj1,"%s%s.project",djynn.config_dir,nm);
				sprintf(prj2,"%s%s." GEANY_PROJECT_EXT,djynn.config_dir,nm);
				djynn_cfg_open();
				n = djynn_cfg_get_int(djynn.workspace_key,djynn.str.project_n)+1;
				djynn_cfg_set_int(djynn.workspace_key,djynn.str.project_n,n);
				sprintf(key,djynn.str.project_d,n);
fprintf(djynn.log,"create_project_clicked_cb(workspace=%s,key=%s,prj1=%s)\n",djynn.workspace,key,prj1);
fflush(djynn.log);
				djynn_cfg_set_str(djynn.workspace_key,key,prj1);
				if(!g_file_test(prj1,G_FILE_TEST_EXISTS)) {
					const gchar *dir,*descr;
					FILE *fp;
					dir = gtk_entry_get_text(GTK_ENTRY(directory));
					descr = gtk_entry_get_text(GTK_ENTRY(description));
					if((fp=fopen(prj1,"w"))) {
						fprintf(fp,"+%s\n"
							"%s\n"
							"%s\n"
							"-src\n",
							nm,dir,prj2);
						fclose(fp);
					} else perror(prj1);
					if((fp=fopen(prj2,"w"))) {
						const GeanyIndentPrefs *ind = editor_get_indent_prefs(NULL);
						fprintf(fp,"\n"
							"[indentation]\n"
							"indent_width=%d\n"
							"indent_type=%d\n"
							"indent_hard_tab_width=%d\n"
							"detect_indent=false\n"
							"indent_mode=2\n"
							"\n[project]\n"
							"name=%s\n"
							"base_path=%s\n"
							"make_in_base_path=false\n"
							"description=%s\n"
							"run_cmd=%s\n",
							ind->width,
							ind->type,
							ind->hard_tab_width,
							nm,
							dir,
							descr,
							nm
						);
						fclose(fp);
					} else perror(prj2);
				}
				djynn_cfg_save();
				djynn_cfg_close();
				djynn_pm_ws_save();
				djynn_pm_ws_load();
				project_close(FALSE);
				project_load_file(prj2);
				break;
			}
		} else break;
	}
	gtk_widget_destroy(dialog);
}

void djynn_pm_folder_dlg(djynn_project_file *f) {
	if(f==NULL) djynn_msgbox_warn(project_manager_title,_("No project, folder or file selected!"),NULL);
	else {
		GtkWidget *dialog;
		GtkWidget *vbox;
		GtkWidget *vbox2;
		GtkWidget *hbox;
		GtkWidget *label;
		GtkWidget *name;
		GtkWidget *add_files;
		GtkWidget *add_folders;
		GtkWidget *directory;
		GtkWidget *button;
		gchar dir[1024] = "",*p;
		gint r;
		if(f->type==DJYNN_PM_FILE) {
			strcpy(dir,f->path);
			p = strrchr(dir,G_DIR_SEPARATOR);
			if(p!=NULL) *p = '\0';
		} else {
			djynn_project_file *f1 = f;
			while(f1->parent!=NULL && f1->type!=DJYNN_PM_PROJECT) f1 = f1->parent;
			if(f1!=NULL) strcpy(dir,f1->path);
		}

		vbox = gtk_vbox_new(FALSE,0);
		vbox2 = gtk_vbox_new(FALSE,2);
		label = gtk_label_new(_("Folder Name:"));
		gtk_misc_set_alignment(GTK_MISC(label),0,0.5);
		gtk_box_pack_start(GTK_BOX(vbox2),label,FALSE,FALSE,0);
		name = gtk_entry_new();
		gtk_entry_set_activates_default(GTK_ENTRY(name),TRUE);
		gtk_box_pack_start(GTK_BOX(vbox2),name,FALSE,FALSE,0);
		gtk_box_pack_start(GTK_BOX(vbox),vbox2,FALSE,FALSE,7);

		vbox2 = gtk_vbox_new(FALSE,2);
		add_files = gtk_check_button_new_with_label(_("Add files in folder"));
		gtk_box_pack_start(GTK_BOX(vbox2),add_files,FALSE,FALSE,0);
		add_folders = gtk_check_button_new_with_label(_("Recurse in subfolders"));
		gtk_box_pack_start(GTK_BOX(vbox2),add_folders,FALSE,FALSE,0);
		hbox = gtk_hbox_new(FALSE,5);
		directory = gtk_entry_new();
		gtk_entry_set_activates_default(GTK_ENTRY(directory),TRUE);
		gtk_entry_set_text(GTK_ENTRY(directory),dir);
		gtk_box_pack_start(GTK_BOX(hbox),directory,TRUE,TRUE,0);
		button = gtk_button_new_with_label(_("Browse..."));
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(browse_select_folder_cb),directory);
		gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,0);
		gtk_box_pack_start(GTK_BOX(vbox2),hbox,FALSE,FALSE,0);
		gtk_box_pack_start(GTK_BOX(vbox),vbox2,FALSE,FALSE,7);

		dialog = projectmanager_dlg(vbox,320,-1);
		while(1) {
			r = gtk_dialog_run(GTK_DIALOG(dialog));
			if(r==GTK_RESPONSE_OK) {
				const gchar *nm = gtk_entry_get_text(GTK_ENTRY(name));
				if(nm==NULL || *nm=='\0') djynn_msgbox_warn(project_manager_title,_("Select a name for the folder!"),NULL);
				else {
					const gchar *dir = gtk_entry_get_text(GTK_ENTRY(directory));
					djynn_pm_folder_add(f,dir,nm,
						gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(add_files)),
						gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(add_folders)));
					break;
				}
			} else break;
		}
		gtk_widget_destroy(dialog);
	}
}



