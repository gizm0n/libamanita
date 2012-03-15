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

static int matchHeading(unsigned int startPos,unsigned int endPos,int state,Accessor &styler) {
	unsigned int i;
	int n;
	char c;
fprintf(stderr,"matchHeading(%u)\n",startPos);
	for(i=startPos+1,n=1; i<endPos; ++i,++n)
		if(styler.SafeGetCharAt(i)!='=') break;
	if(!n) return startPos;
	for(; i<endPos; ++i) {
		c = styler.SafeGetCharAt(i); 
		if(c=='\r' || c=='\n') break;
	}
	if(styler.SafeGetCharAt(i-1)=='=') {
		styler.ColourTo(startPos-1,state);
		styler.ColourTo(i-1,SCE_CREOLE_HEADING);
		return i-1;
	} else return startPos;
}

static int matchCharStyle(unsigned int startPos,unsigned int endPos,int state,Accessor &styler,int &ret) {
	unsigned int i;
	int n;
	for(i=startPos+2,n=2; i<endPos && styler.SafeGetCharAt(i)=='\''; ++i,++n);
fprintf(stderr,"matchcharStyle(startPos: %u, state: %d, n: %d)\n",startPos,state,n);
	if(n<=1) return startPos;
	if(n==2) ret = SCE_CREOLE_ITALIC;
	else if(n<5) ret = SCE_CREOLE_BOLD;
	else if(n>=5) ret = SCE_CREOLE_BOLDITALIC;
	return i-1;
}

static void ColouriseCreoleDoc(
					unsigned int startPos,
					int length,
					int initStyle,
					WordList *keywordlists[],
					Accessor &styler) {

	unsigned int endPos = startPos+length;
	unsigned int i,n;
	int j,state = initStyle,open[256],ret;
	char c1,c2;
	bool newLine = true;

	styler.StartAt(startPos);
	styler.StartSegment(startPos);

	open[0] = state;

fprintf(stderr,"ColouriseCreoleDoc(startPos: %u, length: %d)\n",startPos,length);
	for(i=startPos,n=1; i<endPos; ++i) {
		c1 = styler.SafeGetCharAt(i);
		c2 = styler.SafeGetCharAt(i+1);

fprintf(stderr,"ColouriseCreoleDoc(c1: %c, c2: %c, state: %d)\n",c1,c2,state);
		if(state==SCE_CREOLE_DEFAULT) {
			if(c1=='<' && c2=='!' &&
						styler.SafeGetCharAt(i+2)=='-' &&
						styler.SafeGetCharAt(i+3)=='-') {
				styler.ColourTo(i-1,state);
				open[n++] = state,state = SCE_CREOLE_COMMENT;
			} else if(c1=='=') {
				if(newLine) i = matchHeading(i,endPos,state,styler);
			} else if(c1=='\'' && c2=='\'') {
				styler.ColourTo(i-1,state);
				i = matchCharStyle(i,endPos,state,styler,ret);
				open[n++] = state,state = ret;
			} else if(c1=='[' && c2=='[') {
				styler.ColourTo(i-1,state);
				open[n++] = state,state = SCE_CREOLE_LINK;
/*			} else if (isdigit(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_CREOLE_NUMBER;
			} else if (isalpha(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_CREOLE_IDENTIFIER;
			} else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
			           ch == '=' || ch == '<' || ch == '>' || ch == ':') {
				styler.ColourTo(i - 1, state);
				state = SCE_CREOLE_OPERATOR;
			} else if (ch == '$') {
				styler.ColourTo(i - 1, state);
				state = SCE_CREOLE_CONTROL;*/
			}
		} else if(state==SCE_CREOLE_COMMENT) {
			if(c1=='-' && c2=='-' && styler.SafeGetCharAt(i+2)=='>') {
				i += 2;
				styler.ColourTo(i,state);
				state = n>0? open[--n] : initStyle;
			}
		} else if(state==SCE_CREOLE_BOLD ||
					state==SCE_CREOLE_ITALIC || 
					state==SCE_CREOLE_BOLDITALIC) {
			if(c1=='\'' && c2=='\'') {
				styler.ColourTo(i-1,state);
				i = matchCharStyle(i,endPos,state,styler,ret);
				styler.ColourTo(i,state);
				if(ret==state) state = n>0? open[--n] : initStyle;
				else {
					if(state==SCE_CREOLE_BOLDITALIC) {
						if(ret==SCE_CREOLE_BOLD) state = SCE_CREOLE_ITALIC;
						else if(ret==SCE_CREOLE_ITALIC) state = SCE_CREOLE_BOLD;
						--n;
					}
					else if((state==SCE_CREOLE_BOLD && ret==SCE_CREOLE_ITALIC) ||
								(state==SCE_CREOLE_ITALIC && ret==SCE_CREOLE_BOLD))
						state = open[n++] = SCE_CREOLE_BOLDITALIC;
					else {
						for(j=n-1; j>=0 && open[j]!=SCE_CREOLE_BOLDITALIC; --j);
						if(j>=0 && open[j]==SCE_CREOLE_BOLDITALIC) {
							for(--n; j<=(int)n; ++j) open[j] = open[j+1];
							state = n>0? open[--n] : initStyle;
						} else open[n++] = state,state = ret;
					}
				}
			}
		} else if(state==SCE_CREOLE_LINK) {
			if(c1=='[' && c2=='[') open[n++] = state;
			else if(c1==']' && c2==']') {
				++i;
				styler.ColourTo(i,state);
				state = n>0? open[--n] : initStyle;
			}
/*		} else if (state == SCE_CREOLE_STRING) {
			if (ch == '\'') {
				if (chNext == '\'') {
					i++;
				} else {
					styler.ColourTo(i, state);
					state = SCE_CREOLE_DEFAULT;
				}
			}
		} else if (state == SCE_CREOLE_NUMBER) {
			if (!isdigit(ch) && !isalpha(ch) && ch != '$') {
				i--;
				styler.ColourTo(i, state);
				state = SCE_CREOLE_DEFAULT;
			}
		} else if (state == SCE_CREOLE_IDENTIFIER) {
			if (!isdigit(ch) && !isalpha(ch) && ch != '$') {
				// Get the entire identifier.
				char word[1024];
				int segmentStart = styler.GetStartSegment();
				GetRange(segmentStart, i - 1, styler, word, sizeof(word));

				i--;
				if (keywordlists[0]->InList(word))
					styler.ColourTo(i, SCE_CREOLE_KEYWORD);
				else
					styler.ColourTo(i, state);
				state = SCE_CREOLE_DEFAULT;
			}
		} else if (state == SCE_CREOLE_OPERATOR) {
			if (ch != '=' && ch != '>') {
				i--;
				styler.ColourTo(i, state);
				state = SCE_CREOLE_DEFAULT;
			}
		} else if (state == SCE_CREOLE_CONTROL) {
			if (ch == '\r' || ch == '\n') {
				styler.ColourTo(i - 1, state);
				state = SCE_CREOLE_DEFAULT;
			}*/
		}
		newLine = c1=='\n';
	}
	styler.ColourTo(endPos-1,state);
}


static void FoldCreoleDoc(unsigned int startPos,int length,int,WordList *[],Accessor &styler) {
}

static const char *const creoleWordListDesc[] = {
	"HTML",
	0
};

LexerModule lmCreole(SCLEX_CREOLE,ColouriseCreoleDoc,"creole",FoldCreoleDoc,creoleWordListDesc);
