#ifndef _AMANITA_SQLITE_SQLITE_H
#define _AMANITA_SQLITE_SQLITE_H

/**
 * @file amanita/sqlite/aSQLite.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/sqlite/aResultset.h>


struct sqlite3;


/** This class opens a SQLite database and executes queries to the database.
 * @ingroup sqlite
 */
class aSQLite {
private:
	char file[256];
	sqlite3 *db;

	static int callback(void *source,int argc,char **argv,char **azColName);
	void open(const char *nm);
	void close();

	void error(int msg,char *err);

public:
	aSQLite(const char *path,const char *nm=0);
	~aSQLite();

	void exec(const char *sql);
	void execf(const char *sql, ...);
	void query(aResultset &rs,const char *sql);
	void queryf(aResultset &rs,const char *sql, ...);
	long long lastInsertID();
};


#endif /* _AMANITA_SQLITE_SQLITE_H */
