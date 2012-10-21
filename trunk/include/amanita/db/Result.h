#ifndef _AMANITA_DB_RESULT_H
#define _AMANITA_DB_RESULT_H

/**
 * @file amanita/db/Result.h
 * @author Per Löwgren
 * @date Modified: 2012-03-01
 * @date Created: 2008-09-07
 */ 


#include <amanita/Vector.h>


/** Amanita Namespace */
namespace a {

/** @cond */
class Database;
class Record;
/** @endcond */

/** Resultset.
 * @ingroup sqlite
 */
class Result : protected Vector {
friend class Database;

/** @cond */
Object_Instance(Result)
/** @endcond */

private:
	Database *db;
	Record *cols;

public:
	Result(int n=0) : Vector(n),db(0),cols(0) {}
	virtual ~Result();

	void clear();
	void addRecord(int n,char **a,char **c);

	Record *getColumns() { return cols; }
	size_t rows() { return size(); }
	Record *getRow(size_t i) { return (Record *)(*this)[i]; }

	Vector::iterator iterate() { return Vector::iterate(); }
};

}; /* namespace a */


using namespace a;


#endif /* _AMANITA_DB_RESULT_H */

