
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <amanita/Math.h>
#include <amanita/Thread.h>
#include <amanita/game/Map.h>
#include <amanita/game/Path.h>
#include <amanita/gui/Application.h>
#include <amanita/gui/Graphics.h>
#include <amanita/gui/Image.h>

#ifdef USE_WIN32
#include "_resource.h"
#define amanita32_xpm 0
#endif
#ifdef USE_LINUX
#include "../icons/32x32/amanita.xpm"
#endif

#include "path.xpm"

using namespace a;
using namespace a::gui;

/*
|------------------------------------------------------------------------|TILE_RECT_WIDTH
|                                      __________TILE_CORNER_WIDTH       |
|                                      |        |                        |
------------***************************------------------------------------------
          **                           **       |                        |  | | |
          *                            |*       |                        |  | | |
        **                             | **     |                        |  | | |
        *                              |  *     |                        |  | | |
      **                               |   **----------------------------|--|-|--TILE_HALF_RECT_HEIGHT
      *                                |    *   |                        |  | |
    **                                 |     ** |                        |  | |
    *                                  |      * |                        |  | |
  **                                   |       **                        |  | |
  *                                    |        **************************-----TILE_RECT_HEIGHT
****                                   |       **------------------------|  |
    *                                  |     ** |                        |  |
    **                                 |    *   |                        |  |
      *                                |   **   |                        |  |
      **                               |  *     |                        |  |
        *                              |  *     |                        |  |
        **                             | **     |                        |  |
          *                            |*       |                        |  |
          *******************************-------|----------------------------TILE_HEIGHT
|-----------------------------------------------|TILE_WIDTH                  
|--------------------------------------|
                                       TILE_HALF_RECT_WIDTH
*/

enum {
	TILE_WIDTH					= 45,		// Width of a hex tile.
	TILE_HEIGHT					= 38,		// Height of a hex tile.
	TILE_RECT_WIDTH			= 70,		// Width of a rectangular tile, which is twice the width of a hex minus right corner.
	TILE_RECT_HEIGHT			= 19,		// Height of a rectangular tile, which is about half the hex tile height (lower part fits into other tiles).
	TILE_HALF_RECT_WIDTH		= 35,		// Width of a hex tile minus right corner (right corner isn't calculated because it fits into other tiles).
	TILE_HALF_RECT_HEIGHT	= 10,		// Half the height of a rectangular tile, which is ((tile height)-(rectangular tile height))/2.
	TILE_CORNER_WIDTH			= 10,		// Width of left and right corner of hex tile, which is (tile width)-(half rectangular tile width).
	TILE_HALF_CORNER_WIDTH	= 5,		// Half of tile corner width.
	TILE_CENTER_X				= 23,		// Width to center of tile. It's half the width of hex tile.
	TILE_CENTER_Y				= 19,		// Height to center of tile.
};


Application app(APP_SDL,"Åath","Amanita Path Search");


const int map_width = 32;
const int map_height = 32;

const int width = 1200;
const int height = 640;

Image *terrain;
bool repaint = true;

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

static const int terrain_types[] = { 80,20,40,5,20,0,30,0,0,-1 };
static const int terrain_colors[] = { 0x0000ff,0x99cc00,0x009900,0x999999,0xcccc66,0x00ff00,0xffffff,0x33cc99,0xff0000 };

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

/*static void paint_hex(int x,int y,int w,int h,int c) {
	int i,n = h/2+1,m = 0;
	for(i=0; n-i>=0; ++i) {
		m = i/2;
		g.drawLine(x+m,y+n-i,x+w-m,y+n-i,c);
		if(i>0) g.drawLine(x+m,y+n+i,x+w-m,y+n+i,c);
	}
}*/

static void paint_marker(int x,int y,int c) {
	g.drawLine(x-10,y-10,x+10,y+10,c);
	g.drawLine(x-10,y+10,x+10,y-10,c);
}

