
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipomoea/string_t.h>

/*
typedef struct string {
	size_t len;
	size_t cap;
	char *str;
} string_t;
*/



int is_word(char c) {
	return (c>='0' && c<='9') || (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_';
}

int is_space(char c) {
	return c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v';
}

int xtoi(char c) {
	return c>='0' && c<='9'? c-'0' : (c>='a' && c<='f'? c-87 : (c>='A' && c<='F'? c-55 : 0));
}

char upper_case(char c) {
	return (c>='a' && c<='z')? c-32 : c;
}

char *upper_case_string(char *s) {
	char *s2 = strdup(s),*p = s2;
	for(; *p!='\0'; p++) if(*p>='a' && *p<='z') *p -= 32;
	return s2;
}

char lower_case(char c) {
	return (c>='A' && c<='Z')? c+32 : c;
}

char *lower_case_string(char *s) {
	char *s2 = strdup(s),*p = s2;
	for(; *p!='\0'; p++) if(*p>='A' && *p<='Z') *p += 32;
	return s2;
}

int stricmp(const char *str1,const char *str2) {
	char c1 = lower_case(*str1++),c2 = lower_case(*str2++);
	for(; c1!='\0' && c2!='\0'; c1=lower_case(*str1++),c2=lower_case(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (c1=='\0' && c2=='\0')? 0 : (c1=='\0'? -1 : 1);
}

int strnicmp(const char *str1,const char *str2,size_t num) {
	char c1 = lower_case(*str1++),c2 = lower_case(*str2++);
	for(; num>0 && c1!='\0' && c2!='\0'; c1=lower_case(*str1++),c2=lower_case(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (num==0 || (c1=='\0' && c2=='\0'))? 0 : (c1=='\0'? -1 : 1);
}


void strwhsp(char *str) {
	char *s1 = str,*s2 = str,c1,c2;
	for(c1=';',c2=*s2; c2!='\0'; c1=*s1++=*s2++,c2=*s2)
		if(is_space(c2)) {
			for(c2=*++s2; c2!='\0' && is_space(c2); c2=*++s2);
			if(is_word(c1) && is_word(c2)) *s1++ = ' ';
		}
	*s1 = '\0';
}

int strnchr(const char *str,char c) {
	int n = 0;
	while(*str!='\0') if(*str++==c) n++;
	return n;
}



string_t *str_new(const char *s) {
	string_t *str = (string_t *)malloc(sizeof(string_t));
	if(s==NULL || *s=='\0') str->len = 0,str->cap = 0,str->str = NULL;
	else {
		str->len = strlen(s);
		str->cap = str->len;
		str->str = (char *)malloc(str->cap+1);
		memcpy(str->str,s,str->len+1);
	}
	return str;
}

void str_delete(string_t *str) {
	if(str!=NULL) {
		if(str->str!=NULL) free(str->str);
		free(str);
	}
}

string_t *str_dup(const string_t *str) {
	if(str==NULL) return NULL;
	else {
		string_t *str2 = (string_t *)malloc(sizeof(string_t));
		str2->len = str->len;
		str2->cap = str->cap;
		str2->str = (char *)malloc(str2->cap);
		memcpy(str2->str,str->str,str2->len+1);
		return str2;
	}
}

void str_resize(string_t *str,size_t l) {
	if(str==NULL || l<0 || str->len+l+l<str->cap) return;
	if(!str->cap) str->cap = 1;
	if(l && str->len+l>=str->cap) str->cap = str->len+l+1;
	if(str->cap<1024) str->cap <<= 1;
	else str->cap += 1024;
	if(!str->str) str->str = (char *)malloc(str->cap+1);
	else str->str = (char *)realloc(str->str,str->cap+1);
	str->str[str->cap] = '\0';
}

string_t *str_append(string_t *str,const char *s) {
	size_t l;
	if(s==NULL || *s=='\0') return str;
	l = strlen(s);
	if(str->len+l+1>=str->cap) str_resize(str,l+1);
	memcpy(&str->str[str->len],s,l+1);
	str->len += l;
	str->str[str->len] = '\0';
	return str;
}

string_t *str_append_length(string_t *str,const char *s,size_t l) {
	if(s==NULL || *s=='\0') return str;
	if(str->len+l+1>=str->cap) str_resize(str,l+1);
	memcpy(&str->str[str->len],s,l+1);
	str->len += l;
	str->str[str->len] = '\0';
	return str;
}

string_t *str_append_char(string_t *str,char c) {
	if(str->len+2>=str->cap) str_resize(str,1);
	str->str[str->len++] = c;
	str->str[str->len] = '\0';
	return str;
}

string_t *str_append_int(string_t *str,int n) {
	if(n>=0 && n<=9) return str_append_char(str,'0'+n);
	else {
		char s[22],*p = s+21,c = 0;
		*p-- = '\0';
		if(n<0) c = '-',n = -n;
		for(; n; n/=10) *p-- = '0'+(n%10);
		if(c) *p-- = c;
		return str_append(str,p+1);
	}
}

string_t *str_append_string(string_t *str,const string_t *s) {
	return str_append_length(str,s->str,s->len);
}


string_t *str_toupper(string_t *str) {
	char *p = str->str;
	for(; *p!='\0'; p++) if(*p>='a' && *p<='z') *p -= 32;
	return str;
}

string_t *str_tolower(string_t *str) {
	char *p = str->str;
	for(; *p!='\0'; p++) if(*p>='A' && *p<='Z') *p += 32;
	return str;
}


int str_length(string_t *str) {
	return str!=NULL? (int)str->len : 0;
}

int str_capacity(string_t *str) {
	return str!=NULL? (int)str->cap : 0;
}

void str_print(string_t *str) {
	printf("%s",str->str);
}

