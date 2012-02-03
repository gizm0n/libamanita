

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipomoea/hashtable_t.h>
#include <ipomoea/string_t.h>
#include <ipomoea/vector_t.h>
#include "djynn.h"



enum {
	LINE_COMMENT,
	BLOCK_COMMENT,
	DOXYGEN_COMMENT
};

#define MAX_DOXYGEN_LINE_LEN 60


typedef struct comment_style {
	char *line;
	char *open;
	char *close;
	char *dox_line;
	char *dox_line_back;
	char *dox_open;
	char *dox_block;
	char *dox_close;

	struct {
		int line;
		int open;
		int close;
		int dox_line;
		int dox_line_back;
		int dox_open;
		int dox_block;
		int dox_close;
	} len;

	int lines;
	int blocks;
	int nested_blocks;
	int dox;
	int dox_blocks;
} comment_style;

enum {
	STYLE_ASM,
	STYLE_C,
	STYLE_CAML,
	STYLE_F77,
	STYLE_FORTRAN,
	STYLE_HASKELL,
	STYLE_FREEBASIC,
	STYLE_LATEX,
	STYLE_LUA,
	STYLE_NSIS,
	STYLE_PASCAL,
	STYLE_PERL,
	STYLE_VHDL,
	STYLE_XML,
};

static comment_style comment_styles[] = {
	{ ";",	"",		"",		"",		"",		"",		"",		"",		{1,0,0,0,0,0,0,0},	1,0,0,0,0 }, // STYLE_ASM
	{ "//",	"/*",		"*/",		"//!",	"//!<",	"/**",	" *",		" */",	{2,2,2,3,4,3,2,3},	1,1,0,1,1 }, // STYLE_C
	{ "",		"(*",		"*)",		"",		"",		"",		"",		"",		{0,2,2,0,0,0,0,0},	0,1,0,0,0 }, // STYLE_CAML
	{ "c",	"",		"",		"",		"",		"",		"",		"",		{1,0,0,0,0,0,0,0},	1,0,0,0,0 }, // STYLE_F77
	{ "!",	"",		"",		"",		"",		"",		"",		"",		{1,0,0,0,0,0,0,0},	1,0,0,0,0 }, // STYLE_FORTRAN
	{ "--",	"{-",		"-}",		"",		"",		"",		"",		"",		{2,2,2,0,0,0,0,0},	1,1,1,0,0 }, // STYLE_HASKELL
	{ "'",	"",		"",		"",		"",		"",		"",		"",		{1,0,0,0,0,0,0,0},	1,0,0,0,0 }, // STYLE_FREEBASIC
	{ "%",	"",		"",		"",		"",		"",		"",		"",		{1,0,0,0,0,0,0,0},	1,0,0,0,0 }, // STYLE_LATEX
	{ "--",	"--[[",	"--]]",	"--!",	"--!<",	"--!",	"--!",	"--!",	{2,4,4,3,4,3,3,3},	1,1,0,1,0 }, // STYLE_LUA
	{ ";",	"/*",		"*/",		"",		"",		"",		"",		"",		{1,2,2,0,0,0,0,0},	1,1,0,0,0 }, // STYLE_NSIS
	{ "",		"{",		"}",		"",		"",		"",		"",		"",		{0,1,1,0,0,0,0,0},	0,1,0,0,0 }, // STYLE_PASCAL
	{ "#",	"",		"",		"#!",		"#!<",	"##",		"#",		"#",		{1,0,0,2,3,2,1,1},	1,0,0,1,0 }, // STYLE_PERL
	{ "--",	"",		"",		"--!",	"--!<",	"--!",	"--!",	"--!",	{2,0,0,3,4,3,3,3},	1,0,0,1,0 }, // STYLE_VHDL
	{ "",		"<!--",	"-->",	"",		"",		"",		"",		"",		{0,4,3,0,0,0,0,0},	0,1,0,0,0 }, // STYLE_XML
};


typedef struct filetype_style {
	char *name;
	int style;
} filetype_style;

