
#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <amanita/aPath.h>



aTrail::aTrail() : trail(0),ind(0),len(0) {}
aTrail::~aTrail() {
	if(trail) { free(trail);trail = 0; }
}



int aPath::created = 0;
int aPath::deleted = 0;

//int aPath::coords[4][8][2] = {
//	{ {-1, 1},{-1,-1},{ 0,-1},{ 0, 1},{-1, 0},{ 1, 0},{-1, 1},{-1,-1} },
//	{ { 0, 1},{ 0,-1},{ 1,-1},{ 1, 1},{-1, 0},{ 1, 0},{ 0, 1},{ 0,-1} },
//	{ { 0,-1},{ 0, 1},{-1, 1},{-1,-1},{ 1, 0},{-1, 0},{-1, 1},{-1,-1} },
//	{ { 1,-1},{ 1, 1},{ 0, 1},{ 0,-1},{ 1, 0},{-1, 0},{ 0, 1},{ 0,-1} }
//};


aPath::aPath(int w,int h,int s,void *m,void *o,
		path_area_compare a,
		path_terrain_type t,
		path_move_cost c)
			: width(w),height(h),style(s),map(m),obj(o),
					cb_area_compare(a),
					cb_terrain_type(t),
					cb_move_cost(c) {
	cb_move = moveIso;
	open = 0;
	closed = 0,cap = 11,sz = 0,full = 0;
}

aPath::~aPath() {
}

#define coutput(c) //{putc(c,stderr);fflush(stderr);}
#define soutput(s,d) //{fprintf(stderr,s,d);fflush(stderr);}

