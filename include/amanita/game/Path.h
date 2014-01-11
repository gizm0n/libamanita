#ifndef _AMANITA_GAME_PATH_H
#define _AMANITA_GAME_PATH_H

/**
 * @file amanita/game/Path.h  
 * @author Per Löwgren
 * @date Modified: 2013-02-23
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
	PATH_CANNOT_MOVE					= 0xffff,		//!< Will no include this coordinate in the search
	PATH_AVOID_MOVE					= 0xfffe,		//!< Is not included as a valid coordinate in the search, but accepted as a destination.
};

enum {
	PATH_C,			//!< Centre
	PATH_N,			//!< North
	PATH_E,			//!< East
	PATH_S,			//!< South
	PATH_W,			//!< West
	PATH_NE,			//!< North-East
	PATH_SE,			//!< South-East
	PATH_SW,			//!< South-West
	PATH_NW,			//!< North-West
	PATH_NNE,		//!< North-North-East
	PATH_ENE,		//!< East-North-East
	PATH_ESE,		//!< East-South-East
	PATH_SSE,		//!< South-South-East
	PATH_SSW,		//!< South-South-West
	PATH_WSW,		//!< West-South-West
	PATH_WNW,		//!< West-North-West
	PATH_NNW,		//!< North-North-West
	PATH_UP,			//!< Up
	PATH_DN,			//!< Down
	PATH_R,			//!< Right
	PATH_L,			//!< Left
	PATH_IN,			//!< In
	PATH_OUT,		//!< Out
};



/** @cond */
class Path;
/** @endcond */


extern int dir_oblique(Path &p,int x1,int y1,int x2,int y2);
extern int dir_isometric(Path &p,int x1,int y1,int x2,int y2);
extern int dir_hexagonal(Path &p,int x1,int y1,int x2,int y2);


struct PathNode {
	uint32_t key;				//!< Hash key. Key used in hashtable.
	int16_t x;					//!< X coordinate of node.
	int16_t y;					//!< Y coordinate of node.
	short s;						//!< s value used in A* algorithm. Steps, the number of steps from starting point.
	short g;						//!< g value used in A* algorithm. Accumulated move-cost of all steps to and including this node.
	short h;						//!< h value used in A* algorithm. Heuristic value, approximate distance to destination.
	PathNode *parent;			//!< Parent node in path. Previous step in the path. When a shorter path is found for any node; parent, s and g are adjusted accordingly.
	PathNode *open;			//!< Open node. Linked list in the open stack.
	PathNode *closed;			//!< Closed node. Linked list in the closed hashtable.

	/** Constructor
	 * @param x X-coordinate
	 * @param y Y-coordinate
	 * @param g g value used in the A* algorithm
	 * @param h h value used in the A* algorithm
	 * @param p Parent node, previous step in the path */
	PathNode(int x,int y,int g,int h,PathNode *p)
		: x(x),y(y),g(g),h(h),parent(p),open(0),closed(0) { key = (x<<16)|y,s = p? p->s+1 : 0; }
	/** Destructor */
	~PathNode() {}
};


/** Weight, or cost to move, callback
 * The weight or cost to move from (x1,y1) to (x2,y2), where distance is always one
 * step. Commonly only (x2,y2) is used. A very simple implementation could
 * look like this:
 * @code
int weight(Path &p,PathNode &n,int x,int y) {
	int *map = (int *)p.getMap();
	int t = map[x+y*p.getWidth()];
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
 * Notice that the actual move-cost may be different. The returned value should be a relative
 * weight between 0-65533 in comparison to other terrain-types, distances, time to move,
 * friction, or similar. If you find a need for higher weights-values than a range between 0-65533,
 * it is perhaps better to recalculate to percent or permille, or even permyriad (1/10000).
 * @param p Path object
 * @param n PathNode, current node positioned at
 * @param x X-coordinate for calculating weight to enter
 * @param y Y-coordinate for calculating weight to enter
 * @return The function should return the weight (a value between 0-65533), or if impassable PATH_CANNOT_MOVE or PATH_AVOID_MOVE.
 * @see Path::weight */
