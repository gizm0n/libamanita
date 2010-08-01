#ifndef _LIBAMANITA_REGEX_H
#define _LIBAMANITA_REGEX_H

/**
 * @file libamanita/aRegex.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-03
 * @date Created: 2004-12-30
 */ 


#include <stdint.h>
#include <libamanita/aString.h>


enum REGEX_MODIFIER {
	REGEX_GLOBAL				= 0x00000001,		//!< g-modifier, global mode.
	REGEX_CASELESS				= 0x00000002,		//!< i-modifier, caseless mode.
	REGEX_MULTILINE			= 0x00000004,		//!< m-modifier, multiline mode.
	REGEX_DOTALL				= 0x00000004,		//!< s-modifier, singleline mode.
	REGEX_EXTENDED				= 0x00000004,		//!< x-modifier, freespacing mode.
};


class REBlock;
class REMatch;


/** A simple class for compiling regex code and performing text searches.
 * 
 * To this point there are no plans to make support for UTF-8 or any other formats.
 * It works fine to make searches in UTF-8 formatted text, characters larger than
 * one byte are then simply ignored. It might not work so well with UTF-8 in the
 * regex-code.
 * 
 * Some examples on how to use this class:
 * @code
#include <stdlib.h>
#include <stdio.h>
#include <libamanita/aRegex.h>

int main(int argc, char *argv[]) {
	aRegex r;

	r.match("abc \"def\" \"ghi\" jkl","\".*\"","gi");
	r.print();
	r.match("abc \"def\" \"ghi\" jkl","\".*?\"","gi");
	r.print();
	r.match("abc \"def\" \"ghi\" jkl","def??","gi");
	r.print();
	r.match("abc\ndef","^.","gi");
	r.print();
	r.match("abc\ndef","^.","gim");
	r.print();
	r.match("abc name@host.com def another.name@host.com ghi",
		"\\b([a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,4})\\b","gi");
	r.print();
	r.replace("abc name@host.com def another.name@host.com ghi",
		"\\b([a-z0-9._%+-]{3}@[a-z0-9.-]+\\.[a-z]{2,4})\\b","<a href=\"mailto:\\1\">\\1</a>","gi");
	r.print();
	r.replace("Abc regular expression def regex ghi regexp. Jkl, mno \"regexes\". Pqr, regular expressssions stu.",
		"(reg)(ular\\s)?ex(p|es)?(res+ions?)?","[\\1ex\\3]","gi");
	r.print();
	r.replace("abc 12.34563e+12 def 7865.123 jkl 1738.3680e-23 pqr .123e+12 stu vwx yz.",
		"([0-9]*)\\.[0-9]+([Ee](\\+|-)?[0-9]+)?","int(\\1 \"\\0\")","gi");
	r.print();
	r.replace("abc def ghi jkl mno pqr stu vwx yzabc def ghi jkl mno pqr stu vwx yz",
		"([a-z]{3}\\s+)*","abc(\\1 \"\\0\")","gi");
	r.print();
	r.replace("abc def ghi jkl mno pqr stu vwx yzabc def ghi jkl mno pqr stu vwx yz",
		"([a-z]{3}\\s+){3,4}","abc(\\1 \"\\0\")","gi");
	r.print();
	r.match("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
		"abc|def|xyz","g");
	r.print();
	r.match("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
		"abc(def|xyz)","g");
	r.print();
	r.replace("-{ \"#31\",---3 },...// 31","\"\\#(\\d+)\"","[\\1]\\0","g");
	r.print();
	r.match("abcxxxxxxxxxxyabc","(xx)+y","g");
	r.print();

	exit(0);
	return 0;
}
 * @endcode
 * 
 * To compile, save the code above in "regex.cpp", open a teminal and run:
 * g++ -o regex regex.cpp -lamanita
 * 
 * The simplicity with this class is that it makes very fast searches, almost as quick as strstr() in string.h,
 * and still can handle most of the standard regex operators.
 * 
 * If you are not familiar with regex-code, there is a good article about it on Wikipedia:
 * http://en.wikipedia.org/wiki/Regular_expression
 * 
 * 
 */
class aRegex {
friend class REBlock;
friend class REMatch;

private:
	uint32_t mod;				//!< Modifiers as flags.
	uint32_t flags;			//!< Flags.
	uint32_t depth;			//!< Depth of expression.
	uint32_t found;			//!< Number of found matches.
	uint32_t refs;				//!< Number of backreferences.
	uint32_t size;				//!< Size of text.
	char *exp;					//!< Expression.
	char *mods;					//!< Modifiers.
	char *data;					//!< Loaded data from file.
	char **strings;			//!< Backreferences as strings.
	const char *text;			//!< Text to be searched.
	REBlock *blocks;			//!< Compiled blocks of expression.
	REMatch *hits;				//!< Linked list of matches.
	REMatch **list;			//!< Vector containing all the matches.

	/** Convert char depending on case-mode.
	 * @param c Char to convert.
	 * @return Depending on case mode, either c, or c to lower case. */
	char ic(char c) { return (mod&REGEX_CASELESS)? aString::toLower(c) : c; }

	/** Clear the search, to prepare for another search. */
	void clear() { clearBlocks();clearMatches(); }

	/** Clear the compiled expression. */
	void clearBlocks();

	/** Clear found matches. */
	void clearMatches();

	/** Compile regex expression.
	 * @param e The expression.
	 * @param m Modifiers.
	 * @return Number of blocks compiled. */
	int compile(const char *e,const char *m);

	/** Search a text.
	 * @param t Text to search.
	 * @return Found matches. */
	int search(const char *t);

	/** Replace matches in a text.
	 * @param t Text to search.
	 * @param r String to replace found matches with.
	 * @return The formatted text. */
	const char *replace(const char *t,const char *r);

public:
	aRegex();		//!< Constructor.
	~aRegex();		//!< Destructor.

	/** Search for a match.
	 * @param t Text to search.
	 * @param e Regex expression.
	 * @param m Modifiers, optional.
	 * @return Number of found matches. */
	int match(const char *t,const char *e,const char *m=0);

	/** Replace matches.
	 * @param t Text to search.
	 * @param e Regex expression.
	 * @param r String to replace found matches with.
	 * @param m Modifiers, optional.
	 * @return The formatted text. */
	const char *replace(const char *t,const char *e,const char *r,const char *m=0);

	/** Print data about the compiled expression and found matches.
	 * Actually only interesting for developers of this class, to see how it works. */
	void print();

	void setText(const char *t);							//!< Set search text.
	const char *getText() { return text; }				//!< Get search text.
	long getTextLength() { return size; }				//!< Get length of text in bytes.
	unsigned long matches() { return found; }			//!< Get number of matches.
	const char **getMatches();								//!< Get found matches as char strings.
	const char *getMatch(int n);							//!< Get a match where n is the index of the match, staring with 0.
	unsigned long backrefs() { return refs; }			//!< Get number of backrefs.
	const char *getBackRef(int n,int r);				//!< Get a backref as a char string, where n is index in the table, and r is the number of the backref.
};


#endif /* _LIBAMANITA_REGEX_H */

