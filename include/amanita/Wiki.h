#ifndef _AMANITA_WIKI_H
#define _AMANITA_WIKI_H

/**
 * @file amanita/Wiki.h
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2012-03-01
 */ 

#include <amanita/String.h>
#include <amanita/Hashtable.h>
#include <amanita/Vector.h>


/** Amanita Namespace */
namespace a {

enum {
	WIKI_ARTICLE,
	WIKI_FILE,
	WIKI_IMAGE,
	WIKI_DOCUMENT,
	WIKI_ARCHIVE,
};

enum {
	WIKI_SECTIONS				= 0x00000010,
	WIKI_SMILEYS				= 0x00000020,
	WIKI_TAGS					= 0x00000040,
	WIKI_LINKS					= 0x00000080,
	WIKI_CREOLE					= 0x00001000,
	WIKI_BBCODE					= 0x00002000,
};

enum {
	WIKI_IMAGE_FRAME			= 0x00000001,
	WIKI_IMAGE_THUMB			= 0x00000002,
	WIKI_ALIGN_LEFT			= 0x00000010,
	WIKI_ALIGN_CENTER			= 0x00000020,
	WIKI_ALIGN_RIGHT			= 0x00000040,
};

enum {
	WIKI_TAG_SIMPLE			= 0x00000001,
	WIKI_TAG_NESTED			= 0x00000001,
};


/** @cond */
class Wiki;
class WikiParams;
class WikiLink;
struct WikiTagHandler;
struct WikiLinkHandler;
/** @endcond */


typedef void (*wiki_callback)(WikiParams &p);
typedef void (*wiki_link_callback)(Wiki &w,WikiLink &link);
typedef void (*wiki_smiley_callback)(Wiki &w,int index);


class WikiData {
public:
	Wiki *wiki;
	Vector ref;

	char *target;
	char *base_url;

	wiki_callback template_callback;

	Hashtable tag_handlers;
	Hashtable link_handlers;

	char **primary_smileys;
	char **secondary_smileys;
	wiki_smiley_callback smiley_callback;

	WikiData(Wiki &w);
	~WikiData();

	void setTagHandlers(const WikiTagHandler *th);
	WikiTagHandler *getTagHandler(const char *tag);
	void setLinkHandlers(const WikiLinkHandler *lh);
	WikiLinkHandler *getLinkHandler(const char *nmsp);
	void clearSmileys();
	void setSmileys(const char **s,wiki_smiley_callback cb);
};

class WikiParams {
public:
	Wiki *wiki;
	WikiData *data;
	String *text;
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

	WikiParams(Wiki &w,WikiData &d,String &t);
};

class WikiLink {
public:
	Wiki *wiki;
	char *link;
	char *nmsp;
	char *name;
	char *parenthesis;
	char *section;
	char *title;
	wiki_link_callback func;

	WikiLink();
	~WikiLink();

	void clear();
	void parse(WikiData &data,char *text);
	static char *getName(const char *name);
	void write(Wiki &w,const char *url);
};

class WikiFile {
public:
	WikiLink *link;
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

	WikiFile(WikiLink &l,const char *dir[]);
	~WikiFile();
	void write(Wiki &w,const char *url[]);
};

struct WikiTagHandler {
	const char *tag;
	uint32_t style;
	wiki_callback func;
};

struct WikiLinkHandler {
	const char *nmsp;
	wiki_link_callback func;
};



/** Parse and format WikiText to HTML-code.
 * 
 * This class inherits the String-class and makes use of its methods to parse
 * and format WikiText into HTML-code.
 * 
 * In this example a page is read from a database file, formatted to html and written to a file:
 * @code
#include <stdio.h>
#include <amanita/Wiki.h>
#include <amanita/db/Database.h>

int main(int argc, char *argv[]) {
	const char *page = "WikiPage";
	aResult r;
	Database db("./wiki.db");
	db.queryf(r,"select page from wiki_table where name='%s';",page);
	if(r.rows()) {
		FILE *fp;
		Wiki wiki;
		wiki << r.getRow(0)->getColumn(0);
		wiki.format(page,WIKI_SECTIONS);
		fp = fopen("page.html","wb");
		fwrite(wiki.toCharArray(),wiki.length(),1,fp);
		fclose(fp);
	}
	return 0;
}
 * @endcode
 * 
 * For a more in deapth example of how to use the Wiki class, see:
 * examples/wiki.cpp
 * 
 * @ingroup amanita */
class Wiki : public String {
/** @cond */
Object_Instance(Wiki)
/** @endcond */

protected:
	uint32_t flags;		//!< Flags for how to format the text.
	Wiki *owner;			//!< Owner of this Wiki-object.
	WikiData *data;		//!< Data used in formatting.

	/** @name Format
	 * @{ */
	size_t formatLines(Vector &lines,size_t ln,int lvl,char *ind,int f);
	size_t formatPre(Vector &lines,size_t ln);
	size_t formatTable(Vector &lines,size_t ln,int lvl,char *ind);
	void formatTableStyle(String &style,char *line);
	size_t formatList(Vector &lines,size_t ln,int lvl,char *ind,char *lists,int l=0);
	size_t formatListRow(Vector &lines,size_t ln,int lvl,char *ind,char *lists,int l);
	/** @} */

	/** @name Match
	 * @{ */
	void matchTemplate(WikiParams &p);
	void matchURL(WikiParams &p);
	void matchWikiTag(WikiParams &p);
	void matchWikiLink(WikiParams &p);
	void matchBBTag(WikiParams &p);
	/** @} */

public:

	/** @name Constructors & Destructors
	 * @{ */
	Wiki(size_t c=0);
	Wiki(Wiki &w,size_t c=0);
	~Wiki();
	/** @} */

	/** @name Set data
	 * @{ */
	void setTarget(const char *target);
	void setBaseURL(const char *url);
	void setTemplateHandler(wiki_callback cb) { data->template_callback = cb; }
	void setTagHandlers(const WikiTagHandler *th) { data->setTagHandlers(th); }
	void setLinkHandlers(const WikiLinkHandler *lh) { data->setLinkHandlers(lh); }
	void setSmileys(const char **s,wiki_smiley_callback cb) { data->setSmileys(s,cb); }
	/** @} */

	/** @name Format
	 * @{ */
	void format(String *text,uint32_t f) { if(text) format(text->toCharArray(),f); }
	void format(String &text,uint32_t f) { format(text.toCharArray(),f); }
	void format(const char *text,uint32_t f);
	void format(uint32_t f);

	void formatSections(String &text);

	void formatInline(const char *text);
	void formatInline(String &text,bool pg=false,long o=0,long l=0);
	/** @} */

	/** @name References
	 * @{ */
	void addRef(const char *ref);
	void storeRefs(String &text);
	void injectRefs(String &text);
	/** @} */
};

}; /* namespace a */




#endif /* _AMANITA_WIKI_H */


