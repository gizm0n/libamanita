#ifndef _MANDRAGORA_ASTRO_ASTROLOGY_H
#define _MANDRAGORA_ASTRO_ASTROLOGY_H


#include <stdint.h>
#include <mandragora/calendar.h>
#include <mandragora/astro/astronomy.h>


/** ASTRO_CATEGORIES */
enum {
	ASTRO_ELEMENTS,
	ASTRO_QUALITIES,
	ASTRO_ENERGIES,
	ASTRO_ZODIAC,
	ASTRO_PLANETS,
	ASTRO_HOUSES,
	ASTRO_ASPECTS,
	ASTRO_PATTERNS,
	ASTRO_SHAPINGS,
	ASTRO_FACTORS,
	ASTRO_HSYSTEMS,
	ASTRO_DIRECTIONS,
	ASTRO_CALENDARS,
	ASTRO_CATEGORIES
};

/** ASTRO_ELEMENTS */
enum {
	ASTRO_FIRE,
	ASTRO_EARTH,
	ASTRO_AIR,
	ASTRO_WATER,
ASTRO_NUM_ELEMENTS
};

/** ASTRO_QUALITIES */
enum {
	ASTRO_CARDINAL,
	ASTRO_FIXED,
	ASTRO_MUTABLE,
ASTRO_NUM_QUALITIES
};

/** ASTRO_ENERGIES */
enum {
	ASTRO_MALE,
	ASTRO_FEMALE,
ASTRO_NUM_ENERGIES
};

/** ASTRO_PLANETS */
enum {
	ASTRO_TELLUS,							//!< 0
	ASTRO_SUN,								//!< 1 
	ASTRO_MOON,								//!< 2 
	ASTRO_MERCURY,							//!< 3 
	ASTRO_VENUS,							//!< 4
	ASTRO_MARS,								//!< 5
	ASTRO_JUPITER,							//!< 6
	ASTRO_SATURN,							//!< 7
	ASTRO_URANUS,							//!< 8
	ASTRO_NEPTUNE,							//!< 9
	ASTRO_PLUTO,							//!< 10
	ASTRO_NNODE,							//!< 11
	ASTRO_TNNODE,							//!< 12
	ASTRO_SNODE,							//!< 13
	ASTRO_TSNODE,							//!< 14
	ASTRO_CHIRON,							//!< 15
	ASTRO_CERES,							//!< 16
	ASTRO_PALLAS,							//!< 17
	ASTRO_JUNO,								//!< 18
	ASTRO_VESTA,							//!< 19
	ASTRO_VERTEX,							//!< 20
	ASTRO_EASTPOINT,						//!< 21
	ASTRO_FORTUNE,							//!< 22
	ASTRO_ASCENDANT,						//!< 23
	ASTRO_MC,								//!< 24
	ASTRO_HOUSE,							//!< 25
	ASTRO_MINOR_PLANET,					//!< 26
	ASTRO_FIXED_STAR,						//!< 27
ASTRO_NUM_PLANET_POINTS,
ASTRO_NUM_PLANETS = 11
};

/** ASTRO_ZODIAC */
enum {
	ASTRO_ARIES,
	ASTRO_TAURUS,
	ASTRO_GEMINI,
	ASTRO_CANCER,
	ASTRO_LEO,
	ASTRO_VIRGO,
	ASTRO_LIBRA,
	ASTRO_SCORPIO,
	ASTRO_SAGITTARIUS,
	ASTRO_CAPRICORN,
	ASTRO_AQUARIUS,
	ASTRO_PISCES,
ASTRO_NUM_ZODIAC
};

/** ASTRO_ASPECTS */
enum {
	ASTRO_CONJUNCTION,					//!< 0		0°
	ASTRO_SEMISEXTILE,					//!< 1		30°		1/12
	ASTRO_DECILE,							//!< 2		36°		1/10
	ASTRO_NOVILE,							//!< 3		40°		1/9
	ASTRO_SEMISQUARE,						//!< 4		45°		1/8
	ASTRO_SEPTILE,							//!< 5		51°		1/7
	ASTRO_SEXTILE,							//!< 6		60°		1/6
	ASTRO_QUINTILE,						//!< 7		72°		1/5
	ASTRO_SQUARE,							//!< 8		90°		1/4
	//ASTRO_BISEPTILE,					//!< 			103°		2/7
	ASTRO_TRINE,							//!< 9		120°		1/3
	ASTRO_SESQUIQUADRATE,				//!< 10		135°		3/8
	//ASTRO_BIQUINTILE,					//!< 			144°		2/5
	ASTRO_QUINCUNX,						//!< 11		150°		5/12
	//ASTRO_TRISEPTILE,					//!< 			154°		3/7
	ASTRO_OPPOSITION,						//!< 12		180°		1/2
ASTRO_NUM_ASPECTS,
ASTRO_ASPECT_ORB_LEVELS = 10
};