static void paint_step(int x1,int y1,int x2,int y2,int d,int c) {
	if(x2+3>-TILE_RECT_WIDTH && y2+3>-TILE_RECT_HEIGHT && x2-3<width+TILE_RECT_WIDTH && y2-3<height+TILE_RECT_HEIGHT) {
		g.drawLine(x2-3,y2-3,x2+3,y2+3,c);
		g.drawLine(x2-3,y2+3,x2+3,y2-3,c);
		g.drawLine(x2,y2,
			x2+(d==PATH_NORTH || d==PATH_SOUTH? 0 : (d==PATH_NORTH_EAST || d==PATH_EAST || d==PATH_SOUTH_EAST? 10 : -10)),
			y2+(d==PATH_EAST || d==PATH_WEST? 0 : (d==PATH_NORTH_EAST || d==PATH_NORTH || d==PATH_NORTH_WEST? -10 : 10)),c);
		if(x1+3>-TILE_RECT_WIDTH && y1+3>-TILE_RECT_HEIGHT && x1-3<width+TILE_RECT_WIDTH && y1-3<height+TILE_RECT_HEIGHT)
			g.drawLine(x1,y1,x2,y2,c);
	}
}

static void tile_to_point(int x1,int y1,int &x2,int &y2) {
	x2 = view_x+x1*TILE_RECT_WIDTH+(y1&1)*TILE_HALF_RECT_WIDTH+TILE_CENTER_X;
	if(map.isHWrap()) {
		if(x2<-TILE_RECT_WIDTH) x2 += map.getWidth()*TILE_RECT_WIDTH;
		else if(x2>=width) x2 -= map.getWidth()*TILE_RECT_WIDTH;
	}
	y2 = view_y+y1*TILE_RECT_HEIGHT+TILE_CENTER_Y;
	if(map.isVWrap()) {
		if(y2<-TILE_RECT_HEIGHT) y2 += map.getHeight()*TILE_RECT_HEIGHT;
		else if(y2>=height) x2 -= map.getHeight()*TILE_RECT_HEIGHT;
	}
}

