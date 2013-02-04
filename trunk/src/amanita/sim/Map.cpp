
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amanita/sim/Map.h>

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

void Map::rect(int *m,int x,int y,int w,int h,int t) {
	int i,j;
	for(j=y; j<y+h; ++j) for(i=x; i<x+w; ++i) m[i+j*width] = t;
}

void Map::erode(int *m,int s) {
	int i,j,n,x,y,x1;
	bool hw = (wrap&MAP_WRAP_HORIZ),vw = (wrap&MAP_WRAP_VERT);
	Random &r = *chaos;
	for(y=0; y<height; y++)
		for(x=0; x<width; x++) {
			x1 = x+(y&1),i = 0;
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
				for(j=0,n=0; j<6; j++) if(i&(1<<j)) n++;
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

void Map::makeLayer(int *l,int t,int e1,int e2,int e3) {
	int i,n;
	for(i=0; i<e1; ++i) erode(l,MAP_ERODE_INC);
	for(i=0; i<e2; ++i) erode(l,MAP_ERODE_DEC);
	for(i=0; i<e3; ++i) erode(l,MAP_ERODE_BOTH);
	if(t==MAP_LAND) {
		for(i=0; i<size; ++i) map[i] = l[i]? MAP_LAND : MAP_WATER;
	} else {
		if(t==MAP_DESERT) {
			n = height*16/100;
			rect(l,0,0,width,n,0);
			rect(l,0,height-n,width,n,0);
		} else if(t==MAP_TUNDRA) {
			n = height*16/100;
			rect(l,0,n,width,height-n*2,0);
		}
		for(i=0; i<size; ++i)
			if(l[i] && map[i]!=MAP_WATER) map[i] = t;
	}
}

void Map::generate(int w,int h,int wr,int t[]) {
	int x,y,n;
	bool hw = (wr&MAP_WRAP_HORIZ),vw = (wr&MAP_WRAP_VERT);
//	Random &r = *chaos;

	width = w;
	height = h;
	size = w*h;
	wrap = wr;
	map = (int *)malloc(size*sizeof(int));
	memset(map,0,size*sizeof(int));
	{
		int layer[size];
		memset(layer,0,sizeof(layer));
		for(y=8; y<height; y+=8) rect(layer,0,y,width,1,1);
		makeLayer(layer,MAP_LAND,3,1,3);
		if(t[MAP_FOREST]>=1) {
			memset(layer,0,sizeof(layer));
			for(y=8; y<height; y+=8) rect(layer,0,y,width,1,1);
			makeLayer(layer,MAP_FOREST,3,1,3);
		}
		if(t[MAP_DESERT]>=1) {
			n = height*t[MAP_DESERT]/100;
			memset(layer,0,sizeof(layer));
			rect(layer,0,(height-n)/2,width,n,1);
			makeLayer(layer,MAP_DESERT,1,0,1);
		}
		if(t[MAP_TUNDRA]>=1) {
			n = height*t[MAP_TUNDRA]/100;
			memset(layer,0,sizeof(layer));
			rect(layer,0,0,width,n,1);
			rect(layer,0,height-n,width,n,1);
			makeLayer(layer,MAP_TUNDRA,1,0,1);
		}
	}

	// Make sure unwrapped map edges have water
	if(!hw) for(y=0; y<height; y++) map[y*w] = map[y*w+w-1] = MAP_WATER;
	if(!vw) for(x=0; x<w; x++) map[x] = map[w+x] = map[(height-2)*w+x] = map[(height-1)*w+x] = MAP_WATER;

	for(y=0; y<height; y++) {
		for(x=0; x<width; x++) putchar((map[x+y*width]&MAP_LAND)? '#' : ' ');
		putchar('\n');
	}
}


