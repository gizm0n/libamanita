
#include "_config.h"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#if defined USE_GTK
#include <glib.h>
#include <glib/gstdio.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#ifdef USE_WIN32
#include <windows.h>
#endif

#include <amanita/aFile.h>


const char *aFile::dirsep = aFILE_DIRSEP;


aFile &aFile::open(const char *a,const char *fn, ...) {
	char buf[257];
	va_list args;
   va_start(args,fn);
	vsnprintf(buf,256,fn,args);
   va_end(args);
	if(name) free(name);
	if(fp) fclose(fp);
	if((fp=fopen(buf,a))) {
		dir = strdup(buf);
		char *p1 = strrchr(dir,'/');
		char *p2 = strrchr(dir,'\\');
		name = p1>p2? p1 : p2;
		if(name) *name++ = '\0';
	} else perror(buf);
	return *this;
}

aFile &aFile::close() {
	if(dir) { free(dir);dir = 0,name = 0; }
	if(fp) { fclose(fp);fp = 0; }
	return *this;
}

aFile &aFile::read(char **data,size_t &len) {
	*data = 0;
	len = 0;
	if(fp) {
		len = size();
		if(len>0) {
			*data = (char *)malloc(len);
			fread(*data,len,1,fp);
		}
	}
	return *this;
}

aFile &aFile::write(const char *data,size_t len) {
	if(fp) fwrite(data,len,1,fp);
	return *this;
}

size_t aFile::size() {
	if(!fp) return 0;
	long n1 = ftell(fp);
	fseek(fp,0,SEEK_END);
	long n2 = ftell(fp);
	fseek(fp,n1,SEEK_SET);
	return n2>0? (size_t)n2 : 0;
}



bool aFile::exists(const char *fn) {
#ifdef USE_GLIB
	return g_file_test(fn,G_FILE_TEST_EXISTS);
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t *wfn = char2w(fn);
	bool b = GetFileAttributes(wfn)!=INVALID_FILE_ATTRIBUTES;
	free(wfn);
	return b;
#else
	return GetFileAttributes(fn)!=INVALID_FILE_ATTRIBUTES;
#endif
//#else
//	FILE *fp = fopen(fn,"r");
//	if(!fp) return false;
//	fclose(fp);
//	return true;
#endif
}

time_t aFile::accessed(const char *fn) {
	struct stat attrib;
	if(!exists(fn) || stat(fn,&attrib)==-1) return 0;
	return attrib.st_atime;
}

time_t aFile::modified(const char *fn) {
	struct stat attrib;
	if(!exists(fn) || stat(fn,&attrib)==-1) return 0;
	return attrib.st_mtime;
}

long aFile::copy(const char *s,const char *d) {
	long n = -1;
	FILE *fps = fopen(s,"rb");
	if(fps) {
		FILE *fpd = fopen(d,"wb");
		if(fpd) {
			n = copy(fps,fpd);
			fclose(fps);
		} else perror(d);
		fclose(fpd);
	} else perror(s);
	return n;
}

long aFile::copy(FILE *s,FILE *d) {
	if(!s || !d) return -1;
	fseek(s,0,SEEK_END);
	long n = ftell(s);
	fseek(s,0,SEEK_SET);
	char buf[n+1];
	fread(buf,n,1,s);
	fwrite(buf,n,1,d);
	return n;
}


bool aFile::remove(const char *fn) {
#ifdef USE_GLIB
	return ::g_remove(fn)==0;
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t *wfn = char2w(fn);
	int n = GetFileAttributes(wfn);
	free(wfn);
#else
	int n = GetFileAttributes(fn);
#endif
	if(n&FILE_ATTRIBUTE_READONLY) return false;
	if(n&FILE_ATTRIBUTE_DIRECTORY) return RemoveDirectory((LPCTSTR)fn)!=0;
	else return DeleteFile((LPCTSTR)fn)==TRUE;
//#else
//	return ::remove(fn)==0;
#endif
}


bool aFile::mkdir(const char *dn,int p) {
#ifdef USE_GLIB
	return ::g_mkdir(dn,p)==0;
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t *wdn = char2w(dn);
	bool b = CreateDirectory(wdn,NULL)==TRUE;
	free(wdn);
	return b;
#else
	return CreateDirectory(dn,NULL)==TRUE;
#endif
//#else
//	return ::mkdir(dn,p)==0;
#endif
}


void aFile::getExecutable(char *dir,int l) {
#ifdef USE_GLIB
	char link[64];
	pid_t pid;
	int ret;
	pid = getpid();
	snprintf(link,sizeof(link),"/proc/%i/exe",pid);
	ret = readlink(link,dir,l);
	dir[ret] = '\0';
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t wdir[l];
	GetModuleFileName(NULL,wdir,l);
	w2char(dir,wdir,l);
#else
	GetModuleFileName(NULL,dir,l);
#endif
//#else
//	strcpy(dir,".");
#endif
}

void aFile::getHomeDir(char *dir,int l) {
#ifdef USE_GLIB
	snprintf(dir,l,"%s",g_get_home_dir());
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t wdir[l];
	GetModuleFileName(NULL,wdir,l);
	w2char(dir,wdir,l);
#else
	GetModuleFileName(NULL,dir,l);
#endif
	char *p = strrchr(dir,'\\');
	if(!p) p = strrchr(dir,'/');
	if(p) *p = '\0';
//#else
//	strcpy(dir,".");
#endif
}

void aFile::getApplicationDir(char *dir,int l) {
#ifdef USE_GLIB
	getExecutable(dir,l);
	char *p = strrchr(dir,'/');
	if(p) *p = '\0';
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t wdir[l];
	GetModuleFileName(NULL,wdir,l);
	w2char(dir,wdir,l);
#else
	GetModuleFileName(NULL,dir,l);
#endif
	char *p = strrchr(dir,'\\');
	if(!p) p = strrchr(dir,'/');
	if(p) *p = '\0';
//#else
//	strcpy(dir,".");
#endif
}

void aFile::getFontsDir(char *dir,int l) {
#ifdef USE_GLIB
	snprintf(dir,l,"%s/.fonts",g_get_home_dir());
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t wdir[l];
	GetWindowsDirectory(wdir,l);
	w2char(dir,wdir,l);
#else
	GetWindowsDirectory(dir,l);
#endif
	strcat(dir,"\\fonts");
//#else
//	strcpy(dir,".");
#endif
}

void aFile::getSystemDir(char *dir,int l) {
#ifdef USE_GLIB
	snprintf(dir,l,"%s",g_get_home_dir());
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t wdir[l];
	GetSystemDirectory(wdir,l);
	w2char(dir,wdir,l);
#else
	GetSystemDirectory(dir,l);
#endif
//#else
//	strcpy(dir,".");
#endif
}

void aFile::getCurrentDir(char *dir,int l) {
#ifdef USE_GLIB
	getcwd(dir,l);
#endif
#ifdef USE_WIN32
#ifdef USE_WCHAR
	wchar_t wdir[l];
	GetCurrentDirectory(l,wdir);
	w2char(dir,wdir,l);
#else
	GetCurrentDirectory(l,dir);
#endif
//#else
//	strcpy(dir,".");
#endif
}


