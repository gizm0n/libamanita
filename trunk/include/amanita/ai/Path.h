#ifndef _AMANITA_AI_PATH_H
#define _AMANITA_AI_PATH_H

/**
 * @file amanita/ai/Path.h  
 * @author Per Löwgren
 * @date Modified: 2013-01-20
 * @date Created: 2008-09-07
 */ 

#include <stdio.h>
#include <stdint.h>


/** Amanita Namespace */
namespace a {

enum {
	PATH_VWRAP							= 0x0001,		//!< Wrap the map that is searched vertically, meaning top and bottom sides overlap.
	PATH_HWRAP							= 0x0002,		//!< Wrap the map that is searched horizontally, meaning left and right sides overlap.
	PATH_OBLIQUE						= 0x0004,		//!< The map is oblique (rectangular tiles).
	PATH_ISOMETRIC						= 0x0008,		//!< The map is hexagonal (hexagon shaped tiles).
	PATH_HEXAGONAL						= 0x0010,		//!< The map is isometric (diamond shaped tiles).
};

enum {
	PATH_MAP,
	PATH_CALLBACK_OBJECT,
	PATH_CB_AREA_COMPARE,
	PATH_CB_TERRAIN_TYPE,
	PATH_CB_MOVE_COST,
	PATH_CB_MOVE,
};

enum {
	PATH_NORTH,
	PATH_NORTH_EAST,
	PATH_EAST,
	PATH_SOUTH_EAST,
	PATH_SOUTH,
	PATH_SOUTH_WEST,
	PATH_WEST,
	PATH_NORTH_WEST,
	PATH_UP,
	PATH_DOWN,
	PATH_RIGHT,
	PATH_LEFT,
};

enum {
	PATH_CANNOT_MOVE					= 0xffff,		//!< 
	PATH_AVOID_MOVE					= 0xfffe,		//!< 
};


/** @cond */
class Path;
/** @endcond */


/** Test if can move at coordinate, callback
 * @param p Path object
 * @param x1 X-coordinate, leaving
 * @param y1 Y-coordinate, leaving
 * @param x2 X-coordinate, entering
 * @param y2 Y-coordinate, entering
 * @return True if possible to move from (x1,y1) to (x2,y2).
 * @see Path::can_move */
typedef bool (*path_can_move)(Path &p,int x1,int y1,int x2,int y2);

/** Weight, or cost to move, callback
 * The weight or cost to move from (x1,y1) to (x2,y2), where distance is always one
 * step. Commonly only (x2,y2) is used. A very simple implementation could
 * look like this:
 * @code
int weight(Path &p,int x1,int y1,int x2,int y2) {
	int *map = (int *)p.getMap();
	int t = map[x2+y2*p.getWidth()];
	Piece *piece = (Piece *)p.getObject();
	if(piece.type==HERO) {
		if(t==ROAD || t==CITY) return 1;
		else if(t==LAND) return 2;
		else if(t==MOUNTAIN) return 7;
		else if(t==FOREST) return 4;
		else if(t==WATER) return PATH_CANNOT_MOVE;
		else return PATH_AVOID_MOVE;
	} else if(piece.type==SHIP) {
		if(t==WATER) return 1;
		else return PATH_CANNOT_MOVE;
	} else if(piece.type==PLANE) return 1;
}
 * @endcode
 * 
 * Notice that the actual move-cost may be different. The returned value
 * should be a relative weight between 0-65533 in comparison
 * to other terrain-types, distances, time to move, friction, or similar.
 * @param p Path object
 * @param x1 X-coordinate, leaving
 * @param y1 Y-coordinate, leaving
 * @param x2 X-coordinate, entering
 * @param y2 Y-coordinate, entering
 * @return The function should return the weight (a value between 0-65533), or if impassable PATH_CANNOT_MOVE or PATH_AVOID_MOVE.
 * @see Path::weight */
typedef int (*path_weight)(Path &p,int x1,int y1,int x2,int y2);

/** Move, callback
 * Differs depending on what type of map, i.e. hexagonal, isometric. This can be custom made, or else
 * is handled by internal methods.
 * @param p Path object
 * @param x1 X-coordinate, leaving
 * @param y1 Y-coordinate, leaving
 * @param x2 X-coordinate, entering
 * @param y2 Y-coordinate, entering
 * @param dir
 * @see Path::move */
typedef void (*path_move)(Path &p,int x1,int y1,int &x2,int &y2,int dir);

/** Get direction, callback
 * The direction is used by the path_move-callback function to determine
 * the coordinate for the step and the dir-values of the returned Trail-object.
 * @param p Path object
 * @param x1 X-coordinate, location
 * @param y1 Y-coordinate, location
 * @param x2 X-coordinate, destination
 * @param y2 Y-coordinate, destination
 * @return Direction index, the direction in which to enter (x2,y2) from (x1,y1).
 * @see Path::dir */
typedef int (*path_dir)(Path &p,int x1,int y1,int x2,int y2);

/** Heuristic value, callback
 * An approximate distance between (x1,y1) and (x2,y2), where (x1,y1) is the location the search is
 * positioned at each step in the search and (x2,y2) is the destination. The returned value should be
 * an approximation as close as possible to the real distance, where a lower value generally calculates
 * a more direct path, while a higher value accepts paths a bit more off.
 * @param p Path object
 * @param x1 X-coordinate, leaving
 * @param y1 Y-coordinate, leaving
 * @param x2 X-coordinate, entering
 * @param y2 Y-coordinate, entering
 * @param n A value between 0-3, where 0 means the algorithm is asking for the shortest possible distance, and 3 a generous approximation.
 * @return An approximate evaluation of the distance between (x1,y1) and (x2,y2).
 * @see Path::nodes */
typedef int (*path_heuristic)(Path &p,int x1,int y1,int x2,int y2,int n);


/** A class for storing a path-trail, with step coordinates.
 * 
 * @ingroup amanita */
class Trail {
friend class Path;

protected:
	struct trailstep {
		int16_t x;					//!< X coordinate of step.
		int16_t y;					//!< Y coordinate of step.
		uint8_t dir;				//!< Direction that step is going.
	};
	trailstep *trail;				//!< Trail of steps.
	size_t ind;						//!< Index of step at where the trail is.
	size_t len;						//!< Length of trail. Number of steps.

public:
	/** @name Constructor and Destructor
	 * @{ */
	/** Constructor. */
	Trail() : trail(0),ind(0),len(0) {}
	/** Destructor. */
	~Trail() { if(trail) free(trail);trail = 0; }
	/** @} */

