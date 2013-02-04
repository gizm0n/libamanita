
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <amanita/Math.h>
#include <amanita/Thread.h>
#include <amanita/gui/Application.h>
#include <amanita/gui/Graphics.h>
#include <amanita/ai/Path.h>
#include <amanita/sim/Map.h>

#ifdef USE_WIN32
#include "_resource.h"
#define amanita32_xpm 0
#endif
#ifdef USE_LINUX
#include "../icons/32x32/amanita.xpm"
#endif

using namespace a;
using namespace a::gui;

enum {
	TILE_WIDTH					= 45,		// Width of a hex tile.
	TILE_HEIGHT					= 38,		// Height of a hex tile.
	TILE_RECT_WIDTH			= 70,		// Width of a rectangular tile, which is twice the width of a hex minus right corner.
	TILE_RECT_HEIGHT			= 19,		// Height of a rectangular tile, which is about half the hex tile height (lower part fits into other tiles).
	TILE_HALF_RECT_WIDTH		= 35,		// Width of a hex tile minus right corner (right corner isn't calculated because it fits into other tiles).
	TILE_HALF_RECT_HEIGHT	= 10,		// Half the height of a rectangular tile, which is ((tile height)-(rectangular tile height))/2.
	TILE_CORNER_WIDTH			= 10,		// Width of left and right corner of hex tile, which is (tile width)-(half rectangular tile width).
	TILE_CENTER_X				= 23,		// Width to center of tile. It's half the width of hex tile.
	TILE_CENTER_Y				= 19,		// Height to center of tile.
};


Application app(APP_SDL,"Åath","Amanita Path Search");


const int map_width = 32;
const int map_height = 32;

const int width = 1200;
const int height = 640;

Map map;
int view_x = 0;
int view_y = 0;
int start_x = -1;
int start_y = -1;
int dest_x = -1;
int dest_y = -1;
Trail *trail = 0;

/*	MAP_WATER				= 0x00,
	MAP_LAND					= 0x01,
	MAP_FOREST				= 0x02,
	MAP_MOUNTAIN			= 0x04,
	MAP_DESERT				= 0x08,
	MAP_JUNGLE				= 0x10,
	MAP_TUNDRA				= 0x20,
	MAP_SWAMP				= 0x40,
	MAP_CITY					= 0x80,*/

int terrain_types[] = { 0,100,20,10,10,10,8,8,5 };
int terrain_colors[] = { 0x0000ff,0x99cc00,0x009900,0x999999,0xcccc66,0x00ff00,0xffffff,0x33cc99,0xff0000 };

void point_to_map(int xp,int yp,int &xm,int &ym) {
	static const char data[] = {
		5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	};
	int xh,yh,i;
	ym = (yp-view_y-TILE_HALF_RECT_HEIGHT)/TILE_RECT_HEIGHT;
	xm = (xp-view_x-(ym&1)*TILE_HALF_RECT_WIDTH)/TILE_RECT_WIDTH;
	yh = yp-((ym*TILE_RECT_HEIGHT)+view_y+TILE_HALF_RECT_HEIGHT);
	xh = xp-((xm*TILE_RECT_WIDTH)+view_x+(ym&1)*TILE_HALF_RECT_WIDTH);
	i = data[yh*TILE_RECT_WIDTH+xh];
	if(i==0);
	else if(i==1) xm += ym&1? 1 : 0,ym--;
	else if(i==2) xm += ym&1? 1 : 0,ym++;
	else if(i==4) xm -= ym&1? 0 : 1,ym++;
	else if(i==5) xm -= ym&1? 0 : 1,ym--;
	if(xm>=map.getWidth()) xm -= map.getWidth();
	if(ym>=map.getHeight()) ym -= map.getHeight();
}

static void paint_hex(int x,int y,int w,int h,int c) {
	int i,n = h/2+1,m = 0;
	for(i=0; n-i>=0; ++i) {
		m = i/2;
		g.drawLine(x+m,y+n-i,x+w-m,y+n-i,c);
		if(i>0) g.drawLine(x+m,y+n+i,x+w-m,y+n+i,c);
	}
}

static void paint_marker(int x,int y,int c) {
	g.drawLine(x-10,y-10,x+10,y+10,c);
	g.drawLine(x-10,y+10,x+10,y-10,c);
}

static void paint_step(int x1,int y1,int x2,int y2,int d,int c) {
	if(x2+3>0 && y2+3>0 && x2-3<width && y2-3<=height) {
		g.drawLine(x2-3,y2-3,x2+3,y2+3,c);
		g.drawLine(x2-3,y2+3,x2+3,y2-3,c);
		g.drawLine(x2,y2,
			x2+(d==PATH_NORTH || d==PATH_SOUTH? 0 : (d==PATH_NORTH_EAST || d==PATH_EAST || d==PATH_SOUTH_EAST? 10 : -10)),
			y2+(d==PATH_EAST || d==PATH_WEST? 0 : (d==PATH_NORTH_EAST || d==PATH_NORTH || d==PATH_NORTH_WEST? -10 : 10)),c);
		if(x1+3>0 && y1+3>0 && x1-3<width && y1-3<=height)
			g.drawLine(x1,y1,x2,y2,c);
	}
}

