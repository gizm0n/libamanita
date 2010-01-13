

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

#include <libamanita/aApplication.h>
#include <libamanita/aFile.h>
#include <libamanita/net/aHttp.h>



//#define _QUOTE_MACRO(x) #x
//#define QUOTE_MACRO(x) _QUOTE_MACRO(x)


const char *aApplication::property_key[] = {
	"app_exe","app_dir_home","app_dir_data","app_dir_fonts","app_dir_lang","lang"
};


aApplication::aApplication(const char *nm) : app_lang_data(),app_properties() {
	FILE *fp;
	char str[257],home[257],data[257];
	int n;

	app_name = strdup(nm);
	app_agent = app_name;
	app_local_id = 0;
	app_local_time = 0;

	getExecutable(str,256);
	setProperty(property_key[APP_EXE],str);
fprintf(stderr,"app_exe=%s\n",getExecutable());
fflush(stderr);

	getHomeDir(str,256);
#if defined __linux__
	sprintf(home,"%s/.%s/",str,app_name);
#elif defined WIN32
	sprintf(home,"%s\\",str);
#endif
	setProperty(property_key[APP_DIR_HOME],home);
	if(!aFile::exists(home)) aFile::mkdir(home);
fprintf(stderr,"app_dir_home=%s\n",getHomeDir());
fflush(stderr);

#if defined __linux__
	n = 0;
	snprintf(data,256,"/usr/share/%s/",app_name);
	if(!aFile::exists(data)) {
		n = 1;
		snprintf(data,256,"/usr/local/share/%s/",app_name);
		if(!aFile::exists(data)) {
			n = 2;
			snprintf(data,256,"%s/.local/share/%s/",home,app_name);
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
fprintf(stderr,"app_dir_data=%s\n",getDataDir());
fflush(stderr);

	char fonts[257];
	getFontsDir(str,256);
	sprintf(fonts,"%s" FILE_DIRSEP,str);
	setProperty(property_key[APP_DIR_FONTS],fonts);
	if(!aFile::exists(fonts)) aFile::mkdir(fonts);
fprintf(stderr,"app_dir_fonts=%s\n",getFontsDir());
fflush(stderr);

	sprintf(str,"%slang" FILE_DIRSEP,data);
	setProperty(property_key[APP_DIR_LANG],str);
	if(n==3 && !aFile::exists(str)) aFile::mkdir(str);
fprintf(stderr,"app_dir_lang=%s\n",getLanguageDir());
fflush(stderr);

	sprintf(str,"%s%s.log",home,app_name);
	app_last_access = aFile::accessed(str);
fprintf(stderr,"logfile=%s\nlast_access=%" PRIu64 "\n\n",str,(uint64_t)app_last_access);
fflush(stderr);
	app_out = fopen(str,"w");
	fp = freopen(str,"a",app_out);
	fp = freopen(str,"a",stdout);
	fp = freopen(str,"a",stderr);
printf("aApplication::aApplication()");

}

aApplication::~aApplication() {
printf("aApplication::~aApplication()");
	if(app_agent && app_agent!=app_name) { free(app_agent);app_agent = 0; }
	if(app_name) { free(app_name);app_name = 0; }
printf("Completely finalized, could not have gone better! You the Man!");
	fclose(app_out);
}

int aApplication::install(const char *host,const char *path,const char **files) {
	int i,n,f,pl = path? strlen(path) : 0;
	const char *home = getHomeDir();
	const char *fonts = getFontsDir();
	const char *file,*p,*e,*dest;
	char str[256];
	aHttp http;
	http.setUserAgent(app_agent);
	FILE *fp;
	size_t sz;
	for(i=0,n=0,f=0; files[i]; i++) {
		p = files[i];
		if(pl && strncmp(p,path,pl)==0) p += pl;
		else if(*p=='/') p++;
		e = &p[strlen(p)-1];
		if(strncmp(e-3,".ttf",4)==0) dest = fonts,f++;
		else dest = home;
		sprintf(str,"%s%s",dest,p);
fprintf(stderr,"str=%s\n",str);
fflush(stderr);
		if(*e=='/') {
			if(!aFile::exists(str)) aFile::mkdir(str);
			continue;
		}
fprintf(stderr,"http.get(%s,%s)\n",host,files[i]);
fflush(stderr);
		file = http.get(host,files[i]);
fprintf(stderr,"http.get()\n");
fflush(stderr);
		sz = http.getFileSize();
fprintf(stderr,"sz=%zu\n",sz);
fflush(stderr);
		fp = fopen(str,"wb");
		if(sz) sz = fwrite(file,sz,1,fp);
		fclose(fp);
		n++;
	}
#if defined __linux__
	if(f>0) {
		gchar *so,*se;
		g_spawn_command_line_sync(p="fc-cache -f -v",&so,&se,NULL,NULL);
fprintf(stderr,"spawn: %s\nstdout:\n%sstderr:\n\n%s",p,so,se);
fflush(stderr);
	}
#endif
	return n;
}

void aApplication::loadProperties() {
	const char *str = getHomeDir();
	if(!str) return;
	char fn[257];
	sprintf(fn,"%s%s.cfg",str,app_name);
	app_properties.load(fn);
	if(!(str=getProperty(property_key[APP_LANG]))) str = "en";
	setLanguage(str);
}

void aApplication::saveProperties() {
	const char *str = getHomeDir();
	if(!str) return;
	char fn[257];
	sprintf(fn,"%s%s.cfg",str,app_name);
	app_properties.save(fn);
}

void aApplication::setLanguage(const char *l) {
	char dir[257],fn[257];
	const char *langdir = getLanguageDir();
	const char *lang = getProperty(property_key[APP_LANG]);
	strncpy(app_lang,l,3);
	if(!lang || strcmp(lang,app_lang)) setProperty(property_key[APP_LANG],app_lang);
	sprintf(dir,"%s%s/",langdir,app_lang);
	setProperty(property_key[APP_DIR_LANG],dir);
	sprintf(fn,"%slang.txt",dir);
	app_lang_data.removeAll();
	app_lang_data.load(fn);
printf("aApplication::loadLanguage(%s)",fn);
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




