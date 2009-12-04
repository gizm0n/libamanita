#ifndef _LIBAMANITA_SQLITE_SQLITE_H
#define _LIBAMANITA_SQLITE_SQLITE_H

/**
 * @file libamanita/sqlite/aSQLite.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <libamanita/sqlite/aResultset.h>


struct sqlite3;


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


#endif /* _LIBAMANITA_SQLITE_SQLITE_H */
