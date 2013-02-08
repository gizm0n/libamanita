
#include "../_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amanita/game/Map.h>

using namespace a;


Map::Map(Random *r) {
	width = 0;
	height = 0;
	size = 0;
	wrap = 0;
	terrain = 0;
	map = 0;
	chaos = r? r : &rnd;
}

Map::~Map() {
	if(map) free(map);
	map = 0;
}

void Map::clear(int *m,int x,int y,int w,int h) {
	int i,j;
	for(j=y; j<y+h; ++j)
		for(i=x; i<x+w; ++i) m[i+j*width] = 0;
}

void Map::rect(int *m,int x,int y,int w,int h,int p) {
	int i,j;
	Random &r = *chaos;
	for(j=y; j<y+h; ++j)
		for(i=x; i<x+w; ++i)
			if(r.int32(100)<=p) m[i+j*width] = 1;
}

void Map::erode(int *m,int s) {
	int i,j,n,x,y,x1;
	bool hw = (wrap&MAP_WRAP_HORIZ),vw = (wrap&MAP_WRAP_VERT);
	Random &r = *chaos;
	for(y=0; y<height; ++y)
		for(x=0; x<width; ++x) {
			i = 0;
			x1 = x+(y&1);
			if(hw || x1<width) {
				if(x1>=width) x1 -= width;
				i |= y-1>=0? m[(y-1)*width+x1] : (vw? m[(y-1)*width+x1] : 0);
				i |= y+1<height? (m[(y+1)*width+x1]<<1) : (vw? (m[(y-height+1)*width+x1]<<1) : 0);
			}
			x1 = x+(y&1)-1;
			if(hw || x1>=0) {
				if(x1<0) x1 += width;
				i |= y+1<height? (m[(y+1)*width+x1]<<3) : (vw? (m[(y+1-height)*width+x1]<<3) : 0);
				i |= y-1>=0? (m[(y-1)*width+x1]<<4) : (vw? (m[(y+height-1)*width+x1]<<4) : 0);
			}
			i |= y+2<height? (m[(y+2)*width+x]<<2) : (vw? (m[(y+2-height)*width+x]<<2) : 0);
			i |= y-2>=0? (m[(y-2)*width+x]<<5) : (vw? (m[(y+height-2)*width+x]<<5) : 0);
			if(i) {
				for(j=0,n=0; j<6; ++j) if(i&(1<<j)) ++n;
				if(s==MAP_ERODE_BOTH) {
					if(m[y*width+x]) { if(n<6 && (n==0 || !r.uint32(n))) m[y*width+x] = 0; }
					else if(n>0 && (n==6 || !r.uint32(6-n))) m[y*width+x] = 1;
				} else if(s==MAP_ERODE_INC) {
					if(!m[y*width+x] && (n==6 || !r.uint32(6-n))) m[y*width+x] = 1;
				} else if(s==MAP_ERODE_DEC) {
					if(m[y*width+x] && (n==0 || !r.uint32(n))) m[y*width+x] = 0;
				}
			}
		}
}

void Map::makeLayer(int *l,int t,int p) {
	int i,n,e1,e2,e3;
debug_output("Map::makeLayer(t:%d, p:%d)\n",t,p);
	memset(l,0,size*sizeof(int));
	if(t==MAP_LAND) {
		rect(l,0,0,width,height,p);
		e1 = 3,e2 = 2,e3 = 3;
	} else if(t==MAP_FOREST) {
		n = height*20/100;
		rect(l,0,n,width,n,p);
		rect(l,0,height-n*2,width,n,p);
		e1 = 2,e2 = 1,e3 = 2;
	} else if(t==MAP_MOUNTAIN) {
		rect(l,0,0,width,height,p);
		e1 = 1,e2 = 0,e3 = 2;
	} else if(t==MAP_DESERT) {
		n = height*10/100;
		rect(l,0,(height-n)/2,width,n,p);
		e1 = 1,e2 = 0,e3 = 1;
	} else if(t==MAP_JUNGLE) {
		n = height*30/100;
		rect(l,0,(height-n)/2,width,n,p);
		e1 = 1,e2 = 0,e3 = 1;
	} else if(t==MAP_TUNDRA) {
		n = height*8/100;
		rect(l,0,0,width,n,p);
		rect(l,0,height-n,width,n,p);
		e1 = 2,e2 = 1,e3 = 2;
	} else if(t==MAP_SWAMP) {
		n = height*8/100;
		rect(l,0,0,width,n,p);
		rect(l,0,height-n,width,n,p);
		e1 = 1,e2 = 0,e3 = 1;
	} else if(t==MAP_CITY) {
		rect(l,0,0,width,height,p);
		e1 = 0,e2 = 0,e3 = 0;
	}
debug_output("Map::makeLayer(erode)\n");
	for(i=0; i<e1; ++i) erode(l,MAP_ERODE_INC);
	for(i=0; i<e2; ++i) erode(l,MAP_ERODE_DEC);
	for(i=0; i<e3; ++i) erode(l,MAP_ERODE_BOTH);
debug_output("Map::makeLayer(transfer)\n");
	if(t==MAP_LAND) {
		for(i=0; i<size; ++i) map[i] = l[i]? MAP_LAND : MAP_WATER;
	} else {
		if(t==MAP_DESERT) {
			n = height*16/100;
			clear(l,0,0,width,n);
			clear(l,0,height-n,width,n);
		} else if(t==MAP_JUNGLE) {
			n = height*30/100;
			clear(l,0,0,width,n);
			clear(l,0,height-n,width,n);
		} else if(t==MAP_SWAMP || t==MAP_TUNDRA) {
			n = height*16/100;
			clear(l,0,n,width,height-n*2);
		}
		for(i=0; i<size; ++i)
			if(l[i] && (map[i]!=MAP_WATER)) map[i] = t;
	}
debug_output("Map::makeLayer(done)\n");
}

void Map::generate(int w,int h,const int t[],int wr) {
	int x,y,i;
	int layer[w*h];
	bool hw = (wr&MAP_WRAP_HORIZ),vw = (wr&MAP_WRAP_VERT);
//	Random &r = *chaos;

debug_output("Map::generate(w:%d, h:%d)\n",w,h);
	width = w;
	height = h;
	size = w*h;
	wrap = wr;
	map = (int *)malloc(size*sizeof(int));
	memset(map,0,size*sizeof(int));

	for(i=1; t[i]!=-1; ++i)
		if(t[i]>0) makeLayer(layer,i,t[i]);

	// Make sure unwrapped map edges have water
	if(!hw) for(y=0; y<height; y++) map[y*w] = map[y*w+w-1] = MAP_WATER;
	if(!vw) for(x=0; x<w; x++) map[x] = map[w+x] = map[(height-2)*w+x] = map[(height-1)*w+x] = MAP_WATER;

	for(y=0; y<height; y++) {
		for(x=0; x<width; x++) putchar((map[x+y*width]&MAP_LAND)? '#' : ' ');
		putchar('\n');
	}
}


