#ifndef _AMANITA_FLOOD_H
#define _AMANITA_FLOOD_H

/**
 * @file amanita/aFlood.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 



/** Flood fill 2D-vectors.
 * 
 * This class flood fills an area in a 2D vector. This can be useful, for example,
 * when generating maps for games.
 * @ingroup amanita
 */
class aFlood {
private:
	int *table;
	int size,*stack,pointer,stackMax,max,from,to;
	int left,top,right,bottom,width,height;
	char wrap;
	bool lSeek,rSeek;

protected:
	int pop() { return  pointer>0? stack[pointer--] : -1; }
	bool push(int x,int y,bool b);

public:
	aFlood(int wr=0);
	~aFlood();

	void setWrapping(int wr) { wrap = wr; }
	int fill(int *tbl,int w,int h,int x,int y,int t,int rx=0,int ry=0);
	int getLeft() { return left; }
	int getTop() { return top; }
	int getRight() { return right; }
	int getBottom() { return bottom; }
};



#endif /* _AMANITA_FLOOD_H */

