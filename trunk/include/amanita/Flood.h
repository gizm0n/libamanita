#ifndef _AMANITA_FLOOD_H
#define _AMANITA_FLOOD_H

/**
 * @file amanita/Flood.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 


/** Amanita Namespace */
namespace a {

enum {
	FLOOD_HWRAP			= 0x01,	//<! Flood fill wrap around edges horizontally.
	FLOOD_VWRAP			= 0x02,	//<! Flood fill wrap around edges vertically.
};

/** Flood fill 2D-vectors.
 * 
 * This class flood fills an area in a 2D vector. This can be useful, for example,
 * when generating maps for games.
 * @ingroup amanita
 */
class Flood {
private:
	int *table;
	int size;
	int *stack;
	int pointer;
	int stackMax;
	int max;
	int from;
	int to;
	int left;
	int top;
	int right;
	int bottom;
	int width;
	int height;
	char wrap;
	bool lSeek;
	bool rSeek;

protected:
	int pop() { return  pointer>0? stack[pointer--] : -1; }
	bool push(int x,int y,bool b);

public:
	Flood(int wr=0);
	~Flood();

	/** Set flags for how to handle wrapping during filling. Use WRAP_* enum values. */
	void setWrapping(int wr) { wrap = wr; }

	/** Fill the vector tbl at location x,y with t.
	 * Filling is done vertically and horizontally, and no diagonal search is done. The
	 * value at x,y will be replaced by t.
	 * @param tbl A 2D vector that should be filled. 
	 * @param w Width of tbl.
	 * @param h Height of tbl.
	 * @param x X-Position in tbl to start flood-filling.
	 * @param y Y-Position in tbl to start flood-filling.
	 * @param t Value to replace with.
	 * @param rx Range to fill, left and right of x. If rx is zero, will fill to edges of vector.
	 * @param ry Range to fill, above and below of y. If ry is zero, will fill to edges of vector.
	 * @return Number of values in tbl that has changed. */
	int fill(int *tbl,int w,int h,int x,int y,int t,int rx=0,int ry=0);

	int getLeft() { return left; }		//<! The left most value (x) in the vector that has been filled.
	int getTop() { return top; }			//<! The top most value (y) in the vector that has been filled.
	int getRight() { return right; }		//<! The right most value (x) in the vector that has been filled.
	int getBottom() { return bottom; }	//<! The bottom most value (y) in the vector that has been filled.
};

}; /* namespace a */


using namespace a;



#endif /* _AMANITA_FLOOD_H */

