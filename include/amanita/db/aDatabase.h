#ifndef _AMANITA_DB_DATABASE_H
#define _AMANITA_DB_DATABASE_H

/**
 * @file amanita/db/aDatabase.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-01
 * @date Created: 2008-09-07
 */ 


struct sqlite3;
class aResult;


/** This class opens a SQLite database and executes queries to the database.
 * @ingroup db
 */
class aDatabase {
private:
	char file[256];
	sqlite3 *db;

	void open(const char *nm);
	void close();

	void error(int msg,char *err);

public:
	aDatabase(const char *path,const char *nm=0);
	~aDatabase();

	void exec(const char *sql);
	void execf(const char *sql, ...);
	void query(aResult &rs,const char *sql);
	void queryf(aResult &rs,const char *sql, ...);
	long long lastInsertID();
};


#endif /* _AMANITA_DB_DATABASE_H */