static filetype_style filetype_styles[] = {
	{ "ActionScript",	STYLE_C				},	//		//
	{ "Ada",				STYLE_VHDL			},	//		--
	{ "ASM",				STYLE_ASM			},	//		;
	{ "C",				STYLE_C				},	//		/* */
	{ "C#",				STYLE_C				},	//		//
	{ "C++",				STYLE_C				},	//		//
	{ "CAML",			STYLE_CAML			},	//		(* *)
	{ "CMake",			STYLE_PERL			},	//		#
	{ "Conf",			STYLE_PERL			},	//		#
	{ "CSS",				STYLE_C				},	//		/* */
	{ "D",				STYLE_C				},	//		//
	{ "Diff",			STYLE_PERL			},	//		#
	{ "Docbook",		STYLE_XML			},	//		<!-- -->
	{ "F77",				STYLE_F77			},	//		c
	{ "Ferite",			STYLE_C				},	//		/* */
	{ "Fortran",		STYLE_FORTRAN		},	//		!
	{ "FreeBasic",		STYLE_FREEBASIC	},	//		'
	{ "GLSL",			STYLE_C				},	//		/* */
	{ "Haskell",		STYLE_HASKELL		},	//		--
	{ "Haxe",			STYLE_C				},	//		//
	{ "HTML",			STYLE_XML			},	//		<!-- -->
	{ "Java",			STYLE_C				},	//		/* */
	{ "Javascript",	STYLE_C				},	//		//
	{ "LaTeX",			STYLE_LATEX			},	//		%
	{ "Lua",				STYLE_LUA			},	//		--
	{ "Make",			STYLE_PERL			},	//		#
	{ "Matlab",			STYLE_LATEX			},	//		%
	{ "NSIS",			STYLE_NSIS			},	//		;
	{ "Pascal",			STYLE_PASCAL		},	//		{ }
	{ "Perl",			STYLE_PERL			},	//		#
	{ "PHP",				STYLE_C				},	//		//
	{ "Po",				STYLE_PERL			},	//		#
	{ "Python",			STYLE_PERL			},	//		#
	{ "R",				STYLE_PERL			},	//		#
	{ "Ruby",			STYLE_PERL			},	//		#
	{ "Sh",				STYLE_PERL			},	//		#
	{ "SQL",				STYLE_C				},	//		/* */
	{ "Tcl",				STYLE_PERL			},	//		#
	{ "Vala",			STYLE_C				},	//		//
	{ "Verilog",		STYLE_C				},	//		/* */
	{ "VHDL",			STYLE_VHDL			},	//		--
	{ "XML",				STYLE_XML			},	//		<!-- -->
	{ "YAML",			STYLE_PERL			},	//		#
{NULL,0}};


static hashtable_t *styles = NULL;
static comment_style *c = NULL;
static char *eol;
static int eol_len = 0;

static void skip_string(char **s) {
	char *s1 = *s,c = *s1;
	for(s1++; *s1!='\0' && *s1!=c; s1++) {
		if(*s1=='\\') {
			s1++;
			if(eol_len==2 && *s1==*eol && s1[1]==eol[1]) s1++;
		}
	}
	*s = s1+1;
}

static void copy_string(char **d,char **s) {
	char *d1 = *d,*s1 = *s,c = *s1;
	for(*d1++=*s1++; *s1!='\0' && *s1!=c; *d1++=*s1++) {
		if(*s1=='\\') {
			*d1++ = *s1++;
			if(eol_len==2 && *s1==*eol && s1[1]==eol[1]) *d1++ = *s1++;
		}
	}
	*d1++ = *s1++,*d = d1,*s = s1;
}

static int find_line_comment(gchar *text) {
	int i = 0,n = 0;
	gchar *p1 = text,p0 = '\n';
	// If first line starts with line comment return true.
	while(*p1!='\0') { // Search for line comments at start of line.
		if((*p1=='"' || *p1=='\'') && i==0) {
			p0 = *p1;
			skip_string(&p1);
		}
		if(p0=='\n' && *p1==*c->line && !strncmp(p1,c->line,c->len.line)) return 1; 
		else if(c->lines && *p1==*c->line && !strncmp(p1,c->line,c->len.line)) p0 = *p1,p1 += c->len.line,i = 1;
		else if(c->lines && i==1 && *p1==*eol && (eol_len==1 || p1[1]==eol[1])) p0 = '\n',p1 += eol_len,i = 0;
		else if(c->blocks && *p1==*c->open && !strncmp(p1,c->open,c->len.open)) p0 = *p1,p1 += c->len.open,i = 2,n++;
		else if(c->blocks && *p1==*c->close && !strncmp(p1,c->close,c->len.close)) {
			p0 = *p1,p1 += c->len.close,n--;
			if(!c->nested_blocks || n==0) i = 0;
		} else if(*p1==*eol && (eol_len==1 || p1[1]==eol[1])) p0 = '\n',p1 += eol_len;
		else p0 = *p1++;
	}
	return 0;
}

