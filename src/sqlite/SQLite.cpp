
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sqlite3.h>
#include <libamanita/sqlite/SQLite.h>


int SQLite::callback(void *source,int argc,char **argv,char **azColName) {
	if(source) {
		ResultSet &rs = *(ResultSet *)source;
		if(rs.size()==0) rs.cols = new Record(rs,azColName,argc);
		rs += new Record(rs,argv,argc);
	}
	return 0;
}

SQLite::SQLite(const char *path,const char *nm) : db(0) {
	if(path && nm) {
		int l1 = strlen(path),l2 = strlen(nm);
		char str[l1+l2+5];
		*str = '\0';
		if(l1 && l2) {
			strncpy(str,path,l1);
			if(str[l1-1]!='/') str[l1++] = '/';
			strncpy(&str[l1],nm,l2);
			strncpy(&str[l1+l2],".db",4);
			open(str);
		} else fprintf(stderr,"SQLite: Can't open database '%s/%s.db'\n",path,nm);
	} else if(path) open(path);
	else if(nm) open(nm);
}

SQLite::~SQLite() { close(); }

void SQLite::open(const char *nm) {
	*file = 0;
	if(sqlite3_open(nm,&db)) fprintf(stderr,"SQLite: Can't open database '%s'.\nError message:'%s'\n",nm,sqlite3_errmsg(db));
	else strcpy(file,nm);
}

void SQLite::close() {
fprintf(stdout,"SQLite::close()\n");
fflush(stdout);
	if(db) { sqlite3_close(db);db = 0; }
}

void SQLite::error(int msg,char *err) {
	if(err) {
		fprintf(stderr,"SQLite: Error - %d, '%s'\n",msg,err);
		sqlite3_free(err);
	} else fprintf(stderr,"SQLite: Error - %d\n",msg);
}

void SQLite::exec(const char *sql) {
fprintf(stdout,"SQLite::exec(sql=%s)\n",sql);
fflush(stdout);
	int msg = 0;
	char *err = 0;
	if(db && (msg=sqlite3_exec(db,sql,0,0,&err))!=SQLITE_OK) error(msg,err);
fprintf(stdout,"SQLite::exec(done)\n");
fflush(stdout);
}

void SQLite::execf(const char *sql, ...) {
	char buf[2049];
	va_list args;
   va_start(args,sql);
	vsnprintf(buf,2048,sql,args);
   va_end(args);
	exec(buf);
}

void SQLite::query(ResultSet &rs,const char *sql) {
fprintf(stdout,"SQLite::query(sql=%s)\n",sql);
fflush(stdout);
	int msg = 0;
	char *err = 0;
	rs.db = this;
	if(db && (msg=sqlite3_exec(db,sql,callback,&rs,&err))!=SQLITE_OK) error(msg,err);
fprintf(stdout,"SQLite::query(done)\n");
fflush(stdout);
}

void SQLite::queryf(ResultSet &rs,const char *sql, ...) {
	char buf[2049];
	va_list args;
   va_start(args,sql);
	vsnprintf(buf,2048,sql,args);
   va_end(args);
	query(rs,buf);
}


long long SQLite::lastInsertID() {
	return sqlite3_last_insert_rowid(db);
}

