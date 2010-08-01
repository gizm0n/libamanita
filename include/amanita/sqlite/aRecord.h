#ifndef _AMANITA_SQLITE_RECORD_H
#define _AMANITA_SQLITE_RECORD_H

/**
 * @file amanita/sqlite/aRecord.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <stdlib.h>
#include <string.h>


class aResultset;


/** Record.
 * @ingroup sqlite
 */
class aRecord {
private:
	char **data;
	size_t sz;

public:
	aRecord(aResultset &rs,char **d,unsigned int s);
	~aRecord();

	int size() { return sz; }
	char *getString(int n) { return data[n]; }
	int getInt(int n) { return atoi(data[n]); }
	float getFloat(int n) { return (float)atof(data[n]); }
	double getDouble(int n) { return atof(data[n]); }
	long long getLongLong(int n) { return atoll(data[n]); }
};



#endif /* _AMANITA_SQLITE_RECORD_H */
