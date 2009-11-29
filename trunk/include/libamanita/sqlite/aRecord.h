#ifndef _LIBAMANITA_SQLITE_RECORD_H
#define _LIBAMANITA_SQLITE_RECORD_H

#include <stdlib.h>
#include <string.h>


class ResultSet;


class Record {
private:
	char **data;
	size_t sz;

public:
	Record(ResultSet &rs,char **d,unsigned int s);
	~Record();

	int size() { return sz; }
	char *getString(int n) { return data[n]; }
	int getInt(int n) { return atoi(data[n]); }
	float getFloat(int n) { return (float)atof(data[n]); }
	double getDouble(int n) { return atof(data[n]); }
	long long getLongLong(int n) { return atoll(data[n]); }
};



#endif /* _LIBAMANITA_SQLITE_RECORD_H */
