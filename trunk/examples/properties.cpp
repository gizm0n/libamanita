
#include <stdlib.h>
#include <stdio.h>
#include <amanita/aProperties.h>

int main(int argc, char *argv[]) {
	const char *fn = argc>=2? argv[1] : "properties.cfg";
	int lang = strstr(fn,".ini")? aLANG_INI : strstr(fn,".properties")? aLANG_PROPERTIES : aLANG_CFG;
	aProperties cfg(lang);
	const char *str;
	printf("Load configuration file \"%s\"...\n",fn);
	if(cfg.load(fn)) {
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
		cfg.set("inserted key13","value13");
		cfg.set("Section 1","inserted key5","s1-value5");
		cfg.set("Section 3","inserted key1","s3-value1");
		cfg.remove("key9");
		printf("Print configurations:\n");
		cfg.print(stdout);
		printf("Print hashtable keys:\n");
		cfg.printTable(stdout);
	}
	return 0;
}
