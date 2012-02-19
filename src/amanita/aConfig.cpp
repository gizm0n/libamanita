
#include "_config.h"

#ifdef USE_WIN32
#include <stdlib.h>
#include <string.h>
#include <windows.h>

wchar_t *char2w(const char *c) {
	if(c) {
		int l = strlen(c);
		wchar_t *w = (wchar_t *)malloc(sizeof(wchar_t)*(l+1));
		if(MultiByteToWideChar(CP_UTF8,0,c,-1,w,l+1)) return w;
		else free(w);
	}
	return 0;
}

void char2w(wchar_t *w,const char *c,int l) {
	if(w && c) {
		if(!*c) *w = L'\0';
		else MultiByteToWideChar(CP_UTF8,0,c,-1,w,l);
	}
}

char *w2char(const wchar_t *w) {
	if(w) {
		int l = wcslen(w);
		char *c = (char *)malloc(l+1);
		if(WideCharToMultiByte(CP_UTF8,0,w,-1,(char*)c,l+1,0,0)) return c;
		else free(c);
	}
	return 0;
}

void w2char(char *c,const wchar_t *w,int l) {
	if(c && w) {
		if(!*w) *c = '\0';
		else WideCharToMultiByte(CP_UTF8,0,w,-1,(char*)c,l,0,0);
	}
}
#endif