static int find_block_comment(gchar *text) {
	int i = 0;
	gchar *p1 = text;
	while(*p1!='\0') { // Search for block comments inside selected text.
		if((*p1=='"' || *p1=='\'') && i==0) skip_string(&p1);
		if((*p1==*c->open && !strncmp(p1,c->open,c->len.open)) ||
			(*p1==*c->close && !strncmp(p1,c->close,c->len.close))) return 1;
		else if(c->lines && *p1==*c->line && !strncmp(p1,c->line,c->len.line)) p1 += c->len.line,i = 1;
		else if(c->lines && i==1 && *p1==*eol && (eol_len==1 || p1[1]==eol[1])) p1 += eol_len,i = 0;
		else p1++;
	}
	return 0;
}

static void toggle_block_comment(ScintillaObject *sci,gint start,gint end) {
	gchar *text = sci_get_contents_range(sci,start,end);
	int i = 0,len = strlen(text);
	gchar buf[len+129];
	gchar *p1 = text,*p2 = buf;
	if(!c->nested_blocks && find_block_comment(p1)) { // Remove block comments.
		while(*p1!='\0') {
			if((*p1=='"' || *p1=='\'') && i==0) copy_string(&p2,&p1);
			else if(*p1==*c->open && !strncmp(p1,c->open,c->len.open)) p1 += c->len.open,i = 2;
			else if(*p1==*c->close && !strncmp(p1,c->close,c->len.close)) p1 += c->len.close,i = 0;
			else if(c->lines && *p1==*c->line && !strncmp(p1,c->line,c->len.line)) {
				memcpy(p2,c->line,c->len.line);
				p2 += c->len.line,p1 += c->len.line,i = 1;
			} else if(c->lines && i==1 && *p1==*eol && (eol_len==1 || p1[1]==eol[1])) {
				if(eol_len==2) *p2++ = *p1++;
				*p2++ = *p1++,i = 0;
			} else *p2++ = *p1++;
		}
		*p2 = '\0';
	} else if(c->nested_blocks && !strncmp(p1,c->open,c->len.open)) { // Remove nested block comment.
		int n = 0;
		p1 += c->len.open;
		while(*p1!='\0') {
			if((*p1=='"' || *p1=='\'') && i==0) skip_string(&p1);
			else if(*p1==*c->open && !strncmp(p1,c->open,c->len.open)) {
				memcpy(p2,c->open,c->len.open);
				p2 += c->len.open,p1 += c->len.open,i = 2,n++;
			} else if(*p1==*c->close && !strncmp(p1,c->close,c->len.close)) {
				if(--n==0) {
					strcpy(p2,p1+c->len.close);
					p2 += strlen(p2);
					break;
				} else {
					memcpy(p2,c->close,c->len.close);
					p2 += c->len.close,p1 += c->len.close,i = 0;
				}
			} else if(c->lines && *p1==*c->line && !strncmp(p1,c->line,c->len.line)) {
				memcpy(p2,c->line,c->len.line);
				p2 += c->len.line,p1 += c->len.line,i = 1;
			} else if(c->lines && i==1 && *p1==*eol && (eol_len==1 || p1[1]==eol[1])) {
				if(eol_len==2) *p2++ = *p1++;
				*p2++ = *p1++,i = 0;
			} else *p2++ = *p1++;
		}
		*p2 = '\0';
	} else { // Insert block comment.
		sprintf(p2,"%s%s%s",c->open,p1,c->close);
	}
	sci_replace_sel(sci,buf);
	g_free(text);
}


typedef struct function_data {
	int indent;
	char *name;
	char *ret_type;
	int nparams;
	char **params;
} function_data;

