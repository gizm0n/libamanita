
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <amanita/db/Database.h>

int main(int argc, char *argv[]) {
	int i,j,exists = 0;
	const char *fn = "database.db";
	FILE *fp = fopen(fn,"rb");
	if(fp) {
		exists = 1;
		fclose(fp);
	} else perror(fn);

	Database db(fn);
	Result rs;
	Record *r;

	if(!exists) {
		time_t t = time(0);
		char str[65];
		db.execf("create table example (id INTEGER PRIMARY KEY,data TEXT,created DATE);");
		for(i=0; i<20; ++i) {
			sprintf(str,"text %d",i);
			db.execf("insert into example values(null,'%s',%lu);",str,t);
		}
	}

	db.queryf(rs,"select * from example;");
	if(rs.rows()) {
		r = rs.getColumns();
		for(j=0; j<r->columns(); ++j) fprintf(stdout,"%-12s",r->getColumn(j));
		fprintf(stdout,"\n");
		for(j=0; j<r->columns(); ++j) fprintf(stdout,"----------- ");
		fprintf(stdout,"\n");
		for(i=0; i<(int)rs.rows(); ++i) {
			r = rs.getRow(i);
			for(j=0; j<r->columns(); ++j) fprintf(stdout,"%-12s",r->getColumn(j));
			fprintf(stdout,"\n");
		}
		fflush(stdout);
	}
	return 0;
}
