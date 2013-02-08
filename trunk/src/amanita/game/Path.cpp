
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
	static const int xcoords[8] = {  0, 0, 1, 0, 0, 0,-1, 0 };
	static const int ycoords[8] = { -1, 0, 0, 0, 1, 0, 0, 0 };
	static const int dirs[] = { PATH_NORTH,PATH_EAST,PATH_SOUTH,PATH_WEST,-1 };
	x = n.x+xcoords[dirs[i]];
	y = n.y+ycoords[dirs[i]];
	move_adjust(p,x,y);
	return dirs[i+1];
}

static int move_isometric(Path &p,PathNode &n,int &x,int &y,int i) {
	static const int xcoords[2][8] = {{  0, 0, 0, 0, 0,-1, 0,-1 },{  0, 1, 0, 1, 0, 0, 0, 0 }};
	static const int ycoords[8] = {  0,-1, 0, 1, 0, 1, 0,-1 };
	static const int dirs[] = { PATH_NORTH_EAST,PATH_SOUTH_EAST,PATH_SOUTH_WEST,PATH_NORTH_WEST,-1 };
	x = n.x+xcoords[n.y&1][dirs[i]];
	y = n.y+ycoords[dirs[i]];
	move_adjust(p,x,y);
	return dirs[i+1];
}

static int move_hexagonal(Path &p,PathNode &n,int &x,int &y,int i) {
	static const int xcoords[2][8] = {{  0, 0, 0, 0, 0,-1, 0,-1 },{  0, 1, 0, 1, 0, 0, 0, 0 }};
	static const int ycoords[8] = { -2,-1, 0, 1, 2, 1, 0,-1 };
	static const int dirs[] = { PATH_NORTH,PATH_NORTH_EAST,PATH_SOUTH_EAST,PATH_SOUTH,PATH_SOUTH_WEST,PATH_NORTH_WEST,-1 };
	x = n.x+xcoords[n.y&1][dirs[i]];
	y = n.y+ycoords[dirs[i]];
	move_adjust(p,x,y);
	return dirs[i+1];
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
	return y2==y1? (x2<x1? PATH_WEST : PATH_EAST) : (y2<y1? PATH_NORTH : PATH_SOUTH);
}

static int dir_isometric(Path &p,int x1,int y1,int x2,int y2) {
	dir_adjust(p,x1,y1,x2,y2);
	return y2<y1? (x2<x1+(y1&1)? PATH_NORTH_WEST : PATH_NORTH_EAST) : (x2<x1+(y1&1)? PATH_SOUTH_WEST : PATH_SOUTH_EAST);
}

static int dir_hexagonal(Path &p,int x1,int y1,int x2,int y2) {
	dir_adjust(p,x1,y1,x2,y2);
	x2 -= x1+(y1&1);
	if(abs(y2-y1)==2) return y2<y1? PATH_NORTH : PATH_SOUTH;
	return y2<y1? (x2<0? PATH_NORTH_WEST : PATH_NORTH_EAST) : (x2<0? PATH_SOUTH_WEST : PATH_SOUTH_EAST);
}

static int heuristic(Path &p,int x,int y,int n) {
	int dx,dy;
	p.getDestination(dx,dy);
	dir_adjust(p,x,y,dx,dy);
	x = abs(x-dx),y = abs(y-dy);
	if(n==1) return x>y? x+y/2 : y+x/2;
	else if(n==2) return x+y;
	else if(n==3) return (x+y)*2;
	else return (x>y? x : y);
}


//int Path::coords[4][8][2] = {
//	{ {-1, 1},{-1,-1},{ 0,-1},{ 0, 1},{-1, 0},{ 1, 0},{-1, 1},{-1,-1} },
//	{ { 0, 1},{ 0,-1},{ 1,-1},{ 1, 1},{-1, 0},{ 1, 0},{ 0, 1},{ 0,-1} },
//	{ { 0,-1},{ 0, 1},{-1, 1},{-1,-1},{ 1, 0},{-1, 0},{-1, 1},{-1,-1} },
//	{ { 1,-1},{ 1, 1},{ 0, 1},{ 0,-1},{ 1, 0},{-1, 0},{ 0, 1},{ 0,-1} }
//};


Path::Path(int w,int h,int s,void *m,path_weight pw)
			: width(w),height(h),style(s),map(m),weight(pw) {
	if(isOblique()) move = move_oblique,dir = dir_oblique;
	else if(isIsometric()) move = move_isometric,dir = dir_isometric;
	else if(isHexagonal()) move = move_hexagonal,dir = dir_hexagonal;
	heur = heuristic;
	obj = 0;
	open = 0;
	closed = 0,cap = 11,sz = 0,full = 0;
}

