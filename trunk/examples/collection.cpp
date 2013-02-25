
#include <stdlib.h>
#include <stdio.h>
#include <amanita/Hashtable.h>
#include <amanita/List.h>
#include <amanita/Vector.h>
#include <amanita/game/Word.h>

using namespace a;

int main(int argc, char *argv[]) {
	int i;
	const char *key,*value;
	Hashtable table;
	Vector vector;
	List list;
	Word word;

	printf("Generate hashtable with values...\n");
	for(i=0; i<128; ++i) {
		key = word.generate(4,8);
		value = word.generate(8,12);
		table.put(key,value);
		if((i%8)==0) vector << key;
		list << value;
	}

	printf("Vector as it is:\n");
	vector.print();

	printf("Remove every 8 values...\n");
	for(i=0; i<(int)vector.size(); ++i)
		table.remove(vector[i]);

	printf("Table as it is:\n");
	table.print();

	printf("Pop list-elements:\n");
	for(i=0; list.size()>0; ++i) {
		value = (char *)list.pop();
		printf("List[%d] \"%s\"\n",i,value);
	}
	return 0;
}
