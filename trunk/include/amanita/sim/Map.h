#ifndef _AMANITA_SIM_MAP_H
#define _AMANITA_SIM_MAP_H

/**
 * @file amanita/sim/Map.h  
 * @author Per Löwgren
 * @date Modified: 2013-02-03
 * @date Created: 2013-02-03
 */ 

#include <amanita/Random.h>

/** Amanita Namespace */
namespace a {


enum WRAP {
	MAP_WRAP_NONE			= 0,
	MAP_WRAP_HORIZ			= 1,
	MAP_WRAP_VERT			= 2,
	MAP_WRAP_BOTH			= 3
};

enum {
	MAP_WATER,
	MAP_LAND,
	MAP_FOREST,
	MAP_MOUNTAIN,
	MAP_DESERT,
	MAP_JUNGLE,
	MAP_TUNDRA,
	MAP_SWAMP,
	MAP_CITY,
};

enum {
	MAP_ERODE_INC,
	MAP_ERODE_DEC,
	MAP_ERODE_BOTH
};

class Map {
private:
	int width;									//!< Map width - Number of points
	int height;									//!< Map height - Number of points
	int size;									//!< Map width*height - Number of points
	int wrap;									//!< Wrapping of the planet - Default horizontal wrapping, not at the poles
	int terrain;
	int *map;

	Random *chaos;

	void rect(int *m,int x,int y,int w,int h,int t);
	void erode(int *m,int s);
	void makeLayer(int *l,int t,int e1,int e2,int e3);

public:
	Map(Random *r=0);
	~Map();

	int getWidth() { return width; }
	int getHeight() { return height; }
	int getTerrain(int x,int y) { return x>=0 && y>=0 && x<width && y<height? map[x+y*width] : MAP_WATER; }

	void generate(int w,int h,int wr,int t[]);
};



}; /* namespace a */


#endif /* _AMANITA_SIM_MAP_H */