Path::~Path() {
}

void Path::setCallbackFunctions(path_move pm,path_dir pd,path_heuristic ph) {
	if(pm) move = pm;
	if(pd) dir = pd;
	if(ph) heur = ph;
}


Trail *Path::search(void *o,int x1,int y1,int x2,int y2,int l) {
	int i,d,x,y,c;
	Trail *t = 0;
	PathNode *p1 = 0,*p2 = 0;
debug_output("Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);

	obj = o,sx = x1,sy = y1,dx = x2,dy = y2;
	if(sx==dx && sy==dy) return t;

debug_output("Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);
	cap = heur(*this,sx,sy,0)*8+1;
	p1 = new PathNode(sx,sy,0,heur(*this,sx,sy,2),0);
	closest = p1;
	put(p1);
	push(p1);

debug_output("Path::search(x1=%d,y1=%d,x2=%d,y2=%d)\n",x1,y1,x2,y2);

	while(open) {
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
				open = 0;
				break;
			} else if(c!=PATH_CANNOT_MOVE && c!=PATH_AVOID_MOVE) {
				if((p2=get(x,y))) {
					if(p1->g+c<p2->g) {
						remove(p2);
						p2->parent = p1,p2->g = p1->g+c,p2->s = p1->s+1;
						push(p2);
					}
				} else {
					p2 = new PathNode(x,y,p1->g+c,heur(*this,x,y,2),p1);
					if(p2->h<closest->h || (p2->h==closest->h && p2->g<closest->g)) closest = p2;
					put(p2);
					push(p2);
				}
			}
		}
	}

char mem[height][width];memset(mem,' ',width*height);
for(int y=0; y<height; ++y) for(int x=0; x<width; x++) if(get(x,y)) mem[y][x] = '+';

	p1 = closest;
	if(p1 && p1->g) {
		t = new Trail();
//debug_output("Path::search(p.x=%d,p.y=%d,p.g=%d)\n",p1->x,p1->y,p1->g);
		for(t->len=1,p1->open=0; p1->parent && p1->parent!=p1; ++t->len,p1=p1->parent) p1->parent->open = p1;
//debug_output("Path::search(trail.lenght=%d)\n",(int)t->len);
		if(t->len>1) {
			t->trail = (Trail::step *)malloc(t->len*sizeof(Trail::step));
			for(i=0,c='a'; p1; ++i,p1=p2) {
				p2 = p1->open;
//debug_output("key=%04x\tx1=%d\ty1=%d\tx2=%d\ty2=%d\tdir=%d\n",
//p1->key,p1->x,p1->y,p2? p2->x : -1,p2? p2->y : -1,p2? dir(*this,p1->x,p1->y,p2->x,p2->y) : 5);
				t->trail[i] = (Trail::step){ p1->x,p1->y,(uint8_t)(p2? dir(*this,p1->x,p1->y,p2->x,p2->y) : 5) };

debug_output("Path::trail[%d]: x=%d, y=%d, dir=%d\n",i,(int)t->trail[i].x,(int)t->trail[i].y,(int)t->trail[i].dir);
mem[p1->y][p1->x] = (char)c++;
if(c=='z'+1) c = 'A';
else if(c=='Z'+1) c = 'a';
			}
//debug_output("Path::search()\n");
		}
	}
for(int y=0; y<height; ++y) {
for(int x=0; x<width; ++x) putc(mem[y][x],stderr);
fputc('\n',stderr);fflush(stderr);}
	clear();
	return t;
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
	int i=0;
	while(p1 && (p->g>p1->g || (p->g==p1->g && p->h>p1->h)))
		p0 = p1,p1 = p1->open,i++;
	if(!p0) p->open = open,open = p;
	else p0->open = p,p->open = p1;
}

PathNode *Path::pop() {
	PathNode *p = open;
	if(p) open = p->open;
	return p;
}

void Path::remove(PathNode *p) {
	if(!open) return;
	if(open!=p) {
		PathNode *p1 = open;
		while(p1->open && p1->open!=p) p1 = p1->open;
		if(p1->open) p1->open = p1->open->open;
	} else open = open->open;
}

void Path::clear() {
	if(closed) {
		PathNode *n1,*n2;
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

}; /* namespace a */