/** ASTRO_PATTERNS */
enum {
	ASTRO_STELLIUM,
	ASTRO_TSQUARE,
	ASTRO_GSQUARE,
	ASTRO_GCROSS,
	ASTRO_GTRINE,
	ASTRO_YOD,
	ASTRO_MYSTRECT,
	ASTRO_KITE,
	ASTRO_GQUINTILE,
	ASTRO_HEXAGRAM,
ASTRO_NUM_PATTERNS
};

/** ASTRO_SHAPINGS */
enum {
	ASTRO_BOWL,
	ASTRO_WEDGE,
	ASTRO_SEESAW,
	ASTRO_SPLAY,
	ASTRO_LOCOMOTIVE,
	ASTRO_BUCKET,
	ASTRO_BUNDLE,
	ASTRO_SPLASH,
ASTRO_NUM_SHAPINGS
};

/** ASTRO_FACTORS */
enum {
	ASTRO_RULER_PLANET,
	ASTRO_RULER_HOUSE,
	ASTRO_ASCENDING,
	ASTRO_ANGULAR,
	ASTRO_MRECEPTIVE,
ASTRO_NUM_FACTORS
};

/** ASTRO_HSYSTEMS */
enum {
	ASTRO_PLACIDUS,
	ASTRO_KOCH,
ASTRO_NUM_HSYSTEMS
};

/** ASTRO_DIRECTIONS */
enum {
	ASTRO_LONGITUDE,
	ASTRO_LATITUDE,
	ASTRO_EAST,
	ASTRO_WEST,
	ASTRO_NORTH,
	ASTRO_SOUTH,
ASTRO_NUM_DIRECTIONS
};

/** ASTRO_CALENDARS */
enum {
	ASTRO_JULIAN,
	ASTRO_JULIAN_DAY,
	ASTRO_GREGORIAN,
ASTRO_NUM_CALENDARS
};


typedef struct astro_planet {
	int id;
	int sign;
	double lon;
	int house;
	int factors;
} astro_planet;

typedef struct astro_house {
	int sign;
	double cusp;
	double decl;
} astro_house;


typedef struct astro_aspect {
	int type;
	double orb;
} astro_aspect;


typedef struct astro_pattern {
	int type;
	int nplanets;
	uint32_t *planets;
} astro_pattern;


extern const int astro_default_planets[16];							//!< Default planets for a horoscope.
extern int astro_ruling_planets[ASTRO_NUM_ZODIAC];					//!< The planets ruling which sign in the zodiac.
extern const int astro_planet_hierarchy[ASTRO_NUM_PLANETS];	//!< The hierarchy among the planets, which planet is learning from which.
extern const double astro_aspect_ratios[ASTRO_NUM_ASPECTS];		//!< Difference in degrees between two planets to be an aspect.
extern const int astro_aspect_signs[ASTRO_NUM_ASPECTS];			//!< Difference in signs between two planets to be an aspect.

/** Aspect orbs.
 * For each aspect there are 10 levels:
 *  0: Sun or Moon.
 *  1: Ascendant.
 *  2: MC.
 *  3: Ruling planet of the Ascendant.
 *  4: Ruling planet of the Sun or Moon.
 *  5: Mercury, Venus or Mars.
 *  6: Jupiter or Saturn.
 *  7: Uranus, Neptune, Pluto.
 *  8: Nodes.
 *  9: Used for all other aspects.
 * 
 * A value set to <0 means the group of planets will not be used for that aspect,
 * and to disable an entire aspect set all levels to -1.
 */
extern double astro_aspect_orbs[ASTRO_NUM_ASPECTS*ASTRO_ASPECT_ORB_LEVELS];

/** Index of a planet in the astro_aspect_orbs array. */
extern const int astro_aspect_orb_planet_index[ASTRO_NUM_PLANET_POINTS];

extern const int astro_element[ASTRO_NUM_ZODIAC];					//!< Elements of the zodiac.
extern const int astro_quality[ASTRO_NUM_ZODIAC];					//!< Qualities of the zodiac.
extern const int astro_energy[ASTRO_NUM_ZODIAC];					//!< Energies of the zodiac.

void astro_init();

void astro_set_ruling_planets(const int pl[ASTRO_NUM_ZODIAC]);
void astro_set_aspect_orbs(const double orbs[ASTRO_NUM_ASPECTS*ASTRO_ASPECT_ORB_LEVELS]);

int astro_zodiac(double l);
double astro_midheaven(double ra,double ob);
double astro_ascendant(double ra,double ob,double la);
double astro_fortune(double p1,double p2,double p3);
double astro_eastpoint(double ra,double ob);
double astro_vertex(double ra,double ob,double la);
int astro_placidus(astro_house *h,double ra,double ob,double la);
int astro_koch(astro_house *h,double ra,double ob,double la);
double astro_angle(double p1,double p2);
double astro_composite_angle(double a1,double a2);


#endif /* _MANDRAGORA_ASTRO_ASTROLOGY_H */


