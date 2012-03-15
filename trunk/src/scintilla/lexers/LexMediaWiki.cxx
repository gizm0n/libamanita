// Copyright (c) 1990-2007, Scientific Toolworks, Inc.
// Author: Jason Haslam
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif



class LexMediaWiki {
private:
	struct style_node {
		int pos;		// Position.
		int style;	// Style.
	};

	int start;
	int end;
	int init;
	WordList **keywords;
	Accessor *acc;
	int stack_p;
	style_node stack[65];
	int state;
	bool newline;
	int table;

	void clearStack(int pos);
	int countStyle(int style);
	void push(int pos,int style,bool c=false);
	void pop(style_node &sn,int pos=0,int style=0,bool c=false);

	void colourise();

	unsigned int findSection(int pos);
	bool isComment(int pos);
	int skipComment(int pos);
	void matchComment(int pos,int state,style_node &sn);
	void matchHeading(int pos,int state,style_node &sn);
	void matchCharStyle(int pos,int state,style_node &sn);

	int scanParam(int pos,bool br);

public:
	static const char *const WordListDesc[];

	static void ColouriseDoc(unsigned int startPos,int length,int initStyle,WordList *keywordlists[],Accessor &styler);
	static void FoldDoc(unsigned int startPos,int length,int,WordList *[],Accessor &styler);

	LexMediaWiki(unsigned int startPos,int length,int initStyle,WordList *keywordlists[],Accessor &styler);
};


LexMediaWiki::LexMediaWiki(unsigned int startPos,int length,int initStyle,WordList *keywordlists[],Accessor &styler) {
	start = startPos;
	end = startPos+length;
	init = initStyle;
	keywords = keywordlists;
	acc = &styler;
}

void LexMediaWiki::clearStack(int pos) {
	stack_p = 1;
	stack[0].pos = pos;
	stack[0].style = SCE_MEDIAWIKI_DEFAULT;
	stack[1].pos = -1;
}

int LexMediaWiki::countStyle(int style) {
	int i,n;
	for(i=0,n=0; i<stack_p; ++i)
		if(stack[i].style==style) ++n;
	return n;
}

void LexMediaWiki::push(int pos,int style,bool c) {
	if(c && pos>0) acc->ColourTo(pos-1,style);
	stack[stack_p].pos = pos-1;
	stack[stack_p].style = style;
	stack[++stack_p].pos = -1;
}

void LexMediaWiki::pop(style_node &sn,int pos,int style,bool c) {
	if(c && pos>0) acc->ColourTo(pos-1,style);
	--stack_p;
	if(stack_p<0) stack_p = 0;
	sn = stack[stack_p];
	stack[stack_p].pos = -1;
}

