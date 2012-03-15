#ifndef _AMANITA_DB_RESULT_H
#define _AMANITA_DB_RESULT_H

/**
 * @file amanita/db/aResult.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-01
 * @date Created: 2008-09-07
 */ 


#include <amanita/aVector.h>


class aDatabase;
class aRecord;


/** Resultset.
 * @ingroup sqlite
 */
class aResult : protected aVector {
friend class aDatabase;

/** @cond */
aObject_Instance(aResult)
/** @endcond */

private:
	aDatabase *db;
	aRecord *cols;

public:
	aResult(int n=0) : aVector(n),db(0),cols(0) {}
	virtual ~aResult();

	void clear();
	void addRecord(int n,char **a,char **c);

	aRecord *getColumns() { return cols; }
	size_t rows() { return size(); }
	aRecord *getRow(size_t i) { return (aRecord *)(*this)[i]; }

	aVector::iterator iterate() { return aVector::iterate(); }
};


#endif /* _AMANITA_DB_RESULT_H */

