

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

#include <amanita/aApplication.h>
#include <amanita/gui/aWindow.h>
#include <amanita/gui/aMenu.h>
//#include <amanita/aFile.h>
//#include <amanita/aVector.h>
//#include <amanita/net/aHttp.h>


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

//#define _QUOTE_MACRO(x) #x
//#define QUOTE_MACRO(x) _QUOTE_MACRO(x)

aApplication::aApplication() : app_thread()/*: app_lang_data(),app_properties()*/ {
//	FILE *fp;
//	char str[257],home[257],data[257];
//	int n;

	app_init_params = 0;
	app_out = 0;
	app_project = 0;
	app_name = 0;
	app_user_agent = 0;
	app_locale_dir = 0;
	app_local_id = 0;
	app_local_time = 0;
	app_last_access = 0;
	window = 0;

/*
	getExecutable(str,256);
	setProperty(property_key[APP_EXE],str);
debug_output("app_exe=%s\n",getExecutable());

	getHomeDir(str,256);
#if defined USE_GTK
	sprintf(home,"%s/.%s/",str,app_project);
#elif defined USE_WIN32
	sprintf(home,"%s\\",str);
#endif
	setProperty(property_key[APP_DIR_HOME],home);
	if(!aFile::exists(home)) aFile::mkdir(home);
debug_output("app_dir_home=%s\n",getHomeDir());

#if defined USE_GTK
	n = 0;
	snprintf(data,256,"/usr/share/%s/",app_project);
	if(!aFile::exists(data)) {
		n = 1;
		snprintf(data,256,"/usr/local/share/%s/",app_project);
		if(!aFile::exists(data)) {
			n = 2;
			snprintf(data,256,"%s/.local/share/%s/",home,app_project);
			if(!aFile::exists(data)) {
				n = 3;
				strcpy(data,home);
			}
		}
	}
#elif defined USE_WIN32
	n = 3;
	strcpy(data,home);
#endif
	setProperty(property_key[APP_DIR_DATA],data);
	if(n==3 && !aFile::exists(data)) aFile::mkdir(data);
debug_output("app_dir_data=%s\n",getDataDir());

	char fonts[257];
	getFontsDir(str,256);
	sprintf(fonts,"%s" FILE_DIRSEP,str);
	setProperty(property_key[APP_DIR_FONTS],fonts);
	if(!aFile::exists(fonts)) aFile::mkdir(fonts);
debug_output("app_dir_fonts=%s\n",getFontsDir());

	sprintf(str,"%slang" FILE_DIRSEP,data);
	setProperty(property_key[APP_DIR_LANG],str);
	if(n==3 && !aFile::exists(str)) aFile::mkdir(str);
debug_output("app_dir_lang=%s\n",getLanguageDir());

	sprintf(str,"%s%s.log",home,app_name);
	app_last_access = aFile::modified(str);
debug_output("logfile=%s\nlast_access=%" PRIu64 "\n\n",str,(uint64_t)app_last_access);
	app_out = fopen(str,"w");
	fp = freopen(str,"a",app_out);
	fp = freopen(str,"a",stdout);
	fp = freopen(str,"a",stderr);
debug_output("aApplication::aApplication()");
*/
}

aApplication::aApplication(const char *prj,const char *nm) : app_thread() {
	app_init_params = 0;
	app_out = 0;
	app_project = 0;
	app_name = 0;
	app_user_agent = 0;
	setApplicationName(nm);
	setProjectName(prj);
	app_locale_dir = 0;
	app_local_id = 0;
	app_local_time = 0;
	app_last_access = 0;
	window = 0;
}

aApplication::~aApplication() {
debug_output("aApplication::~aApplication()\n");
	if(app_user_agent && app_user_agent!=app_project) free(app_user_agent);
	app_user_agent = 0;
	if(app_name && app_name!=app_project) free(app_name);
	app_name = 0;
	if(app_project) free(app_project);
	app_project = 0;

	if(app_locale_dir) free(app_locale_dir);
	app_locale_dir = 0;

	if(app_out) fclose(app_out);
	app_out = 0;

	if(window) delete window;
	window = 0;
debug_output("Completely finalized, could not have gone better! You the Man!\n");
}


