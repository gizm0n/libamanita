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
	PATH_RECTANGULAR_MAP				= 0x0004,		//!< The map is isometric.
	PATH_HEXAGONAL_MAP				= 0x0008,		//!< The map is isometric.
	PATH_ISOMETRIC_MAP				= 0x0010,		//!< The map is isometric.
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
	PATH_MOVE_COST_CANNOT_MOVE		= 0xffff,		//!< 
};


/** @cond */
class Path;
/** @endcond */


/** Compare area callback.
 * @see Path::cb_area_compare
 */
typedef int (*path_area_compare)(int,int,int,int,void *);

/** Type of terrain callback.
 * @see Path::cb_terrain_type
 */
typedef int (*path_terrain_type)(int,int,void *);

/** Cost to move callback.
 * @see Path::cb_move_cost
 */
typedef int (*path_move_cost)(int,int,int,void *,void *);

/** Move callback.
 * Differs depending on what type of map, i.e. hexagonal, isometric. This can be custom made, or else
 * is handled by internal methods.
 * @see Path::cb_move
 * */
typedef void (*path_move)(Path &,int,int,int &,int &,int);



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
		short f;						//!< f value used in A* algorithm.
		node *parent;				//!< Parent node in path.
		node *open;					//!< Open node.
		node *closed;				//!< Closed node.

		node(int x,int y,int g,int h,node *p)		//!< Constructor
			: x(x),y(y),g(g),h(h),f(g+h),parent(p),open(0),closed(0) { key = (x<<8)|y,s = p? p->s+1 : 0,Path::created++; }
		~node() { Path::deleted++; }					//!< Destructor
	};

	int width;			//!< Width of map.
	int height;			//!< Height of map.
	int style;			//!< Style of map, if edges are overlapping etc.
	void *map;			//!< Map.
	void *obj;			//!< Object used in callbacks.
	node *open;			//!< Open nodes.
	node **closed;		//!< Closed nodes in a hashtable.
	size_t cap;			//!< Capacity of hashtable.
	size_t sz;			//!< Number of nodes in the hashtable.
	size_t full;		//!< Load of hashtable.

	/** Move on an isometric map.
	 * Used as callback.
	 * @see move
	 */
	static void moveIso(Path &p,int x1,int y1,int &x2,int &y2,int dir);

	/** @name Callbacks
	 * @{ */
	/** Compare area callback.
	 */
	path_area_compare cb_area_compare;

	/** Type of terrain callback.
	 */
	path_terrain_type cb_terrain_type;

	/** Cost to move callback.
	 */
	path_move_cost cb_move_cost;

	/** Move callback.
	 * Differs depending on what type of map, i.e. hexagonal, isometric. This can be custom made, or else
	 * is handled by internal methods.
	 * @see moveIso
	 * */
	path_move cb_move;
	/** @} */

	int getHeuristic(int x1,int y1,int x2,int y2,int n);
	int getDir(int16_t x1,int16_t y1,int16_t x2,int16_t y2);

	void put(node *n);
	node *get(unsigned short key);
	void rehash();
	void push(node *n);
	node *pop();
	void remove(node *n);
	void clear();

public:
	static int created,deleted;

	Path(int w,int h,int s,void *m,void *o,path_area_compare a,path_terrain_type t,path_move_cost c);
	~Path();

	void setParam(int type,void *param);
	void *getParam(int type);

	Trail *search(int x1,int y1,int x2,int y2,int l=0);
};

}; /* namespace a */



#endif /* _AMANITA_AI_PATH_H */

