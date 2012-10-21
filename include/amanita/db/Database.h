#ifndef _AMANITA_DB_DATABASE_H
#define _AMANITA_DB_DATABASE_H

/**
 * @file amanita/db/Database.h
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <amanita/db/Record.h>
#include <amanita/db/Result.h>


/** @cond */
struct sqlite3;
/** @endcond */


/** Amanita Namespace */
namespace a {


/** This class opens a SQLite database and executes queries to the database.
 * 
 * @ingroup db */
class Database {
private:
	char file[256];
	sqlite3 *db;

	void open(const char *nm);
	void close();

	void error(int msg,char *err);

public:
	Database(const char *path,const char *nm=0);
	~Database();

	void exec(const char *sql);
	void execf(const char *sql, ...);
	void query(Result &rs,const char *sql);
	void queryf(Result &rs,const char *sql, ...);
	long long lastInsertID();
};

}; /* namespace a */



#endif /* _AMANITA_DB_DATABASE_H */
