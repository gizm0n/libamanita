
#include <stdlib.h>
#include <stdio.h>
#include <amanita/Hashtable.h>
#include <amanita/Vector.h>
#include <amanita/Word.h>

int main(int argc, char *argv[]) {
	int i;
	const char *key,*value;
	Hashtable table;
	Vector vector;
	Word word;

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
