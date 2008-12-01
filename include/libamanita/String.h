#ifndef _LIBAMANITA_STRING_H
#define _LIBAMANITA_STRING_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <libamanita/Object.h>



class String : public Object {
RttiObjectInstance(String)

private:
	char *str;
	size_t len,cap;

	void resize(size_t n);

public:
	struct entity {
		const char *name;
		size_t len;
	};
	static const entity HTMLentities[256];
	static const char *blank;
	static const char *endline;
	static const char *whitespace;

	String(size_t c=0);
	String(const char *s,size_t l=0ul);
	String(String *s);
	String(String &s);
	~String();

	operator const char *() const { return str; }
	operator int() { return toInt(); }
	String &operator=(const String *s) { clear();if(s) append(s->str,s->len);return *this; }
	String &operator=(const String &s) { return clear().append(s.str,s.len); }
	String &operator=(const char *s) { return clear().append(s); }
	char operator[](size_t i) { return str && i>=0ul && i<len? str[i] : '\0'; }
	bool operator==(String *s) { return equals(s); }
	bool operator==(String &s) { return equals(s); }
	bool operator==(const char *s) { return equals(s); }
	String &operator+=(const char c) { return append(c); }
	String &operator+=(String *s) { if(s) append(s->str,s->len);return *this; }
	String &operator+=(String &s) { return append(s.str,s.len); }
	String &operator+=(const char *s) { return append(s,0); }
	String &operator+=(int16_t i) { return append((int32_t)i); }
	String &operator+=(uint16_t i) { return append((uint32_t)i); }
	String &operator+=(int32_t i) { return append(i); }
	String &operator+=(uint32_t i) { return append(i); }
	String &operator+=(int64_t i) { return append(i); }
	String &operator+=(uint64_t i) { return append(i); }
	String &operator+=(float f) { return append(f); }
	String &operator+=(double d) { return append(d); }

	String &append(char c);
	String &append(char c,size_t n);
	String &append(String *s) { if(s) append(s->str,s->len);return *this; }
	String &append(String *s,size_t n) { if(s) append(s->str,s->len,n);return *this; }
	String &append(String &s) { return append(s.str,s.len); }
	String &append(String &s,size_t n) { return append(s.str,s.len,n); }
	String &append(const char *s,size_t l=0ul);
	String &append(const char *s,size_t l,size_t n);
	String &append(int16_t i) { return append((int32_t)i); }
	String &append(uint16_t i) { return append((uint32_t)i); }
	String &append(int32_t i);
	String &append(int32_t i,int base);
	String &append(uint32_t i);
	String &append(int64_t i);
	String &append(uint64_t i);
	String &append(double f,int n=2,char c='.');
	String &append(FILE *fp,bool uesc=true) { return appendUntil(fp,0,0,uesc); }
	String &appendln() { return append(endline); }

	String &appendf(const char *f, ...);
	String &vappendf(const char *f,va_list list);