uint32_t aApplication::open(int argc,char *argv[],uint32_t params) {
	app_init_params = params;

	if((app_init_params&aINIT_GETTEXT)) {
debug_output("aApplication::init(AMANITA_INIT_GETTEXT,project=%s,localedir=%s)",app_project,app_locale_dir);
		setlocale(LC_ALL,"");
		bindtextdomain(app_project,app_locale_dir? app_locale_dir : ".");
		textdomain(app_project);
	}

#ifdef USE_GTK
	gtk_init(&argc,&argv);
	if((app_init_params&aINIT_GUI)) {
		gtk_rc_parse_string (""
			"style \"scrollbar-style\" {\n"
			"  GtkScrolledWindow::scrollbar-spacing = 0\n"
			"}\n\n"
			"style \"notebook-style\" {\n"
			"  GtkWidget::focus-padding = 0\n"
			"  GtkWidget::focus-line-width = 0\n"
			"  GtkNotebook::tab-curvature = 0\n"
			"  GtkNotebook::tab-overlap = -1\n"
			"  xthickness = 0\n"
			"  ythickness = 0\n"
			"}\n\n"
			"style \"tab-close-button-style\" {\n"
			"  GtkWidget::focus-padding = 0\n"
			"  GtkWidget::focus-line-width = 0\n"
//			"  xthickness = 0\n"
//			"  ythickness = 0\n"
			"}\n\n"
			"widget \"*GtkScrolledWindow*\" style \"scrollbar-style\""
			"widget \"*GtkNotebook*\" style \"notebook-style\""
			"widget \"*.tab-close-button\" style \"tab-close-button-style\""
		);
	}
	g_type_init();
	if((app_init_params&aINIT_THREADS)) {
		if(!g_thread_supported()) g_thread_init(0);
		gdk_threads_init();
	}
#endif

	if((app_init_params&aINIT_SOCKETS)) {
/*#ifdef USE_SDL
		if(SDLNet_Init()==-1) return aINIT_SOCKETS;
#endif*/
#ifdef USE_WIN32
		WSADATA wsadata;
		if(WSAStartup(MAKEWORD(2,0),&wsadata)!=0) {
debug_output("Socket Initialization Error.\n");
			WSACleanup();
			return aINIT_SOCKETS;
		}
#endif
	}
#ifdef USE_WIN32
	if((app_init_params&aINIT_GUI)) {
		INITCOMMONCONTROLSEX commctrl = { sizeof(INITCOMMONCONTROLSEX),0xffffffff };
		OleInitialize(0);
		InitCommonControlsEx(&commctrl);
	}
#endif
	return 0;
}


uint32_t aApplication::close() {
	if((app_init_params&aINIT_SOCKETS)) {
#ifdef USE_SDL
		SDLNet_Quit();
#endif
#ifdef USE_WIN32
		WSACleanup();
#endif
	}
	if((app_init_params&aINIT_GUI)) {
#ifdef USE_WIN32
		OleUninitialize();
#endif
	}
	return 0;
}

int aApplication::main() {
#ifdef USE_GTK
	create();
	if((app_init_params&aINIT_THREADS)) {
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
			int mod;
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

void aApplication::quit() {
debug_output("aApplication::exit()\n");
#ifdef USE_GTK
	gtk_main_quit();
#endif
#ifdef USE_WIN32
	PostQuitMessage(0);
#endif
}


void aApplication::create() {
}

void aApplication::destroy() {
	quit();
}
/*
void aApplication::add(aComponent c,aWidget *w) {
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

/*
int aApplication::install(const char *host,const char *path,aVector &files,install_function func,void *obj) {
debug_output("aApplication::install(host=\"%s\",path=\"%s\")\n",host,path);
	int i,n,f,pl = path? strlen(path) : 0,st = 0;
	const char *home = getHomeDir();
	const char *fonts = getFontsDir();
	const char *file,*p,*e,*dest;
	char str[256];
	aHttp http(*this);
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
			if(!aFile::exists(str)) aFile::mkdir(str);
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

#if defined USE_GTK
void aApplication::updateFontCache() {
	const char *p;
	gchar *so,*se;
	g_spawn_command_line_sync(p="fc-cache -f -v",&so,&se,NULL,NULL);
debug_output("spawn: %s\nstdout:\n%sstderr:\n\n%s",p,so,se);
}
#endif
*/

void aApplication::setProjectName(const char *prj) {
	if(!prj || !*prj) return;
	if(app_project) free(app_project);
	app_project = strdup(prj);
	if(!app_name || app_name==app_project) app_name = app_project;
	if(!app_user_agent || app_user_agent==app_project) app_user_agent = app_project;
}

void aApplication::setApplicationName(const char *nm) {
	if(!nm || !*nm) return;
	if(app_name && app_name!=app_project) free(app_name);
	app_name = strdup(nm);
}

void aApplication::setUserAgent(const char *ua) {
	if(!ua || !*ua) return;
	if(app_user_agent && app_user_agent!=app_project) free(app_user_agent);
	app_user_agent = strdup(ua);
}

void aApplication::setDirectories(const char *dir) {
	if(app_locale_dir) free(app_locale_dir);
	app_locale_dir = strdup(dir);
}



