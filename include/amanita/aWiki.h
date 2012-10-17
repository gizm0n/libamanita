#ifndef _AMANITA_WIKI_H
#define _AMANITA_WIKI_H

/**
 * @file amanita/aWiki.h
 * @author Per Löwgren
 * @date Modified: 2012-03-13
 * @date Created: 2012-03-01
 */ 

#include <amanita/aString.h>
#include <amanita/aHashtable.h>
#include <amanita/aVector.h>

enum {
	aWIKI_ARTICLE,
	aWIKI_FILE,
	aWIKI_IMAGE,
	aWIKI_DOCUMENT,
	aWIKI_ARCHIVE,
};

enum {
	aWIKI_SECTIONS				= 0x00000010,
	aWIKI_SMILEYS				= 0x00000020,
	aWIKI_TAGS					= 0x00000040,
	aWIKI_LINKS					= 0x00000080,
	aWIKI_CREOLE				= 0x00001000,
	aWIKI_BBCODE				= 0x00002000,
};

enum {
	aWIKI_IMAGE_FRAME			= 0x00000001,
	aWIKI_IMAGE_THUMB			= 0x00000002,
	aWIKI_ALIGN_LEFT			= 0x00000010,
	aWIKI_ALIGN_CENTER		= 0x00000020,
	aWIKI_ALIGN_RIGHT			= 0x00000040,
};

enum {
	aWIKI_TAG_SIMPLE			= 0x00000001,
	aWIKI_TAG_NESTED			= 0x00000001,
};


class aWiki;
class aWikiParams;
class aWikiLink;
struct aWikiTagHandler;
struct aWikiLinkHandler;


typedef void (*wiki_callback)(aWikiParams &p);
typedef void (*wiki_link_callback)(aWiki &w,aWikiLink &link);
typedef void (*wiki_smiley_callback)(aWiki &w,int index);


class aWikiData {
public:
	aWiki *wiki;
	aVector ref;

	char *target;
	char *base_url;

	wiki_callback template_callback;

	aHashtable tag_handlers;
	aHashtable link_handlers;

	char **primary_smileys;
	char **secondary_smileys;
	wiki_smiley_callback smiley_callback;

	aWikiData(aWiki &w);
	~aWikiData();

	void setTagHandlers(const aWikiTagHandler *th);
	aWikiTagHandler *getTagHandler(const char *tag);
	void setLinkHandlers(const aWikiLinkHandler *lh);
	aWikiLinkHandler *getLinkHandler(const char *nmsp);
	void clearSmileys();
	void setSmileys(const char **s,wiki_smiley_callback cb);
};

class aWikiParams {
public:
	aWiki *wiki;
	aWikiData *data;
	aString *text;
	int32_t first;
	int32_t start;
	int32_t offset;
	int32_t length;
	int32_t end;
	char tag[32];
	int32_t param;
	int32_t param_length;
	wiki_callback func;
	char replace[8];

	aWikiParams(aWiki &w,aWikiData &d,aString &t);
};

class aWikiLink {
public:
	aWiki *wiki;
	char *link;
	char *nmsp;
	char *name;
	char *parenthesis;
	char *section;
	char *title;
	wiki_link_callback func;

	aWikiLink();
	~aWikiLink();

	void clear();
	void parse(aWikiData &data,char *text);
	static char *getName(const char *name);
	void write(aWiki &w,const char *url);
};

class aWikiFile {
public:
	aWikiLink *link;
	char *path;
	char *ext;
	int type;
	char *name;
	int style;
	float upright;
	int width;
	int height;
	char *href;
	char *alt;
	char *text;
	int image_width;
	int image_height;

	aWikiFile(aWikiLink &l,const char *dir[]);
	~aWikiFile();
	void write(aWiki &w,const char *url[]);
};

struct aWikiTagHandler {
	const char *tag;
	uint32_t style;
	wiki_callback func;
};

struct aWikiLinkHandler {
	const char *nmsp;
	wiki_link_callback func;
};



/** Parse and format WikiText to HTML-code.
 * 
 * This class inherits the aString-class and makes use of its methods to parse
 * and format WikiText into HTML-code.
 * 
 * In this example a page is read from a database file, formatted to html and written to a file:
 * @code
#include <stdio.h>
#include <amanita/aWiki.h>
#include <amanita/db/aDatabase.h>

int main(int argc, char *argv[]) {
	const char *page = "WikiPage";
	aResult r;
	aDatabase db("./wiki.db");
	db.queryf(r,"select page from wiki_table where name='%s';",page);
	if(r.rows()) {
		FILE *fp;
		aWiki wiki;
		wiki << r.getRow(0)->getColumn(0);
		wiki.format(page,aWIKI_SECTIONS);
		fp = fopen("page.html","wb");
		fwrite(wiki.toCharArray(),wiki.length(),1,fp);
		fclose(fp);
	}
	return 0;
}
 * @endcode
 * 
 * For a more in deapth example of how to use the aWiki class, see:
 * examples/wiki.cpp
 * @ingroup amanita */
class aWiki : public aString {
protected:
	uint32_t flags;		//!< Flags for how to format the text.
	aWiki *owner;			//!< Owner of this aWiki-object.
	aWikiData *data;		//!< Data used in formatting.

	/** @name Format
	 * @{ */
	size_t formatSections(aVector &lines,size_t ln,int lvl,char *ind,int f);
	size_t formatPre(aVector &lines,size_t ln);
	size_t formatTable(aVector &lines,size_t ln,int lvl,char *ind);
	void formatTableStyle(aString &style,char *line);
	size_t formatList(aVector &lines,size_t ln,int lvl,char *ind,char *lists);
	size_t formatListRow(aVector &lines,size_t ln,int lvl,char *ind,char *lists);
	/** @} */

	/** @name Match
	 * @{ */
	void matchTemplate(aWikiParams &p);
	void matchURL(aWikiParams &p);
	void matchWikiTag(aWikiParams &p);
	void matchWikiLink(aWikiParams &p);
	void matchBBTag(aWikiParams &p);
	/** @} */

public:

	/** @name Constructors & Destructors
	 * @{ */
	aWiki(size_t c=0);
	aWiki(aWiki &w,size_t c=0);
	~aWiki();
	/** @} */

	/** @name Set data
	 * @{ */
	void setTarget(const char *target);
	void setBaseURL(const char *url);
	void setTemplateHandler(wiki_callback cb) { data->template_callback = cb; }
	void setTagHandlers(const aWikiTagHandler *th) { data->setTagHandlers(th); }
	void setLinkHandlers(const aWikiLinkHandler *lh) { data->setLinkHandlers(lh); }
	void setSmileys(const char **s,wiki_smiley_callback cb) { data->setSmileys(s,cb); }
	/** @} */

	/** @name Format
	 * @{ */
	void format(aString *text,uint32_t f) { if(text) format(text->toCharArray(),f); }
	void format(aString &text,uint32_t f) { format(text.toCharArray(),f); }
	void format(const char *text,uint32_t f);
	void format(uint32_t f);

	void formatSections(aString &text);

	void formatInline(const char *text);
	void formatInline(aString &text,bool pg=false,long o=0,long l=0);
	/** @} */

	/** @name References
	 * @{ */
	void addRef(const char *ref);
	void storeRefs(aString &text);
	void injectRefs(aString &text);
	/** @} */
};



#endif /* _AMANITA_WIKI_H */


