#ifndef _AMANITA_STRING_H
#define _AMANITA_STRING_H

/**
 * @file amanita/aString.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-16
 * @date Created: 2003-11-30
 */ 

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <amanita/aObject.h>

enum {
	aLANG_C,
	aLANG_CPP,
	aLANG_JAVA,
	aLANG_JAVASCRIPT,
	aLANG_PHP,
	aLANG_PERL,
	aLANG_BASH,
	aLANG_SHELL,
	aLANG_HTML,
	aLANG_XML,
	aLANG_CFG,
	aLANG_INI,
	aLANG_LANGS,
};

/** Escape flags.
 * @see void escape(long o,long l,const char *,int) */
enum {
	aESCAPE_QUOTE		= 0x00000001,	//!< Escape/Unescape quotes.
	aESCAPE_SL_EOL		= 0x00000002,	//!< Escape '\n' so that it becomes '\\' and '\n' instead of "\\n".
	aESCAPE_HEX			= 0x00000004,	//!< Escape/Unescape hexadecimal characters, so that '~' becomes "\\x7F".
	aESCAPE_UNICODE	= 0x00000008,	//!< Escape/Unescape unicode characters.
	aESCAPE_ALL			= 0x00000009,	//!< Escape all characters that can be escaped. Default.
	aUNESCAPE_ALL		= 0x0000000f,	//!< Unescape all characters that can be escaped. Default.
};

/** Find flags
 * @see void find(const char *) */
enum {
	aTOKEN_LTRIM		= 0x00000001,	//!< Trim tokens on left side.
	aTOKEN_RTRIM		= 0x00000002,	//!< Trim tokens on right side.
	aTOKEN_TRIM			= 0x00000004,	//!< Trim tokens on either side.
	aTOKEN_ESCAPE		= 0x00000008,	//!< Match escape sequences in tokens.
	aTOKEN_DELIM_CH	= 0x00000000,	//!< Each char in the delimiter matches.
	aTOKEN_DELIM_STR	= 0x00000010,	//!< The entire string is used as delimiter.
};

/** Encode HTML flags
 * @see void encodeHTML(int) */
enum {
	aHTML_QUOTE			= 0x00000001,	//!<
	aHTML_AMP			= 0x00000002,	//!<
	aHTML_LTGT			= 0x00000004,	//!<
	aHTML_NAMED			= 0x000000ff,	//!<
	aHTML_CODES			= 0x00000100,	//!<
	aHTML_UNICODE		= 0x00000200,	//!<
	aHTML_ALL			= 0xffffffff,	//!<
};

/** Trim flags.
 * @see void trim(long &,long &,const char *,int) */
enum {
	aTRIM_LEFT			= 0x0001,		//!<
	aTRIM_RIGHT			= 0x0002,		//!<
	aTRIM_BOTH			= 0x0003,		//!<
	aTRIM_UNTRIM		= 0x0004,		//!<
};

/** @cond */
#ifdef USE_UNIX
#define aSTRING_ENDL "\n"
#endif
#ifdef USE_WIN32
#define aSTRING_ENDL "\r\n"
#endif
#define aSTRING_WHITESPACE " \t\n\r\f\v"
/** @endcond */


/** A generic string class.
 * 
 * This string class is built to replace the standard C++ string class. It contains a
 * number of methods for string handling and conversion between formats.
 * 
 * For all methonds in the aString class that has o-offset or l-length parameters of the type
 * long, a negative value counts from the end of the string. For example, in a string 
 * "abc def ghi jkl mno pqr stu vwx y. abc def" (that has length 42), a call to
 * <tt>find("abc",-20,-2)</tt> will search in the string for "abc" from the position 22 until
 * position 40. A value of zero for the l-length parameter, searches until the end of the string. 
 * @ingroup amanita */
class aString : public aObject {
friend class aHashtable;
friend class aVector;

/** @cond */
aObject_Instance(aString)
/** @endcond */

protected:
	char *str;		//!< Char array containing the string data.
	size_t len;		//!< Length of string. Len must always be >= 0 and <= cap.
	size_t cap;		//!< Capacity of string, when len==cap it's time to increase size of capacity.

	/** Increase capacity of string by n.
	 * @param n Number of bytes to increase string capacity by. */
	void resize(size_t l);
	/** Move a section of the string, and resize capasity if needed.
	 * Used by all <tt>insert()</tt>-methods.
	 * @param o Offset, position to start moving, a negative value counts from the end.
	 * @param l Length in bytes to move. Positive value moves to the right, negative to the left. */
	long move(long o,long l);

