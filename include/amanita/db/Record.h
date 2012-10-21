#ifndef _AMANITA_DB_RECORD_H
#define _AMANITA_DB_RECORD_H

/**
 * @file amanita/db/Record.h
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/** Amanita Namespace */
namespace a {


/** @cond */
class Result;
/** @endcond */


/** Record.
 * 
 * @ingroup db */
class Record {
private:
	size_t sz;
	char **data;

public:
	Record(Result &rs,int n,char **d);
	~Record();

	int columns() { return sz; }
	char *getColumn(int n) { return data[n]; }

	uint32_t uint32(int n) { return (uint32_t)atoi(data[n]); }
	int32_t int32(int n) { return atoi(data[n]); }
	float real32(int n) { return (float)atof(data[n]); }
	double real64(int n) { return atof(data[n]); }

	uint64_t uint64(int n) {
#if _WORDSIZE == 64
		return atoul(data[n]);
#else
		return (uint64_t)atoll(data[n]);
#endif
	}

	int64_t int64(int n) {
#if _WORDSIZE == 64
		return atol(data[n]);
#else
		return atoll(data[n]);
#endif
	}
};

}; /* namespace a */



#endif /* _AMANITA_DB_RECORD_H */