void LexMediaWiki::colourise() {
	int i,n,state = init,set;
	style_node sn;
	char c0,c1,c2;

fprintf(stderr,"ColouriseMediaWikiDoc(%4u",start);

	i = findSection(start);
	clearStack(i);

	acc->StartAt(i);
	acc->StartSegment(i);

fprintf(stderr," %4u %4d %2d \"",i,end,init);
for(n=i; n<i+50 && n<end; ++n) {
	c1 = acc->SafeGetCharAt(n);
	if(c1!='\n' && c1!='\r') fputc(c1,stderr);
}
fprintf(stderr,"\")\n");

	table = 0;
	for(; i<end; ++i) {
		c0 = acc->SafeGetCharAt(i-1,'\n');
		c1 = acc->SafeGetCharAt(i);
		c2 = acc->SafeGetCharAt(i+1);
		newline = c0=='\n' || c0=='\r';

//fprintf(stderr,"ColouriseMediaWikiDoc(c1: %c, c2: %c, state: %d)\n",c1,c2,state);
		if((c1=='<' && c2=='!' &&
				acc->SafeGetCharAt(i+2)=='-' &&
				acc->SafeGetCharAt(i+3)=='-') ||
				state==SCE_MEDIAWIKI_COMMENT) {
			matchComment(i,state,sn);
			i = sn.pos;
			if(state==SCE_MEDIAWIKI_COMMENT) state = SCE_MEDIAWIKI_DEFAULT;
		} else if(c1=='=' && newline) {
			matchHeading(i,state,sn);
			i = sn.pos;
		} else if(c1=='\'' && c2=='\'') {
			matchCharStyle(i,state,sn);
			if(state==SCE_MEDIAWIKI_BOLD ||
					state==SCE_MEDIAWIKI_ITALIC || 
					state==SCE_MEDIAWIKI_BOLDITALIC) {
				if(c1=='\'' && c2=='\'') {
					acc->ColourTo(i-1,state);
					set = state,i = sn.pos;
					if(sn.style==state) {
						pop(sn);
						state = sn.style;
					} else if(state==SCE_MEDIAWIKI_BOLDITALIC) {
						if(sn.style==SCE_MEDIAWIKI_BOLD) state = SCE_MEDIAWIKI_ITALIC;
						else if(sn.style==SCE_MEDIAWIKI_ITALIC) state = SCE_MEDIAWIKI_BOLD;
						pop(sn);
					} else if((state==SCE_MEDIAWIKI_BOLD && sn.style==SCE_MEDIAWIKI_ITALIC) ||
								(state==SCE_MEDIAWIKI_ITALIC && sn.style==SCE_MEDIAWIKI_BOLD)) {
						state = SCE_MEDIAWIKI_BOLDITALIC,set = -1;
						push(i,state);
					} else {
						for(n=stack_p-1; n>=0 && stack[n].style!=SCE_MEDIAWIKI_BOLDITALIC; --n);
						if(n>=0 && stack[n].style==SCE_MEDIAWIKI_BOLDITALIC) {
							for(--stack_p; n<=stack_p; ++n) stack[n] = stack[n+1];
							pop(sn);
							state = sn.style;
						} else {
							push(i,state);
							state = sn.style,set = -1;
						}
					}
					if(set>=0) acc->ColourTo(i,set);
				}
			} else {
				push(i,state,true);
				i = sn.pos,state = sn.style;
			}
		} else if(newline && ((c1=='{' && c2=='|') || c1=='!' || c1=='|')) {
			n = 0;
			if(c1=='{' || (c1=='|' && (c2=='+' || c2=='-' || c2=='}'))) ++n;
			acc->ColourTo(i-1,state);
			acc->ColourTo(i+n,SCE_MEDIAWIKI_TABLE);
			table = (c1=='|' && c2=='}')? 0 : 1;
			i += n,state = SCE_MEDIAWIKI_DEFAULT;
			if(table && ((c1=='{' && c2=='|') || 
				((n=scanParam(i+1,false))!=-1 && acc->SafeGetCharAt(n+1)!='|')))
						state = SCE_MEDIAWIKI_PARAM;
			clearStack(i);
		} else if((c1=='[' && c2=='[') || (c1=='{' && c2=='{')) {
			push(i,state,true);
			push(i+2,c1=='['? SCE_MEDIAWIKI_LINK : SCE_MEDIAWIKI_TEMPLATE,true);
			state = SCE_MEDIAWIKI_PARAM,++i;
		} else if((c1==']' && c2==']') || (c1=='}' && c2=='}')) {
			pop(sn,i,state,true);
			pop(sn,i+2,sn.style,true);
			state = sn.style,++i;
		} else if(c1=='|' && state==SCE_MEDIAWIKI_PARAM && table!=1) {
			acc->ColourTo(i-1,state);
			acc->ColourTo(i,SCE_MEDIAWIKI_SEPARATOR);
		} else if(c1=='|' && table) {
			if(c2=='|') n = 1,table = 2;
			else n = 0;
			acc->ColourTo(i-1,state);
			acc->ColourTo(i+n,SCE_MEDIAWIKI_TABLE);
			i += n,state = SCE_MEDIAWIKI_DEFAULT;
		} else if(newline && strchr("*#:;",c1)) {
			for(n=0; strchr("*#:;",acc->SafeGetCharAt(i+n+1)); ++n);
			acc->ColourTo(i-1,state);
			acc->ColourTo(i+n,SCE_MEDIAWIKI_LIST);
			i += n,state = SCE_MEDIAWIKI_DEFAULT;
			clearStack(i);
		} else if(c0=='\n' && (c1=='\n' || (c1=='\r' && c2=='\n'))) {
			acc->ColourTo(i-1,state);
			state = SCE_MEDIAWIKI_DEFAULT;
			clearStack(i);
		}
	}
	acc->ColourTo(end-1,state);
}


unsigned int LexMediaWiki::findSection(int pos) {
	int n = pos,comment = 0;
	char c0 = 0,c1 = acc->SafeGetCharAt(n),c2 = acc->SafeGetCharAt(n-1,'\n'),c3 = acc->SafeGetCharAt(n-2,'\n');
	for(; n>0; --n) {
		c0 = c1,c1 = c2,c2 = c3,c3 = acc->SafeGetCharAt(n-2,'\n');
		if(comment) {
			if(--comment==0) return pos;
			if(c3=='<' && c2=='!' && c1=='-' && c0=='-') comment = 0;
		} else {
			if((c0=='\n' && (c1=='\n' || (c1=='\r' && c2=='\n'))) ||
				(c1=='\n' && strchr("= *#:;|",c0)) ||
				(c2=='\n' && c1=='{' && c0=='|')) break;
			else if(c2=='-' && c1=='-' && c0=='>') comment = 2048;
		}
	}
	return n;
}

