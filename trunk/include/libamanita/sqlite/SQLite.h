#ifndef _DB_SQLITE_H
#define _DB_SQLITE_H

#include <libamanita/sqlite/ResultSet.h>


struct sqlite3;


class SQLite {
private:
	char file[256];
	sqlite3 *db;

	static int callback(void *source,int argc,char **argv,char **azColName);
	void open(const char *nm);
	void close();

	void error(int msg,char *err);

public:
	SQLite(const char *path,const char *nm=0);
	~SQLite();

	void exec(const char *sql);
	void execf(const char *sql, ...);
	void query(ResultSet &rs,const char *sql);
	void queryf(ResultSet &rs,const char *sql, ...);
	long long lastInsertID();
};


#endif
