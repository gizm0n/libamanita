
#include "../_config.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sqlite3.h>
#include <amanita/sqlite/aSQLite.h>


int aSQLite::callback(void *source,int argc,char **argv,char **azColName) {
	if(source) {
		aResultset &rs = *(aResultset *)source;
		if(rs.size()==0) rs.cols = new aRecord(rs,azColName,argc);
		rs += new aRecord(rs,argv,argc);
	}
	return 0;
}

aSQLite::aSQLite(const char *path,const char *nm) : db(0) {
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
		} else fprintf(stderr,"aSQLite: Can't open database '%s/%s.db'\n",path,nm);
	} else if(path) open(path);
	else if(nm) open(nm);
}

aSQLite::~aSQLite() { close(); }

void aSQLite::open(const char *nm) {
	*file = 0;
	if(sqlite3_open(nm,&db)) fprintf(stderr,"aSQLite: Can't open database '%s'.\nError message:'%s'\n",nm,sqlite3_errmsg(db));
	else strcpy(file,nm);
}

void aSQLite::close() {
debug_output("aSQLite::close()\n");
	if(db) { sqlite3_close(db);db = 0; }
}

void aSQLite::error(int msg,char *err) {
	if(err) {
		fprintf(stderr,"aSQLite: Error - %d, '%s'\n",msg,err);
		sqlite3_free(err);
	} else fprintf(stderr,"aSQLite: Error - %d\n",msg);
}

void aSQLite::exec(const char *sql) {
debug_output("aSQLite::exec(sql=%s)\n",sql);
	int msg = 0;
	char *err = 0;
	if(db && (msg=sqlite3_exec(db,sql,0,0,&err))!=SQLITE_OK) error(msg,err);
debug_output("aSQLite::exec(done)\n");
}

void aSQLite::execf(const char *sql, ...) {
	char buf[2049];
	va_list args;
   va_start(args,sql);
	vsnprintf(buf,2048,sql,args);
   va_end(args);
	exec(buf);
}

void aSQLite::query(aResultset &rs,const char *sql) {
debug_output("aSQLite::query(sql=%s)\n",sql);
	int msg = 0;
	char *err = 0;
	rs.db = this;
	if(db && (msg=sqlite3_exec(db,sql,callback,&rs,&err))!=SQLITE_OK) error(msg,err);
debug_output("aSQLite::query(done)\n");
}

void aSQLite::queryf(aResultset &rs,const char *sql, ...) {
	char buf[2049];
	va_list args;
   va_start(args,sql);
	vsnprintf(buf,2048,sql,args);
   va_end(args);
	query(rs,buf);
}


long long aSQLite::lastInsertID() {
	return sqlite3_last_insert_rowid(db);
}

