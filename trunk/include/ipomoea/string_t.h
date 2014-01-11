#ifndef _IPOMOEA_STRING_T_H
#define _IPOMOEA_STRING_T_H

/**
 * @file ipomoea/string_h.h  
 * @author Per Löwgren
 * @date Modified: 2013-12-29
 * @date Created: 2012-12-23
 */ 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct string {
	size_t len;
	size_t cap;
	char *str;
} string_t;



int is_word(char c);
int is_space(char c);
int xtoi(char c);
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
void str_resize(string_t *str,size_t l);
string_t *str_append(string_t *str,const char *s);
string_t *str_append_length(string_t *str,const char *s,size_t l);
string_t *str_append_char(string_t *str,char c);
string_t *str_append_int(string_t *str,int n);
string_t *str_append_string(string_t *str,const string_t *s);
string_t *str_toupper(string_t *str);
string_t *str_tolower(string_t *str);
int str_length(string_t *str);
int str_capacity(string_t *str);
void str_print(string_t *str);


#ifdef __cplusplus
}
#endif

#endif /* _IPOMOEA_STRING_T_H */

