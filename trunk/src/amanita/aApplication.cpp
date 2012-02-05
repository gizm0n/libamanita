

#include "_config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <locale.h>

#if defined(__linux__)

#elif defined(WIN32)
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#ifndef WINVER
#define WINVER 0x0501
#endif
#define _WIN32_IE	0x0501
#include <windows.h>
//#include <ctype.h>
#include <commctrl.h>
//#include <richedit.h>
#endif

#include <amanita/aApplication.h>
//#include <amanita/aFile.h>
//#include <amanita/aVector.h>
//#include <amanita/net/aHttp.h>


#if defined(__linux__)
static gboolean delete_event_callback(GtkWidget *widget,GtkWidget *event,gpointer data) {
	aApplication *app = (aApplication *)data;
	return !app->destroy();
}


#elif defined(WIN32)
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

static char *w2char(LPCWSTR b) {
	if(b) {
		int l = wcslen(b);
		char *c = (char *)malloc(l+1);
		if(WideCharToMultiByte(CP_ACP,0,b,-1,(char*)c,l+1,0,0)) return c;
		else free(c);
	}
	return 0;
}

LRESULT CALLBACK AmanitaMainWndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	aApplication *app = 0;
	if(msg==WM_CREATE) {
debug_output("AmanitaMainWndProc(WM_CREATE)\n");
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lparam;
		app = (aApplication *)cs->lpCreateParams;
		app->window = (aHandle)hwnd;
		SetProp(hwnd,"app",(HANDLE)app);
		if(app->create()) return 0;
	} else {
		app = (aApplication *)GetProp(hwnd,"app");
		if(app) {
			switch(msg) {
				case WM_DESTROY:
					RemoveProp(hwnd,"app");
					if(app->destroy()) return 0;
					break;
				case WM_SIZE:
					if(wparam!=SIZE_MINIMIZED)
						MoveWindow((HWND)app->child->getHandle(),0,0,LOWORD(lparam),HIWORD(lparam),true);
					return 0;
				default:if(app->handleEvent(msg,wparam,lparam)) return 0;
			}
		}
	}
	return DefWindowProc(hwnd,msg,wparam,lparam);
}

int WINAPI WinMain(HINSTANCE hi,HINSTANCE hp,LPSTR cmd,int show) {
	int i,argc;
	LPWSTR *args = CommandLineToArgvW(GetCommandLineW(),&argc);
	char *argv[argc];
	for(i=0; i<argc; i++) {
		argv[i] = w2char(args[i]);
		fprintf(stderr,"Cmd[%d]: %s\n",i,argv[i]);
	}
	LocalFree(args);
	i = main(argc,argv);
	while(--argc>=0) free(argv[argc]);
	return i;
}
#endif

//#define _QUOTE_MACRO(x) #x
//#define QUOTE_MACRO(x) _QUOTE_MACRO(x)

/*
const char *aApplication::property_key[] = {
	"_app_exe","_app_dir_home","_app_dir_data","_app_dir_fonts","_app_dir_lang",0,"lang",0,
};
*/

aApplication::aApplication() /*: app_lang_data(),app_properties()*/ {
//	FILE *fp;
//	char str[257],home[257],data[257];
//	int n;

	app_project = 0;
	app_name = 0;
	app_user_agent = 0;
	app_local_id = 0;
	app_local_time = 0;

	app_out = 0;

/*
	getExecutable(str,256);
	setProperty(property_key[APP_EXE],str);
debug_output("app_exe=%s\n",getExecutable());

	getHomeDir(str,256);
#if defined __linux__
	sprintf(home,"%s/.%s/",str,app_project);
#elif defined WIN32
	sprintf(home,"%s\\",str);
#endif
	setProperty(property_key[APP_DIR_HOME],home);
	if(!aFile::exists(home)) aFile::mkdir(home);
debug_output("app_dir_home=%s\n",getHomeDir());

#if defined __linux__
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
#elif defined WIN32
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

aApplication::aApplication(const char *prj,const char *nm) /*: app_lang_data(),app_properties()*/ {
	app_project = 0;
	app_name = 0;
	app_user_agent = 0;
	setApplicationName(nm);
	setProjectName(prj);
	app_locale_dir = 0;
	app_local_id = 0;
	app_local_time = 0;

	app_out = 0;
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

debug_output("Completely finalized, could not have gone better! You the Man!\n");
	if(app_out) fclose(app_out);
	app_out = 0;
}



#if defined(__linux__)
#elif defined(WIN32)
#ifdef UNICODE 
extern const WCHAR *amanita_browser_class;
#else
extern const char *amanita_browser_class;
#endif
extern LRESULT CALLBACK AmanitaBrowserProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);
#endif


