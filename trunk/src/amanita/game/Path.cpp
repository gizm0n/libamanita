
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <amanita/game/Path.h>



namespace a {


void Trail::setSteps(const step *s,int l,int i) {
	if(trail) free(trail);
	trail = (step *)malloc(l*sizeof(step));
	memcpy(trail,s,l*sizeof(step));
	ind = i,len = l;
}


static inline void move_adjust(Path &p,int &x,int &y) {
	if(p.isHWrap()) {
		if(x<0) x += p.getWidth();
		else if(x>=p.getWidth()) x -= p.getWidth();
	} else {
		if(x<0) x = 0;
		else if(x>=p.getWidth()) x = p.getWidth()-1;
	}
	if(p.isVWrap()) {
		if(y<0) y += p.getHeight();
		else if(y>=p.getHeight()) y -= p.getHeight();
	} else {
		if(y<0) y = 0;
		else if(y>=p.getHeight()) y = p.getHeight()-1;
	}
}


static int move_oblique(Path &p,PathNode &n,int &x,int &y,int i) {
	static const int xcoords[8] = {  0, 1, 0,-1 };
	static const int ycoords[8] = { -1, 0, 1, 0 };
	x = n.x+xcoords[i];
	y = n.y+ycoords[i];
	move_adjust(p,x,y);
	return i<3? 1 : -1;
}

static int move_isometric(Path &p,PathNode &n,int &x,int &y,int i) {
	static const int xcoords[2][8] = {{  0, 0,-1,-1 },{   1, 1, 0, 0 }};
	static const int ycoords[8] = { -1, 1, 1,-1 };
	x = n.x+xcoords[n.y&1][i];
	y = n.y+ycoords[i];
	move_adjust(p,x,y);
	return i<3? 1 : -1;
}

static int move_hexagonal(Path &p,PathNode &n,int &x,int &y,int i) {
	static const int xcoords[2][8] = {{  0, 0, 0, 0,-1,-1 },{  0, 1, 1, 0, 0, 0 }};
	static const int ycoords[8] = { -2,-1, 1, 2, 1,-1 };
	x = n.x+xcoords[n.y&1][i];
	y = n.y+ycoords[i];
	move_adjust(p,x,y);
	return i<5? 1 : -1;
}

static inline void dir_adjust(Path &p,int &x1,int &y1,int &x2,int &y2) {
	if(p.isHWrap()) {
		if(x1+p.getWidth()-x2<x2-x1) x1 += p.getWidth();
		else if(x2+p.getWidth()-x1<x1-x2) x2 += p.getWidth();
	}
	if(p.isVWrap()) {
		if(y1+p.getHeight()-y2<y2-y1) y1 += p.getHeight();
		else if(y2+p.getHeight()-y1<y1-y2) y2 += p.getHeight();
	}
}

static int dir_oblique(Path &p,int x1,int y1,int x2,int y2) {
	dir_adjust(p,x1,y1,x2,y2);
	return y2==y1? (x2<x1? PATH_W : PATH_E) : (y2<y1? PATH_N : PATH_S);
}

static int dir_isometric(Path &p,int x1,int y1,int x2,int y2) {
	dir_adjust(p,x1,y1,x2,y2);
	return y2<y1? (x2<x1+(y1&1)? PATH_NW : PATH_NE) : (x2<x1+(y1&1)? PATH_SW : PATH_SE);
}

static int dir_hexagonal(Path &p,int x1,int y1,int x2,int y2) {
	dir_adjust(p,x1,y1,x2,y2);
	x2 -= x1+(y1&1);
	if(abs(y2-y1)==2) return y2<y1? PATH_N : PATH_S;
	return y2<y1? (x2<0? PATH_NW : PATH_NE) : (x2<0? PATH_SW : PATH_SE);
}

static int heuristic_oblique(Path &p,int x,int y) {
	int dx,dy;
	p.getDestination(dx,dy);
	dir_adjust(p,x,y,dx,dy);
	x = abs(x-dx),y = abs(y-dy);
	return x>y? x+y/2 : y+x/2;
//	return x+y;
//	return (x+y)*2;
//	return (x>y? x : y);
}

static int heuristic_isometric(Path &p,int x,int y) {
	int dx,dy;
	p.getDestination(dx,dy);
	dir_adjust(p,x,y,dx,dy);
	x = abs(x-dx)/2,y = abs(y-dy);
	return x>y? x+y/2 : y+x/2;
//	return x+y;
//	return (x+y)*2;
//	return (x>y? x : y);
}

static int heuristic_hexagonal(Path &p,int x,int y) {
	int dx,dy;
	p.getDestination(dx,dy);
	dir_adjust(p,x,y,dx,dy);
	x = abs(x-dx)*2,y = abs(y-dy);
	return (x>y? x+y/2 : y+x/2);
//	return x+y;
//	return (x+y)*2;
//	return (x>y? x : y);
}


//int Path::coords[4][8][2] = {
//	{ {-1, 1},{-1,-1},{ 0,-1},{ 0, 1},{-1, 0},{ 1, 0},{-1, 1},{-1,-1} },
//	{ { 0, 1},{ 0,-1},{ 1,-1},{ 1, 1},{-1, 0},{ 1, 0},{ 0, 1},{ 0,-1} },
//	{ { 0,-1},{ 0, 1},{-1, 1},{-1,-1},{ 1, 0},{-1, 0},{-1, 1},{-1,-1} },
//	{ { 1,-1},{ 1, 1},{ 0, 1},{ 0,-1},{ 1, 0},{-1, 0},{ 0, 1},{ 0,-1} }
//};


Path::Path(int w,int h,int s,void *m,path_weight pw)
			: width(w),height(h),style(s),map(m),weight(pw) {
	if(isOblique()) move = move_oblique,dir = dir_oblique,heur = heuristic_oblique;
	else if(isIsometric()) move = move_isometric,dir = dir_isometric,heur = heuristic_isometric;
	else if(isHexagonal()) move = move_hexagonal,dir = dir_hexagonal,heur = heuristic_hexagonal;
	step = 0;
	obj = 0;
	open = 0;
	closed = 0,cap = 11,sz = 0,full = 0;
}

Path::~Path() {
	clear();
}

void Path::setCallbackFunctions(path_move pm,path_dir pd,path_heuristic ph,path_step ps) {
	if(pm) move = pm;
	if(pd) dir = pd;
	if(ph) heur = ph;
	if(ps) step = ps;
}


Trail *Path::getTrail(PathNode *n) {
	Trail *t = 0;
	if(n) {
		int i,x,y;
		PathNode *p1 = 0,*p2 = 0;

char mem[height][width];memset(mem,' ',width*height);
for(y=0; y<height; ++y) for(x=0; x<width; ++x) if(get(x,y)) mem[y][x] = '+';

		if(n && n->g) {
//debug_output("Path::search(p.x=%d,p.y=%d,p.g=%d)\n",p1->x,p1->y,p1->g);
			for(p1=n,i=0; p1 && p1->parent!=p1; ++i,p1=p1->parent);
//debug_output("Path::search(trail.lenght=%d)\n",(int)t->len);
			if(!p1 && i>1) {
				t = new Trail();
				t->obj = obj;
				t->len = i;
				t->trail = (Trail::step *)malloc(t->len*sizeof(Trail::step));
				for(p1=n,p2=0,i=t->len-1; p1; --i,p2=p1,p1=p1->parent) {
//debug_output("key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\tdir=%d\n",
//p1->key,p1->x,p1->y,p2? p2->x : -1,p2? p2->y : -1,p2? dir(*this,p1->x,p1->y,p2->x,p2->y) : 5);
					t->trail[i] = (Trail::step){ p1->x,p1->y,(uint8_t)(p2? dir(*this,p1->x,p1->y,p2->x,p2->y) : 5) };

debug_output("Path::trail[%d]: x=%d, y=%d, dir=%d\n",i,(int)t->trail[i].x,(int)t->trail[i].y,(int)t->trail[i].dir);
mem[p1->y][p1->x] = (char)('a'+((t->len-1)%('z'-'a')));
				}
//debug_output("Path::search()\n");
			}
		}
for(y=0; y<height; ++y) {
for(x=0; x<width; ++x) putc(mem[y][x],stderr);
fputc('\n',stderr);fflush(stderr);}
	}
	return t;
}

bool Path::isOpen(int x,int y) {
	if(open) {
		PathNode *n = get(x,y),*p = open;
		for(; p; p=p->open) if(p==n) return true;
	}
	return false;
}

Trail *Path::searchPath(void *o,int x1,int y1,int x2,int y2,int l) {
	int i,d,x,y,c;
	PathNode *p1 = 0,*p2 = 0;
debug_output("Path::searchPath(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);

	clear();

	obj = o,sx = x1,sy = y1,dx = x2,dy = y2;
	if(sx==dx && sy==dy) return 0;

debug_output("Path::searchPath(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
	cap = heur(*this,sx,sy)*8+1;
	p1 = new PathNode(sx,sy,0,heur(*this,sx,sy),0);
	closest = p1;
	put(p1);
	push(p1);

debug_output("Path::searchPath(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);

	while(open) {
		if(step) step(*this,*open);
		p1 = pop();
debug_output("key=%08x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\ts=%d\tg=%d\th=%d\n",p1->key,p1->x,p1->y,
p1->parent? p1->parent->x : -1,p1->parent? p1->parent->y : -1,p1->s,p1->g,p1->h);
		if(!l || p1->s<l) for(i=0,d=0; d!=-1; ++i) {
			d = move(*this,*p1,x,y,i);
			c = weight(*this,*p1,x,y);
			if(x==dx && y==dy) {
				if(c!=PATH_CANNOT_MOVE) {
					p1 = new PathNode(x,y,p1->g+1,0,p1);
					closest = p1;
					put(p1);
				}
				goto finished;
			} else if(c!=PATH_CANNOT_MOVE && c!=PATH_AVOID_MOVE) {
				if((p2=get(x,y))) {
					if(p1->g+c<p2->g) {
						remove(p2);
						p2->parent = p1,p2->g = p1->g+c,p2->s = p1->s+1;
						push(p2);
					}
				} else {
					p2 = new PathNode(x,y,p1->g+c,heur(*this,x,y),p1);
					if(p2->h<closest->h || (p2->h==closest->h && p2->g<closest->g)) closest = p2;
					put(p2);
					push(p2);
				}
			}
		}
	}

finished:

	return getTrail(closest);
}


void Path::searchReach(void *o,int x,int y,int s,int m) {
	int i,d,c;
	PathNode *p1 = 0,*p2 = 0;
debug_output("Path::searchReach(x=%d,y=%d)\n",x,y);

	clear();
	closest = 0;
	obj = o,sx = x,sy = y,dx = -1,dy = -1;
	if(sx<0 || sy<0 || sx>=width || sy>=height || (s==0 && m==0)) return;

	cap = m*m*3;
	p1 = new PathNode(sx,sy,0,0,0);
	put(p1);
	push(p1);


	while(open) {
		if(step) step(*this,*open);
		p1 = pop();
debug_output("key=%08x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\ts=%d\tg=%d\th=%d\n",p1->key,p1->x,p1->y,
p1->parent? p1->parent->x : -1,p1->parent? p1->parent->y : -1,p1->s,p1->g,p1->h);
		for(i=0,d=0; d!=-1; ++i) {
			d = move(*this,*p1,x,y,i);
			c = weight(*this,*p1,x,y);
			if(c!=PATH_CANNOT_MOVE && c!=PATH_AVOID_MOVE) {
				if((p2=get(x,y))) {
					if(p1->g+c<p2->g) {
						remove(p2);
						p2->parent = p1,p2->g = p1->g+c,p2->s = p1->s+1;
						push(p2);
					}
				} else if((s==0 || p1->s+1<=s) && (m==0 || p1->g+c<=m)) {
					p2 = new PathNode(x,y,p1->g+c,0,p1);
					put(p2);
					push(p2);
				}
			}
		}
	}

{char mem[height][width];memset(mem,' ',width*height);
for(y=0; y<height; ++y) for(x=0; x<width; ++x) if((p1=get(x,y))) mem[y][x] = '0'+p1->g;
for(y=0; y<height; ++y) {
for(x=0; x<width; ++x) putc(mem[y][x],stderr);
fputc('\n',stderr);fflush(stderr);}}
}





void Path::put(PathNode *n) {
	if(sz==full) rehash();
	int h = n->key%cap;
	n->closed = closed[h],closed[h] = n;
	sz++;
}

PathNode *Path::get(uint32_t key) {
	if(!sz) return 0;
	PathNode *n = closed[key%cap];
	while(n && n->key!=key) n = n->closed;
	return n;
}

void Path::rehash() {
	if(!closed) {
		closed = (PathNode **)malloc(cap*sizeof(PathNode *));
		memset(closed,0,cap*sizeof(PathNode *));
	} else {
		int c = cap<<1;
		if(!(c&1)) c++;
		size_t i,h;
		PathNode **t = (PathNode **)malloc(c*sizeof(PathNode *));
		memset(t,0,c*sizeof(PathNode *));
		PathNode *n1,*n2;
		for(i=0; i<cap; i++) if((n1=closed[i])) while(n1)
			n2 = n1->closed,h = n1->key%c,n1->closed = t[h],t[h] = n1,n1 = n2;
		free(closed);
		closed = t,cap = c;
	}
	full = cap>>1;
}

void Path::push(PathNode *p) {
	PathNode *p0 = 0,*p1 = open;
//	while(p1 && (p->g>p1->g || (p->g==p1->g && p->h>p1->h))) p0 = p1,p1 = p1->open;
//	while(p1 && (p->h>p1->h || (p->h==p1->h && p->g>p1->g))) p0 = p1,p1 = p1->open;
	while(p1 && p->g+p->h>p1->g+p1->h) p0 = p1,p1 = p1->open;
	if(!p0) p->open = open,open = p;
	else p0->open = p,p->open = p1;
}

PathNode *Path::pop() {
	PathNode *p = open;
	if(p) open = p->open,p->open = 0;
	return p;
}

void Path::remove(PathNode *p) {
	if(!open) return;
	if(open!=p) {
		PathNode *p1 = open;
		while(p1->open && p1->open!=p) p1 = p1->open;
		if(p1->open==p) p1->open = p1->open->open,p->open = 0;
	} else open = open->open,p->open = 0;
}

void Path::clear() {
	if(closed) {
		size_t i;
		PathNode *n1,*n2;
		for(i=0; i<cap; ++i) if((n1=closed[i])) do {
			n2 = n1->closed;
			delete n1;
			n1 = n2;
		} while(n1);
		free(closed);
		closed = 0;
	}
	open = 0;
}

}; /* namespace a */


