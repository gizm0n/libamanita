#ifndef _AMANITA_STRING_H
#define _AMANITA_STRING_H

/**
 * @file amanita/aString.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-01
 * @date Created: 2003-11-30
 */ 

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <amanita/aObject.h>

enum {
	aSTRING_HTML_QUOTES		= 0x00000001,
	aSTRING_HTML_AMP			= 0x00000002,
	aSTRING_HTML_LTGT			= 0x00000004,
	aSTRING_HTML_NAMED		= 0x000000ff,
	aSTRING_HTML_CODES		= 0x00000100,
	aSTRING_HTML_UNICODE		= 0x00000200,
	aSTRING_HTML_ALL			= 0xffffffff,
};

/** A generic string class.
 * 
 * This string class is built to replace the standard C++ string class. It contains a great
 * number of methods for string handling and conversion between formats.
 * 
 * Something that differs this string class from ordinary C strings is that it can contain
 * any type of data, both binary data, text data, formatted data of any kind. The string is
 * not ended with a '\0' char, but instead contains an integer value with the length of the
 * string.
 * @ingroup amanita */
class aString : public aObject {
friend class aHashtable;
friend class aVector;

/** @cond */
aObject_Instance(aString)
/** @endcond */

protected:
	static const char upper_hex[17];
	static const char lower_hex[17];

	char *str;		//!< Char array containing the string data.
	size_t len;		//!< Length of string. Len must always be >= 0 and <= cap.
	size_t cap;		//!< Capacity of string, when len==cap it's time to increase size of capacity.

	/** Increase capacity of string by n.
	 * @param n Number of bytes to increase string capacity by. */
	void resize(size_t l);
	long move(long n,size_t l);

	/** @name Insert integer
	 * @{ */
#if __WORDSIZE < 64
	size_t inserti32(long n,int32_t i);		//!< Insert signed 32 bit integer.
	size_t insertu32(long n,uint32_t i);	//!< Insert unsigned 32 bit integer.
#endif
	size_t inserti64(long n,int64_t i);		//!< Insert signed 64 bit integer.
	size_t insertu64(long n,uint64_t i);	//!< Insert unsigned 64 bit integer.
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
	aString(const aString *s);
	aString(const aString &s);
	~aString();
	/** @} */

	/** @name Operators
	 * @{ */
	operator const char *() const { return str; }
	operator bool() const { return (str && len>0); }
	operator int() const { return len; }
	operator unsigned int() const { return len; }
	operator long() const { return len; }
	operator unsigned long() const { return len; }
	aString &operator=(const aString *s) { clear();if(s) insert(0,s->str,s->len);return *this; }
	aString &operator=(const aString &s) { clear();insert(0,s.str,s.len);return *this; }
	aString &operator=(const char *s) { clear();insert(0,s,0);return *this; }
	char operator[](long i) { return charAt(i); }
	bool operator==(const aString *s) { return equals(s); }
	bool operator==(const aString &s) { return equals(s); }
	bool operator==(const char *s) { return equals(s,0,0); }
	aString &operator<<(char c) { insert(len,c);return *this; }
	aString &operator<<(const aString *s) { insert(len,s);return *this; }
	aString &operator<<(const aString &s) { insert(len,s);return *this; }
	aString &operator<<(const char *s) { insert(len,s);return *this; }
#if __WORDSIZE == 64
	aString &operator<<(short i) { inserti64(len,(int64_t)i);return *this; }
	aString &operator<<(unsigned short i) { insertu64(len,(uint64_t)i);return *this; }
	aString &operator<<(int i) { inserti64(len,(int64_t)i);return *this; }
	aString &operator<<(unsigned int i) { insertu64(len,(uint64_t)i);return *this; }
	aString &operator<<(long int i) { inserti64(len,(int64_t)i);return *this; }
	aString &operator<<(unsigned long int i) { insertu64(len,(uint64_t)i);return *this; }
#else
	aString &operator<<(short i) { inserti32(len,(int32_t)i);return *this; }
	aString &operator<<(unsigned short i) { insertu32(len,(uint32_t)i);return *this; }
	aString &operator<<(int i) { inserti32(len,(int32_t)i);return *this; }
	aString &operator<<(unsigned int i) { insertu32(len,(uint32_t)i);return *this; }
	aString &operator<<(long int i) { inserti32(len,(int32_t)i);return *this; }
	aString &operator<<(unsigned long int i) { insertu32(len,(uint32_t)i);return *this; }
#endif
	aString &operator<<(long long int i) { inserti64(len,(int64_t)i);return *this; }
	aString &operator<<(unsigned long long int i) { insertu64(len,(uint64_t)i);return *this; }
	aString &operator<<(float f) { insert(len,f);return *this; }
	aString &operator<<(double d) { insert(len,d);return *this; }
	/** @} */

