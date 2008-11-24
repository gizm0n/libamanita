#ifndef _LIBAMANITA_FLOOD_H
#define _LIBAMANITA_FLOOD_H



class Flood {
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
	Flood(int wr=0);
	~Flood();

	void setWrapping(int wr) { wrap = wr; }
	int fill(int *tbl,int w,int h,int x,int y,int t,int rx=0,int ry=0);
	int getLeft() { return left; }
	int getTop() { return top; }
	int getRight() { return right; }
	int getBottom() { return bottom; }
};



#endif /* _LIBAMANITA_FLOOD_H */

