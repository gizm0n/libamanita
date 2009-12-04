#ifndef _LIBAMANITA_SQLITE_RESULTSET_H
#define _LIBAMANITA_SQLITE_RESULTSET_H

/**
 * @file libamanita/sqlite/aResultset.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 


#include <libamanita/aVector.h>
#include <libamanita/sqlite/aRecord.h>


class aSQLite;


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


#endif /* _LIBAMANITA_SQLITE_RESULTSET_H */