	/** @name Insert
	 * @{ */
	size_t insert(long n,char c);
	size_t insert(long n,const aString *s) { if(s) return insert(n,s->str,s->len);return 0; }
	size_t insert(long n,const aString &s) { return insert(n,s.str,s.len); }
	size_t insert(long n,const char *s,size_t l=0);
	/** Insert substring offset o, length l of s. */
	size_t insert(long n,const aString *s,long o,long l) { if(s) return insert(n,s->str,o,l);return 0; }
	size_t insert(long n,const aString &s,long o,long l) { return insert(n,s.str,o,l); }
	size_t insert(long n,const char *s,long o,long l);
#if __WORDSIZE == 64
	size_t insert(long n,short i) { return inserti64(n,(int64_t)i); }
	size_t insert(long n,unsigned short i) { return insertu64(n,(uint64_t)i); }
	size_t insert(long n,int i) { return inserti64(n,(int64_t)i); }
	size_t insert(long n,unsigned int i) { return insertu64(n,(uint64_t)i); }
	size_t insert(long n,long int i) { return inserti64(n,(int64_t)i); }
	size_t insert(long n,unsigned long int i) { return insertu64(n,(uint64_t)i); }
#else
	size_t insert(long n,short i) { return inserti32(n,(int32_t)i); }
	size_t insert(long n,unsigned short i) { return insertu32(n,(uint32_t)i); }
	size_t insert(long n,int i) { return inserti32(n,(int32_t)i); }
	size_t insert(long n,unsigned int i) { return insertu32(n,(uint32_t)i); }
	size_t insert(long n,long int i) { return inserti32(n,(int32_t)i); }
	size_t insert(long n,unsigned long int i) { return insertu32(n,(uint32_t)i); }
#endif
	size_t insert(long n,long long int i) { return inserti64(n,(int64_t)i); }
	size_t insert(long n,unsigned long long int i) { return insertu64(n,(uint64_t)i); }
	size_t insert(long n,double f,int d=2,char c='.');
	size_t insertln(long n) { return insert(n,endline,0); }

	size_t insertHex(long n,uint64_t i,bool upper=true);
	size_t insertBase(long n,int64_t i,int base);
	size_t insertBase(long n,uint64_t i,int base);
	/** @} */

	/** @name Insert
	 * @{ */
	size_t append(char c) { return insert(len,c); }
	size_t append(const aString *s) { if(s) return insert(len,s->str,s->len);return 0; }
	size_t append(const aString &s) { return insert(len,s.str,s.len); }
	size_t append(const char *s,size_t l=0) { return insert(len,s,l); }
	/** Insert substring offset o, length l of s. */
	size_t append(const aString *s,long o,long l) { if(s) return insert(len,s->str,o,l);return 0; }
	size_t append(const aString &s,long o,long l) { return insert(len,s.str,o,l); }
	size_t append(const char *s,long o,long l) { return insert(len,s,o,l); }
#if __WORDSIZE == 64
	size_t append(short i) { return inserti64(len,(int64_t)i); }
	size_t append(unsigned short i) { return insertu64(len,(uint64_t)i); }
	size_t append(int i) { return inserti64(len,(int64_t)i); }
	size_t append(unsigned int i) { return insertu64(len,(uint64_t)i); }
	size_t append(long int i) { return inserti64(len,(int64_t)i); }
	size_t append(unsigned long int i) { return insertu64(len,(uint64_t)i); }
#else
	size_t append(short i) { return inserti32(len,(int32_t)i); }
	size_t append(unsigned short i) { return insertu32(len,(uint32_t)i); }
	size_t append(int i) { return inserti32(len,(int32_t)i); }
	size_t append(unsigned int i) { return insertu32(len,(uint32_t)i); }
	size_t append(long int i) { return inserti32(len,(int32_t)i); }
	size_t append(unsigned long int i) { return insertu32(len,(uint32_t)i); }
#endif
	size_t append(long long int i) { return inserti64(len,(int64_t)i); }
	size_t append(unsigned long long int i) { return insertu64(len,(uint64_t)i); }
	size_t append(double f,int d=2,char c='.') { return insert(len,f,d,c); }
	size_t append(FILE *fp,bool uesc=true) { return appendUntil(fp,0,0,uesc); }
	size_t appendln() { return insert(len,endline,0); }