static void paint(void *data) {
	int x,y,px,py,i,j,k,t;

	g.fillRect(0,0,width,height,0x000000);
	g.lock();

	for(y=view_y,j=0; y<height; y+=TILE_RECT_HEIGHT,++j)
		for(x=view_x,i=0; x<width; x+=TILE_RECT_WIDTH,++i)
			if(x+TILE_RECT_WIDTH+TILE_CORNER_WIDTH>0 && y+TILE_HEIGHT>0)
				if(i>=0 && j>=0 && i<map.getWidth() && j<map.getHeight()) {
					t = terrain_colors[map.getTerrain(i,j)];
					px = x+(j&1)*36;
					py = y;
					paint_hex(px,py,TILE_WIDTH,TILE_HEIGHT,t);
					if((start_x==i && start_y==j) || (dest_x==i && dest_y==j))
						paint_marker(px+TILE_CENTER_X,py+TILE_CENTER_Y,0x660000);
				}


	if(trail) {
//debug_output("paint(tr=%p,index=%d,length=%d)\n",u->tr,u->tr->index(),u->tr->length());fflush(stdout);
		i = trail->index();
		k = trail->getDir();
		x = view_x+start_x*TILE_RECT_WIDTH+(start_y&1)*TILE_HALF_RECT_WIDTH+TILE_CENTER_X;
		y = view_y+start_y*TILE_RECT_HEIGHT+TILE_CENTER_Y;
		while(trail->hasMoreSteps()) {
			trail->next();
			j = (k+3)%6;
			k = trail->getDir();
			px = view_x+trail->getX()*TILE_RECT_WIDTH+(trail->getY()&1)*TILE_HALF_RECT_WIDTH+TILE_CENTER_X;
			py = view_y+trail->getY()*TILE_RECT_HEIGHT+TILE_CENTER_Y;
			paint_step(x,y,px,py,trail->getDir(),0x660000);
			x = px,y = py;
		}
		trail->setIndex(i);
	}

	g.unlock();
	g.flip();
}

/*
	MAP_WATER,
	MAP_LAND,
	MAP_FOREST,
	MAP_MOUNTAIN,
	MAP_DESERT,
	MAP_JUNGLE,
	MAP_TUNDRA,
	MAP_SWAMP,
	MAP_CITY,
*/
int weight(Path &p,int x1,int y1,int x2,int y2) {
	static const int land[] = { -1,2,3,6,5,4,5,4,1 };
//	static const int water[] = { 1,-1,-1,-1,-1,-1,-1,-1,-1 };
//	static const int air[] = { 1,1,1,1,1,1,1,1,1 };
	int t = map.getTerrain(x2,y2);
	t = land[t];
	if(t==-1) return PATH_CANNOT_MOVE;
	return t;
}

static void key(uint32_t sym,uint32_t mod,uint32_t unicode,bool down) {
fprintf(stderr,"key: [sym: 0x%x, mod: 0x%x unicode: %c]\n",sym,mod,unicode);
	if(down) {
		switch(sym) {
			case KEY_ESC:app.quit();break;
			case KEY_SPACE:
			case KEY_RETURN:
				map.generate(map_width,map_height,MAP_WRAP_HORIZ,terrain_types);
				paint(0);
				break;
			case KEY_1:break;
		}
	}
}

static void mouse(uint16_t x,uint16_t y,uint8_t button,uint16_t clicks,bool down) {
	if(button==1 && down) {
		if(start_x==-1 || start_y==-1 || trail) {
			point_to_map(x,y,start_x,start_y);
			dest_x = -1;
			dest_y = -1;
			if(trail) delete trail;
			trail = 0;
		} else {
			Path p(map_width,map_height,PATH_HWRAP|PATH_HEXAGONAL,0,weight);
			point_to_map(x,y,dest_x,dest_y);
			trail = p.search(0,start_x,start_y,dest_x,dest_y);
		}
	} else if(button==3 && down) {
		view_x += width/2-x;
		view_y += height/2-y;
	}
	paint(0);
}

static void motion(uint16_t x,uint16_t y,uint16_t dx,uint16_t dy,bool drag) {}

int main(int argc,char *argv[]) {
//fprintf(stderr,"main\n");
	const WindowIcon icons[] = {
		{ 16,AMANITA_ICON,0,0 },
		{ 32,0,amanita32_xpm,0 },
		{ 48,AMANITA_ICON,0,0 },
	{0}};
	app.setIcons(icons);
	app.setEvents(key,mouse,motion);
	app.open(argc,argv);
	app.init(width,height);
	map.generate(map_width,map_height,MAP_WRAP_HORIZ,terrain_types);
	paint(0);
	app.main();
	app.close();

	if(trail) delete trail;
	trail = 0;
	return 0;
}
