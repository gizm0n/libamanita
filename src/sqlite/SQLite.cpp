
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <libamanita/sqlite/SQLite.h>


Record::Record(char **d,unsigned int s) {
	if(d && s) {
		data = new char*[s];
		sz = s;
		for(size_t i=0,l=0; i<sz; i++) {
			l = strlen(d[i])+1,data[i] = new char[l];
			memcpy(data[i],d[i],l);
		}
	} else data = 0,sz = 0;
}

Record::~Record() {
	if(data && sz) {
		for(size_t i=0; i<sz; i++) delete[] data[i];
		delete[] data;
		data = 0,sz = 0;
	}
}


ResultSet::~ResultSet() {
	if(cols) { delete cols;cols = 0; }
	for(int i=size()-1; i>=0; i--) delete (Record *)(*this)[i];
}




int SQLite::callback(void *source,int argc,char **argv,char **azColName) {
	if(source) {
		ResultSet &rs = *(ResultSet *)source;
		if(rs.size()==0) rs.cols = new Record(azColName,argc);
		rs += new Record(argv,argc);
	}
	return 0;
}

SQLite::SQLite(const char *nm) : db(0) { open(nm); }
SQLite::~SQLite() { close(); }

void SQLite::open(const char *nm) {
	*file = 0;
	if(sqlite3_open(nm,&db)) fprintf(stderr,"SQLite: Can't open database '%s'\n",sqlite3_errmsg(db));
	else strcpy(file,nm);
}

void SQLite::close() {
fprintf(stdout,"SQLite::close()\n");
fflush(stdout);
	if(db) { sqlite3_close(db);db = 0; }
}

void SQLite::openDatabaseFile(const char *nm) {
	if(*file && !strcmp(file,nm)) return;
	close();
	open(nm);
}

void SQLite::query(char *sql,ResultSet *rs) {
fprintf(stdout,"SQLite::query(sql=%s)\n",sql);
fflush(stdout);
	int msg = 0;
	char *err = 0;
	if(db && (msg=sqlite3_exec(db,sql,rs? callback : 0,rs,&err))!=SQLITE_OK) {
		if(err) {
			fprintf(stderr,"SQLite: Error - %d, '%s'\n",msg,err);
			sqlite3_free(err);
		} else fprintf(stderr,"SQLite: Error - %d\n",msg);
	}
fprintf(stdout,"SQLite::query(done)\n");
fflush(stdout);
}