	/** @name Steps
	 * @{ */
	int getX() const { return ind<len? trail[ind].x : -1; }					//!< Get x coordinate of step at index.
	int getY() const { return ind<len? trail[ind].y : -1; }					//!< Get y coordinate of step at index.
	int getDir() { return ind<len? trail[ind].dir : -1; }						//!< Get direction that step at index is going.
	size_t index() { return ind; }													//!< Get index of step at where the trail is.
	size_t setIndex(size_t i) { return ind = i>=0 && i<len? i : 0; }		//!< Set index of step at where the trail is.
	void first() { ind = 0; }						//!< Set index to first step.
	void next() { if(ind<len) ind++; }			//!< Set index to next step.
	void previous() { if(ind>0) ind--; }		//!< Set index to previous step.
	void last() { if(len>0) ind = len-1; }		//!< Set index to last step.
	size_t length() { return len; }				//!< Length of trail. Number of steps.
	size_t steps() { return len-ind; }			//!< Number of steps left from where index is at.
	bool hasMoreSteps() { return ind<len; }	//!< Return true if there are more steps left from where index is at.
	/** @} */
};




/** A class for making path searches.
 * 
 * The class implements the A* algorithm.
 * 
 * @ingroup amanita */
class Path {
protected:
	struct node {
		unsigned short key;		//!< Hash key.
		int16_t x;					//!< X coordinate of node
		int16_t y;					//!< Y coordinate of node.
		short s;						//!< s value used in A* algorithm.
		short g;						//!< g value used in A* algorithm.
		short h;						//!< h value used in A* algorithm.
		node *parent;				//!< Parent node in path.
		node *open;					//!< Open node.
		node *closed;				//!< Closed node.

		node(int x,int y,int g,int h,node *p)		//!< Constructor
			: x(x),y(y),g(g),h(h),parent(p),open(0),closed(0) { key = (x<<8)|y,s = p? p->s+1 : 0,Path::created++; }
		~node() { Path::deleted++; }					//!< Destructor
	};

	int width;			//!< Width of map.
	int height;			//!< Height of map.
	int style;			//!< Style of map, if edges are overlapping etc.
	const int *dirs;	//!< Array of indexed directions to iterate through for each node. Should end with a -1.
	void *map;			//!< Map.
	void *obj;			//!< Object used in callbacks.
	node *open;			//!< Open nodes in a stack.
	node **closed;		//!< Closed nodes in a hashtable.
	node *closest;		//!< Node closest to target.
	size_t cap;			//!< Capacity of hashtable.
	size_t sz;			//!< Number of nodes in the hashtable.
	size_t full;		//!< Load of hashtable.

	/** @name Callbacks
	 * @{ */
	/** Test if can move at coordinate, callback */
	path_can_move can_move;
	/** Weight or cost to move, callback
	 * This is the only mandatory callback function, the other are optional or internally assigned. */
	path_weight weight;
	/** Move, callback */
	path_move move;
	/** Direction, callback */
	path_dir dir;
	/** Heuristic value, callback */
	path_heuristic heur;
	/** @} */

	void put(node *n);
	node *get(unsigned short key);
	void rehash();
	void push(node *n);
	node *pop();
	void remove(node *n);
	void clear();

public:
	static int created,deleted;

	/** Constructor
	 * @param w Width of map to search
	 * @param h Height of map to search
	 * @param s Style
	 * @param m Map, user data
	 * @param pw Weight callback function */
	Path(int w,int h,int s,void *m,path_weight pw);
	~Path();

	void setDirections(const int *d);
	void setCallbackFunctions(path_can_move cm,path_move pm,path_dir pd,path_heuristic ph);

	bool isHWrap() { return style&PATH_HWRAP; }
	bool isVWrap() { return style&PATH_VWRAP; }
	bool isOblique() { return style&PATH_OBLIQUE; }
	bool isIsometric() { return style&PATH_ISOMETRIC; }
	bool isHexagonal() { return style&PATH_HEXAGONAL; }

	int getWidth() { return width; }
	int getHeight() { return height; }
	void *getMap() { return map; }
	void *getObject() { return obj; }

	/** Calculate the A* path search
	 * @param o Object to perform search, user data
	 * @param x1 X-coordinate to start search
	 * @param y1 Y-coordinate to start search
	 * @param x2 X-coordinate to end search
	 * @param y2 Y-coordinate to end search
	 * @param l Length to search. No nodes at a distance from (x1,y1) greater than l are searched. If set to zero, all possible nodes are searched.
	 * @return A Trail-object containing the found path, or to the closest point. If no path is found, NULL is returned. */
	Trail *search(void *o,int x1,int y1,int x2,int y2,int l=0);
};

}; /* namespace a */



#endif /* _AMANITA_AI_PATH_H */

