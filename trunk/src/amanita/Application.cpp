

#include "_config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <locale.h>

#ifdef USE_WIN32
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#include <winsock2.h>
#include <windows.h>
//#include <ctype.h>
#include <commctrl.h>
//#include <richedit.h>
#endif

#include <amanita/Application.h>
#include <amanita/gui/aWindow.h>
#include <amanita/gui/aMenu.h>
#include <amanita/File.h>
#include <amanita/String.h>
//#include <amanita/Vector.h>
//#include <amanita/net/Http.h>


#ifdef USE_WIN32
HINSTANCE hMainInstance;

int WINAPI WinMain(HINSTANCE hi,HINSTANCE hp,LPSTR cmd,int show) {
	int i,argc;
	LPWSTR *args = CommandLineToArgvW(GetCommandLineW(),&argc);
	char *argv[argc];
	for(i=0; i<argc; i++) {
		argv[i] = w2char(args[i]);
		fprintf(stderr,"Cmd[%d]: %s\n",i,argv[i]);
	}
	LocalFree(args);
	hMainInstance = hi;
	i = main(argc,argv);
	while(--argc>=0) free(argv[argc]);
	return i;
}
#endif


namespace a {

//#define _QUOTE_MACRO(x) #x
//#define QUOTE_MACRO(x) _QUOTE_MACRO(x)

Application::Application(uint32_t params,const char *prj,const char *nm) : app_thread() {
	FILE *fp;
	char str[257],home[257],data[257];

	app_init_params = params;
	app_out = 0;
	app_project = 0;
	app_name = 0;
	app_user_agent = 0;
	app_home_dir = 0;
	app_data_dir = 0;
	app_locale_dir = 0;
	app_fonts_dir = 0;
	app_local_id = 0;
	app_local_time = 0;
	app_last_access = 0;
	window = 0;

	setProjectName(prj);
	setApplicationName(nm);

	if((app_init_params&INIT_DIRECTORIES)) {
		homedir(str,256);
#if defined USE_GTK
		sprintf(home,"%s/.%s/",str,app_project);
#elif defined USE_WIN32
		sprintf(home,"%s\\",str);
#endif
//	setProperty(property_key[APP_DIR_HOME],home);
		if(!exists(home)) mkdir(home);

#if defined USE_GTK
		snprintf(data,256,"/usr/share/%s/",app_project);
		if(!exists(data)) {
			snprintf(data,256,"/usr/local/share/%s/",app_project);
			if(!exists(data)) {
				snprintf(data,256,"%s/.local/share/%s/",home,app_project);
				if(!exists(data)) sprintf(data,"%sdata" aFILE_DIRSEP,home);
			}
		}
#elif defined USE_WIN32
		sprintf(data,"%sdata" aFILE_DIRSEP,home);
#endif

		fontsdir(str,256);
		strcat(str,aFILE_DIRSEP);
		if(!exists(str)) mkdir(str);
debug_output("app_dir_fonts=%s\n",str);
		app_fonts_dir = strdup(str);
	} else {
		currentdir(str,256);
		sprintf(home,"%s" aFILE_DIRSEP,str);
		strcpy(data,home);
	}

	app_home_dir = strdup(home);
debug_output("app_home_dir: %s\n",home);
	app_data_dir = strdup(data);
debug_output("app_data_dir: %s\n",data);
	
	if((app_init_params&INIT_LOG)) {
		sprintf(str,"%s%s.log",app_home_dir,app_project);
		app_last_access = modified(str);
debug_output("logfile: %s\nlast_access %" PRIu64 "\n\n",str,(uint64_t)app_last_access);
		if(!(app_out=fopen(str,"wb")) ||
			!(fp=freopen(str,"ab",app_out)) ||
			!(fp=freopen(str,"ab",stdout)) ||
			!(fp=freopen(str,"ab",stderr))) perror(str);
debug_output("Application::Application()");
	} else {
		executable(str,256);
		app_last_access = accessed(str);
	}
}

Application::~Application() {
debug_output("Application::~Application()\n");
	if(app_user_agent && app_user_agent!=app_project) free(app_user_agent);
	app_user_agent = 0;
	if(app_name && app_name!=app_project) free(app_name);
	app_name = 0;
	if(app_project) free(app_project);
	app_project = 0;

	if(app_home_dir) free(app_home_dir);
	app_home_dir = 0;
	if(app_data_dir) free(app_data_dir);
	app_data_dir = 0;
	if(app_locale_dir) free(app_locale_dir);
	app_locale_dir = 0;
	if(app_fonts_dir) free(app_fonts_dir);
	app_fonts_dir = 0;

	if(app_out) fclose(app_out);
	app_out = 0;

	if(window) delete window;
	window = 0;
debug_output("Completely finalized, could not have gone better! You the Man!\n");
}


uint32_t Application::open(int argc,char *argv[]) {
	if((app_init_params&INIT_GETTEXT)) {
debug_output("Application::init(AMANITA_INIT_GETTEXT,project=%s,localedir=%s)",app_project,app_locale_dir);
		setlocale(LC_ALL,"");
		bindtextdomain(app_project,app_locale_dir? app_locale_dir : ".");
		textdomain(app_project);
	}

#ifdef USE_GTK
	gtk_init(&argc,&argv);
	if((app_init_params&INIT_GUI)) {
		gtk_rc_parse_string (""
			"style \"scrollbar-style\" {\n"
			"  GtkScrolledWindow::scrollbar-spacing = 0\n"
			"}\n\n"
			"style \"notebook-style\" {\n"
			"  GtkWidget::focus-padding = 0\n"
			"  GtkWidget::focus-line-width = 0\n"
			"  GtkNotebook::tab-curvature = 0\n"
			"  GtkNotebook::tab-overlap = -1\n"
//			"  xthickness = 0\n"
//			"  ythickness = 0\n"
			"}\n\n"
			"style \"tab-close-button-style\" {\n"
			"  GtkWidget::focus-padding = 0\n"
			"  GtkWidget::focus-line-width = 0\n"
//			"  xthickness = 0\n"
//			"  ythickness = 0\n"
			"}\n\n"
			"widget \"*GtkScrolledWindow*\" style \"scrollbar-style\"\n"
			"widget \"*GtkNotebook*\" style \"notebook-style\"\n"
			"widget \"*.tab-close-button\" style \"tab-close-button-style\"\n"
		);
	}
	g_type_init();
	if((app_init_params&INIT_THREADS)) {
		if(!g_thread_supported()) g_thread_init(0);
		gdk_threads_init();
	}
#endif

	if((app_init_params&INIT_SOCKETS)) {
/*#ifdef USE_SDL
		if(SDLNet_Init()==-1) return INIT_SOCKETS;
#endif*/
#ifdef USE_WIN32
		WSADATA wsadata;
		if(WSAStartup(MAKEWORD(2,0),&wsadata)!=0) {
debug_output("Socket Initialization Error.\n");
			WSACleanup();
			return INIT_SOCKETS;
		}
#endif
	}
#ifdef USE_WIN32
	if((app_init_params&INIT_GUI)) {
		INITCOMMONCONTROLSEX commctrl = { sizeof(INITCOMMONCONTROLSEX),0xffffffff };
		OleInitialize(0);
		InitCommonControlsEx(&commctrl);
	}
#endif
	return 0;
}


uint32_t Application::close() {
	if((app_init_params&INIT_SOCKETS)) {
#ifdef USE_SDL
		SDLNet_Quit();
#endif
#ifdef USE_WIN32
		WSACleanup();
#endif
	}
	if((app_init_params&INIT_GUI)) {
#ifdef USE_WIN32
		OleUninitialize();
#endif
	}
	return 0;
}

int Application::main() {
#ifdef USE_GTK
	create();
	if((app_init_params&INIT_THREADS)) {
		gdk_threads_enter();
		gtk_main();
		gdk_threads_leave();
	} else {
		gtk_main();
	}
	return 0;
#endif

#ifdef USE_WIN32
	HACCEL hacc = 0;
	HWND hwnd = 0;
	MSG msg;

	create();

	if(window && window->getMenu()) {
		aMenuItem *items = window->getMenu()->items,*m;
		int i,n,nitems = window->getMenu()->nitems;
		for(i=0,n=0,m=items; m && i<nitems; ++i,++m) if(m->acc!=-1) ++n;
		if(n>0) {
			ACCEL accel[n],*a;
			for(i=0,m=items,a=accel; i<nitems; ++i,++m) if(m->acc!=-1) {
				a->fVirt = (m->acc_mod==aKEY_CONTROL? FCONTROL : (m->acc_mod==aKEY_ALT? FALT : 0))|FVIRTKEY;
				a->key = m->acc;
				a->cmd = aWIDGET_MAKE_ID(aWIDGET_MENU,m->index);
				++a;
			}
			hacc = CreateAcceleratorTable(accel,n);
			hwnd = (HWND)window->getComponent();
		}
	}

	while(GetMessage(&msg,NULL,0,0)) {
		if((!hacc || !TranslateAccelerator(hwnd,hacc,&msg))) {
			if(msg.message==WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
#endif
}

void Application::quit() {
debug_output("Application::exit()\n");
#ifdef USE_GTK
	gtk_main_quit();
#endif
#ifdef USE_WIN32
	PostQuitMessage(0);
#endif
}


void Application::create() {
}

void Application::destroy() {
	quit();
}
/*
void Application::add(aComponent c,aWidget *w) {
	aComponent handle;
	handle = w->create(window,0);
#if defined(USE_GTK)
	gtk_container_add(GTK_CONTAINER(window),(GtkWidget *)handle);
#elif defined(USE_WIN32)
	w->hinst = hinst;
#endif
	child = w;
}
*/

enum {
	HOME,
	DATA,
	LOCALE,
	FONTS,
	HOST,
};

static void extract_dir(char *dir,const char *str,const char **dirs) {
	const char *p;
	char tag[33];
	int n;
	for(p=0; *str; ++str) {
		if(*str=='[' && !p) p = str+1;
		else if(*str==']' && p) {
			memcpy(tag,p,(size_t)(str-p));
			tag[(size_t)(str-p)] = '\0';
			if(!strcmp(tag,"home")) n = HOME;
			else if(!strcmp(tag,"data")) n = DATA;
			else if(!strcmp(tag,"locale")) n = LOCALE;
			else if(!strcmp(tag,"fonts")) n = FONTS;
			else if(!strcmp(tag,"host")) n = HOST;
			else n = -1;
			if(n>=0) {
				strcpy(dir,dirs[n]);
				dir += strlen(dir);
			}
			p = 0;
		} else if(!p) *dir++ = *str;
	}
	if(*dir!=*File::dirsep) *dir++ = *File::dirsep;
	*dir = '\0';
}

int Application::install(const char *host,const char *files,install_function func,void *obj) {
	char str[257],*p,from[257],to[257],from_dir[257],to_dir[257];
	const char *p1,*p2,*dirs[] = { app_home_dir,app_data_dir,app_locale_dir,app_fonts_dir,host };
	int n,http,cp;
	for(p1=files,n=0,http=0,cp=0; p1 && *p1; ) {
		if(!(p2=strchr(p1,'\n'))) break;
		if(*p1=='\n') cp = 0;
		else {
			memcpy(str,p1,(size_t)(p2-p1));
			str[(size_t)(p2-p1)] = '\0';
			p = strchr(str,'>');
			if(p) *p++ = '\0';
			else p = str;
			a::trim(str);
			if(p!=str) a::trim(p);
			if(cp==0) {
				extract_dir(from_dir,str,dirs);
				if(p!=str) extract_dir(to_dir,p,dirs);
				else strcpy(to_dir,from_dir);
				http = !strncmp(from_dir,"http://",7);
				cp = (!http && !exists(from_dir)) || !exists(to_dir)? 2 : 1;
			} else if(cp==1) {
				sprintf(to,"%s%s",to_dir,p);
				if(http) {
				} else {
					sprintf(from,"%s%s",from_dir,str);
					if(strcmp(from,to) && (!exists(to) ||
							(modified(from)>modified(to)))) copy(from,to);
				}
			}
		}
		p1 = p2+1;
	}
	return n;
}

/*
int Application::install(const char *host,const char *path,Vector &files,install_function func,void *obj) {
debug_output("Application::install(host=\"%s\",path=\"%s\")\n",host,path);
	int i,n,f,pl = path? strlen(path) : 0,st = 0;
	const char *home = getHomeDir();
	const char *fonts = getFontsDir();
	const char *file,*p,*e,*dest;
	char str[256];
	Http http(*this);
	FILE *fp;
	size_t sz;
	for(i=0,n=0,f=0; i<(int)files.size(); i++) {
		p = (const char *)files[i];
debug_output("file=%s\n",p);
		if(pl && strncmp(path,p,pl)==0) p += pl;
		if(*p=='/') p++;
		e = &p[strlen(p)-1];
		if(e-3>=p && strncmp(e-3,".ttf",4)==0) {
			dest = fonts,f++,st |= 1;
			if(strchr(p,'/')) p = strrchr(p,'/');
			else if(strchr(p,'\\')) p = strrchr(p,'\\');
		} else dest = home;
		sprintf(str,"%s%s",dest,p);
debug_output("str=%s\n",str);
		if(*e=='/') {
			if(!File::exists(str)) File::mkdir(str);
			continue;
		}
debug_output("http.get(%s,%s)\n",host,(const char *)files[i]);
		file = http.get(host,(const char *)files[i]);
debug_output("http.get()\n");
		sz = http.getFileSize();
debug_output("sz=%lu\n",(unsigned long)sz);
		fp = fopen(str,"wb");
		if(sz) sz = fwrite(file,sz,1,fp);
		fclose(fp);
		if(func) func(obj,(const char *)files[i],i,files.size()-1,st);
		n++;
	}
	return n;
}
*/

#if defined USE_GTK
void Application::updateFontCache() {
	const char *p;
	gchar *so,*se;
	g_spawn_command_line_sync(p="fc-cache -f -v",&so,&se,NULL,NULL);
debug_output("spawn: %s\nstdout:\n%sstderr:\n\n%s",p,so,se);
}
#endif

void Application::setProjectName(const char *prj) {
	if(!prj || !*prj) return;
	if(app_project) free(app_project);
	app_project = strdup(prj);
	if(!app_name || app_name==app_project) app_name = app_project;
	if(!app_user_agent || app_user_agent==app_project) app_user_agent = app_project;
}

void Application::setApplicationName(const char *nm) {
	if(!nm || !*nm) return;
	if(app_name && app_name!=app_project) free(app_name);
	app_name = strdup(nm);
}

void Application::setUserAgent(const char *ua) {
	if(!ua || !*ua) return;
	if(app_user_agent && app_user_agent!=app_project) free(app_user_agent);
	app_user_agent = strdup(ua);
}

void Application::setHomeDir(const char *dir) {
	if(app_home_dir) free(app_home_dir);
	app_home_dir = strdup(dir);
}

void Application::setDataDir(const char *dir) {
	if(app_data_dir) free(app_data_dir);
	app_data_dir = strdup(dir);
}

void Application::setLocaleDir(const char *dir) {
	if(app_locale_dir) free(app_locale_dir);
	app_locale_dir = strdup(dir);
}

void Application::setFontsDir(const char *dir) {
	if(app_locale_dir) free(app_fonts_dir);
	app_fonts_dir = strdup(dir);
}


}; /* namespace a */