static void paint(void *data) {
	int x,y,px,py,i,t;
	rect16_t r = { 0,0,TILE_WIDTH,TILE_HEIGHT+1 };

	while(app.isRunning()) {
		if(repaint) {
			app.lock();
			g.fillRect(0,0,width,height,0x000000);

			if(map.isHWrap()) {
				if(view_x<-(map.getWidth()*TILE_RECT_WIDTH+TILE_CORNER_WIDTH)) view_x += map.getWidth()*TILE_RECT_WIDTH;
				else if(view_x>-TILE_CORNER_WIDTH) view_x -= map.getWidth()*TILE_RECT_WIDTH;
			}
			if(map.isVWrap()) {
				if(view_y<=-(map.getHeight()*TILE_RECT_HEIGHT)) view_y += map.getHeight()*TILE_RECT_HEIGHT;
				else if(view_y>TILE_RECT_HEIGHT) view_y -= map.getHeight()*TILE_RECT_HEIGHT;
			}

			for(y=0,py=view_y; py<height; ++y,py+=TILE_RECT_HEIGHT) {
				if(py<=-TILE_HEIGHT) continue;
				if(y<0) { if(map.isVWrap()) y += map.getHeight();else continue; }
				else if(y>=map.getHeight()) { if(map.isVWrap()) y -= map.getHeight();else break; }
				for(x=0,px=view_x+(y&1)*TILE_HALF_RECT_WIDTH; px<width; ++x,px+=TILE_RECT_WIDTH) {
					if(px<=-TILE_WIDTH) continue;
					if(x<0) { if(map.isHWrap()) x += map.getWidth();else continue; }
					else if(x>=map.getWidth()) { if(map.isHWrap()) x -= map.getWidth();else break; }

					t = map.getTerrain(x,y);
					r.x = t*(TILE_WIDTH+1);
					t = terrain_colors[t];
/*					if(t==MAP_WATER) t = terrain_colors[0],r.x = 0;
					else if(t&MAP_CITY) t = terrain_colors[8],r.x = TILE_WIDTH+1;
					else if(t&MAP_SWAMP) t = terrain_colors[7],r.x = (TILE_WIDTH+1)*7;
					else if(t&MAP_TUNDRA) t = terrain_colors[6],r.x = (TILE_WIDTH+1)*6;
					else if(t&MAP_JUNGLE) t = terrain_colors[5],r.x = (TILE_WIDTH+1)*5;
					else if(t&MAP_FOREST) t = terrain_colors[2],r.x = (TILE_WIDTH+1)*2;
					else if(t&MAP_DESERT) t = terrain_colors[4],r.x = (TILE_WIDTH+1)*4;
					else if(t&MAP_MOUNTAIN) t = terrain_colors[3],r.x = (TILE_WIDTH+1)*3;
					else if(t&MAP_LAND) t = terrain_colors[1],r.x = TILE_WIDTH+1;*/
					terrain->draw(px,py-1,r);
				}
			}

			g.lock();

			if(trail) {
		//debug_output("paint(tr=%p,index=%d,length=%d)\n",u->tr,u->tr->index(),u->tr->length());fflush(stdout);
				i = trail->index();
				tile_to_point(start_x,start_y,x,y);
				while(trail->hasMoreSteps()) {
					trail->next();
					if(trail->getX()==-1 && trail->getY()==-1) break;
					tile_to_point(trail->getX(),trail->getY(),px,py);
					paint_step(x,y,px,py,trail->getDir(),0x660000);
					x = px,y = py;
				}
				trail->setIndex(i);
			}
			if(start_x!=-1 && start_y!=-1) {
				tile_to_point(start_x,start_y,px,py);
				paint_marker(px,py,0x660000);
			}
			if(dest_x!=-1 && dest_y!=-1) {
				tile_to_point(dest_x,dest_y,px,py);
				paint_marker(px,py,0x660000);
			}

			g.unlock();
			app.unlock();
			g.flip();
			repaint = false;
		}
		app.pauseFPS(12);
	}
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
int weight(Path &p,PathNode &n,int x,int y) {
	static const int land[] = { -1,2,3,6,5,4,5,4,1 };
//	static const int water[] = { 1,-1,-1,-1,-1,-1,-1,-1,-1 };
//	static const int air[] = { 1,1,1,1,1,1,1,1,1 };
	int t = map.getTerrain(x,y);
	t = land[t];
/*	if(t==MAP_WATER) t = land[0];
	else if(t&MAP_CITY) t = land[8];
	else if(t&MAP_SWAMP) t = land[7];
	else if(t&MAP_TUNDRA) t = land[6];
	else if(t&MAP_JUNGLE) t = land[5];
	else if(t&MAP_FOREST) t = land[2];
	else if(t&MAP_DESERT) t = land[4];
	else if(t&MAP_MOUNTAIN) t = land[3];
	else if(t&MAP_LAND) t = land[1];*/
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
				if(trail) {
					delete trail;
					trail = 0;
				}
				start_x = -1,start_y = -1,dest_x = -1,dest_y = -1,view_x = 0,view_y = 0;
				map.generate(map_width,map_height,terrain_types,MAP_WRAP_HORIZ);
				break;
			case KEY_1:break;
		}
		repaint = true;
	}
}

static int md = 0,mx = -1,my = -1;

static void mouse(uint16_t x,uint16_t y,uint8_t button,uint16_t clicks,bool down) {
	if(button==1) {
		if(down) md = 0;
		else if(md==0) {
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
		}
	}
	repaint = true;
}

static void motion(uint16_t x,uint16_t y,uint16_t dx,uint16_t dy,bool drag) {
	if(drag) {
//		app.lock();
		if(md==0) mx = view_x,my = view_y;
		view_x = mx+x-dx;
		view_y = my+y-dy;
		md = 1;
//		app.unlock();
		repaint = true;
	}
}

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
	map.generate(map_width,map_height,terrain_types,MAP_WRAP_HORIZ);
	terrain = new Image(path_xpm);
	app.start(paint);
	app.main();
	app.close();

	if(trail) delete trail;
	trail = 0;
	return 0;
}