uint32_t aApplication::init(int argc,char *argv[],uint32_t params) {
//	loadProperties();
#if defined(__linux__)
#elif defined(WIN32)
	hinst = GetModuleHandle(0);
#endif
	app_init_params = params;
	if((app_init_params&AMANITA_INIT_GETTEXT)) {
debug_output("aApplication::init(AMANITA_INIT_GETTEXT,project=%s,localedir=%s)",app_project,app_locale_dir);
		setlocale(LC_ALL,"");
		bindtextdomain(app_project,app_locale_dir? app_locale_dir : ".");
		textdomain(app_project);
	}
	if((app_init_params&AMANITA_INIT_SOCKETS)) {
#if defined(__linux__)
#elif defined(WIN32)
#endif
	}
	if((app_init_params&AMANITA_INIT_GUI)) {
#if defined(__linux__)
#elif defined(WIN32)
		INITCOMMONCONTROLSEX commctrl = { sizeof(INITCOMMONCONTROLSEX),0xffffffff };
		OleInitialize(0);
		InitCommonControlsEx(&commctrl);
#endif
		if((app_init_params&AMANITA_INIT_BROWSER)) {
#if defined(__linux__)
#elif defined(WIN32)
			WNDCLASS wc;
			if(!GetClassInfo(hinst,amanita_browser_class,&wc)) {
				memset(&wc,0,sizeof(wc));
				wc.style         = CS_DBLCLKS|CS_GLOBALCLASS|CS_NOCLOSE;
				wc.lpfnWndProc   = AmanitaBrowserProc;
				wc.hInstance     = hinst;
				wc.hCursor       = LoadCursor(0,IDC_ARROW);
				wc.lpszClassName = amanita_browser_class;
				if(!RegisterClass(&wc)) return AMANITA_INIT_BROWSER;
			}
#endif
		}
	}

#if defined(__linux__)
	gtk_init(&argc,&argv);

	gtk_rc_parse_string (
		"style \"tab-close-button-style\"\n"
		"{\n"
		"  GtkWidget::focus-padding = 0\n"
		"  GtkWidget::focus-line-width = 0\n"
		"  xthickness = 0\n"
		"  ythickness = 0\n"
		"}\n"
		"widget \"*.tab-close-button\" style \"tab-close-button-style\"");

	if((app_init_params&AMANITA_INIT_THREADS)) {
		if(!g_thread_supported()) g_thread_init(0);
		gdk_threads_init();
	}

#elif defined(WIN32)
	WNDCLASSEX wndclass = {
		sizeof(wndclass),
		CS_HREDRAW|CS_VREDRAW,
		AmanitaMainWndProc,
		0,0,hinst,
		LoadIcon(NULL,IDI_APPLICATION),
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		0,"amanita_main_class",LoadIcon(NULL,IDI_APPLICATION)
	};
	if(!RegisterClassEx(&wndclass))
		MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
#endif
	return 0;
}

int aApplication::open(int w,int h,int minw,int minh,bool res) {
#if defined(__linux__)
	GtkWidget *wnd;
	wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wnd),app_name);
	gtk_window_set_default_size(GTK_WINDOW(wnd),w,h);
	gtk_widget_set_size_request(wnd,minw>0? minw : w,minh>0? minh : h);
	gtk_window_set_resizable(GTK_WINDOW(wnd),res);
	gtk_window_set_position(GTK_WINDOW(wnd),GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(wnd),"delete_event",G_CALLBACK(delete_event_callback),this);
	window = (aHandle)wnd;
	create();
	if((app_init_params&AMANITA_INIT_THREADS)) {
		gdk_threads_enter();
		gtk_main();
		gdk_threads_leave();
	} else {
		gtk_main();
	}
	return 0;
