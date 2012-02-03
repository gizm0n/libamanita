#ifndef _IPOMOEA_STRING_T_H
#define _IPOMOEA_STRING_T_H

#include <stdint.h>


typedef struct string {
	size_t size;
	size_t cap;
	char *str;
} string_t;



#define is_word(c) ((c>='0' && c<='9') || (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_')
#define is_space(c) (c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v')


char upper_case(char c);
char *upper_case_string(char *s);
char lower_case(char c);
char *lower_case_string(char *s);
int stricmp(const char *str1,const char *str2);
int strnicmp(const char *str1,const char *str2,size_t num);
void strwhsp(char *str);
int strnchr(const char *str,char c);


string_t *str_new(const char *s);
void str_delete(string_t *str);
string_t *str_dup(const string_t *str);
void str_resize(string_t *str,size_t c);
string_t *str_append(string_t *str,const char *v);
string_t *str_append_length(string_t *str,const char *v,size_t l);
string_t *str_append_char(string_t *str,char v);
string_t *str_append_int(string_t *str,int v);
string_t *str_append_string(string_t *str,const string_t *v);
string_t *str_toupper(string_t *str);
string_t *str_tolower(string_t *str);
int str_length(string_t *str);
int str_capacity(string_t *str);
void str_print(string_t *str);

#endif /* _IPOMOEA_STRING_T_H */