typedef int (*path_weight)(Path &p,PathNode &n,int x,int y);

/** Move, callback
 * Move from node n to next position on the map (x,y), in the direction indexed by i. The direction
 * index i does not contain an actual direction value, but an index starting with zero and adding
 * for each new direction. The function should handle direction internally, and return -1 when
 * there are no more directions to search after the current call.
 * @param p Path object
 * @param n PathNode, current node to search from
 * @param x X-coordinate, next position to move in the search
 * @param y Y-coordinate, next position to move in the search
 * @param i Direction index, starting with zero and adding for each call to this function
 * @return Should return -1 at the last direction to search.
 * @see Path::move */
typedef int (*path_move)(Path &p,PathNode &n,int &x,int &y,int i);

/** Get direction, callback
 * The direction is used by the path_move-callback function to determine
 * the coordinate for the step and the dir-values of the returned Trail-object.
 * @param p Path object
 * @param x1 X-coordinate, location
 * @param y1 Y-coordinate, location
 * @param x2 X-coordinate, destination
 * @param y2 Y-coordinate, destination
 * @return Direction, the direction in which destination (x2,y2) orient to (x1,y1).
 * @see Path::dir */
typedef int (*path_dir)(Path &p,int x1,int y1,int x2,int y2);

/** Heuristic value, callback
 * An approximate distance between (x1,y1) and (x2,y2), where (x1,y1) is the location the search is
 * positioned at each step in the search and (x2,y2) is the destination. The returned value should be
 * an approximation as close as possible to the real distance, where a lower value generally calculates
 * a more direct path, while a higher value accepts paths a bit more off.
 * @param p Path object. Use Path::getDestination().
 * @param x X-coordinate for calculating distance to destination from
 * @param y Y-coordinate for calculating distance to destination from 
 * @return An approximate evaluation of the distance between (x1,y1) and (x2,y2).
 * @see Path::nodes */
typedef int (*path_heuristic)(Path &p,int x,int y);


typedef void (*path_step)(Path &p,PathNode &n);


/** A class for storing a path-trail, with step coordinates.
 * 
 * @ingroup amanita */
class Trail {
friend class Path;

public:
	struct step {
		int16_t x;				//!< X coordinate of step.
		int16_t y;				//!< Y coordinate of step.
		uint8_t dir;			//!< Direction that step is going.
	};

protected:
	step *trail;				//!< Trail of steps.
	size_t ind;					//!< Index of step at where the trail is.
	size_t len;					//!< Length of trail. Number of steps.
	void *obj;					//!< Object moving.

public:
	/** @name Constructor and Destructor
	 * @{ */
	/** Constructor. */
	Trail() : trail(0),ind(0),len(0),obj(0) {}
	/** Destructor. */
	~Trail() { if(trail) free(trail);trail = 0; }
	/** @} */

	/** @name Steps
	 * @{ */
	const step *getSteps() { return trail; }
	void setSteps(const step *s,int l,int i=0);
	int getX() const { return ind<len? trail[ind].x : -1; }					//!< Get x coordinate of step at index.
	int getY() const { return ind<len? trail[ind].y : -1; }					//!< Get y coordinate of step at index.
	int getDir() { return ind<len? trail[ind].dir : -1; }						//!< Get direction that step at index is going.
	void *getObject() { return obj; }
	size_t index() { return ind; }													//!< Get index of step at where the trail is.
	size_t setIndex(size_t i) { return ind = i>=0 && i<len? i : 0; }		//!< Set index of step at where the trail is.
	void first() { ind = 0; }						//!< Set index to first step.
	void next() { if(ind<len-1) ++ind; }			//!< Set index to next step.
	void previous() { if(ind>0) --ind; }		//!< Set index to previous step.
	void last() { if(len>0) ind = len-1; }		//!< Set index to last step.
	size_t length() { return len; }				//!< Length of trail. Number of steps.
	size_t steps() { return len-ind; }			//!< Number of steps left from where index is at.
	bool hasMoreSteps() { return ind<len-1; }	//!< Return true if there are more steps left from where index is at.
	/** @} */
};




