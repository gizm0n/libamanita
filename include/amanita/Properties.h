#ifndef _AMANITA_PROPERTIES_H
#define _AMANITA_PROPERTIES_H

/**
 * @file amanita/Properties.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-18
 * @date Created: 2012-10-12
 */ 

#include <string.h>
#include <stdlib.h>
#include <amanita/Collection.h>
#include <amanita/String.h>


enum {
	aPROP_STYLE_CASE_INSENSITIVE	= 0x01,	//!< 
	aPROP_STYLE_STORE_COMMENTS		= 0x02,	//!< 
	aPROP_STYLE_VALUE_COMMENTS		= 0x04,	//!< 
	aPROP_STYLE_KEY_INDENT			= 0x08,	//!< 
	aPROP_STYLE_STORE_DELIM			= 0x10,	//!< 
	aPROP_STYLE_SECTIONS				= 0x20,	//!< 
	aPROP_STYLE_DEFAULT				= 0xfe	//!< Default style for Properties objects.
};


/** A properties-file parser.
 * 
 * This parser reads typical .ini, .cfg and .properties files and stores in a
 * linked list to maintain index order and a hashtable for fast access to
 * key/value-pairs.
 * @ingroup amanita */
class Properties : protected Collection {

/** @cond */
Object_Instance(Properties)
/** @endcond */

protected:
	struct node {
		char *key;
		hash_t hash;
		value_t value;
		type_t v_type;
		node *list;
		node *table;
		char *ind;
		char *delim;
		char quot;
		char *comment;
		node(const char *k,const char *v,const char *i,const char *d,char q,const char *c);
		~node();
		void setValue(const char *v);
	};
	node *first;
	node *last;
	node **table;
	size_t full;
	int lang;
	style_t style;
	const char *cc;
	const char *kec;
	const char *kdc;

	void getKey(String &s,const char *section,const char *key);

	/** Rehash the entire table.
	 * This is done automatically when the hashtable reach a certain fullness compared to
	 * it's capacity, i.e. it's load. The load is defined by the ld-member and can be set
	 * manually. Default value of ld is 0.5, meaning it doubles in size when it grows
	 * (cap/ld).
	 * @param st Rehashing style. */
	void rehash(style_t st);

	void addNode(node *n);
	void putNode(node *n);
	node *removeNode(const char *key);
	node *getNode(const char *key);

public:

	/** @name Constructos and Destructors
	 * @{ */
	Properties(int l=LANG_CFG,style_t st=aPROP_STYLE_DEFAULT);
	virtual ~Properties();
	/** @} */

	void set(const char *key,const char *value) { set(0,key,value); }
	void set(const char *section,const char *key,const char *value);
	void remove(const char *key) { remove(0,key); }
	void remove(const char *section,const char *key);
	value_t get(const char *key) { type_t type;return get(0,key,type); }
	value_t get(const char *key,type_t &type);
	value_t get(const char *section,const char *key) { type_t type;return get(section,key,type); }
	value_t get(const char *section,const char *key,type_t &type);
	const char *getString(const char *key) { type_t type;return (const char *)get(0,key,type); }
	const char *getString(const char *section,const char *key) { type_t type;return (const char *)get(section,key,type); }

	/** @name Input and Output methods
	 * @{ */
	size_t print(const char *fn);
	size_t print(FILE *fp=stdout);
	size_t printTable(FILE *fp=stdout);
	size_t load(const char *fn);
	size_t load(FILE *fp);
	size_t load(String &data);
	size_t save(const char *fn);
	size_t save(FILE *fp);
	/** @} */
};


#endif /* _AMANITA_PROPERTIES_H */

