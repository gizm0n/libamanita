#ifndef _LIBAMANITA_REGEX_H
#define _LIBAMANITA_REGEX_H


#include <stdint.h>


enum REGEX_MODIFIER {
	REGEX_GLOBAL				= 0x00000001,		// g
	REGEX_CASELESS				= 0x00000002,		// i
	REGEX_MULTILINE			= 0x00000004,		// m
	REGEX_DOTALL				= 0x00000004,		// s
	REGEX_EXTENDED				= 0x00000004,		// x
};


class REBlock;
class REMatch;


class RegEx {
friend class REBlock;
friend class REMatch;

private:
	uint32_t mod,flags,depth,found,refs,size;
	char *exp,*mods,*data,**strings;
	const char *text;
	REBlock *blocks;
	REMatch *hits,**list;

	static char toLower(char c) { return (c>='A' && c<='Z')? c+32 : c; }
	static int fromHex(char c) { return c>='0' && c<='9'? c-'0' : (c>='a' && c<='f'? c-87 : (c>='A' && c<='F'? c-55 : 0)); }
	static bool isLower(char c) { return (c>='a' && c<='z'); }
	static bool isUpper(char c) { return (c>='A' && c<='Z'); }
	static bool isAlpha(unsigned char c) { return isLower(c) || isUpper(c); }
	static bool isDigit(unsigned char c) { return c>='0' && c<='9'; }
	static bool isHex(unsigned char c) { return (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'); }
	static bool isWord(unsigned char c) { return isAlpha(c) || isDigit(c) || c=='_'; }
	static bool isSpace(unsigned char c) { return c==' ' || c=='\n' || c=='\t' || c=='\r' || c=='\f' || c=='\v'; }
	static bool isBreak(unsigned char c) { return c=='\n' || c=='\r'; }
	static bool isPrint(unsigned char c) { return c>='\x20' && c<='\x7e'; }

	char ic(char c) { return (mod&REGEX_CASELESS)? toLower(c) : c; }
	void clear() { clearBlocks();clearMatches(); }
	void clearBlocks();
	void clearMatches();
	int compile(const char *e,const char *m);
	int search(const char *t);
	const char *replace(const char *t,const char *r);

public:
	RegEx();
	~RegEx();

	int match(const char *t,const char *e,const char *m=NULL);
	const char *replace(const char *t,const char *e,const char *r,const char *m=NULL);
	void print();

	void setText(const char *t);
	const char *getText() { return text; }
	long getTextLength() { return size; }
	unsigned long matches() { return found; }
	const char **getMatches();
	const char *getMatch(int n);
	unsigned long backrefs() { return refs; }
	const char *getBackRef(int n,int r);
};


#endif /* _LIBAMANITA_REGEX_H */

