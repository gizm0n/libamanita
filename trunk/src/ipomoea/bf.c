/* Brainfuck interpreter - Per Löwgren 2012
 * Compile: gcc -DBF -o bf bf.c
 * (To build compiler use flag -DBFC instead.)
 **/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAPE_SIZE		1024

#ifdef BFC
int main(int argc,char *argv[]) {
	char tmp[257],str[257],*script = malloc(TAPE_SIZE),*p,c;
	int l,n,t;
	FILE *fp;
	if(argc<3) {
		printf("Usage: bfc INFILE OUTFILE\n");
		return 1;
	}
	if((fp=fopen(argv[1],"rb"))==NULL) {
		printf("Could not open file: %s\n",argv[1]);
		return 1;
	}
	for(l=0,t=TAPE_SIZE; (n=fgetc(fp))!=EOF; ) {
		if(strchr("><+-.,[]",n)==NULL) continue;
		if(l==t) t += TAPE_SIZE,script = realloc(script,t+1);
		script[l++] = (char)n;
	}
	fclose(fp);
	script[l] = '\0';
	tmpnam(str);
	sprintf(tmp,"%s-bf.c",str);
	fp = fopen(tmp,"w");
	fprintf(fp,"#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\nint main() {\nchar tape[1024],*p = tape;\nmemset(tape,0,sizeof(tape));\n");
	for(p=script; *p; ++p) {
		n = 1;
		if(strchr("><+-",*p))
			for(c=*p; p[1]==c; ++p,++n);
		switch(*p) {
			case '>':
				if(n==1) fprintf(fp,"++p;\n");
				else fprintf(fp,"p += %d;\n",n);
				break;
			case '<':
				if(n==1) fprintf(fp,"--p;\n");
				else fprintf(fp,"p -= %d;\n",n);
				break;
			case '+':
				if(n==1) fprintf(fp,"++*p;\n");
				else fprintf(fp,"*p += %d;\n",n);
				break;
			case '-':
				if(n==1) fprintf(fp,"--*p;\n");
				else fprintf(fp,"*p -= %d;\n",n);
				break;
			case '.':fprintf(fp,"putchar(*p);\n");break;
			case ',':fprintf(fp,"*p = getchar();\n");break;
			case '[':
				if(!strncmp("[-]",p,3)) fprintf(fp,"*p = 0;\n");
				else fprintf(fp,"while(*p) {\n");
				break;
			case ']':fprintf(fp,"}\n");break;
		}
	}
	fprintf(fp,"return 0;\n}\n");
	fclose(fp);
	sprintf(str,"gcc -O3 -o %s %s",argv[2],tmp);
	system(str);
	sprintf(str,"strip %s",argv[2]);
	system(str);
	remove(tmp);
	free(script);
	return 0;
}
#else

/** Execute a Brainfuck script
 * @param out File to write output to, if NULL output is written to an allocated string and returned (it have to be deallocated with free()).
 * @param src Script to execute.
 * @param in Input to send to script, if NULL stdin is used instead.
 * @return If out is NULL, an allocated string containing the output, otherwise NULL. */
char *bf_exec(FILE *out,const char *src,const char *in) {
	const char *p = src;
	char tape[TAPE_SIZE],*buf = NULL;
	size_t sz = 0;
	int n,t;
	FILE *fp = out;
	if(out==NULL) fp = open_memstream(&buf,&sz);
	memset(tape,0,TAPE_SIZE);
	for(n=0; *p; ++p) {
		switch(*p) {
			case '>':if(++n==TAPE_SIZE) n = 0;break;
			case '<':if(--n<0) n = TAPE_SIZE-1;break;
			case '+':++tape[n];break;
			case '-':--tape[n];break;
			case '.':fputc(tape[n]==10? '\n' : tape[n],fp);break;
			case ',':tape[n] = (char)((t=(in!=NULL? (*in=='\0'? '\0' : *in++) : getchar()))=='\n'? 10 : t);break;
			case '[':
				if(!tape[n]) {
					for(++p,t=0; *p && (*p!=']' || t>0); ++p)
						if(*p=='[') ++t;else if(*p==']') --t;
					if(t) { fprintf(stderr,"BF: No matching ']'!\n");goto bf_end; }
				}
				break;
			case ']':
				if(tape[n]) {
					for(--p,t=0; p>=src && (*p!='[' || t>0); --p)
						if(*p==']') ++t;else if(*p=='[') --t;
					if(t) { fprintf(stderr,"BF: No matching '['!\n");goto bf_end; }
				}
				break;
		}
	}

bf_end:

	if(out==NULL) {
		fputc('\0',fp);
		fclose(fp);
	}
	return buf;
}

#ifdef BF
int main(int argc,char *argv[]) {
	char *script = malloc(TAPE_SIZE),str[81],*p;
	int l,t;
	FILE *fp = stdin;
	if(argc>=2 && (fp=fopen(argv[1],"rb"))==NULL) {
		printf("Could not open file: %s\n",argv[1]);
		return 1;
	} else if(fp==stdin) printf(
			"Brainfuck Interpreter ver. 1.0\n"
			"Enter BF-code, type 'help' for instructions.\n"
			"> ");
	for(l=0,t=TAPE_SIZE; fgets(str,80,fp)!=NULL; ) {
		if(strstr(str,"exit")!=NULL) { free(script);return 0; }
		if(strstr(str,"eof")!=NULL || strstr(str,"run")!=NULL) break;
		if(strstr(str,"help")!=NULL) printf(
			"Commands:\n"
			"  help              print this list of cammands\n"
			"  eof               end-of-file, end input and run script\n"
			"  exit              exit program\n"
			"  run               run script, same as 'eof'\n");
		for(p=str; *p; ++p) {
			if(strchr("><+-.,[]",*p)==NULL) continue;
			if(l==t) t += TAPE_SIZE,script = realloc(script,t+1);
			script[l++] = *p;
		}
		if(fp==stdin) printf("> ");
	}
	script[l] = '\0';
	if(fp==stdin) printf("Run script:\n");
	else fclose(fp);
	bf_exec(stdout,script,NULL);
	free(script);
	if(fp==stdin) printf("\ndone\n");
	return 0;
}
#endif /* BF */
#endif /* BFC */