	size_t appendHex(uint64_t i,bool upper=true) { return insertHex(len,i,upper); }
	size_t appendBase(int64_t i,int base) { return insertBase(len,i,base); }
	size_t appendBase(uint64_t i,int base) { return insertBase(len,i,base); }

	size_t appendf(const char *f, ...);
	size_t vappendf(const char *f,va_list list);

	size_t appendWord(const char **s,bool uesc=true) { return appendUntil(s,whitespace,whitespace,uesc); }
	size_t appendWord(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace,whitespace,uesc); }
	size_t appendTab(const char **s,bool uesc=true) { return appendUntil(s,whitespace+1,whitespace+1,uesc); }
	size_t appendTab(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace+1,whitespace+1,uesc); }
	size_t appendLine(const char **s,bool uesc=true) { return appendUntil(s,whitespace+2,whitespace+2,uesc); }
	size_t appendLine(FILE *fp,bool uesc=true) { return appendUntil(fp,whitespace+2,whitespace+2,uesc); }
	size_t appendUntil(const char **s,const char *end,const char *trim=0,bool uesc=true);
	size_t appendUntil(FILE *fp,const char *end=0,const char *trim=0,bool uesc=true);
	/** @} */

	/** @name Repeat
	 * @{ */
	size_t repeat(char c,size_t n);
	size_t repeat(const aString *s,size_t n) { if(s) repeat(s->str,s->len,n);return *this; }
	size_t repeat(const aString &s,size_t n) { return repeat(s.str,s.len,n); }
	size_t repeat(const char *s,size_t l,size_t n);
	/** @} */

	/** @name Input and Output
	 * @{ */
	size_t include(const char *fn);
	size_t includef(const char *format, ...);

	size_t print(FILE *fp);
	size_t println(FILE *fp);
	/** @} */

	/** @name Find & Match methods.
	 * @{ */
	/** Find a char.
	 * @param c Char to find.
	 * @param o Offset position.
	 * @param l Length in string to search.
	 * @return Position in string where char is, or -1 if no found. */
	long find(char c,long o=0,long l=0);
	/** @see long find(const char *,long,long,long) */
	long find(const aString *s,long o=0,long l=0) { return s? find(s->str,o,l,s->len) : -1; }
	/** @see long find(const char *,long,long,long) */
	long find(const aString &s,long o=0,long l=0) { return find(s.str,o,l,s.len); }
	/** Find a string.
	 * @param s String to find.
	 * @param o Offset position.
	 * @param l Length in string to search.
	 * @return Position in string where string is, or -1 if no found. */
	long find(const char *s,long o=0,long l=0,long sl=0);
	/** Find any char from s int string.
	 * @param s Any chars in string s will be matched.
	 * @param o Offset position.
	 * @param l Length in string to search.
	 * @return Position in string where char is, or -1 if no found. */
	long findChar(const char *s,long o=0,long l=0);

	/** Match nested tags.
	 * For the string "abc [b]def [b]ghi[/b] jkl[/b] mno." will return 25, the index after "jkl".
	 * @param tag1 Opening tag, does not have to be complete, such as "[b", but make sure c1 is set.
	 * @param tag2 Closing tag, does not have to be complete, such as "[/b", but make sure c2 is set.
	 * @param o Offset, if negative offset is counted from ending of string.
	 * @param l Length to search in string, if zero or negative length is calculated from the entire string length and backward.
	 * @param c1 String of chars to accept completing of tag1, e.g. " \n\t]" will accept "[b" to match with any of these chars, but "[bar" will not match.
	 * @param c1 String of chars to accept completing of tag2, e.g. " \n\t]" will accept "[/b" to match with any of these chars, but "[/bar" will not match.
	 * @return Zero based index of beginning of last nested tag.
	 */
	long matchNestedTags(const char *tag1,const char *tag2,long o=0,long l=0,const char *c1=0,const char *c2=0);
	/** @} */

	/** @name Equals methods.
	 * @{ */
	/** @see bool equals(const char *,long,long) */
	bool equals(const aString *s,long o=0,long l=0) { return s? equals(s->str,o,l!=0? l : s->len) : false; }
	/** @see bool equals(const char *,long,long) */
	bool equals(const aString &s,long o=0,long l=0) { return equals(s.str,o,l!=0? l : s.len); }
	/** Compare string (or substring) with s and return true if equal, or false. If o and l is set,
	 * a substring is compared, otherwise the entire string.
	 * @param s String to compare.
	 * @param o Offset position.
	 * @param l Length in string to compare.
	 * @return True if equal, otherwise false. */
	bool equals(const char *s,long o=0,long l=0);
	/** @} */


	/** @name Compare methods.
	 * @{ */
	/** @see int compare(const char *,long,long) */
	int compare(const aString *s,long o=0,long l=0) { return s? compare(s->str,o,l!=0? l : s->len) : -1; }
	/** @see int compare(const char *,long,long) */
	int compare(const aString &s,long o=0,long l=0) { return compare(s.str,o,l!=0? l : s.len); }
	/** Compare string (or substring) with s and return result. If o and l is set,
	 * a substring is compared, otherwise the entire string.
	 * @see int strncmp (const char *,const char *,size_t)
	 * @param s String to compare.
	 * @param o Offset position.
	 * @param l Length in string to compare.
	 * @return Zero if equal, a positive value if string is greater than s, otherwise a negative value. */
	int compare(const char *s,long o=0,long l=0);
	/** @} */

	/** @name Count methods.
	 * @{ */
	/** Count the number of times c appears in string.
	 * @param c Char to look for.
	 * @return Number of times c is found in string. */
	size_t count(char c);
	/** Count the number of times s appears in string.
	 * @param s String to look for.
	 * @return Number of times s is found in string. */
	size_t count(const char *s);
	/** @} */

	/** @name Replace methods.
	 * @{ */
	size_t replace(const char *s,const char *r);
	size_t replace(const char *s, ...);
	size_t replace(const char **arr);
	/** @} */

	/** @name Strip methods.
	 * @{ */
	size_t stripComments();
	size_t stripHTML();
	size_t stripHTMLComments();
	/** @} */

	/** @name Substring methods.
	 * @{ */
	size_t substr(aString *s,long o,long l) { if(s) return substr(*s,o,l);return 0; }
	size_t substr(aString &s,long o,long l);
	size_t substr(char *s,long o,long l);
	/** @} */

	/** @name Encoding methods.
	 * @{ */
	void newline(const char *nl);
	void escape();
	void unescape();
	void quote(const char c);
	void unquote();
	void encodeURL();
	void decodeURL();
	void encodeHTML(int f=aSTRING_HTML_ALL);
	void decodeHTML();
	/** @} */

	/** @name Capacity methods.
	 * @{ */
	void trim() { len = trim(str); }
	void trim(long &o,long &l);
	void clear() { if(str) *str = '\0',len = 0; }
	void free();
	void setCapacity(size_t n);
	void increaseCapacity(size_t n) { resize(n); }

	size_t length() { return len; }
	size_t capacity() { return cap; }
	/** @} */

	/** @name Type conversion methods.
	 * @{ */
	char charAt(long i);
	const char *toCharArray() { return str; }
	long toInt();
	size_t toIntArray(long *n,char c=',');
	/** @} */

	/** @name Tokenizing methods.
	 * @{ */
	static size_t nextWord(const char **s,const char *c=whitespace);
	/** @} */

	/** @name Static functions.
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
	static size_t reverse(char *str);
	static size_t trim(char *str);
	static void printUTF8(char *d,const char *s,size_t o,size_t l);
	/** @} */

	/** @name Test char methods.
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


#endif /* _AMANITA_STRING_H */

