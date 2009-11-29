#ifndef _LIBAMANITA_SQLITE_ARESULTSET_H
#define _LIBAMANITA_SQLITE_ARESULTSET_H


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


#endif /* _LIBAMANITA_SQLITE_ARESULTSET_H */