aTrail *aPath::search(int x1,int y1,int x2,int y2,int l) {
debug_output("aPath::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
	aTrail *t = 0;
	int cost = 0;
	if(cb_area_compare) while(cb_area_compare(x1,y1,x2,y2,map)!=0) {
//debug_output("aPath::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
		cb_move(*this,x2,y2,x2,y2,getDir(x2,y2,x1,y1));
	}
	if(cb_terrain_type) cost = cb_terrain_type(x1,y1,map);

//debug_output("aPath::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
	node *p1 = 0,*p2 = 0;
	if(x1!=x2 || y1!=y2) {
		cap = getHeuristic(x1,y1,x2,y2,0)*8+1;
		p1 = new node(x1,y1,0,getHeuristic(x1,y1,x2,y2,2),0);
		put(p1);
		push(p1);
	}
//debug_output("aPath::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);

	int i,x,y,c;
	while(open) {
		p1 = pop();
//debug_output("key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\ts=%d\tg=%d\th=%d\tf=%d\tsz=%d ",p1->key,p1->x,p1->y,
//	p1->parent? p1->parent->x : -1,p1->parent? p1->parent->y : -1,p1->s,p1->g,p1->h,p1->f,size());
		if(!l || p1->s<l) for(i=0; i<=3; i++) {
			cb_move(*this,p1->x,p1->y,x,y,i);
			c = cb_move_cost(x,y,cost,map,obj);														soutput("(%d",x)soutput(",%d)",y)
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
		t = new aTrail();
//debug_output("aPath::search(p.x=%d,p.y=%d,p.g=%d)\n",p1->x,p1->y,p1->g);
		for(t->len=1,p1->open=0; p1->parent && p1->parent!=p1; t->len++,p1=p1->parent)
			p1->parent->open = p1;
//debug_output("aPath::search(trail.lenght=%d)\n",t->len);
		if(t->len>1) {
			t->trail = (aTrail::trailstep *)malloc(sizeof(aTrail::trailstep)*t->len);
			for(i=0,c='a'; p1; i++,p1=p2) {
				p2 = p1->open;
//debug_output("key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\tdir=%d\n",
//p1->key,p1->x,p1->y,p2? p2->x : -1,p2? p2->y : -1,p2? getDir(p1->x,p1->y,p2->x,p2->y) : 5);
				t->trail[i] = (aTrail::trailstep){ p1->x,p1->y,p2? getDir(p1->x,p1->y,p2->x,p2->y) : 5 };
//				mem[p1->y][p1->x] = (char)c++;
//				if(c=='z'+1) c = 'A';
//				else if(c=='Z'+1) c = 'a';
			}
//debug_output("aPath::search()\n");
		}
	}
//for(int y=0; y<height; y++) {
//for(int x=0; x<width; x++) putc(mem[y][x],stderr);
//fputc('\n',stderr);
//}
	clear();
	return t;
}


void aPath::put(node *n) {
	if(sz==full) rehash();
	int h = n->key%cap;
	n->closed = closed[h],closed[h] = n;
	sz++;
}

aPath::node *aPath::get(unsigned short key) {
	if(!sz) return 0;
	node *n = closed[key%cap];
	while(n && n->key!=key) n = n->closed;
	return n;
}

void aPath::rehash() {
	if(!closed) {
		closed = (node **)malloc(cap*sizeof(node *));
		memset(closed,0,cap*sizeof(node *));
	} else {
		int c = cap<<1;
		if(!(c&1)) c++;
		size_t i,h;
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


void aPath::push(node *p) {														coutput('p');
	node *p0 = 0,*p1 = open;
	int i=0;
	while(p1 && (p->f>p1->f || (p->f==p1->f && p->h>p1->h)))
		p0 = p1,p1 = p1->open,i++;												soutput("%dp",i);
	if(!p0) p->open = open,open = p;
	else p0->open = p,p->open = p1;
}

aPath::node *aPath::pop() {
	node *p = open;
	if(p) open = p->open;
	return p;
}

void aPath::remove(node *p) {
	if(!open) return;
	if(open!=p) {
		node *p1 = open;
		while(p1->open && p1->open!=p) p1 = p1->open;
		if(p1->open) p1->open = p1->open->open;
	} else open = open->open;
}

void aPath::clear() {
	if(closed) {
		node *n1,*n2;
		for(size_t i=0; i<cap; i++) if((n1=closed[i])) do {
			n2 = n1->closed;
			delete n1;
			n1 = n2;
		} while(n1);
		free(closed);
		closed = 0;
	}
	open = 0;
}


void aPath::setParam(int type,void *param) {
	switch(type) {
		case PATH_MAP:
			map = param;
			break;
		case PATH_CALLBACK_OBJECT:
			obj = param;
			break;
		case PATH_CB_AREA_COMPARE:
			cb_area_compare = (path_area_compare)param;
			break;
		case PATH_CB_TERRAIN_TYPE:
			cb_terrain_type = (path_terrain_type)param;
			break;
		case PATH_CB_MOVE_COST:
			cb_move_cost = (path_move_cost)param;
			break;
		case PATH_CB_MOVE:
			cb_move = (path_move)param;
			break;
	}
}

void *aPath::getParam(int type) {
	switch(type) {
		case PATH_MAP:
			return map;
		case PATH_CALLBACK_OBJECT:
			return obj;
		case PATH_CB_AREA_COMPARE:
			return (void *)cb_area_compare;
		case PATH_CB_TERRAIN_TYPE:
			return (void *)cb_terrain_type;
		case PATH_CB_MOVE_COST:
			return (void *)cb_move_cost;
		case PATH_CB_MOVE:
			return (void *)cb_move;
	}
	return 0;
}

int aPath::getHeuristic(int x1,int y1,int x2,int y2,int n) {
	if(style&PATH_HWRAP) { if(x1+width-x2<x2-x1) x1 += width;else if(x2+width-x1<x1-x2) x2 += width; }
	if(style&PATH_VWRAP) { if(y1+height-y2<y2-y1) y1 += height;else if(y2+height-y1<y1-y2) y2 += height; }
	x1 = abs(x1-x2),y1 = abs(y1-y2);
	if(n==1) return (x1>y1? x1 : y1)+(x1<y1? x1 : y1)/2;
	else if(n==2) return x1+y1;
	else return (x1>y1? x1 : y1);
}
int aPath::getDir(int x1,int y1,int x2,int y2) {
	if(style&PATH_HWRAP) { if(x1+width-x2<x2-x1) x1 += width;else if(x2+width-x1<x1-x2) x2 += width; }
	if(style&PATH_VWRAP) { if(y1+height-y2<y2-y1) y1 += height;else if(y2+height-y1<y1-y2) y2 += height; }
	//if(abs(x1-x2)<=1 && abs(y1-y2)<=1)
	return y2<y1? (x2<x1+(y1&1)? 3 : 0) : (x2<x1+(y1&1)? 2 : 1);

}
void aPath::moveIso(aPath &p,int x1,int y1,int &x2,int &y2,int dir) {
	static const int xcoords[4] = {  0, 0,-1,-1 };
	static const int ycoords[4] = { -1, 1, 1,-1 };
//debug_output("aPath::moveIso(x=%d,y=%d,dir=%d)\n",x1,y1,dir);
	x1 += xcoords[dir]+(y1&1),y1 += ycoords[dir];
	if(p.style&PATH_HWRAP) { if(x1<0) x1 += p.width;else if(x1>=p.width) x1 -= p.width; }
	else { if(x1<0) x1 = 0;else if(x1>=p.width) x1 = p.width-1; }
	if(p.style&PATH_VWRAP) { if(y1<0) y1 += p.height;else if(y1>=p.height) y1 -= p.height; }
	else { if(y1<0) y1 = 0;else if(y1>=p.height) y1 = p.height-1; }
	x2 = x1,y2 = y1;
}
//int aPath::wrapX(int x) {
//	if(style&PATH_HWRAP) { if(x<0) x += width;else if(x>=width) x -= width; }
//	else { if(x<0) x = 0;else if(x>=width) x = width-1; }
//	return x;
//}
//int aPath::wrapY(int y) {
//	if(style&PATH_VWRAP) { if(y<0) y += height;else if(y>=height) y -= height; }
//	else { if(y<0) y = 0;else if(y>=height) y = height-1; }
//	return y;
//}