	String &appendWord(const char *s,bool uesc=true) { return appendUntil(s,whitespace,uesc); }
	String &appendWord(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace,whitespace,uesc); }
	String &appendTab(const char *s,bool uesc=true) { return appendUntil(s,whitespace+1,uesc); }
	String &appendTab(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace+1,whitespace+1,uesc); }
	String &appendLine(const char *s,bool uesc=true) { return appendUntil(s,whitespace+2,uesc); }
	String &appendLine(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace+2,whitespace+2,uesc); }
	String &appendUntil(const char *s,const char *end,bool uesc=true);
	String &appendUntil(FILE *fp,const char *end=0,const char *trim=0,bool uesc=true);

	String &print(FILE *fp);
	String &println(FILE *fp);

	long indexOf(String *s) { return s? indexOf(s->str,s->len) : -1l; }
	long indexOf(String &s) { return indexOf(s.str,s.len); }
	long indexOf(const char *s,size_t l=0ul);
	bool startsWith(String *s) { return s? startsWith(s->str,s->len) : false; }
	bool startsWith(String &s) { return startsWith(s.str,s.len); }
	bool startsWith(const char *s,size_t l=0ul);
	bool equals(String *s) { return s? equals(s->str) : false; }
	bool equals(String &s) { return equals(s.str); }
	bool equals(const char *s);
	int compare(String *s) { return s? compare(s->str) : 256; }
	int compare(String &s) { return compare(s.str); }
	int compare(const char *s);

	String &escape();
	String &unescape();
	String &quote(const char c);
	String &unquote();
	String &encodeURL();
	String &decodeURL();
	String &encodeHTML();
	String &decodeHTML();

	String &trim() { len = trim(str);return *this; }
	String &clear() { if(str) *str = '\0',len = 0ul;return *this; }
	String &free();
	void setCapacity(size_t n);

	size_t length() { return len; }
	size_t capacity() { return cap; }

	const char *toString() { return str; }
	int toInt();

	static size_t nextWord(const char **s,const char *c=whitespace);

	static char toLower(const char c) { return (c>='A' && c<='Z') || (c!='×' && c>='À' && c<='Þ')? c+32 : c; }
	static char toUpper(const char c) { return (c>='a' && c<='z') || (c!='÷' && c>='à' && c<='þ')? c-32 : c; }
	static int fromHex(char c) { return c>='0' && c<='9'? c-'0' : (c>='a' && c<='f'? c-87 : (c>='A' && c<='F'? c-55 : 0)); }
	static unsigned long fromHex(char *str);
	static char *toHex(char *h,unsigned long i);
	static char *toLower(char *str);
	static char *toUpper(char *str);
	static int stricmp(const char *str1,const char *str2);
	static int strnicmp(const char *str1,const char *str2,size_t n);
	static char *stristr(char *str1,const char *str2);
	static int countTokens(char *str,const char *delim,bool cins=false);
	static char **split(char **list,char *str,const char *delim,bool cins=false);
	static size_t trim(char *str);

	static bool isLower(char c) { return (c>='a' && c<='z') || (c>='à' && c<='ÿ' && c!='÷'); }
	static bool isUpper(char c) { return (c>='A' && c<='Z') || (c>='À' && c<='ß' && c!='×'); }
	static bool isAlpha(unsigned char c) { return isLower(c) || isUpper(c); }
	static bool isLatin(unsigned char c) { return (c>='a' && c<='z') || (c>='A' && c<='Z'); }
	static bool isDigit(unsigned char c) { return c>='0' && c<='9'; }
	static bool isHex(unsigned char c) { return (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'); }
	static bool isWord(unsigned char c) { return isAlpha(c) || isDigit(c) || c=='_'; }
	static bool isSpace(unsigned char c) { return c==' ' || c=='\n' || c=='\t' || c=='\r' || c=='\f' || c=='\v'; }
	static bool isEscSpace(unsigned char c) { return c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v'; }
	static bool isURLEncoded(unsigned char c) { return (c<'0' && c!='-' && c!='.') || (c>'9' && c<'A') || (c>'Z' && c<'a' && c!='_') || c>'z'; }
	static bool isHTMLEntity(unsigned char c) { return HTMLentities[c].name!=0; }
	static bool isBreak(unsigned char c) { return c=='\n' || c=='\r'; }
	static bool isPunct(unsigned char c) { return isPrint(c) && !isLower(c) && !isUpper(c) && c!=' ' && !isDigit(c); }
	static bool isPrint(unsigned char c) { return c>='\x20' && c<='\x7e'; }
	static bool isGraph(unsigned char c) { return c>='\x21' && c<='\x7e'; }
	static bool isCntrl(unsigned char c) { return c<='\x1F' || c=='\x7f'; }
};

String operator+(String &s,const char c);
String operator+(const char c,String &s);
String operator+(String &s,String *s1);
String operator+(String *s,String &s1);
String operator+(String &s,String &s1);
String operator+(String &s,const char *s1);
String operator+(const char *s,String &s1);
String operator+(String &s,int16_t i);
String operator+(int16_t i,String &s);
String operator+(String &s,uint16_t i);
String operator+(uint16_t i,String &s);
String operator+(String &s,int32_t i);
String operator+(int32_t i,String &s);
String operator+(String &s,uint32_t i);
String operator+(uint32_t i,String &s);
String operator+(String &s,int64_t i);
String operator+(int64_t i,String &s);
String operator+(String &s,uint64_t i);
String operator+(uint64_t i,String &s);
String operator+(String &s,float f);
String operator+(float f,String &s);
String operator+(String &s,double d);
String operator+(double d,String &s);


#endif /* _LIBAMANITA_STRING_H */

