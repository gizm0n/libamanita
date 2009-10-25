
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <libamanita/sqlite/Record.h>
#include <libamanita/sqlite/ResultSet.h>
#include <libamanita/sqlite/SQLite.h>


Record::Record(ResultSet &rs,char **d,unsigned int s) {
	if(d && s) {
		data = (char **)malloc(s*sizeof(char *));
		sz = s;
		for(size_t i=0,l; i<sz; i++) {
			if(d[i]) {
				l = (strlen(d[i])+1)*sizeof(char);
				data[i] = (char *)malloc(l);
				memcpy(data[i],d[i],l);
			} else data[i] = 0;
		}
	} else data = 0,sz = 0;
}

Record::~Record() {
	if(data && sz) {
		for(size_t i=0; i<sz; i++) free(data[i]);
		free(data);
		data = 0,sz = 0;
	}
}


