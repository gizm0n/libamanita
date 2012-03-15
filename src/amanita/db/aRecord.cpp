
#include "../_config.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <amanita/db/aRecord.h>
#include <amanita/db/aResult.h>


aRecord::aRecord(aResult &rs,int n,char **d) {
	if(d && n) {
		size_t i,l;
		sz = n;
		data = (char **)malloc(sz*sizeof(char *));
		for(i=0; i<sz; i++) {
			if(d[i]) {
				l = (strlen(d[i])+1)*sizeof(char);
				data[i] = (char *)malloc(l);
				memcpy(data[i],d[i],l);
			} else data[i] = 0;
		}
	} else data = 0,sz = 0;
}

aRecord::~aRecord() {
	if(data && sz) {
		size_t i;
		for(i=0; i<sz; i++) free(data[i]);
		free(data);
		data = 0,sz = 0;
	}
}