	/** @name Insert integer
	 * @{ */
#if __WORDSIZE < 64
	size_t inserti32(long n,int32_t i);		//!< Insert signed 32 bit integer. (Only in 32bit-systems)
	size_t insertu32(long n,uint32_t i);	//!< Insert unsigned 32 bit integer. (Only in 32bit-systems)
#endif
	size_t inserti64(long n,int64_t i);		//!< Insert signed 64 bit integer.
	size_t insertu64(long n,uint64_t i);	//!< Insert unsigned 64 bit integer.
	/** @} */

public:
	static const char *blank;			//!< A blank string, e.g. "".
	static const char *endl;			//!< A line ending, e.g. "\n" for Linux, and "\r\n" for Windows.
	static const char *whitespace;	//!< White space characters, e.g. " \t\n\r".

	/** @name Constructors & Destructors
	 * @{ */
	/** Create a string with capacity c.
	 * @param c Optional value for initial capacity of string.
	 * To avoid reallocating string, set to intended size to use. */
	aString(size_t c=0);
	/** Create a string and initiate to s.
	 * @param s String to set as initial value.
	 * @param l Length of s, if zero length of s is counted using <tt>strlen()</tt>. */
	aString(const char *s,size_t l=0);
	/** Create a string and initiate to s.
	 * @param s String to set as initial value. */
	aString(const aString *s);
	/** Create a string and initiate to s.
	 * @param s String to set as initial value. */
	aString(const aString &s);
	~aString();
	/** @} */

	/** @name Operators
	 * @{ */
	/** Return string as a const char *. */
	operator const char *() const { return str; }
	/** Return true if length is >0. */
	operator bool() const { return (str && len>0); }
	/** Return length as an int. */
	operator int() const { return len; }
	/** Return length as an unsigned int. */
	operator unsigned int() const { return len; }
	/** Return length as a long. */
	operator long() const { return len; }
	/** Return length as an unsigned long. */
	operator unsigned long() const { return len; }
	/** Set string to s.
	 * @param s String to set as value.
	 * @return A reference to this string. */
	aString &operator=(const aString *s) { clear();if(s) insert(0,s->str,s->len);return *this; }
	/** Set string to s.
	 * @param s String to set as value.
	 * @return A reference to this string. */
	aString &operator=(const aString &s) { clear();insert(0,s.str,s.len);return *this; }
	/** Set string to s.
	 * @param s String to set as value.
	 * @return A reference to this string. */
	aString &operator=(const char *s) { clear();insert(0,s,0);return *this; }
	/** Get char at position i.
	 * @param i Position in string. If less than zery, count from end of string.
	 * @return Char at position i. */
	char operator[](long i) const { return charAt(i); }
	bool operator==(const aString *s) const { return equals(s); }
	bool operator==(const aString &s) const { return equals(s); }
	bool operator==(const char *s) const { return equals(s,0,0); }
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
	size_t insertln(long n) { return insert(n,endl,0); }

	size_t insertHex(long n,uint64_t i,bool upper=true);
	size_t insertBase(long n,int64_t i,int base);
	size_t insertBase(long n,uint64_t i,int base);
	/** @} */

	/** @name Append
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
	size_t appendln() { return insert(len,endl,0); }

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

	/** @name Input & Output
	 * @{ */
	size_t includef(const char *format, ...);
	size_t include(const char *fn);
	size_t include(FILE *fp);

	size_t print(FILE *fp=stdout) const;
	size_t println(FILE *fp=stdout) const;
	/** @} */

	/** @name Find, Match & Skip
	 * @{ */
	/** Find a char.
	 * @param c Char to find.
	 * @param o Offset position.
	 * @param l Length in string to search.
	 * @return Position in string where char is, or -1 if no found. */
	long find(char c,long o=0,long l=0) const;
	/** @see long find(const char *,long,long,long) */
	long find(const aString *s,long o=0,long l=0) const { return s? find(s->str,o,l,s->len) : -1; }
	/** @see long find(const char *,long,long,long) */
	long find(const aString &s,long o=0,long l=0) const { return find(s.str,o,l,s.len); }
	/** Find a string.
	 * @param s String to find.
	 * @param o Offset position.
	 * @param l Length in string to search.
	 * @param sl Length of s, if zero or negative length is calculated from the entire length and backward.
	 * @return Position in string where string is, or -1 if no found. */
	long find(const char *s,long o=0,long l=0,long sl=0) const;
	/** Find any char from s in string.
	 * @param s Any chars in string s will be matched.
	 * @param o Offset position.
	 * @param l Length in string to search.
	 * @return Position in string where char is, or -1 if no found. */
	long findChar(const char *s,long o=0,long l=0) const;

