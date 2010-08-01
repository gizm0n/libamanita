

#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#if defined __linux__
#include <glib.h>
#include <glib/gstdio.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined WIN32
#include <windows.h>
#endif

#include <amanita/aApplication.h>
#include <amanita/aFile.h>
#include <amanita/aVector.h>
#include <amanita/net/aHttp.h>



//#define _QUOTE_MACRO(x) #x
//#define QUOTE_MACRO(x) _QUOTE_MACRO(x)


const char *aApplication::property_key[] = {
	"_app_exe","_app_dir_home","_app_dir_data","_app_dir_fonts","_app_dir_lang",0,"lang",0,
};


aApplication::aApplication(const char *prj,const char *nm) : app_lang_data(),app_properties() {
	FILE *fp;
	char str[257],home[257],data[257];
	int n;

	app_project = strdup(prj);
	app_name = nm? strdup(nm) : app_project;
	app_user_agent = app_project;
	app_local_id = 0;
	app_local_time = 0;

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

}


aApplication::~aApplication() {
debug_output("aApplication::~aApplication()");
	if(app_user_agent && app_user_agent!=app_project) free(app_user_agent);
	app_user_agent = 0;
	if(app_name && app_name!=app_project) free(app_name);
	app_name = 0;
	if(app_project) { free(app_project);app_project = 0; }
debug_output("Completely finalized, could not have gone better! You the Man!");
	fclose(app_out);
}


void aApplication::init() {
	loadProperties();
}

void aApplication::exit() {
	saveProperties();
}


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
debug_output("sz=%zu\n",sz);
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

void aApplication::setUserAgent(const char *a) {
	if(app_user_agent && app_user_agent!=app_project) free(app_user_agent);
	app_user_agent = strdup(a);
}

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


void aApplication::printf(const char *format, ...) {
	char buf[275];
	sprintf(buf,"[%" PRIu32 "]%" PRIu64 ": ",app_local_id,(uint64_t)app_local_time);
	va_list args;
	va_start(args,format);
	vsnprintf(&buf[strlen(buf)],256,format,args);
	va_end(args);
	strcat(buf,"\n");
	fputs(buf,app_out);
	fflush(app_out);
}



void aApplication::getExecutable(char *dir,int l) {
#if defined __linux__
	char link[64];
	pid_t pid;
	int ret;
	pid = getpid();
	snprintf(link,sizeof(link),"/proc/%i/exe",pid);
	ret = readlink(link,dir,l);
	dir[ret] = '\0';
#elif defined WIN32
	GetModuleFileName(NULL,dir,l);
#else
	strcpy(dir,".");
#endif
}

void aApplication::getHomeDir(char *dir,int l) {
#if defined __linux__
	snprintf(dir,l,"%s",g_get_home_dir());
#elif defined WIN32
	GetModuleFileName(NULL,dir,l);
	char *p = strrchr(dir,'\\');
	if(!p) p = strrchr(dir,'/');
	if(p) *p = '\0';
#else
	strcpy(dir,".");
#endif
}

void aApplication::getApplicationDir(char *dir,int l) {
#if defined __linux__
	getExecutable(dir,l);
	char *p = strrchr(dir,'/');
	if(p) *p = '\0';
#elif defined WIN32
	GetModuleFileName(NULL,dir,l);
	char *p = strrchr(dir,'\\');
	if(!p) p = strrchr(dir,'/');
	if(p) *p = '\0';
#else
	strcpy(dir,".");
#endif
}

void aApplication::getFontsDir(char *dir,int l) {
#if defined __linux__
	snprintf(dir,l,"%s/.fonts",g_get_home_dir());
#elif defined WIN32
	GetWindowsDirectory((LPTSTR)dir,(UINT)l);
	strcat(dir,"\\fonts");
#else
	strcpy(dir,".");
#endif
}

void aApplication::getSystemDir(char *dir,int l) {
#if defined __linux__
	snprintf(dir,l,"%s",g_get_home_dir());
#elif defined WIN32
	GetSystemDirectory((LPTSTR)dir,(UINT)l);
#else
	strcpy(dir,".");
#endif
}

void aApplication::getCurrentDir(char *dir,int l) {
#if defined __linux__
	char *p;
	p = getcwd(dir,l);
#elif defined WIN32
	GetCurrentDirectory((DWORD)l,(LPTSTR)dir);
#else
	strcpy(dir,".");
#endif
}




