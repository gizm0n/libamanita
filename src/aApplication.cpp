

#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#if defined __linux__
#include <glib.h>
#include <glib/gstdio.h>
#endif

#include <libamanita/aApplication.h>
#include <libamanita/aFile.h>
#include <libamanita/net/aHttp.h>



//#define _QUOTE_MACRO(x) #x
//#define QUOTE_MACRO(x) _QUOTE_MACRO(x)


aApplication::aApplication(const char *nm) {
	FILE *fp;
	char fn[256];
	const char *p;

	name = strdup(nm);
	agent = name;
	local_id = 0;
	local_time = 0;

	p = aFile::getHomeDir();
	sprintf(dir.home,"%s/.%s/",p,name);
	sprintf(dir.fonts,"%s/.fonts/",p);
	sprintf(dir.lang,"%slang/",dir.home);

fprintf(stderr,"dir.home=%s\ndir.fonts=%s\ndir.lang=%s\nfile_test(%s)\n",dir.home,dir.fonts,dir.lang,dir.home);
fflush(stderr);
	if(!aFile::exists(dir.home)) aFile::mkdir(dir.home);
	if(!aFile::exists(dir.fonts)) aFile::mkdir(dir.fonts);

	sprintf(fn,"%s%s.log",dir.home,name);
	last_access = aFile::accessed(fn);
fprintf(stderr,"logfile=%s\nlast_access=%" PRIu64 "\n\n",fn,(uint64_t)last_access);
fflush(stderr);
	out = fopen(fn,"w");
	fp = freopen(fn,"a",out);
	fp = freopen(fn,"a",stdout);
	fp = freopen(fn,"a",stderr);
printf("aApplication::aApplication()");

}

aApplication::~aApplication() {
printf("aApplication::~aApplication()");
	if(agent && agent!=name) { free(agent);agent = 0; }
	if(name) { free(name);name = 0; }
printf("Completely finalized, could not have gone better! You the Man!");
	fclose(out);
}

int aApplication::install(const char *host,const char *path,const char **files) {
	int i,n,fonts,pl = path? strlen(path) : 0;
	const char *file,*p,*e,*dest;
	char str[256];
	aHttp http;
	http.setUserAgent(agent);
	FILE *fp;
	size_t sz;
	for(i=0,n=0,fonts=0; files[i]; i++) {
		p = files[i];
		if(pl && strncmp(p,path,pl)==0) p += pl;
		else if(*p=='/') p++;
		e = &p[strlen(p)-1];
		if(strncmp(e-3,".ttf",4)==0) dest = dir.fonts,fonts++;
		else dest = dir.home;
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
	if(fonts>0) {
		gchar *so,*se;
		g_spawn_command_line_sync(p="fc-cache -f -v",&so,&se,NULL,NULL);
fprintf(stderr,"spawn: %s\nstdout:\n%sstderr:\n\n%s",p,so,se);
fflush(stderr);
	}
#endif
	return n;
}

void aApplication::loadSettings() {
	const char *str;
	char fn[128];
	sprintf(fn,"%ssettings.txt",dir.home);
	settings.load(fn);
	if(!(str=settings.getString("lang"))) str = "en";
	setLanguage(str);
}

void aApplication::saveSettings() {
	char fn[128];
	sprintf(fn,"%ssettings.txt",dir.home);
	settings.save(fn);
}

void aApplication::setLanguage(const char *l) {
printf("aApplication::loadLanguage(lang=%s)",l);
	char fn[128],*p = settings.getString("lang");
	strncpy(lang,l,3);
	if(!p || strcmp(p,lang)) settings.put("lang",lang);
	sprintf(dir.lang,"%slang/%s/",dir.home,lang);
	sprintf(fn,"%slang.txt",dir.lang);
	_lang.removeAll();
	_lang.load(fn);
}

const char *aApplication::getf(const char *format, ...) {
	if(!format) return aString::blank;
	char key[33];
	va_list args;
   va_start(args,format);
	vsnprintf(key,32,format,args);
   va_end(args);
	char *str = _lang.getString(key);
	return str;
}

const char *aApplication::get(const char *key) {
	if(!key) return aString::blank;
	char *str = _lang.getString(key);
	return str;
}


void aApplication::printf(const char *format, ...) {
	char buf[275];
	sprintf(buf,"[%" PRIu32 "]%" PRIu64 ": ",local_id,(uint64_t)local_time);
	va_list args;
   va_start(args,format);
	vsnprintf(&buf[strlen(buf)],256,format,args);
   va_end(args);
	strcat(buf,"\n");
	fputs(buf,out);
	fflush(out);
}






