#ifndef _LIBAMANITA_STRING_H
#define _LIBAMANITA_STRING_H

/**
 * @file libamanita/aString.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2003-11-30
 */ 

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <libamanita/aObject.h>


/** A generic string class.
 * 
 * This string class is built to replace the standard C++ string class. It contains a great
 * number of methods for string handling and conversion between formats.
 * 
 * Something that differs this string class from ordinary C strings is that it can contain
 * any type of data, both binary data, text data, formatted data of any kind. The string is
 * not ended with a '\0' char, but instead contains an integer value with the length of the
 * string.
 * @ingroup libamanita
 */
class aString : public aObject {
/** @cond */
aObject_Instance(aString)
/** @endcond */

private:
	static const char upper_hex[17];
	static const char lower_hex[17];

	char *str;		//!< Char array containing the string data.
	size_t len;		//!< Length of string. Len must always be >= 0 and <= cap.
	size_t cap;		//!< Capacity of string, when len==cap it's time to increase size of capacity.

	/** Increase capacity of string by n.
	 * @param n Number of bytes to increase string capacity by. */
	void resize(size_t n);

	/** @name Append integer
	 * @{ */
#if __WORDSIZE < 64
	aString &appendi32(int32_t i);		//!< Append signed 32 bit integer.
	aString &appendu32(uint32_t i);		//!< Append unsigned 32 bit integer.
#endif
	aString &appendi64(int64_t i);		//!< Append signed 64 bit integer.
	aString &appendu64(uint64_t i);		//!< Append unsigned 64 bit integer.
	/** @} */

public:
	struct entity {
		const char *name;
		size_t len;
	};
	static const entity HTMLentities[256];
	static const char *blank;
	static const char *endline;
	static const char *whitespace;

	/** @name Constructors and Destructors
	 * @{ */
	aString(size_t c=0);
	aString(const char *s,size_t l=0);
	aString(aString *s);
	aString(aString &s);
	~aString();
	/** @} */

	/** @name Operators
	 * @{ */
	operator const char *() const { return str; }
	operator int() { return toInt(); }
	aString &operator=(const aString *s) { clear();if(s) append(s->str,s->len);return *this; }
	aString &operator=(const aString &s) { return clear().append(s.str,s.len); }
	aString &operator=(const char *s) { return clear().append(s); }
	char operator[](size_t i) { return str && i>=0ul && i<len? str[i] : '\0'; }
	bool operator==(aString *s) { return equals(s); }
	bool operator==(aString &s) { return equals(s); }
	bool operator==(const char *s) { return equals(s); }
	aString &operator+=(const char c) { return append(c); }
	aString &operator+=(aString *s) { if(s) append(s->str,s->len);return *this; }
	aString &operator+=(aString &s) { return append(s.str,s.len); }
	aString &operator+=(const char *s) { return append(s,0); }
#if __WORDSIZE == 64
	aString &operator+=(short i) { return appendi64((int64_t)i); }
	aString &operator+=(unsigned short i) { return appendu64((uint64_t)i); }
	aString &operator+=(int i) { return appendi32((int64_t)i); }
	aString &operator+=(unsigned int i) { return appendu32((uint64_t)i); }
	aString &operator+=(long int i) { return appendi64((int64_t)i); }
	aString &operator+=(unsigned long int i) { return appendu64((uint64_t)i); }
#else
	aString &operator+=(short i) { return appendi32((int32_t)i); }
	aString &operator+=(unsigned short i) { return appendu32((uint32_t)i); }
	aString &operator+=(int i) { return appendi32((int32_t)i); }
	aString &operator+=(unsigned int i) { return appendu32((uint32_t)i); }
	aString &operator+=(long int i) { return appendi32((int32_t)i); }
	aString &operator+=(unsigned long int i) { return appendu32((uint32_t)i); }
#endif
	aString &operator+=(long long int i) { return appendi64((int64_t)i); }
	aString &operator+=(unsigned long long int i) { return appendu64((uint64_t)i); }
	aString &operator+=(float f) { return append(f); }
	aString &operator+=(double d) { return append(d); }
	/** @} */

	/** @name Append
	 * @{ */
	aString &append(char c);
	aString &append(char c,size_t n);
	aString &append(aString *s) { if(s) append(s->str,s->len);return *this; }
	aString &append(aString *s,size_t n) { if(s) append(s->str,s->len,n);return *this; }
	aString &append(aString &s) { return append(s.str,s.len); }
	aString &append(aString &s,size_t n) { return append(s.str,s.len,n); }
	aString &append(const char *s,size_t l=0);
	aString &append(const char *s,size_t l,size_t n);
#if __WORDSIZE == 64
	aString &append(short i) { return appendi64((int64_t)i); }
	aString &append(unsigned short i) { return appendu64((uint64_t)i); }
	aString &append(int i) { return appendi64((int64_t)i); }
	aString &append(unsigned int i) { return appendu64((uint64_t)i); }
	aString &append(long int i) { return appendi64((int64_t)i); }
	aString &append(unsigned long int i) { return appendu64((uint64_t)i); }
#else
	aString &append(short i) { return appendi32((int32_t)i); }
	aString &append(unsigned short i) { return appendu32((uint32_t)i); }
	aString &append(int i) { return appendi32((int32_t)i); }
	aString &append(unsigned int i) { return appendu32((uint32_t)i); }
	aString &append(long int i) { return appendi32((int32_t)i); }
	aString &append(unsigned long int i) { return appendu32((uint32_t)i); }
#endif
	aString &append(long long int i) { return appendi64((int64_t)i); }
	aString &append(unsigned long long int i) { return appendu64((uint64_t)i); }
	aString &append(double f,int n=2,char c='.');
	aString &append(FILE *fp,bool uesc=true) { return appendUntil(fp,0,0,uesc); }
	aString &appendln() { return append(endline); }

