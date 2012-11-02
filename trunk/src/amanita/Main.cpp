

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
#endif
#include <amanita/Main.h>
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
//debug_output("hMainInstance: %x\n",hMainInstance);
	i = main(argc,argv);
	while(--argc>=0) free(argv[argc]);
	return i;
}
#endif


namespace a {

//#define _QUOTE_MACRO(x) #x
//#define QUOTE_MACRO(x) _QUOTE_MACRO(x)

Main::Main(uint32_t p,const char *prj,const char *nm) : app_thread() {
	FILE *fp;
	char str[257],home[257],data[257];

	app_params = p;
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

	setProjectName(prj);
	setApplicationName(nm);

	if((app_params&APP_DIRECTORIES)) {
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
				if(!exists(data)) sprintf(data,"%sdata" FILE_DIRSEP,home);
			}
		}
#elif defined USE_WIN32
		sprintf(data,"%sdata" FILE_DIRSEP,home);
#endif

		fontsdir(str,256);
		strcat(str,FILE_DIRSEP);
		if(!exists(str)) mkdir(str);
//debug_output("app_dir_fonts=%s\n",str);
		app_fonts_dir = strdup(str);
	} else {
		currentdir(str,256);
		sprintf(home,"%s" FILE_DIRSEP,str);
		strcpy(data,home);
	}

	app_home_dir = strdup(home);
//debug_output("app_home_dir: %s\n",home);
	app_data_dir = strdup(data);
//debug_output("app_data_dir: %s\n",data);
	
	if((app_params&APP_LOG)) {
		sprintf(str,"%s%s.log",app_home_dir,app_project);
		app_last_access = modified(str);
//debug_output("logfile: %s\nlast_access %" PRIu64 "\n\n",str,(uint64_t)app_last_access);
		if(!(app_out=fopen(str,"wb")) ||
			!(fp=freopen(str,"ab",app_out)) ||
			!(fp=freopen(str,"ab",stdout)) ||
			!(fp=freopen(str,"ab",stderr))) perror(str);
//debug_output("Main::Main()");
	} else {
		executable(str,256);
		app_last_access = accessed(str);
	}
}

Main::~Main() {
//debug_output("Main::~Main()\n");
	stop();
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
//debug_output("Completely finalized, could not have gone better! You the Man!\n");
	printf("\n");
}


uint32_t Main::open(int argc,char *argv[]) {
	if((app_params&APP_GETTEXT)) {
//debug_output("Main::init(AMANITA_APP_GETTEXT,project=%s,localedir=%s)",app_project,app_locale_dir);
		setlocale(LC_ALL,"");
		bindtextdomain(app_project,app_locale_dir? app_locale_dir : ".");
		textdomain(app_project);
	}

	if(app_params&APP_SOCKETS) {
#ifdef USE_WIN32
		WSADATA wsadata;
		if(WSAStartup(MAKEWORD(2,0),&wsadata)!=0) {
//debug_output("Socket Initialization Error.\n");
			WSACleanup();
			return APP_SOCKETS;
		}
#endif
	}
	return 0;
}


uint32_t Main::close() {
	if(app_params&APP_SOCKETS) {
#ifdef USE_WIN32
		WSACleanup();
#endif
	}
	return 0;
}

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

int Main::install(const char *host,const char *files,install_function func,void *obj) {
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
int Main::install(const char *host,const char *path,Vector &files,install_function func,void *obj) {
debug_output("Main::install(host=\"%s\",path=\"%s\")\n",host,path);
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

static void Main_run(void *data) {
	((Main *)data)->run();
}

void Main::start(thread_function f) {
	app_thread.start(f? f : Main_run,this);
}

void Main::stop() {
	app_thread.stop();
}

void Main::kill() {
	app_thread.kill();
}

void Main::run() {
}


void Main::setProjectName(const char *prj) {
	if(!prj || !*prj) return;
	if(app_project) free(app_project);
	app_project = strdup(prj);
	if(!app_name || app_name==app_project) app_name = app_project;
	if(!app_user_agent || app_user_agent==app_project) app_user_agent = app_project;
}

void Main::setApplicationName(const char *nm) {
	if(!nm || !*nm) return;
	if(app_name && app_name!=app_project) free(app_name);
	app_name = strdup(nm);
}

void Main::setUserAgent(const char *ua) {
	if(!ua || !*ua) return;
	if(app_user_agent && app_user_agent!=app_project) free(app_user_agent);
	app_user_agent = strdup(ua);
}

void Main::setHomeDir(const char *dir) {
	if(app_home_dir) free(app_home_dir);
	app_home_dir = strdup(dir);
}

void Main::setDataDir(const char *dir) {
	if(app_data_dir) free(app_data_dir);
	app_data_dir = strdup(dir);
}

void Main::setLocaleDir(const char *dir) {
	if(app_locale_dir) free(app_locale_dir);
	app_locale_dir = strdup(dir);
}

void Main::setFontsDir(const char *dir) {
	if(app_locale_dir) free(app_fonts_dir);
	app_fonts_dir = strdup(dir);
}


}; /* namespace a */


