
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libamanita/Path.h>



Trail::Trail() : trail(0),ind(0),len(0) {}
Trail::~Trail() {
	if(trail) { free(trail);trail = 0; }
}



int Path::created = 0;
int Path::deleted = 0;

//int Path::coords[4][8][2] = {
//	{ {-1, 1},{-1,-1},{ 0,-1},{ 0, 1},{-1, 0},{ 1, 0},{-1, 1},{-1,-1} },
//	{ { 0, 1},{ 0,-1},{ 1,-1},{ 1, 1},{-1, 0},{ 1, 0},{ 0, 1},{ 0,-1} },
//	{ { 0,-1},{ 0, 1},{-1, 1},{-1,-1},{ 1, 0},{-1, 0},{-1, 1},{-1,-1} },
//	{ { 1,-1},{ 1, 1},{ 0, 1},{ 0,-1},{ 1, 0},{-1, 0},{ 0, 1},{ 0,-1} }
//};


Path::Path(int w,int h,int s,void *m,void *o,
		int (*a)(int,int,int,int,void *),
		int (*t)(int,int,void *),
		int (*c)(int,int,int,void *,void *))
			: width(w),height(h),style(s),map(m),obj(o),areacmp(a),terraintype(t),movecost(c) {
	move = moveIso;
	open = 0;
	closed = 0,cap = 11,sz = 0,full = 0;
}

Path::~Path() {
}

#define coutput(c) //{putc(c,stderr);fflush(stderr);}
#define soutput(s,d) //{fprintf(stderr,s,d);fflush(stderr);}

