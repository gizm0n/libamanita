

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#if defined __linux__
#include <glib.h>
#include <glib/gstdio.h>
#elif defined WIN32
#include <windows.h>
#endif

#include <libamanita/aFile.h>



bool aFile::exists(const char *fn) {
#if defined __linux__
	return g_file_test(fn,G_FILE_TEST_EXISTS);
#elif defined WIN32
	return GetFileAttributes(fn)!=INVALID_FILE_ATTRIBUTES;
#else
	FILE *fp = fopen(fn,"r");
	if(!fp) return false;
	fclose(fp);
	return true;
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
	FILE *fps = fopen(s,"rb");
	if(!fps) return -1;
	FILE *fpd = fopen(d,"wb");
	if(!fpd) return -1;
	long n = copy(fps,fpd);
	fclose(fps);
	fclose(fpd);
	return n;
}

long aFile::copy(FILE *s,FILE *d) {
	if(!s || !d) return -1;
	size_t i;
	fseek(s,0,SEEK_END);
	long n = ftell(s);
	fseek(s,0,SEEK_SET);
	char buf[n+1];
	i = fread(buf,n,1,s);
	i = fwrite(buf,n,1,d);
	return n;
}


bool aFile::remove(const char *fn) {
#if defined __linux__
	return ::g_remove(fn)==0;
#elif defined WIN32
	int n = GetFileAttributes(fn);
	if(n&FILE_ATTRIBUTE_READONLY) return false;
	if(n&FILE_ATTRIBUTE_DIRECTORY) return RemoveDirectory((LPCTSTR)fn)!=0;
	else return DeleteFile((LPCTSTR)fn)==TRUE;
#else
	return ::remove(fn)==0;
#endif
}


bool aFile::mkdir(const char *dn,int p) {
#if defined __linux__
	return ::g_mkdir(dn,p)==0;
#elif defined WIN32
	return CreateDirectory((LPCTSTR)dn,NULL)==TRUE;
#else
	return ::mkdir(dn,p)==0;
#endif
}


aFile &aFile::open(const char *a,const char *fn, ...) {
	char buf[257];
	va_list args;
   va_start(args,fn);
	vsnprintf(buf,256,fn,args);
   va_end(args);
	if(name) free(name);
	if(file) fclose(file);
	file = fopen(buf,a);
	if(file) {
		dir = strdup(buf);
		char *p1 = strrchr(dir,'/');
		char *p2 = strrchr(dir,'\\');
		name = p1>p2? p1 : p2;
		if(name) *name = '\0',name++;
	}
	return *this;
}

aFile &aFile::close() {
	if(dir) { free(dir);dir = 0,name = 0; }
	if(file) { fclose(file);file = 0; }
	return *this;
}

aFile &aFile::read(char **data,size_t &len) {
	*data = 0;
	len = 0;
	if(file) {
		len = size();
		if(len>0) {
			*data = (char *)malloc(len);
			size_t n;
			n = fread(*data,len,1,file);
		}
	}
	return *this;
}

aFile &aFile::write(const char *data,size_t len) {
	if(file) fwrite(data,len,1,file);
	return *this;
}

size_t aFile::size() {
	if(!file) return 0;
	long n1 = ftell(file);
	fseek(file,0,SEEK_END);
	long n2 = ftell(file);
	fseek(file,n1,SEEK_SET);
	return n2>0? (size_t)n2 : 0;
}


