#ifndef _AMANITA_PROPERTIES_H
#define _AMANITA_PROPERTIES_H

/**
 * @file amanita/aProperties.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-11
 * @date Created: 2004-09-30
 */ 

#include <string.h>
#include <stdlib.h>
#include <amanita/aCollection.h>


/** @cond */
class aString;
/** @endcond */


/** A properties files parser.
 * 
 * This parser reads typical .ini files and stores in a linked list to maintain index order and a
 * hashtable for fast access to key/value-pairs.
 * @ingroup amanita */
class aProperties : protected aCollection {

/** @cond */
aObject_Instance(aProperties)
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
		node(const char *k,value_t v,type_t vt,const char *i,const char *d,char q,const char *c);
		~node();
	};
	node *first;
	node *last;
	node **table;
	size_t full;
	style_t style;

	void getKey(aString &s,const char *section,const char *key);

	/** Rehash the entire table.
	 * This is done automatically when the hashtable reach a certain fullness compared to
	 * it's capacity, i.e. it's load. The load is defined by the ld-member and can be set
	 * manually. Default value of ld is 0.5, meaning it doubles in size when it grows
	 * (cap/ld).
	 * @param st Rehashing style. */
	void rehash(style_t st);

	void putNode(node *n);
	node *getNode(const char *key);
	void addNode(node *n);

public:

	/** @name Constructos and Destructors
	 * @{ */
	aProperties(style_t st=0);
	virtual ~aProperties();
	/** @} */

	void set(const char *section,const char *key,const char *value);
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
	size_t load(aString &data);
	size_t save(const char *fn);
	size_t save(FILE *fp);
	/** @} */
};


#endif /* _AMANITA_PROPERTIES_H */