static void extract_function(ScintillaObject *sci,gint line,function_data *fd) {
	gchar *text;
	char *p1;
	text = sci_get_line(sci,line);
	for(fd->indent=0,p1=text; *p1=='\t'; p1++,fd->indent++);
	if(strchr(text,'(')!=NULL) {
		int i,lines = sci_get_line_count(sci);
		char f[1025],*p2,*p3;
		strcpy(f,text);
		for(i=1; strchr(f,')')==NULL && i<=20 && line+i<lines; i++) {
			g_free(text);
			text = sci_get_line(sci,line+i);
			strcat(f,text);
		}
		if(i<=20) {
			strwhsp(f);
fprintf(djynn.log,"function: %s\n",f);
fflush(djynn.log);
			p2 = strchr(f,'('),*p2++ = '\0';
			if((p1=strrchr(f,'*'))!=NULL || (p1=strrchr(f,' '))!=NULL) p1++;
			else p1 = f;
			fd->name = strdup(p1);
			if(p1!=f) {
				*p1 = '\0';
				strwhsp(f);
				fd->ret_type = strdup(f);
			} else fd->ret_type = NULL;
			p1 = p2,p2 = strchr(p1,')'),*p2 = '\0';
			if(*p1=='\0') fd->nparams = 0;
			else {
				fd->nparams = strnchr(p1,',')+1;
				fd->params = (char **)malloc(sizeof(char *)*(fd->nparams*2));
				for(i=0; p1!=NULL && *p1!='\0'; i+=2,p1=p2) {
					if((p2=strchr(p1,','))!=NULL) *p2++ = '\0';
					if((p3=strrchr(p1,'*'))!=NULL || (p3=strrchr(p1,' '))!=NULL) p3++;
					if(p3!=NULL) {
						fd->params[i+1] = strdup(p3);
						*p3 = '\0';
						strwhsp(p1);
						fd->params[i] = strdup(p1);
					} else {
						fd->params[i] = strdup("");
						fd->params[i+1] = strdup(p1);
					}
				}
			}
fprintf(djynn.log,"name=%s, ret_type=%s, nparams=%d\n",fd->name,fd->ret_type,fd->nparams);
fflush(djynn.log);
for(i=0; i<fd->nparams; i++)
fprintf(djynn.log,"param[%d]=[%s] %s\n",i,fd->params[i*2],fd->params[i*2+1]);
fflush(djynn.log);
		}
	}
	g_free(text);
}

static void delete_function(function_data *fd) {
	if(fd->name!=NULL) free(fd->name);
	if(fd->ret_type!=NULL) free(fd->ret_type);
	if(fd->nparams>0) {
		int i;
		for(i=0; i<fd->nparams*2; i++) free(fd->params[i]);
		free(fd->params);
	}
	free(fd);
}


