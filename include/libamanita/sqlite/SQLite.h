#ifndef _DB_SQLITE_H
#define _DB_SQLITE_H

#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <libamanita/Vector.h>


class SQLite;


class Record {
private:
	char **data;
	size_t sz;

public:
	Record(char **d,unsigned int s);
	~Record();

	int size() { return sz; }
	char *getString(int n) { return data[n]; }
	int getInt(int n) { return atoi(data[n]); }
	float getFloat(int n) { return (float)atof(data[n]); }
	double getDouble(int n) { return atof(data[n]); }
	long long getLongLong(int n) { return atoll(data[n]); }
};


class ResultSet : public Vector {
private:
	Record *cols;

public:
	friend class SQLite;

	ResultSet() : Vector(),cols(0) {}
	ResultSet(int n) : Vector(n),cols(0) {}
	~ResultSet();
};


class SQLite {
private:
	char file[256];
	sqlite3 *db;

	static int callback(void *source,int argc,char **argv,char **azColName);
	void open(const char *nm);
	void close();

public:
	SQLite(const char *nm);
	~SQLite();

	void openDatabaseFile(const char *nm);
	void query(char *sql,ResultSet *rs=0);
	long long lastInsertID() { return sqlite3_last_insert_rowid(db); }
};


#endif
