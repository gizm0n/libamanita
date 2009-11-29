#ifndef _LIBAMANITA_PATH_H
#define _LIBAMANITA_PATH_H

#include <stdio.h>


#define PATH_MOVE_COST_CANNOT_MOVE			0xffff

enum {
	PATH_VWRAP				= 1,
	PATH_HWRAP				= 2,
	PATH_ISOMETRIC_MAP	= 4
};


class Trail {
friend class Path;

protected:
	struct trailstep {
		unsigned char x,y,dir;
	};
	trailstep *trail;
	size_t ind,len;

public:
	Trail();
	~Trail();

	int getX() const { return ind<len? trail[ind].x : -1; }
	int getY() const { return ind<len? trail[ind].y : -1; }
	int getDir() { return ind<len? trail[ind].dir : -1; }
	size_t index() { return ind; }
	size_t setIndex(size_t i) { return ind = i>=0 && i<len? i : 0; }
	void first() { ind = 0; }
	void next() { if(ind<len) ind++; }
	void previous() { if(ind>0) ind--; }
	void last() { if(len>0) ind = len-1; }
	size_t length() { return len; }
	size_t steps() { return len-ind; }
	bool hasMoreSteps() { return ind<len; }
};


class Path {
protected:
	struct node {
		unsigned short key;
		short x,y,s,g,h,f;
		node *parent,*open,*closed;

		node(int x,int y,int g,int h,node *p)
			: x(x),y(y),g(g),h(h),f(g+h),parent(p),open(0),closed(0) { key = (x<<8)|y,s = p? p->s+1 : 0,Path::created++; }
		~node() { Path::deleted++; }
	};

	int width,height,style;
	void *map,*obj;
	node *open,**closed;
	size_t cap,sz,full;

	int (*areacmp)(int,int,int,int,void *);
	int (*terraintype)(int,int,void *);
	int (*movecost)(int,int,int,void *,void *);
	void (*move)(Path &,int,int,int &,int &,int);

	int getHeuristic(int x1,int y1,int x2,int y2,int n);
	int getDir(int x1,int y1,int x2,int y2);
	static void moveIso(Path &p,int x1,int y1,int &x2,int &y2,int dir);

	void put(node *n);
	node *get(unsigned short key);
	void rehash();
	void push(node *n);
	node *pop();
	void remove(node *n);
	void clear();

public:
	static int created,deleted;

	Path(int w,int h,int s,void *m,void *o,
			int (*a)(int,int,int,int,void *),
			int (*t)(int,int,void *),
			int (*c)(int,int,int,void *,void *));
	~Path();
	Trail *search(int x1,int y1,int x2,int y2,int l=0);
};




#endif /* _LIBAMANITA_PATH_H */