	/** Match nested tags.
	 * Returns the position at the beginning of the last ending tag. For the string
	 * "abc [b]def [b]ghi[/b] jkl[/b] mno." will return 25, the position at the last "[/b]".
	 * 
	 * For example, you may wish to match the last tag for a nested table in a HTML-document, from position p,
	 * and so you call <tt>matchNestedTags("<table","</table>",p,0," \n\t>")</tt>.
	 * 
	 * Another example can be matching nested curly-braces, then you call <tt>matchNestedTags("{","}",p)</tt>.
	 * @param tag1 Opening tag, does not have to be complete, such as "[b", but make sure c1 is set.
	 * @param tag2 Closing tag, does not have to be complete, such as "[/b", but make sure c2 is set.
	 * @param o Offset, if negative offset is counted from ending of string.
	 * @param l Length to search in string, if zero or negative length is calculated from the entire string length and backward.
	 * @param c1 String of chars to accept completing of tag1, e.g. " \n\t]" will accept "[b" to match with any of these chars, so "[b ar]" will match, but "[bar]" will not match.
	 * @param c1 String of chars to accept completing of tag2, e.g. " \n\t]" will accept "[/b" to match with any of these chars, so "[/b ar]" will match, "but "[/bar]" will not match.
	 * @return Zero based index of beginning of last nested tag, or -1 on fail.
	 */
	long matchNestedTags(const char *tag1,const char *tag2,long o=0,long l=0,const char *c1=0,const char *c2=0) const;

	/** Match a quoted string.
	 * Returns the position after the ending quote sign of the string. Escaped quotes are included
	 * in the match so that "abc\"def"@ will return 11, or the position at '@'. If the char at o
	 * is not a quot-sign - " or ', o is returned. Ending quote-sign has to be the same as the
	 * starting quote-sign.
	 * @param o Offset, if negative offset is counted from ending of string.
	 * @param l Length to search in string, if zero or negative length is calculated from the entire string length and backward.
	 * @return Zero based index after ending quote sign, or -1 on fail.
	 */
	long matchQuotes(long o=0,long l=0) const;

	/** Match a string value.
	 * Similar to matchQuotes, except will accept an unquoted string and in such a case instead of returning
	 * at end quote, returns at first white space before end of line or first white space before a comment.
	 * Type of comment is defined by lang.
	 * @param delim Delimiter separating tokens.
	 * @param o Offset, if negative offset is counted from ending of string.
	 * @param l Length to search in string, if zero or negative length is calculated from the entire string length and backward.
	 * @return Zero based index after ending quote sign, or -1 on fail.
	 */
	long matchToken(const char *delim,long o=0,long l=0,int f=0) const;

	/** Match a string value.
	 * Similar to matchQuotes, except will accept an unquoted string and in such a case instead of returning
	 * at end quote, returns at first white space before end of line or first white space before a comment.
	 * Type of comment is defined by lang.
	 * @param lang Language by which comments to match.
	 * @param o Offset, if negative offset is counted from ending of string.
	 * @param l Length to search in string, if zero or negative length is calculated from the entire string length and backward.
	 * @return Zero based index after ending quote sign, or -1 on fail.
	 */
	long matchValue(int lang,long o=0,long l=0) const;

	long skipComment(int lang,long o=0,long l=0) const;
	long skipComments(int lang,long o=0,long l=0) const;
	/** @} */

	/** @name Equals
	 * @{ */
	/** @see bool equals(const char *,long,long) */
	bool equals(const aString *s,long o=0,long l=0) const { return s? equals(s->str,o,l!=0? l : s->len) : false; }
	/** @see bool equals(const char *,long,long) */
	bool equals(const aString &s,long o=0,long l=0) const { return equals(s.str,o,l!=0? l : s.len); }
	/** Compare string (or substring) with s and return true if equal, or false. If o and l is set,
	 * a substring is compared, otherwise the entire string.
	 * @param s String to compare.
	 * @param o Offset position.
	 * @param l Length in string to compare.
	 * @return True if equal, otherwise false. */
	bool equals(const char *s,long o=0,long l=0) const;
	/** @} */


	/** @name Compare
	 * @{ */
	/** @see int compare(const char *,long,long) */
	int compare(const aString *s,long o=0,long l=0) const { return s? compare(s->str,o,l!=0? l : s->len) : -1; }
	/** @see int compare(const char *,long,long) */
	int compare(const aString &s,long o=0,long l=0) const { return compare(s.str,o,l!=0? l : s.len); }
	/** Compare string (or substring) with s and return result. If o and l is set,
	 * a substring is compared, otherwise the entire string.
	 * @see int strncmp (const char *,const char *,size_t)
	 * @param s String to compare.
	 * @param o Offset position.
	 * @param l Length in string to compare.
	 * @return Zero if equal, a positive value if string is greater than s, otherwise a negative value. */
	int compare(const char *s,long o=0,long l=0) const;
	/** @} */

