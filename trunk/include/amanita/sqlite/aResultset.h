#ifndef _AMANITA_SQLITE_RESULTSET_H
#define _AMANITA_SQLITE_RESULTSET_H

/**
 * @file amanita/sqlite/aResultset.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 


#include <amanita/aVector.h>
#include <amanita/sqlite/aRecord.h>


class aSQLite;


/** Resultset.
 * @ingroup sqlite
 */
class aResultset : protected aVector {
private:
	aSQLite *db;
	aRecord *cols;

public:
	friend class aSQLite;

	aResultset(int n=0) : aVector(n),db(0),cols(0) {}
	~aResultset();

	aRecord *getCols() { return cols; }
	size_t rows() { return size(); }
	aRecord *getRow(size_t i) { return (aRecord *)(*this)[i]; }
};


#endif /* _AMANITA_SQLITE_RESULTSET_H */

