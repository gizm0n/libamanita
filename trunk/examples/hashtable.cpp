
#include <stdlib.h>
#include <stdio.h>
#include <amanita/aHashtable.h>
#include <amanita/aVector.h>
#include <amanita/aWord.h>

int main(int argc, char *argv[]) {
	int i;
	const char *key,*value;
	aHashtable table;
	aVector vector;
	aWord word;

	printf("Generate hashtable with values...\n");
	for(i=0; i<512; ++i) {
		key = word.generate(8,16);
		value = word.generate(16,32);
		table.put(key,value);
		if((i%32)==0) vector << key;
	}
	printf("Remove every 32nd value...\n");
	for(i=0; i<(int)vector.size(); ++i)
		table.remove(vector[i]);
	printf("Table as it is:\n");
	table.print();
	return 0;
}
