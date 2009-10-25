#ifndef _LIBAMANITA_SQLITE_RESULTSET_H
#define _LIBAMANITA_SQLITE_RESULTSET_H


#include <libamanita/Vector.h>
#include <libamanita/sqlite/Record.h>


class SQLite;


class ResultSet : protected Vector {
private:
	SQLite *db;
	Record *cols;

public:
	friend class SQLite;

	ResultSet(int n=0) : Vector(n),db(0),cols(0) {}
	~ResultSet();

	Record *getCols() { return cols; }
	size_t rows() { return size(); }
	Record *getRow(size_t i) { return (Record *)(*this)[i]; }
};


#endif /* _LIBAMANITA_SQLITE_RESULTSET_H */

