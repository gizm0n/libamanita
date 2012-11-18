#ifndef _AMANITA_COLORCYCLE_H
#define _AMANITA_COLORCYCLE_H

/**
 * @file amanita/Colorcycle.h  
 * @author Per Löwgren
 * @date Modified: 2012-11-02
 * @date Created: 2004-03-07
 */ 


#include <stdint.h>
#include <stdio.h>


/** Amanita Namespace */
namespace a {


enum {
	CC_DAYLIGHT,
	CC_PALETTE,
	CC_NEON,
	CC_GOLD,
	CC_SILVER,
	CC_EBONY,
	CC_BLOOD,
	CC_LAPIZ_LAZULI,
	CC_AMETHYST,
	CC_FIRE,
	CC_FOLIAGE,
	CC_SCHEMES,
};


/** Blend two 16bit colours where alpha is 50%, that is mixing colours 50/50. Very fast!
 * @param from Colour to blend from
 * @param to Colour to blend to
 * @return A 16bit colour */
inline uint16_t blend16(uint16_t from,uint16_t to) { return ((from&0xf7df)+(to&0xf7df))>>1; }

/** Blend two 16bit colours
 * @param from Colour to blend from
 * @param to Colour to blend to
 * @param a Alpha value of blending, ranging between 0-1023
 * @return A 16bit colour */
extern uint32_t blend16a(uint32_t from,uint32_t to,uint32_t a);

/** Blend two 32bit (actually 24bit) colours where alpha is 50%, that is mixing colours 50/50. Very fast!
 * @param from Colour to blend from
 * @param to Colour to blend to
 * @return A 32bit (24bit) colour */
inline uint32_t blend24(uint32_t from,uint32_t to) { return ((from&0xfefeff)+(to&0xfefeff))>>1; }

/** Blend two 32bit (actually 24bit) colours
 * @param from Colour to blend from
 * @param to Colour to blend to
 * @param a Alpha value of blending, ranging between 0-1023
 * @return A 32bit (24bit) colour */
extern uint32_t blend24a(uint32_t from,uint32_t to,uint32_t a);


typedef uint32_t (*blend_func)(uint32_t,uint32_t,uint32_t);

/** A class for cycling colours
 * 
 * See examples/fractal.cpp on how to use this class.
 * 
 * @ingroup amanita */
class Colorcycle {
private:
	int scheme;						//<! 
	uint32_t *colors;				//<! Array of colours to cycle between
	int len;							//<! Length of colors-array
	int off;							//<! Offset in colour cycle, it can be up to len*1024
	int var;							//<! Variance of cycling between colours, 1-1024, where 1 goes through every degree of shade and 1024 simply iterated the colours
	int speed;						//<! Speed of cycling between calling writeColors, same as for var
	int bits;
	blend_func blend;				//<! Blending function

	/** Delete resources */
	void clear();

public:
	/** Constructor */
	Colorcycle();
	/** Constructor */
	Colorcycle(Colorcycle &c);
	/** Constructor
	 * @param c Colour cycle, any of the CC_* enum
	 * @param o Offset to start cycling, can be up to l*1024
	 * @param v Variance of cycling between colours, 1-1024, where 1 goes through every degree of shade and 1024 simply iterated the colours
	 * @param s Speed of cycling between calling writeColors, same as for var
	 * @param b 16 or 32 bit colours */
	Colorcycle(int c,int o=0,int v=0,int s=0,int b=32);
	/** Constructor
	 * @param c Array of colours to cycle between
	 * @param l Length of c
	 * @param o Offset to start cycling, can be up to l*1024
	 * @param v Variance of cycling between colours, 1-1024, where 1 goes through every degree of shade and 1024 simply iterated the colours
	 * @param s Speed of cycling between calling writeColors, same as for var
	 * @param b 16 or 32 bit colours */
	Colorcycle(const uint32_t *c,int l,int o=0,int v=0,int s=0,int b=32);
	/** Destructor */
	~Colorcycle();

	/** Set colours
	 * @param c Colour cycle to copy settings from */
	void setScheme(Colorcycle &c);
	/** Set colours
	 * @param c Colour cycle, any of the CC_* enum
	 * @param o Offset to start cycling, can be up to l*1024
	 * @param v Variance of cycling between colours, 1-1024, where 1 goes through every degree of shade and 1024 simply iterated the colours
	 * @param s Speed of cycling between calling writeColors, same as for var
	 * @param b 16 or 32 bit colours */
	void setScheme(int c,int o=0,int v=0,int s=0,int b=32);
	/** Set colours
	 * @param c Array of colours to cycle between
	 * @param l Length of c
	 * @param o Offset to start cycling, can be up to l*1024
	 * @param v Variance of cycling between colours, 1-1024, where 1 goes through every degree of shade and 1024 simply iterated the colours
	 * @param s Speed of cycling between calling writeColors, same as for var
	 * @param b 16 or 32 bit colours */
	void setScheme(const uint32_t *c,int l,int o=0,int v=0,int s=0,int b=32);

	/** Get colour at offset
	 * @param o Colour at offset, can be up to l*1024
	 * @return The colour value at offset */
	uint32_t getColor(int o);
	/** Write colours to an array, after calling this method offset is increased by speed to cause cycling effect
	 * @param c Array to write to
	 * @param l Length of c */
	void write(uint32_t *c,int l,bool a=true);
	/** Write colours to an array, after calling this method offset is increased by speed to cause cycling effect
	 * @param c Array to write to
	 * @param l Length of c
	 * @param s Scale to adjust variance, a value of <1 causes a larger spread, while >1 becomes more dense */
	void write(uint32_t *c,int l,double s,bool a=true);

	void advance();
	void advance(double s);

	const char *getSchemeName();

	void write(FILE *fp);
	void read(FILE *fp);
};

}; /* namespace a */


#endif /* _AMANITA_COLORCYCLE_H */