static void toggle_comment(int st) {
	GeanyDocument *doc = document_get_current();
	if(doc==NULL) return;
	else {
		ScintillaObject *sci = doc->editor->sci;
		gboolean sel = sci_has_selection(sci);
		gint pos,line,start,end,start_line,end_line,end_col;
		gchar *text,*buf,*p1,*p2;
		int i = scintilla_send_message(sci,SCI_GETEOLMODE,0,0);
		switch(i) {
			case SC_EOL_CRLF:eol = "\r\n",eol_len = 2;break;
			case SC_EOL_CR:eol = "\r",eol_len = 1;break;
			default:eol = "\n",eol_len = 1;break;
		}
		pos = sci_get_current_position(sci);
		line = sci_get_current_line(sci);
		start = sci_get_selection_start(sci);
		end = sci_get_selection_end(sci);
		c = (comment_style *)ht_get(styles,doc->file_type->name);
fprintf(djynn.log,"toggle_comment(file_type=%s,st=%d)\n",doc->file_type->name,st);
fflush(djynn.log);
		if(st==DOXYGEN_COMMENT && !c->dox) {
			ui_set_statusbar(FALSE,_("Doxygen does not support this language!"));
		} else if(sel) { // If there is a selection
			if((st==BLOCK_COMMENT && c->blocks) || (st==LINE_COMMENT && !c->lines)) {
				toggle_block_comment(sci,start,end);
			} else {
				start_line = sci_get_line_from_position(sci,start);
				end_line = sci_get_line_from_position(sci,end);
				if(end_line>start_line) { // If many lines are selected.
					vector_t *vec = vec_new(0);
					start = sci_get_position_from_line(sci,start_line);
					end_col = sci_get_col_from_position(sci,end);
					if(end_col>0) end_line++;
					end = sci_get_position_from_line(sci,end_line);
					text = sci_get_contents_range(sci,start,end);
					vec_split_all(vec,text,eol);
					if(st==LINE_COMMENT || st==BLOCK_COMMENT) {
						if(find_line_comment(text)) { // Remove line comments.
							for(i=0; i<vec->size; i++) {
								p1 = (char *)vec->list[i].value;
								if(*p1==*c->line && !strncmp(p1,c->line,c->len.line)) {
									vec->list[i].value = strdup(&p1[c->len.line]);
									free(p1);
								}
							}
						} else { // Insert line comments.
							for(i=0; i<vec->size; i++) {
								p1 = (char *)vec->list[i].value;
								if(*p1=='\0') continue;
								p2 = (char *)malloc(sizeof(char)*(c->len.line+strlen(p1)+1));
								strcpy(p2,c->line);
								strcat(p2,p1);
								free(p1);
								vec->list[i].value = p2;
							}
						}
					} else if(st==DOXYGEN_COMMENT) { // Insert doxygen comments for many lines.
						if(vec->size>=3 && // Insert a doxygen group comment when first and last selected line is empty.
								*((char *)vec->list[0].value)=='\0' &&
								*((char *)vec->list[vec->size-1].value)=='\0') {
							int l1,l2,t;
							for(l1=0; l1+1<vec->size; l1++)
								if(*((char *)vec->list[l1].value)=='\0' && *((char *)vec->list[l1+1].value)!='\0') break;
							if(l1+1<vec->size) {
								for(l2=vec->size-1; l2>0; l2--)
									if(*((char *)vec->list[l2].value)=='\0' && *((char *)vec->list[l2-1].value)!='\0') break;
fprintf(djynn.log,"l1=%d\tl2=%d\n",l1,l2);
fflush(djynn.log);
								free(vec->list[l1].value);
								free(vec->list[l2].value);
								p1 = (char *)vec->list[l1+1].value;
								for(t=0; *p1=='\t'; p1++,t++);
fprintf(djynn.log,"p1=%s\tt=%d\n",p1,t);
fflush(djynn.log);
								{
									char t1[t+1]; // Indentation
									char buf[1025];

									for(i=0; i<t; i++) t1[i] = '\t';
									t1[i] = '\0';

									p1 = buf;
									p1 += sprintf(p1,"%s%s%s @name %s%s%s @{",eol,t1,c->dox_open,eol,t1,c->dox_block);
									if(c->dox_blocks) p1 += sprintf(p1,"%s",c->dox_close);
									vec->list[l1].value = strdup(buf);

									p1 = buf;
									p1 += sprintf(p1,"%s%s @}",t1,c->dox_open);
									if(c->dox_blocks) p1 += sprintf(p1,"%s",c->dox_close);
									strcpy(p1,eol);
									vec->list[l2].value = strdup(buf);
								}
							}
						} else { // Insert back comment after each line.
							int ind = sci_get_tab_width(sci),len,tlen,max = 0,l,t;
							for(i=0; i<vec->size; i++) {
								p1 = (char *)vec->list[i].value;
								if(*p1=='\0' || strstr(p1,c->dox_line)!=NULL) continue;
								len = strlen(p1);
								for(p2=p1,tlen=len; *p2!='\0'; p2++) if(*p2=='\t') tlen += ind-1;
								if(tlen<=MAX_DOXYGEN_LINE_LEN && tlen>max) max = tlen;
fprintf(djynn.log,"line=%s\tlen=%d, max=%d\n",p1,len,max);
fflush(djynn.log);
							}
							if(max>MAX_DOXYGEN_LINE_LEN) max = MAX_DOXYGEN_LINE_LEN;
							max -= (max%ind);
fprintf(djynn.log,"ind=%d, max=%d\n",ind,max);
fflush(djynn.log);
							for(i=0; i<vec->size; i++) {
								p1 = (char *)vec->list[i].value;
								if(*p1=='\0' || strstr(p1,c->dox_line)!=NULL) continue;
								len = strlen(p1);
								for(p2=p1,tlen=len,l=0; *p2!='\0'; p2++)
									if(*p2=='\t' && l<ind-1) tlen += ind-1-l,l = 0;
									else l = l+1==ind? 0 : l+1;
								l = (tlen-(ind*2)>MAX_DOXYGEN_LINE_LEN); // If line too long, place comment on new line.
								t = l? max/ind+2 : (max-(tlen-(tlen%ind)))/ind+2; // Calculate indentation
fprintf(djynn.log,"len=%d, t=%d, sum=%d\n",len,t,(int)sizeof(char)*(len+t+129));
fflush(djynn.log);
								buf = (char *)malloc(sizeof(char)*(len+t+129));
								p2 = buf;
								memcpy(p2,p1,len);
								p2 += len;
								if(l) {
									*p2++ = *eol;
									if(eol_len==2) *p2++ = eol[1];
								}
								for(; t>0; t--) *p2++ = '\t';
								p2 += sprintf(p2,"%s ",c->dox_line_back);
								if(!c->lines) p2 += sprintf(p2,"%s",c->dox_close);
								vec->list[i].value = buf;
								free(p1);
fprintf(djynn.log,"line=%s\n",buf);
fflush(djynn.log);
							}
						}
					}
//vec_print(vec);
					buf = vec_join(vec,eol);
fprintf(djynn.log,"buf=%s\n",buf);
fflush(djynn.log);
					sci_set_selection_start(sci,start);
					sci_set_selection_end(sci,end);
					sci_replace_sel(sci,buf);
					free(buf);
					g_free(text);
					vec_delete(vec);
				} else { // If selection is within one line.
					if(st==LINE_COMMENT) { // Selecting text within one line becomes a block comment.
						toggle_block_comment(sci,start,end);
					} else if(st==DOXYGEN_COMMENT) {
					}
				}
			}
		} else { // No selected text.
			if(st==BLOCK_COMMENT) {
				if(c->blocks) {
					char buf[32];
					sprintf(buf,"%s  %s",c->open,c->close);
					sci_insert_text(sci,pos,buf);
				}
			} else {
				if(st==LINE_COMMENT) { // Toggle comment for one line without selected text.
					start_line = sci_get_line_from_position(sci,start);
					text = sci_get_line(sci,start_line);
					start = sci_get_position_from_line(sci,start_line);
					if(*text==*c->line && !strncmp(text,c->line,c->len.line)) {
						sci_set_selection_start(sci,start);
						sci_set_selection_end(sci,start+c->len.line);
						sci_replace_sel(sci,"");
					} else {
						sci_insert_text(sci,start,c->line);
					}
					g_free(text);
				} else if(st==DOXYGEN_COMMENT) {
					char str[2049];
					function_data *fd = (function_data *)malloc(sizeof(function_data));
					memset(fd,0,sizeof(function_data));
					extract_function(sci,line,fd);
					{
						char t[fd->indent+1];
						for(i=0,p1=t; i<fd->indent; i++) *p1++ = '\t';
						*p1 = '\0';
						p1 = str;
						p1 += sprintf(p1,"%s%s%s",t,c->dox_open,eol);
						for(i=0; i<fd->nparams; i++)
							p1 += sprintf(p1,"%s%s @param %s %s",t,c->dox_block,fd->params[i*2+1],eol);
						if(fd->ret_type!=NULL &&
								(strstr(fd->ret_type,"void")==NULL || strstr(fd->ret_type,"void*")!=NULL))
							p1 += sprintf(p1,"%s%s @return %s",t,c->dox_block,eol);
						if(c->dox_blocks) p1 += sprintf(p1,"%s%s%s",t,c->dox_close,eol);
						pos = sci_get_position_from_line(sci,line);
						sci_insert_text(sci,pos,str);
					}
					delete_function(fd);
				}
			}
		}
	}
}