bool LexMediaWiki::isComment(int pos) {
	return (
		acc->SafeGetCharAt(pos)=='<' &&
		acc->SafeGetCharAt(pos+1)=='!' &&
		acc->SafeGetCharAt(pos+2)=='-' &&
		acc->SafeGetCharAt(pos+3)=='-');
}

int LexMediaWiki::skipComment(int pos) {
	char c;
	for(int i=pos+4; 1; ++i) {
//fprintf(stderr,"skipComment(%c)\n",acc->SafeGetCharAt(i));
		c = acc->SafeGetCharAt(i,'\0');
		if(c=='\0') break;
		if(c=='-' &&
			acc->SafeGetCharAt(i+1)=='-' &&
			acc->SafeGetCharAt(i+2)=='>') return i+3;
	}
	return end;
}

void LexMediaWiki::matchComment(int pos,int state,style_node &sn) {
	int i;
//fprintf(stderr,"matchComment(");
	for(i=(state==SCE_MEDIAWIKI_COMMENT? pos : pos+4); i+2<end; ++i) {
//fputc(acc->SafeGetCharAt(i),stderr);
		if(acc->SafeGetCharAt(i)=='-' &&
			acc->SafeGetCharAt(i+1)=='-' &&
			acc->SafeGetCharAt(i+2)=='>') break;
	}
//fprintf(stderr,")\n");
	acc->ColourTo(pos-1,state);
	acc->ColourTo(i+2,SCE_MEDIAWIKI_COMMENT);
	sn.pos = i+2;
}

void LexMediaWiki::matchHeading(int pos,int state,style_node &sn) {
	int i,n;
	char c;
//fprintf(stderr,"matchHeading(%u)\n",pos);
	for(i=pos+1,n=1; i<end; ++i,++n)
		if(acc->SafeGetCharAt(i)!='=') break;
	sn.pos = pos;
	if(n) {
		for(; i<end; ++i) {
			c = acc->SafeGetCharAt(i); 
			if(c=='\r' || c=='\n') break;
		}
		if(acc->SafeGetCharAt(i-1)=='=') {
			acc->ColourTo(pos-1,state);
			acc->ColourTo(i-1,SCE_MEDIAWIKI_HEADING);
			sn.pos = i-1;
		}
	}
}

void LexMediaWiki::matchCharStyle(int pos,int state,style_node &sn) {
	int i,n;
	for(i=pos+2,n=2; i<end && acc->SafeGetCharAt(i)=='\''; ++i,++n);
//fprintf(stderr,"matchcharStyle(pos: %u, state: %d, n: %d)\n",pos,state,n);
	if(n<=1) sn.pos = pos;
	else {
		if(n==2) sn.style = SCE_MEDIAWIKI_ITALIC;
		else if(n<5) sn.style = SCE_MEDIAWIKI_BOLD;
		else if(n>=5) sn.style = SCE_MEDIAWIKI_BOLDITALIC;
		sn.pos = i-1;
	}
}

int LexMediaWiki::scanParam(int pos,bool br) {
	int i;
	char c;
	for(i=pos; 1; ++i) {
		c = acc->SafeGetCharAt(i,'\0');
//fprintf(stderr,"scanParam(%c)\n",c);
		if(c=='|') return i;
		else if((c=='\n' && !br) || c=='\0') break;
		else if(c=='<' && isComment(i)) i = skipComment(i)-1;
	}
//fprintf(stderr,"scanParam(end, %d, %d)\n",i,end);
	return -1;
}


void LexMediaWiki::ColouriseDoc(unsigned int startPos,int length,int initStyle,WordList *keywordlists[],Accessor &styler) {
	LexMediaWiki mediawiki(startPos,length,initStyle,keywordlists,styler);
	mediawiki.colourise();
}

void LexMediaWiki::FoldDoc(unsigned int startPos,int length,int,WordList *[],Accessor &styler) {
}

const char *const LexMediaWiki::WordListDesc[] = {
	"HTML",
	0
};

LexerModule lmMediaWiki(SCLEX_MEDIAWIKI,LexMediaWiki::ColouriseDoc,"mediawiki",LexMediaWiki::FoldDoc,LexMediaWiki::WordListDesc);