	aString &appendHex(uint64_t i,bool upper=true);
	aString &appendBase(int64_t i,int base);
	aString &appendBase(uint64_t i,int base);

	aString &appendf(const char *f, ...);
	aString &vappendf(const char *f,va_list list);

	aString &appendWord(const char **s,bool uesc=true) { return appendUntil(s,whitespace,whitespace,uesc); }
	aString &appendWord(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace,whitespace,uesc); }
	aString &appendTab(const char **s,bool uesc=true) { return appendUntil(s,whitespace+1,whitespace+1,uesc); }
	aString &appendTab(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace+1,whitespace+1,uesc); }
	aString &appendLine(const char **s,bool uesc=true) { return appendUntil(s,whitespace+2,whitespace+2,uesc); }
	aString &appendLine(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace+2,whitespace+2,uesc); }
	aString &appendUntil(const char **s,const char *end,const char *trim=0,bool uesc=true);
	aString &appendUntil(FILE *fp,const char *end=0,const char *trim=0,bool uesc=true);
	/** @} */

	/** @name Input and Output
	 * @{ */
	aString &include(const char *fn);
	aString &includef(const char *format, ...);

	aString &print(FILE *fp);
	aString &println(FILE *fp);
	/** @} */

	/** @name String formats
	 * @{ */
	static void printUTF8(char *d,const char *s,size_t offset,size_t len);
	/** @} */

	/** @name Compare
	 * @{ */
	long indexOf(aString *s) { return s? indexOf(s->str,s->len) : -1l; }
	long indexOf(aString &s) { return indexOf(s.str,s.len); }
	long indexOf(const char *s,size_t l=0ul);
	bool startsWith(aString *s) { return s? startsWith(s->str,s->len) : false; }
	bool startsWith(aString &s) { return startsWith(s.str,s.len); }
	bool startsWith(const char *s,size_t l=0ul);
	bool equals(aString *s) { return s? equals(s->str) : false; }
	bool equals(aString &s) { return equals(s.str); }
	bool equals(const char *s);
	int compare(aString *s) { return s? compare(s->str) : 256; }
	int compare(aString &s) { return compare(s.str); }
	int compare(const char *s);
	/** @} */

	/** @name Count
	 * @{ */
	size_t count(char c);
	size_t count(const char *s);
	/** @} */

	/** @name Replace
	 * @{ */
	aString &replace(const char *s,const char *r);
	/** @} */

	/** @name Encoding
	 * @{ */
	aString &escape();
	aString &unescape();
	aString &quote(const char c);
	aString &unquote();
	aString &encodeURL();
	aString &decodeURL();
	aString &encodeHTML();
	aString &decodeHTML();
	/** @} */

	/** @name Capacity
	 * @{ */
	aString &trim() { len = trim(str);return *this; }
	aString &clear() { if(str) *str = '\0',len = 0ul;return *this; }
	aString &free();
	void setCapacity(size_t n);
	void increaseCapacity(size_t n) { resize(n); }

	size_t length() { return len; }
	size_t capacity() { return cap; }
	/** @} */

	/** @name Types
	 * @{ */
	const char *toCharArray() { return str; }
	int toInt();
	size_t toIntArray(int *n,char c=',');
	/** @} */

	/** @name Tokens
	 * @{ */
	static size_t nextWord(const char **s,const char *c=whitespace);
	/** @} */

	/** @name Static functions
	 * @{ */
	static char toLower(const char c) { return (c>='A' && c<='Z')? c+32 : c; }
	static char toUpper(const char c) { return (c>='a' && c<='z')? c-32 : c; }
	static int fromHex(char c) { return c>='0' && c<='9'? c-'0' : (c>='a' && c<='f'? c-87 : (c>='A' && c<='F'? c-55 : 0)); }
	static uint64_t fromHex(const char *str);
	static char *toHex(char *h,uint64_t i,bool upper=true);
	static char *toLower(char *str);
	static char *toUpper(char *str);
	static int stricmp(const char *str1,const char *str2);
	static int strnicmp(const char *str1,const char *str2,size_t n);
	static char *stristr(char *str1,const char *str2);
	static int countTokens(char *str,const char *delim,bool cins=false);
	static char **split(char **list,char *str,const char *delim,bool cins=false);
	static size_t trim(char *str);
	/** @} */

	/** @name Test char
	 * @{ */
	static bool isLower(char c) { return (c>='a' && c<='z'); }
	static bool isUpper(char c) { return (c>='A' && c<='Z'); }
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
	/** @} */
};


/** @name String addition operator
 * @{ */
aString operator+(aString &s,const char c);
aString operator+(const char c,aString &s);
aString operator+(aString &s,aString *s1);
aString operator+(aString *s,aString &s1);
aString operator+(aString &s,aString &s1);
aString operator+(aString &s,const char *s1);
aString operator+(const char *s,aString &s1);
aString operator+(aString &s,int16_t i);
aString operator+(int16_t i,aString &s);
aString operator+(aString &s,uint16_t i);
aString operator+(uint16_t i,aString &s);
aString operator+(aString &s,int32_t i);
aString operator+(int32_t i,aString &s);
aString operator+(aString &s,uint32_t i);
aString operator+(uint32_t i,aString &s);
aString operator+(aString &s,int64_t i);
aString operator+(int64_t i,aString &s);
aString operator+(aString &s,uint64_t i);
aString operator+(uint64_t i,aString &s);
aString operator+(aString &s,float f);
aString operator+(float f,aString &s);
aString operator+(aString &s,double d);
aString operator+(double d,aString &s);
/** @} */


#endif /* _LIBAMANITA_STRING_H */