static void item_activate_cb(GtkMenuItem *menuitem, gpointer gdata) {
	djynn_comment_action((int)((intptr_t)gdata));
}

void djynn_comment_action(int id) {
	switch(id) {
		case DJYNN_TOGGLE_COMMENT:toggle_comment(LINE_COMMENT);break;
		case DJYNN_TOGGLE_BLOCK:toggle_comment(BLOCK_COMMENT);break;
		case DJYNN_INSERT_DOXYGEN_COMMENT:toggle_comment(DOXYGEN_COMMENT);break;
	}
}

void djynn_comment_init(GeanyData *data,int *menu_index) {
	int i;
	djynn_menu_item *m;
	filetype_style *fts;
	
	styles = ht_new(0);
	for(i=0; 1; i++) {
		fts = &filetype_styles[i];
		if(fts->name==NULL) break;
		ht_put(styles,fts->name,(void *)&comment_styles[fts->style]);
	}

	if(*menu_index>0) {
		gtk_container_add(GTK_CONTAINER(djynn.tools_menu),gtk_separator_menu_item_new());
		*menu_index += 1;
	}

	for(i=DJYNN_TOGGLE_COMMENT; i<=DJYNN_INSERT_DOXYGEN_COMMENT; i++) {
		m = &djynn.menu_items[i];
		if(m->menu_stock==NULL) m->menu_item = gtk_menu_item_new_with_label(m->label);
		else m->menu_item = ui_image_menu_item_new(m->menu_stock,m->label);
		g_signal_connect(m->menu_item,"activate",G_CALLBACK(item_activate_cb),(gpointer)((intptr_t)i));
		gtk_menu_append(GTK_MENU(djynn.tools_menu),m->menu_item);
		*menu_index += 1;
	}
}


void djynn_comment_cleanup() {
	ht_delete(styles);
}

