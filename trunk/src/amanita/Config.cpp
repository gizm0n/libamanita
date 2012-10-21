
#include "_config.h"

#ifdef USE_WIN32
#include <stdlib.h>
#include <string.h>
#include <windows.h>

wchar_t *char2w(const char *s) {
	if(s) {
		int l = strlen(s);
		wchar_t *w = (wchar_t *)malloc(sizeof(wchar_t)*(l+1));
		if(MultiByteToWideChar(CP_UTF8,0,s,-1,w,l+1)) return w;
		else free(w);
	}
	return 0;
}

void char2w(wchar_t *w,const char *s,int l) {
	if(w && s) {
		if(!*s) *w = L'\0';
		else {
			if(l==0) l = strlen(s);
			MultiByteToWideChar(CP_UTF8,0,s,-1,w,l+1);
		}
	}
}

char *w2char(const wchar_t *w) {
	if(w) {
		int l = wcslen(w);
		char *s = (char *)malloc(l+1);
		if(WideCharToMultiByte(CP_UTF8,0,w,-1,(char*)s,l+1,0,0)) return s;
		else free(s);
	}
	return 0;
}

void w2char(char *s,const wchar_t *w,int l) {
	if(s && w) {
		if(!*w) *s = '\0';
		else {
			if(l==0) l = wcslen(w);
			WideCharToMultiByte(CP_UTF8,0,w,-1,(char*)s,l+1,0,0);
		}
	}
}

tchar_t *tstrdup(const char *s,int l) {
#ifdef USE_WCHAR
	wchar_t *w = 0;
	if(s) {
		if(l==0) l = s? strlen(s) : 0;
		w = (wchar_t *)malloc((l+1)*sizeof(wchar_t));
		char2w(w,s,l);
	}
	return (tchar_t *)w;
#else
	return (tchar_t *)s;
#endif
}

void tfree(tchar_t *s) {
#ifdef USE_WCHAR
	free(s);
#endif
}

#endif

