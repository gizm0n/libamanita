#ifndef _AMANITA_FONTMETRICS_H
#define _AMANITA_FONTMETRICS_H

/**
 * @file amanita/aFontmetrics.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2009-12-03
 */ 

struct aGlyphmetrics {
	int minx;
	int maxx;
	int miny;
	int maxy;
	int advance;
};



struct aFontmetrics {
	aGlyphmetrics glyphs[256];
	int height;
	int ascent;
	int descent;
	int lineskip;
	int adjx;
	int adjy;
	int spacing;
};


#endif /* _AMANITA_FONTMETRICS_H */