#elif defined(WIN32)
	HWND hwnd;
	MSG msg;
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"amanita_main_class","aBrowser Example",
		WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,900,600,0,0,hinst,this);
	if(!hwnd) {
		MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	while(GetMessage(&msg,NULL,0,0)) {
		if(msg.message==WM_QUIT) {
			DestroyWindow(hwnd);
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
#endif
}

uint32_t aApplication::close() {
	if((app_init_params&AMANITA_INIT_SOCKETS)) {
#if defined(__linux__)
#elif defined(WIN32)
#endif
	}
	if((app_init_params&AMANITA_INIT_GUI)) {
		if((app_init_params&AMANITA_INIT_BROWSER)) {
#if defined(__linux__)
#elif defined(WIN32)
			WNDCLASS wc;
			if(GetClassInfo(GetModuleHandle(0),amanita_browser_class,&wc) &&
				!UnregisterClass(amanita_browser_class,wc.hInstance))
					return AMANITA_INIT_BROWSER;
#endif
		}
#if defined(__linux__)
#elif defined(WIN32)
		OleUninitialize();
#endif
	}
//	saveProperties();
	return 0;
}

void aApplication::exit() {
debug_output("aApplication::exit()\n");
#if defined(__linux__)
	gtk_main_quit();
#elif defined(WIN32)
	PostQuitMessage(0);
#endif
}

int aApplication::create() {
	return 0;
}

int aApplication::destroy() {
	exit();
	return 1;
}

int aApplication::handleEvent(uint32_t msg,intptr_t p1,intptr_t p2) {
	return 0;
}

void aApplication::add(aHandle c,aWidget *w) {
	aHandle handle;
	handle = w->create(window,0);
#if defined(__linux__)
	gtk_container_add(GTK_CONTAINER(window),(GtkWidget *)handle);
#elif defined(WIN32)
	w->hinst = hinst;
#endif
	child = w;
}

void aApplication::show() {
#if defined(__linux__)
	gtk_widget_show_all((GtkWidget *)window);
#elif defined(WIN32)
	ShowWindow((HWND)window,SW_SHOW);
	UpdateWindow((HWND)window);
#endif
}

void aApplication::hide() {
}


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

#if defined __linux__
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

/*
static const char *properties_file = "%s%s.cfg";
static const char *language_file = "%s%s.txt";

void aApplication::loadProperties() {
	const char *home = getHomeDir();
	if(home) {
		char fn[257];
		sprintf(fn,properties_file,home,app_project);
		if(!aFile::exists(fn)) {
			const char *data = getDataDir();
			sprintf(fn,properties_file,data,app_project);
		}
		app_properties.load(fn);
	}
	const char *lang = getProperty(property_key[APP_LANG]);
	if(!lang) lang = "en";
	setLanguage(lang);
}

void aApplication::saveProperties() {
	const char *home = getHomeDir();
	if(!home) return;
	int i;
	char fn[257];
	sprintf(fn,properties_file,home,app_project);
	for(i=0; i<APP_TEMP_PROPERTIES; i++)
		if(property_key[i]) app_properties.remove(property_key[i]);
	app_properties.save(fn);
}

void aApplication::setPropertyf(const char *key,const char *value, ...) {
	if(!key || !*key || !value || !*value) return;
	char v[1025];
	va_list args;
   va_start(args,value);
	vsnprintf(v,1024,value,args);
   va_end(args);
	app_properties.put(key,v);
}

void aApplication::setLanguage(const char *l) {
	char dir[257],fn[257];
	const char *data = getDataDir();
	const char *lang = getProperty(property_key[APP_LANG]);
	if(l) strncpy(app_lang,l,3);
	if(!lang || strcmp(lang,app_lang)) setProperty(property_key[APP_LANG],app_lang);
	sprintf(dir,"%slang/%s/",data,app_lang);
	setProperty(property_key[APP_DIR_LANG],dir);
	sprintf(fn,language_file,dir,app_project);
	app_lang_data.removeAll();
	app_lang_data.load(fn);
debug_output("aApplication::loadLanguage(%s)",fn);
}

const char *aApplication::getf(const char *format, ...) {
	if(!format) return aString::blank;
	char key[33];
	va_list args;
   va_start(args,format);
	vsnprintf(key,32,format,args);
   va_end(args);
	char *str = app_lang_data.getString(key);
	return str;
}

const char *aApplication::get(const char *key) {
	if(!key) return aString::blank;
	char *str = app_lang_data.getString(key);
	return str;
}
*/