Trail *Path::search(int x1,int y1,int x2,int y2,int l) {
fprintf(stderr,"Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
fflush(stderr);
	Trail *t = 0;
	int cost = 0;
	if(areacmp) while(areacmp(x1,y1,x2,y2,map)!=0) {
//fprintf(stderr,"Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
//fflush(stderr);
		move(*this,x2,y2,x2,y2,getDir(x2,y2,x1,y1));
	}
	if(terraintype) cost = terraintype(x1,y1,map);

//fprintf(stderr,"Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
//fflush(stderr);
	node *p1 = 0,*p2 = 0;
	if(x1!=x2 || y1!=y2) {
		cap = getHeuristic(x1,y1,x2,y2,0)*8+1;
		p1 = new node(x1,y1,0,getHeuristic(x1,y1,x2,y2,2),0);
		put(p1);
		push(p1);
	}
//fprintf(stderr,"Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
//fflush(stderr);

	int i,x,y,c;
	while(open) {
		p1 = pop();
//fprintf(stderr,"key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\ts=%d\tg=%d\th=%d\tf=%d\tsz=%d ",p1->key,p1->x,p1->y,
//	p1->parent? p1->parent->x : -1,p1->parent? p1->parent->y : -1,p1->s,p1->g,p1->h,p1->f,size());
//fflush(stderr);
		if(!l || p1->s<l) for(i=0; i<=3; i++) {
			move(*this,p1->x,p1->y,x,y,i);
			c = movecost(x,y,cost,map,obj);															soutput("(%d",x)soutput(",%d)",y)
			if(x==x2 && y==y2) {																			coutput('!');
				if(c!=PATH_MOVE_COST_CANNOT_MOVE) {
					p1 = new node(x,y,p1->g+c,0,p1);
					put(p1);
				}
				open = 0;
				break;
			} else if(c!=PATH_MOVE_COST_CANNOT_MOVE) {
				if((p2=get((x<<8)|y))) {																coutput('+');
					if(p1->g+c<p2->g) {																	coutput('*');
						remove(p2);
						p2->parent = p1,p2->g = p1->g+c,p2->f = p2->g+p2->h,p2->s = p1->s+1;
						push(p2);
					}
				} else {																						coutput('-');
					p2 = new node(x,y,p1->g+c,getHeuristic(x,y,x2,y2,2),p1);
					put(p2);																					coutput('=');
					push(p2);
				}
			}
		}																										coutput('\n');
	}

//char mem[height][width];
//memset(mem,' ',width*height);
//for(int y=0; y<height; y++) for(int x=0; x<width; x++) if(get((x<<8)|y)) mem[y][x] = '+';

	if(p1 && p1->g) {
		t = new Trail();
//fprintf(stderr,"Path::search(p.x=%d,p.y=%d,p.g=%d)\n",p1->x,p1->y,p1->g);
//fflush(stderr);
		for(t->len=1,p1->open=0; p1->parent && p1->parent!=p1; t->len++,p1=p1->parent)
			p1->parent->open = p1;
//fprintf(stderr,"Path::search(trail.lenght=%d)\n",t->len);
//fflush(stderr);
		if(t->len>1) {
			t->trail = (Trail::trailstep *)malloc(sizeof(Trail::trailstep)*t->len);
			for(i=0,c='a'; p1; i++,p1=p2) {
				p2 = p1->open;
//fprintf(stderr,"key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\tdir=%d\n",
//p1->key,p1->x,p1->y,p2? p2->x : -1,p2? p2->y : -1,p2? getDir(p1->x,p1->y,p2->x,p2->y) : 5);
//fflush(stderr);
				t->trail[i] = (Trail::trailstep){ p1->x,p1->y,p2? getDir(p1->x,p1->y,p2->x,p2->y) : 5 };
//				mem[p1->y][p1->x] = (char)c++;
//				if(c=='z'+1) c = 'A';
//				else if(c=='Z'+1) c = 'a';
			}
//fprintf(stderr,"Path::search()\n");
//fflush(stderr);
		}
	}
//for(int y=0; y<height; y++) {
//for(int x=0; x<width; x++) putc(mem[y][x],stderr);
//fputc('\n',stderr);
//fflush(stderr);
//}
	clear();
	return t;
}


void Path::put(node *n) {
	if(sz==full) rehash();
	int h = n->key%cap;
	n->closed = closed[h],closed[h] = n;
	sz++;
}

Path::node *Path::get(unsigned short key) {
	if(!sz) return 0;
	node *n = closed[key%cap];
	while(n && n->key!=key) n = n->closed;
	return n;
}

void Path::rehash() {
	if(!closed) {
		closed = (node **)malloc(cap*sizeof(node *));
		memset(closed,0,cap*sizeof(node *));
	} else {
		int c = cap<<1;
		if(!(c&1)) c++;
		unsigned long i,h;
		node **t = (node **)malloc(c*sizeof(node *));
		memset(t,0,c*sizeof(node *));
		node *n1,*n2;
		for(i=0; i<cap; i++) if((n1=closed[i])) while(n1)
			n2 = n1->closed,h = n1->key%c,n1->closed = t[h],t[h] = n1,n1 = n2;
		free(closed);
		closed = t,cap = c;
	}
	full = cap>>1;
}


void Path::push(node *p) {														coutput('p');
	node *p0 = 0,*p1 = open;
	int i=0;
	while(p1 && (p->f>p1->f || (p->f==p1->f && p->h>p1->h)))
		p0 = p1,p1 = p1->open,i++;												soutput("%dp",i);
	if(!p0) p->open = open,open = p;
	else p0->open = p,p->open = p1;
}

Path::node *Path::pop() {
	node *p = open;
	if(p) open = p->open;
	return p;
}

void Path::remove(node *p) {
	if(!open) return;
	if(open!=p) {
		node *p1 = open;
		while(p1->open && p1->open!=p) p1 = p1->open;
		if(p1->open) p1->open = p1->open->open;
	} else open = open->open;
}

void Path::clear() {
	if(closed) {
		node *n1,*n2;
		for(unsigned long i=0; i<cap; i++) if((n1=closed[i])) do {
			n2 = n1->closed;
			delete n1;
			n1 = n2;
		} while(n1);
		free(closed);
		closed = 0;
	}
	open = 0;
}



int Path::getHeuristic(int x1,int y1,int x2,int y2,int n) {
	if(style&PATH_HWRAP) { if(x1+width-x2<x2-x1) x1 += width;else if(x2+width-x1<x1-x2) x2 += width; }
	if(style&PATH_VWRAP) { if(y1+height-y2<y2-y1) y1 += height;else if(y2+height-y1<y1-y2) y2 += height; }
	x1 = abs(x1-x2),y1 = abs(y1-y2);
	if(n==1) return (x1>y1? x1 : y1)+(x1<y1? x1 : y1)/2;
	else if(n==2) return x1+y1;
	else return (x1>y1? x1 : y1);
}
int Path::getDir(int x1,int y1,int x2,int y2) {
	if(style&PATH_HWRAP) { if(x1+width-x2<x2-x1) x1 += width;else if(x2+width-x1<x1-x2) x2 += width; }
	if(style&PATH_VWRAP) { if(y1+height-y2<y2-y1) y1 += height;else if(y2+height-y1<y1-y2) y2 += height; }
	//if(abs(x1-x2)<=1 && abs(y1-y2)<=1)
	return y2<y1? (x2<x1+(y1&1)? 3 : 0) : (x2<x1+(y1&1)? 2 : 1);

}
void Path::moveIso(Path &p,int x1,int y1,int &x2,int &y2,int dir) {
	static const int xcoords[4] = {  0, 0,-1,-1 };
	static const int ycoords[4] = { -1, 1, 1,-1 };
//fprintf(stderr,"Path::moveIso(x=%d,y=%d,dir=%d)\n",x1,y1,dir);
//fflush(stderr);
	x1 += xcoords[dir]+(y1&1),y1 += ycoords[dir];
	if(p.style&PATH_HWRAP) { if(x1<0) x1 += p.width;else if(x1>=p.width) x1 -= p.width; }
	else { if(x1<0) x1 = 0;else if(x1>=p.width) x1 = p.width-1; }
	if(p.style&PATH_VWRAP) { if(y1<0) y1 += p.height;else if(y1>=p.height) y1 -= p.height; }
	else { if(y1<0) y1 = 0;else if(y1>=p.height) y1 = p.height-1; }
	x2 = x1,y2 = y1;
}
//int Path::wrapX(int x) {
//	if(style&PATH_HWRAP) { if(x<0) x += width;else if(x>=width) x -= width; }
//	else { if(x<0) x = 0;else if(x>=width) x = width-1; }
//	return x;
//}
//int Path::wrapY(int y) {
//	if(style&PATH_VWRAP) { if(y<0) y += height;else if(y>=height) y -= height; }
//	else { if(y<0) y = 0;else if(y>=height) y = height-1; }
//	return y;
//}




