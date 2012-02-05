
#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <amanita/aFlood.h>


aFlood::aFlood(int wr) : size(0),stack(0),wrap(wr) {}

aFlood::~aFlood() {
	if(stack) { free(stack);stack = 0; }
}

int aFlood::fill(int *tbl,int w,int h,int x,int y,int t,int rx,int ry) {
	table = tbl,width = w,height = h;
	pointer = 0,from = tbl[x+y*width],to = t,left = right = x,top = bottom = y;
	if(to==from) return 0;
	if(!stack) stack = (int *)malloc(sizeof(int)*height*2);
	else if(size<height*2) stack = (int *)realloc(stack,sizeof(int)*height*2);
	size = height*2,stackMax = 0,max = 0;
	int p,x1=0,y1=0,x2=0,y2=0,n = 0,i;
	stack[++pointer] = ((x+w)<<16)|(y+h);
	while((p=pop())>=0) {
		lSeek = rSeek = false;
		x1 = p>>16;
		y1 = (p-(x1<<16))-h;
		x1 -= w;
		x2 = x1<0? x1+w : (x1>=w? x1-w : x1);
		y2 = y1<0? y1+h : (y1>=h? y1-h : y1);
		if((rx>0 && abs(x1-x)>=rx) || (ry>0 && abs(y1-y)>=ry)) continue;
		if(wrap&2) while(y1>=-h && (ry==0 || abs(y1-y)<ry) && table[x2+y2*width]==from) y1--,y2 = y1<0? y1+h : (y1>=h? y1-h : y1);
		else while(y1>=0 && (ry==0 || abs(y1-y)<ry) && table[x2+y2*width]==from) y1--,y2 = y1<0? y1+h : y1;
		lSeek = push(x1-(y1&1),y1,lSeek),rSeek = push(x1+1-(y1&1),y1,rSeek);
		y1++;
		while(true) {
			y2 = y1<0? y1+h : (y1>=h? y1-h : y1);
			if(table[i=(x2+y2*width)]!=from) break;
			table[i] = to;
			n++;
			if(x1<left) left = x1;
			else if(x1>right) right = x1;
			if(y1<top) top = y1;
			else if(y1>bottom) bottom = y1;

			lSeek = push(x1-1,y1,lSeek),rSeek = push(x1+1,y1,rSeek);
			y1++;
			if(ry>0 && abs(y1-y)>=ry) break;
			else if(((wrap&2) && y1>=(height<<1)) || (!(wrap&2) && y1==height)) break;
		}
		lSeek = push(x1-(y1&1),y1,lSeek),rSeek = push(x1+1-(y1&1),y1,rSeek);
	}
	left = left<0? left+w : (left>=w? left-w : left);
	top = top<0? top+h : (top>=h? top-h : top);
	right = right<0? right+w : (right>=w? right-w : right);
	bottom = bottom<0? bottom+h : (bottom>=h? bottom-h : bottom);
	return n;
}

bool aFlood::push(int x,int y,bool b) {
	if(((wrap&1) && (x<-width || x>=(width<<1))) || (!(wrap&1) && (x<0 || x>=width))) return b;
	if(((wrap&2) && (y<-height || y>=(height<<1))) || (!(wrap&2) && (y<0 || y>=height))) return b;
	int m = table[(x<0? x+width : (x>=width? x-width : x))+
		(y<0? y+height : (y>=height? y-height : y))*width];
	if(!b && m==from) {
		if(pointer<size-1) stack[++pointer] = ((x+width)<<16)|(y+height);
		if(pointer>stackMax) stackMax = pointer;
		return true;
	} else if(b && m!=from) return false;
	return b;
}