/** A class for making path searches.
 * 
 * The class implements the A* algorithm.
 * 
 * @ingroup amanita */
class Path {
protected:
	int width;				//!< Width of map.
	int height;				//!< Height of map.
	int style;				//!< Style of map, if edges are overlapping etc.
	void *map;				//!< Map.
	void *obj;				//!< Object used in callbacks.
	int sx;
	int sy;
	int dx;
	int dy;

	PathNode *open;		//!< Open nodes in a stack.
	PathNode **closed;	//!< Closed nodes in a hashtable.
	PathNode *closest;	//!< Node closest to target.
	size_t cap;				//!< Capacity of hashtable.
	size_t sz;				//!< Number of nodes in the hashtable.
	size_t full;			//!< Load of hashtable.

	/** @name Callbacks
	 * @{ */
	/** Weight or cost to move, callback
	 * This is the only mandatory callback function, the other are optional or internally assigned. */
	path_weight weight;
	/** Move, callback */
	path_move move;
	/** Direction, callback */
	path_dir dir;
	/** Heuristic value, callback */
	path_heuristic heur;
	/** Move step, callback */
	path_step step;
	/** @} */

	Trail *getTrail(PathNode *n);

	void put(PathNode *n);
	PathNode *get(int x,int y) { return get((x<<16)|y); }
	PathNode *get(uint32_t key);
	void rehash();
	void push(PathNode *n);
	PathNode *pop();
	void remove(PathNode *n);
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

	void setCallbackFunctions(path_move pm,path_dir pd,path_heuristic ph,path_step ps);

	bool isHWrap() { return style&PATH_HWRAP; }
	bool isVWrap() { return style&PATH_VWRAP; }
	bool isOblique() { return style&PATH_OBLIQUE; }
	bool isIsometric() { return style&PATH_ISOMETRIC; }
	bool isHexagonal() { return style&PATH_HEXAGONAL; }

	int getWidth() { return width; }
	int getHeight() { return height; }
	void *getMap() { return map; }
	void *getObject() { return obj; }
	void getStart(int &x,int &y) { x = sx,y = sy; }
	void getDestination(int &x,int &y) { x = dx,y = dy; }

	bool isWithinReach(int x,int y) { return get(x,y)!=0; }
	bool isOpen(int x,int y);
	int getSteps(int x,int y) { PathNode *n = get(x,y);return n? n->s : -1; }
	int getWeight(int x,int y) { PathNode *n = get(x,y);return n? n->g : -1; }
	void getParent(int x1,int y1,int &x2,int &y2) { PathNode *n = get(x1,y1);if(n && n->parent) n = n->parent,x2 = n->x,y2 = n->y;else x2 = y2 = -1; }
	Trail *getTrail(int x,int y) { return getTrail(get(x,y)); }

	/** Calculate the A* path search
	 * @param o Object to perform search, user data
	 * @param x1 X-coordinate to start search
	 * @param y1 Y-coordinate to start search
	 * @param x2 X-coordinate to end search
	 * @param y2 Y-coordinate to end search
	 * @param l Length to search. No nodes at a distance from (x1,y1) greater than l are searched. If set to zero, all possible nodes are searched.
	 * @return A Trail-object containing the found path, or to the closest point. If no path is found, NULL is returned. */
	Trail *searchPath(void *o,int x1,int y1,int x2,int y2,int l=0);

	/** Calculate the A* path search
	 * @param o Object to perform search, user data
	 * @param x1 X-coordinate to start search
	 * @param y1 Y-coordinate to start search
	 * @param x2 X-coordinate to end search
	 * @param y2 Y-coordinate to end search
	 * @param l Length to search. No nodes at a distance from (x1,y1) greater than l are searched. If set to zero, all possible nodes are searched.
	 * @return A Trail-object containing the found path, or to the closest point. If no path is found, NULL is returned. */
	void searchReach(void *o,int x,int y,int s,int m);
};

}; /* namespace a */



#endif /* _AMANITA_GAME_PATH_H */

