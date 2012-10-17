
#include <stdlib.h>
#include <stdio.h>
#include <amanita/aProperties.h>

int main(int argc, char *argv[]) {
	aProperties cfg;
	const char *str;
	printf("Load configuration file...\n");
	cfg.load("properties.cfg");
	printf("Get values:\n");
	str = cfg.getString("key1");
	printf("Value for \"key1\": \"%s\"\n",str);
	str = cfg.getString("key6?#");
	printf("Value for \"key6?#\": \"%s\"\n",str);
	str = cfg.getString("key7€.");
	printf("Value for \"key7€.\": \"%s\"\n",str);
	str = cfg.getString("key9");
	printf("Value for \"key9\": \"%s\"\n",str);
	str = cfg.getString("key11");
	printf("Value for \"key11\": \"%s\"\n",str);
	str = cfg.getString("Section 1","key1");
	printf("Value for \"Section 1\"=>\"key1\": \"%s\"\n",str);
	printf("Print configurations:\n");
	cfg.print(stdout);
	printf("Print hashtable keys:\n");
	cfg.printTable(stdout);
	return 0;
}
