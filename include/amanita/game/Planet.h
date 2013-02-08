#ifndef _AMANITA_GAME_PLANET_H
#define _AMANITA_GAME_PLANET_H

/**
 * @file amanita/game/Planet.h  
 * @author Per Löwgren
 * @date Modified: 2013-01-20
 * @date Created: 2013-01-20
 */ 


/** Amanita Namespace */
namespace a {


enum WRAP {
	PLANET_WRAP_NONE			= 0,
	PLANET_WRAP_HORIZ			= 1,
	PLANET_WRAP_VERT			= 2,
	PLANET_WRAP_BOTH			= 3
};

enum {
	PLANET_ERA_NEW,
	PLANET_ERA_COOL,
	PLANET_ERA_CLIMATE,
	PLANET_ERA_VEGETATION,
	PLANET_ERA_CIVILISATION,
	PLANET_ERA_COMPLETED,
};

enum {
	PLANET_TEMPERATURE,
	PLANET_ALTITUDE,
	PLANET_SEA,
	PLANET_ENVIRONMENT,
	PLANET_VEGETATION,
	PLANET_POPULATION,
	PLANET_CULTURE,
	PLANET_INLAND,
};

struct RGBA_Color {
	uint8_t r,g,b,a;
};


inline RGBA_Color environmentColor(float c) { return c>=0.99f? (RGBA_Color){0xff,0xff,0xff} : (RGBA_Color){0xff,(uint8_t)(221.0f*c),0}; }
inline RGBA_Color magmaColor(float c) { return (RGBA_Color){0xff,(uint8_t)(221.0f*c),0}; }
inline RGBA_Color glacierColor(float c) { return (RGBA_Color){(uint8_t)(0xff-(int)(153.0f*c)),(uint8_t)(0xff-(int)(102.0f*c)),0xff}; }
inline RGBA_Color mountainColor(float c) { return (RGBA_Color){(uint8_t)(255.0f*c),(uint8_t)(221.0f*c),(uint8_t)(187.0f*c)}; }
inline RGBA_Color oceanColor(float c) { return (RGBA_Color){0,(uint8_t)(0xcc-(int)(204.0f*c)),(uint8_t)(0xff-(int)(204.0f*c))}; }
inline RGBA_Color desertColor(float c) { return (RGBA_Color){(uint8_t)(0xcc+(int)(51.0f*c)),(uint8_t)(0xcc+(int)(51.0f*c)),(uint8_t)(0x33+(int)(153.0f*c))}; }
inline RGBA_Color planctonColor(float c) { return (RGBA_Color){0,(uint8_t)(0xff-(int)(204.0f*c)),(uint8_t)(0xcc-(int)(204.0f*c))}; }
inline RGBA_Color vegetationColor(float c) { return (RGBA_Color){(uint8_t)(0x33-(int)(51.0f*c)),(uint8_t)(0xcc-(int)(102.0f*c)),(uint8_t)(0x33-(int)(51.0f*c))}; }
inline RGBA_Color cityColor(float c) { return (RGBA_Color){(uint8_t)(0x99+(int)(102.0f*c)),0,0}; }
inline RGBA_Color irColor(float value) { return value<1.0f? glacierColor(value) : (value>1.6f? magmaColor(value-1.6f) : vegetationColor(1.0f-(value-1.0f)*(1.0f/.6f))); }

struct PlanetCreationInfo {
	int continents;
	float landmass;
	float mountains;
	float islands;
	float forest;
	float cities;
	float technology;
};

struct PlanetPoint {
	float temperature;
	float altitude;
	float sea;
	float environment;
	float vegetation;
	float population;
	unsigned short culture;
	unsigned char inland;
	PlanetPoint *area[21];
};

class Planet {
private:

	int width;									//!< Map width - Number of points
	int height;									//!< Map height - Number of points
	int size;									//!< Map width*height - Number of points
	int wrap;									//!< Wrapping of the planet - Default horizontal wrapping, not at the poles

	float sea;
	float equator;

	int time;
	int cycle;
	int cycles;
	int era;
	bool completed;

	random_t seed;
	Random chaos;

	PlanetCreationInfo ci;
	PlanetCreationInfo result;
	PlanetPoint *surface;

	void nextEra();

	int cool(float change);
	int climate(float change);
	int vegetation(float change);
	int civilisation(float change);

	void environment();
	void erosion(int i,int member,float change);

	void crater(int x,int y,float radius,float *map,float value,bool add);
	void plateu(int x,int y,float radius,float *map,float value,bool add);
	void segment(int x,int y,int w,int h,float *map,float value,bool add);

	float sparkle();

	float chanceOfMutation();

public:
	Planet();
	~Planet();

	void clear();
	void create(int w,int h,int con,int lnd,int mnt,int isl,int fst,int ppl,int tec);
	void run();

	int getTime() { return time; }
	int getCycle() { return cycle; }
	int getEra() { return era; }
	const char *getEraDescription(int e=-1);
	const PlanetCreationInfo &getResult() { return result; }
	bool isCompleted() { return completed; }

	RGBA_Color getColor(int x,int y,int filter) { return getColor(x+y*width,filter); }
	RGBA_Color getColor(int i,int filter);

	const PlanetPoint &getPoint(int x,int y) { return surface[x+y*width]; }
	void getArea(int x,int y,int radius,PlanetPoint **a);
};



}; /* namespace a */


#endif /* _AMANITA_GAME_PLANET_H */

