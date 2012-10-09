#ifndef _MANDRAGORA_ASTRO_ASTRONOMY_H_
#define _MANDRAGORA_ASTRO_ASTRONOMY_H_


#include <mandragora/calendar.h>


/** ASTRONOMY_DATA */
enum {
	ASTRONOMY_HLON						= 0x01,			//!< Heliocentric longitude
	ASTRONOMY_GLON						= 0x02,			//!< Geocentric longitude
	ASTRONOMY_LAT						= 0x04,			//!< Geocentric latitude
	ASTRONOMY_RADIUS					= 0x08,			//!< 
	ASTRONOMY_SPEED					= 0x10,			//!< 
	ASTRONOMY_CONSTELLATION			= 0x20,			//!< Astronomical constellation planet is in.
	ASTRONOMY_MOON_PHASE				= 0x40,			//!< Moon phase (only used for Moon).
	ASTRONOMY_ALL_DATA				= 0xff,			//!< 
};


extern const double J2000;			//!< 2000 Januari 1.5
extern const double B1950;			//!< 1950 January 0.923 Besselian epoch
extern const double J1900;			//!< 1900 January 0,12h UT

/** @name Conversion factors between degrees and radians.
 * @{ */
extern const double DTR;			//!< Degrees to Radians
extern const double RTD;			//!< Radians to Degrees
extern const double RTH;			//!< Radians to Hours
extern const double HTR;			//!< Hours to Radians
extern const double RTS;			//!< Arc seconds per Radian
extern const double STR;			//!< Radians per Arc second
/** @} */

extern const double PI;				//!< Pi
extern const double PIx2;			//!< Pi * 2
extern const double PIx1_5;		//!< Pi * 1.5
extern const double PIx0_5;		//!< Pi * 0.5


typedef struct astronomy_planet {
	int id;					//!< Planetary ID. Use the ASTRO_PLANETS enumeration in astrology.h.
	double hlon;			//!< Heliocentric longitude
	double glon;			//!< Geocentric longitude
	double lat;				//!< Geocentric latitude
	double radius;			//!< 
	double speed;			//!< 
	int constel;			//!< Astronomical constellation planet is in.
	double phase;			//!< Moon phase (only used for Moon).
} astronomy_planet;


typedef struct astronomy {
	calendar gmt;
	double lon;
	double lat;
	double deltat;
	double sdrlt;
	int nplanets;
	astronomy_planet *planets;
} astronomy;


double mod360(double x);
double mod3600(double x);

double dec_to_hour(double d);
double hour_to_dec(double d);

double dec(double d,double m,double s);
double asn(double a);
double acs(double a);


/** Create an astronomy object to calculate planetary data.
 * @param gmt Calendar date and time. It must be set to GMT.
 * @param lon Longitude of observation. It should be a decimal value, where an eastern degree is negative and a western positive.
 * @param lat Latitude of observation. It should be a decimal value, where a southern degree is negative and a northern positive.
 * @see astronomy_delete()
 * @return An allocated astronomy object. Use astronomy_delete() to delete the allocated astronomy-object.
 */
astronomy *astronomy_new(const calendar *gmt,double lon,double lat);


/** Delete an astronomy object allocated by astronomy_new()
 * @param a An astronomy object allocated by astronomy_new().
 * @see astronomy_new()
 */
void astronomy_delete(astronomy *a);


/** Calculate the astronomical ephemerides for planets.
 * @param pl Array of planets to calculate. Values should be from the ASTRO_PLANETS enumeration in astrology.h,
 * and last value in the array must be -1 to mark end of list.
 * @param data A bitmap set to what data should be calculated, or 0 for all data.
 * Use the values in the ASTRONOMY_DATA enumeration.
 * @see astronomy_new()
 */
void astronomy_get_planets(astronomy *a,const int *pl,int data);


const char *constellation_name(int id);
const char *constellation_abbreviation(int id);

#endif /* _MANDRAGORA_ASTRO_ASTRONOMY_H_ */