	/** @name Count
	 * @{ */
	/** Count the number of times c appears in string.
	 * @param c Char to look for.
	 * @return Number of times c is found in string. */
	size_t count(char c) const;
	/** Count the number of times s appears in string.
	 * @param s String to look for.
	 * @return Number of times s is found in string. */
	size_t count(const char *s) const;
	/** @} */

	/** @name Replace
	 * @{ */
	size_t replace(const char *s,const char *r);
	size_t replace(const char *s, ...);
	size_t replace(const char **arr);
	/** @} */

	/** @name Strip
	 * @{ */
	size_t stripComments(int lang,long o=0,long l=0);
	size_t stripHTML();
	/** @} */

	/** @name Substring
	 * @{ */
	size_t substr(aString *s,long o,long l) const { if(s) return substr(*s,o,l);return 0; }
	size_t substr(aString &s,long o,long l) const;
	size_t substr(char *s,long o,long l) const;
	/** @} */

	/** @name Encoding
	 * @{ */
	void newline(const char *nl);
	/** Escape strings so that '\n' and '\t' become "\\n" and "\\t", and unicode become "\\uNNNN".
	 * Large unicode characters are translated to "\\UNNNNNNNN".
	 * 
	 * Offset and length include the size of unicode characters, so that "ab€" has length 5. This means
	 * that in this string, with offset 2 and length 1, the unicode '€' which is 3 bytes long will
	 * become split, but it is not so, because any multi-byte char that is starting within the length
	 * of l, offset o, is translated.
	 * @param o Offset, if negative from end of string.
	 * @param l Length to trim from o, if negative calculated from end of string.
	 * @param s If set, the chars in this string become escaped with a backslash, eg. '~' become "\\~". With aESCAPE_HEX-flag set chars in s are escaped with hexadecimal value, eg. '~' become "\\x7E".
	 * @param f Escape flags. */
	void escape(long o=0,long l=0,const char *s=0,int f=aESCAPE_ALL);
	/** Unescape strings so that "\\n" and "\\t" become '\n' and '\t', and \\uNNNN" become unicode.
	 * @param o Offset, if negative from end of string.
	 * @param l Length to trim from o, if negative calculated from end of string.
	 * @param f Escape flags.
	 * @see void escape(long,long,const char *,int) */
	void unescape(long o=0,long l=0,int f=aUNESCAPE_ALL);
	void quote(const char c);
	void unquote();
	void encodeURL();
	void decodeURL();
	void encodeHTML(int f=aHTML_ALL);
	void decodeHTML();
	void encodeUTF8();
	void decodeUTF8();
	void encodeBase64();
	void decodeBase64();
	/** @} */

	/** @name Capacity
	 * @{ */
	void reverse(long o=0,long l=0);
	/** @see void trim(const char *,int) */
	void trim(const char *s=0,int f=0) { len = trim(str,s,f); }
	/** Adjust o and l to trim from a set of chars.
	 * @param o Offset, if negative from end of string.
	 * @param l Length to trim from o, if negative calculated from end of string.
	 * @param s Should contain a string of chars to trim. Default is the static variable whitespace.
	 * @param f Use the flags named aTRIM_*. Default is aTRIM_BOTH. */
	void trim(long &o,long &l,const char *s=0,int f=0) const;
	/** Trim left side of string.
	 * @see void trim(long &,long &,const char *,int) */
	void ltrim(long &o,long l=0,const char *s=0) const { trim(o,l,s,aTRIM_LEFT); }
	/** Trim right side of string.
	 * @see void trim(long &,long &,const char *,int) */
	void rtrim(long o,long &l,const char *s=0) const { trim(o,l,s,aTRIM_RIGHT); }
	/** Un-Trim string, expand string to include space instead of trimming off.
	 * @see void trim(long &,long &,const char *,int) */
	void untrim(long &o,long &l,const char *s=0) const { trim(o,l,s,aTRIM_UNTRIM|aTRIM_BOTH); }
	/** Un-Trim left side of string.
	 * @see void trim(long &,long &,const char *,int) */
	void luntrim(long &o,long l=0,const char *s=0) const { trim(o,l,s,aTRIM_UNTRIM|aTRIM_LEFT); }
	/** Un-Trim right side of string.
	 * @see void trim(long &,long &,const char *,int) */
	void runtrim(long o,long &l,const char *s=0) const { trim(o,l,s,aTRIM_UNTRIM|aTRIM_RIGHT); }
	void clear() { if(str) *str = '\0',len = 0; }
	void free();
	void setCapacity(size_t n);
	void increaseCapacity(size_t n) { resize(n); }

