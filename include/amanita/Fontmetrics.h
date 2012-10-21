#ifndef _AMANITA_FONTMETRICS_H
#define _AMANITA_FONTMETRICS_H

/**
 * @file amanita/Fontmetrics.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2009-12-03
 */ 

/** Amanita Namespace */
namespace a {

/** Metrics for glyphs.
 * @ingroup amanita */
struct Glyphmetrics {
	int minx;
	int maxx;
	int miny;
	int maxy;
	int advance;
};

/** Metrics for a font.
 * @ingroup amanita */
struct Fontmetrics {
	Glyphmetrics glyphs[256];
	int height;
	int ascent;
	int descent;
	int lineskip;
	int adjx;
	int adjy;
	int spacing;
};

}; /* namespace a */


#endif /* _AMANITA_FONTMETRICS_H */