	size_t length() const { return len; }
	size_t capacity() const { return cap; }
	/** @} */

	/** @name Type conversion
	 * @{ */
	char charAt(long i) const;
	const char *toCharArray() const { return str; }
	long toInt() const;
	size_t toIntArray(long *n,char c=',') const;
	/** Hash function, inherited from aObject. */
	virtual hash_t crc32() const { return crc32(str,false); }
	/** Hash function, inherited from aObject. */
	virtual hash_t hash() const { return crc32(str,false); }
	/** @} */

	/** @name Tokenizing
	 * @{ */
	static size_t nextWord(const char **s,const char *c=whitespace);
	/** @} */

	/** @name Static functions
	 * @{ */
	/** Cyclic Redundancy Check
	 * Reads in a string s as a command-line argument, and prints out
	 * its 32 bit Cyclic Redundancy Check (CRC32 or Ethernet / AAL5 or ITU-TSS).
	 * 
	 * Uses direct table lookup.
	 * @param s String to calcualte hash from.
	 * @param c Case insensitive, if true calculates hash based on lower case of the string.
	 * @return Hashvalue. */
	static hash_t crc32(const char *s,bool c);
	/** Generate hash value from a char string.
	 * @param s String to generate hash from.
	 * @param c Case insensitive, if true calculates hash based on lower case of the string.
	 * @return Hashvalue. */
	static hash_t hash(const char *s,bool c) { return crc32(s,c); }
	static char toLower(const char c) { return (c>='A' && c<='Z')? c+32 : c; }
	static char toUpper(const char c) { return (c>='a' && c<='z')? c-32 : c; }
	static int fromHex(char c) { return c>='0' && c<='9'? c-'0' : (c>='a' && c<='f'? c-87 : (c>='A' && c<='F'? c-55 : 0)); }
	static uint64_t fromHex(const char *str);
	static char *toHex(char *h,uint64_t i,bool upper=true);
	static char *toLower(char *str);
	static char *toUpper(char *str);
	/** Returns the character for certain escape sequences without the slash, eg. 'n' for '\n'.
	 * If it's not an escape sequence character, returns c. */
	static char escape(char c) { return c=='\0'? '0' : c=='\t'? 't' : c=='\n'? 'n' : c=='\r'? 'r' : c=='\f'? 'f' : c=='\v'? 'v' : c; }
	/** Returns the escape sequence for certain characters without the slash, eg. '\n' for 'n'.
	 * If it's not an escape sequence character, returns c. */
	static char unescape(char c) { return c=='0'? '\0' : c=='t'? '\t' : c=='n'? '\n' : c=='r'? '\r' : c=='f'? '\f' : c=='v'? '\v' : c; }
	static int stricmp(const char *str1,const char *str2);
	static int strnicmp(const char *str1,const char *str2,size_t n);
	static char *stristr(char *str1,const char *str2);
	static int countTokens(char *str,const char *delim,bool cins=false);
	static char **split(char **list,char *str,const char *delim,bool cins=false);
	static void reverse(char *str,long o=0,long l=0);
	/** Trim a string from white space.
	 * @param str String to trim.
	 * @param s If set should contain a string of chars to trim, default is the static variable whitespace.
	 * @param f Use the flags named aTRIM_*, an exception is that aTRIM_UNTRIM is not implemented. Default is aTRIM_BOTH.
	 * @see void trim(const char *,int) */
	static size_t trim(char *str,const char *s=0,int f=0);
	static void printUTF8(char *d,const char *s,size_t o,size_t l);
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
	static bool isHTMLEntity(unsigned char c);
	static bool isBreak(unsigned char c) { return c=='\n' || c=='\r'; }
	static bool isPunct(unsigned char c) { return isPrint(c) && !isLower(c) && !isUpper(c) && c!=' ' && !isDigit(c); }
	static bool isPrint(unsigned char c) { return c>='\x20' && c<='\x7e'; }
	static bool isGraph(unsigned char c) { return c>='\x21' && c<='\x7e'; }
	static bool isCntrl(unsigned char c) { return c<='\x1F' || c=='\x7f'; }
	static bool isQuote(char c) { return c=='\'' || c=='"'; }
	static bool isUTF8(unsigned char c) { return (c&0x80)==0x80; }
	/** @} */
};


#endif /* _AMANITA_STRING_H */

