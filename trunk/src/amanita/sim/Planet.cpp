
#include "../_config.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <amanita/Math.h>
#include <amanita/Random.h>
#include <amanita/sim/Planet.h>


static const char *era_descriptions[] = { "",
	"Birth of the planet. It's a globe of magma, fire, ash and stone.",
	"The great oceans are formed, continents erode by the water, harder minerals becomes mountains.",
	"Life appears in the sea, and spreads.",
	"Sapient beings cultivate and create cities.",
	"All is done."
};


static int planet_point_members[16];

static const float OCEAN_LEVEL				= 0.33f;
static const float COAST_LEVEL				= 0.0f;
static const float MAGMA_LEVEL				= 2.1f;
static const float TUNDRA_LEVEL				= 0.97f;
static const float DESERT_LEVEL				= 1.295f;
static const float PLANCTON_LEVEL			= 0.5f;
static const float FOREST_LEVEL				= 0.55f;
static const float MOUNTAIN_LEVEL			= 0.73f;
static const float CITY_LEVEL					= 0.1f;

static const float sea_level					= 0.5f;
static const float sea_level_multiply		= 1.0f/sea_level;

static const float life_exists				= 0.00001f;
static const float life_spreads				= 0.2f;
static const float life_optimal				= 1.24f;
static const float life_min					= 0.9f;
static const float life_max					= 1.6f;
static const float life_min_multiply		= 1.0f/(life_optimal-life_min);
static const float life_max_multiply		= 1.0f/(life_max-life_optimal);

static const float city_capacity				= 1.0f;


namespace a {

inline float adjust_value(float value) { return value<0.0f? 0.0f : (value>1.0f? 1.0f : value); }

inline RGBA_Color environment_color(float c) { return (RGBA_Color){0xff,(uint8_t)(221.0f*c),0}; }
inline RGBA_Color magma_color(float c) { return (RGBA_Color){0xff,(uint8_t)(221.0f*c),0}; }
inline RGBA_Color glacier_color(float c) { return (RGBA_Color){(uint8_t)(0xff-(int)(153.0f*c)),(uint8_t)(0xff-(int)(102.0f*c)),0xff}; }
inline RGBA_Color mountain_color(float c) { return (RGBA_Color){(uint8_t)(255.0f*c),(uint8_t)(221.0f*c),(uint8_t)(187.0f*c)}; }
inline RGBA_Color ocean_color(float c) { return (RGBA_Color){0,(uint8_t)(0xcc-(int)(204.0f*c)),(uint8_t)(0xff-(int)(204.0f*c))}; }
inline RGBA_Color desert_color(float c) { return (RGBA_Color){(uint8_t)(0xcc+(int)(51.0f*c)),(uint8_t)(0xcc+(int)(51.0f*c)),(uint8_t)(0x33+(int)(153.0f*c))}; }
inline RGBA_Color plancton_color(float c) { return (RGBA_Color){0,(uint8_t)(0xff-(int)(204.0f*c)),(uint8_t)(0xcc-(int)(204.0f*c))}; }
inline RGBA_Color vegetation_color(float c) { return (RGBA_Color){(uint8_t)(0x33-(int)(51.0f*c)),(uint8_t)(0xcc-(int)(102.0f*c)),(uint8_t)(0x33-(int)(51.0f*c))}; }
inline RGBA_Color city_color(float c) { return (RGBA_Color){0xff,(uint8_t)(0xff-(int)(255.0f*c)),0}; }
inline RGBA_Color ir_color(float c) {
	return c<TUNDRA_LEVEL? glacier_color(c) :
				(c>life_max? magma_color(c-life_max) :
					vegetation_color(c<life_optimal? (c-life_min)*life_min_multiply : 1.0f-(c-life_optimal)*life_max_multiply)); }


Planet::Planet() {
	surface = 0;
}

Planet::~Planet() {
	clear();
}

void Planet::clear() {
	if(surface) {
		free(surface);
		surface = 0;
	}
}

void Planet::create(int w,int h,int con,int lnd,int mnt,int isl,int fst,int ppl,int tec) {
debug_output("Planet::create(1)              ");
	int i,n,x,x1,y1,w1;
	float sz;

	clear();

	width = w;
	height = h;
	size = w*h;
	wrap = PLANET_WRAP_HORIZ;

	sea = 0.0f;
	equator = (float)(height/2);

	time = 0;
	cycle = 0;
	era = PLANET_ERA_NEW;

	seed = rnd.getSeed();
	chaos.setSeed(seed);

debug_output("\rPlanet::create(seed=%lu)     ",seed);

	surface = (PlanetPoint *)malloc(size*sizeof(PlanetPoint));
	memset(surface,0,sizeof(PlanetPoint)*size);
	for(i=0; i<size; ++i) getArea(i%width,i/width,4,surface[i].area);

	memset(&result,0,sizeof(PlanetCreationInfo));

	{
		PlanetPoint &p = surface[0];
		planet_point_members[PLANET_TEMPERATURE] = ((char *)&p.temperature)-((char *)&p);
		planet_point_members[PLANET_ALTITUDE] = ((char *)&p.altitude)-((char *)&p);
		planet_point_members[PLANET_SEA] = ((char *)&p.sea)-((char *)&p);
		planet_point_members[PLANET_ENVIRONMENT] = ((char *)&p.environment)-((char *)&p);
		planet_point_members[PLANET_VEGETATION] = ((char *)&p.vegetation)-((char *)&p);
		planet_point_members[PLANET_POPULATION] = ((char *)&p.population)-((char *)&p);
		planet_point_members[PLANET_CULTURE] = ((char *)&p.culture)-((char *)&p);
		planet_point_members[PLANET_INLAND] = ((char *)&p.inland)-((char *)&p);
	}

debug_output("\nPlanet::create(2,sizeof(PlanetPoint)=%lu,surface=%lu)\n",sizeof(PlanetPoint),size*sizeof(PlanetPoint));
//for(i=0; i<100; ++i)
//debug_output("Planet::create(sparkle=%e)\n",sparkle());

	completed = false;

	ci = (PlanetCreationInfo){
		con,
		(float)lnd/100.0f,
		(float)mnt/100.0f,
		(float)isl/100.0f,
		(float)fst/100.0f,
		(float)ppl/100.0f,
		(float)tec/100.0f
	};

	{
		float layer1[width*height];
		float layer2[width*height];

		memset(layer1,0,sizeof(layer1));
		memset(layer2,0,sizeof(layer2));

debug_output("\rPlanet::create(3)");
		for(i=1,n=ci.continents,x=chaos.uint32(width),w1=width/n; i<=n; i++,x+=w1) {
			x1 = x+chaos.uint32(w1),y1 = height/n+chaos.uint32(height-height*2/n);
			if(x1>=width) x1 -= width;
			sz = (float)n/20.0f;
			sz = (1.0f-sz)+chaos.real64()*sz*2.0f;
			sz = (0.1f+ci.landmass*1.0f+0.5f*(n/5.0f))*w1*(1.5f-ci.mountains*0.7f)*sz;
debug_output("\nPlanet::create(x=%d,x1=%d,y1=%d,sz=%f)\n",x,x1,y1,sz);
			plateu(x1,y1,sz,layer1,0.9f,false);
//			segment(x1,height/10,width/10+chaos.int32(width/10),height-height/5,PLANET_TEMPERATURE,0.9f,false);
		}

debug_output("\rPlanet::create(4)");
		for(i=1,n=50; i<=n; i++)
			plateu(chaos.uint32(width),chaos.uint32(height),(height/10+chaos.uint32(height/5)),
				layer1,chaos.real64()*(-0.2f),true);

		for(i=1,n=150; i<=n; i++)
			plateu(chaos.uint32(width),chaos.uint32(height),(height/20+chaos.uint32(height/10))*(0.5f+ci.mountains),
				layer1,chaos.real64()*0.20f,true);
debug_output("\rPlanet::create(5)");

		for(i=1,n=16; i<=n; i++)
			crater(chaos.uint32(width),chaos.uint32(height),(height/16+chaos.uint32(height/8)),
				layer2,ci.mountains*0.3f,true);


		for(i=0,sz=0.2f+ci.islands; i<size; i++) {
			PlanetPoint &p = surface[i];
			p.temperature = layer1[i]+layer2[i];
			p.temperature += chaos.real64()*sz*2.0f-sz;
			p.temperature = 3.0f-p.temperature;
			if(p.temperature<2.0f) p.temperature = 2.0f+chaos.real64()*.2f;
			else if(p.temperature>3.0f) p.temperature = 3.0f-chaos.real64()*.1f;
			p.altitude = 1.0f;
		}
	}

debug_output("\rPlanet::create(6)\n");
}

void Planet::run() {
	int e = 0;
	if(cycle==0 || cycle==cycles) nextEra();
	++time;
	++cycle;
	switch(era) {
		case PLANET_ERA_NEW:cycle = 0;break;
		case PLANET_ERA_COOL:e = cool(1.0f);break;
		case PLANET_ERA_CLIMATE:e = climate(1.0f);break;
		case PLANET_ERA_VEGETATION:e = vegetation(1.0f);break;
		case PLANET_ERA_CIVILISATION:e = civilisation(1.0f);break;
		case PLANET_ERA_COMPLETED:break;
	}
	if(e==0) cycle = 0;
}

void Planet::nextEra() {
	switch(++era) {
		case PLANET_ERA_NEW:break;
		case PLANET_ERA_COOL:cycles = 1000;break;
		case PLANET_ERA_CLIMATE:cycles = 1000;break;
		case PLANET_ERA_VEGETATION:
			environment();
			cycles = 2000;
			break;
		case PLANET_ERA_CIVILISATION:
			cycles = 1000;
//			int i,j,sz = sizeof(PlanetPoint);
//			float l;
//			for(i=0,j; i<size; i++) {
//				PlanetPoint &p = surface[i];
//				for(j=0,l=.0f; j<9; j++)
//					if(p.area[j]) l += p.area[j]->vegetation*(j==0? 1.0f : (j<5? .3f : (j<10? .1f : .03f)));
//				p.population = l;
//			}
			break;
		case PLANET_ERA_COMPLETED:completed = true;break;
	}
	cycle = 0;

debug_output("\r%s                                       \n",era_descriptions[era]);
}



int Planet::cool(float change) {
	int i,e = 0;
	float c1 = 0.01f*change;
	float c2 = 0.005f*change;
	for(i=0; i<size; ++i) {
		PlanetPoint &p = surface[i];
		if(p.temperature>2.0f) {
			p.temperature -= c1;
			p.altitude -= c1;
			++e;
		}
		erosion(i,PLANET_TEMPERATURE,c2);
	}
debug_output("\rcool[%d]: e=%d                          ",cycle,e);
	return e;
}

int Planet::climate(float change) {
	int i,x,y,l;
	float lat;
	float c1 = 0.015f*change;
	float c2 = 0.015f*change;
	float c3 = 0.25f*change;

	sea += 0.005f*change;

	for(y=0; y<height; ++y) {
		lat = 1.0f-pow(fabs(equator-y)/equator,3.0f);
		for(x=0; x<width; ++x) {
			PlanetPoint &p = surface[i=x+y*width];
			if(p.sea<0.05f) {
				if(p.temperature>(0.8f+0.5f*(((1.0f-p.altitude)/(1.0f-sea_level))*lat))) p.temperature -= c1;
			} else {
				if(p.temperature>(0.8f+0.8f*(p.altitude/sea_level)+0.2f*lat)) p.temperature -= c2;
				if(p.altitude<0.001f) p.temperature += c3;
			}
			erosion(i,PLANET_TEMPERATURE,p.sea>0.05? 0.05f : 0.005f);
			p.sea = sea-p.altitude;
		}
	}
	for(i=0,l=0; i<size; ++i) {
		PlanetPoint &p = surface[i];
		erosion(i,PLANET_ALTITUDE,(p.sea>0.0f? (1.0f-p.sea)*0.001f :
			(p.temperature<1.0f? 0.1f*(1.0f-p.temperature) : 0.002f*p.temperature)));
		if(p.sea<=0.0f) ++l;
	}
	if(((float)l/(float)size)<=ci.landmass) {
		printf("\rLandmass reached %d%% at sea level %g...\n",(int)a::round(100.0*((double)l/(double)size)),sea);
		return 0;
	}
debug_output("\rclimate[%d]                             ",cycle);
	return 1;
}

int Planet::vegetation(float change) {
	int i,j,e = 0;
	float c1 = 0.05f*change;
//	float c2 = 0.001f*change;
	for(i=0; i<size; ++i) {
		PlanetPoint &p = surface[i];
		if(p.vegetation>=life_exists) {
			if(p.vegetation>=life_spreads) {
				for(j=1; j<=9; ++j) if(p.area[j]) {
					/*PlanetPoint &pa = *p.area[j];
					if(pa.vegetation==0.0f) {
						if((p.sea<=0.0f && pa.sea<=0.0f) || (p.sea>0.0f && pa.sea>0.0f)) pa.vegetation += life_exists;
						else if(pa.sea<=0.0f) pa.vegetation += 1000.0f*sparkle(),++e;
					}*/
					if(p.area[j]->vegetation==0.0f) p.area[j]->vegetation += life_exists;
				}
			}
			if(p.vegetation>=p.environment) p.vegetation = p.environment;
			else p.vegetation += p.environment*c1,++e;
//			erosion(i,PLANET_VEGETATION,c2);
		} else if(p.sea>0.0f && p.environment>=0.96f) p.vegetation += sparkle(),++e;
	}
debug_output("\rbiology[%d]: e=%d                       ",cycle,e);
	return e;
}

int Planet::civilisation(float change) {
	int i,j,e = 0;
	float l,v;
	float c1 = 0.005f*change;
	float c2 = 0.001f*change;
	float ca1 = 0.005f*change;
	float ca2 = 0.003f*change;
	float ca3 = 0.001f*change;
	float cv1 = 0.01f*change;
	float cv2 = 0.003f*change;
	float cv3 = 0.001f*change;
	vegetation(c1);
	for(i=0; i<size; i++) {
		PlanetPoint &p = surface[i];
		for(j=0,l=0.0f; j<9; j++)
			if(p.area[j]) l += p.area[j]->vegetation*(j==0? 1.0f : (j<5? 0.3f : 0.1f));
		if(p.population>l) p.population = l;
		else {
			p.population += l*c2,++e;
			if(p.population<=city_capacity) for(j=1; j<21; j++)
				if(p.area[j] && p.area[j]->population>0.0f && p.population>p.area[j]->population)
					v = (j<5? ca1 : (j<10? ca2 : ca3)),p.population += v,p.area[j]->population -= v;
		}
		for(j=0; j<9; j++) if(p.area[j]) {
			v = (j==0? cv1 : (j<5? cv2 : cv3));
			if(p.area[j]->vegetation>0.0f) p.area[j]->vegetation -= p.population*v;
		}
		if(p.vegetation<0.0f) p.vegetation = 0.0f;
	}
debug_output("\rpopulate[%d]: e=%d                      ",cycle,e);
	return e;
}

void Planet::environment() {
	int i;
	float t,c;
	for(i=0; i<size; i++) {
		PlanetPoint &p = surface[i];
/*		t = sea_level-p.altitude;
		if(t>0) t = pow(1.0f-(t/sea_level),2);
		else t = pow(1.0f-(fabs(t)/(1.0f-sea_level)),2);
		c = p.temperature>=.7f? (p.temperature-.7f)/.67f : .0f;
		if(c>1.0f) c -= (c-1.0f)*3.0f;
		p.environment = t*c;
		if(p.environment<0.0f) p.environment = 0.0f;*/

		t = 1.0f;//sea_level-p.altitude;
//		if(t>0.0f) t = pow(1.0f-(t/sea_level),2);
//		else t = pow(1.0f-(fabs(t)/(1.0f-sea_level)),2);
		if(p.temperature>=life_optimal && p.temperature<=life_max) c = 1.0f-(p.temperature-life_optimal)*life_max_multiply;
		else if(p.temperature>life_min) c = (p.temperature-life_min)*life_min_multiply;
		else c = 0.0f;
//		if(c>1.0f) c -= (c-1.0f)*3.0f;
		if(c>1.0f) c = 1.0f;
		p.environment = t*c;
//		if(p.environment<0.0f) p.environment = 0.0f;
	}
}

void Planet::erosion(int i,int member,float change) {
	int j;
	PlanetPoint &p = surface[i];
	float n,*n1 = (float *)(((char *)&p)+planet_point_members[member]),*n2;
	for(j=1; j<9; ++j) if(p.area[j]) {
		n2 = (float *)(((char *)p.area[j])+planet_point_members[member]);
		if(*n2<=0.0f) continue;
		n = (((*n2+1.0f)/(*n1+1.0f))-1.0f)*change;
		*n1 += n,*n2 -= n;
	}
}


void Planet::crater(int x,int y,float radius,float *map,float value,bool add) {
	int i,j,n,s = a::round(radius);
	float d,min = 100.0f;
	for(i=x-s; i<x+s*2; ++i)
		for(j=y-s; j<y+s*2; ++j)
			if(((wrap&PLANET_WRAP_HORIZ) || (i>=0 && i<width)) && ((wrap&PLANET_WRAP_VERT) || (j>=0 && j<height))) {
				d = a::distance(x+(width*2),y+(height*2),i+(width*2),j+(height*2));
				if(d<=radius) {
					n = (i<0? width+i : (i>=width? i-width : i))+(j<0? height+j : (j>=height? j-height : j))*width;
					if(d>radius*0.9f) {
						if(add) map[n] += value;
						else if(map[n]<value) map[n] = value;
					} else if(map[n]<min) min = map[n];
				}
			}
	for(i=x-s; i<x+s*2; ++i)
		for(j=y-s; j<y+s*2; ++j)
			if(((wrap&PLANET_WRAP_HORIZ) || (i>=0 && i<width)) && ((wrap&PLANET_WRAP_VERT) || (j>=0 && j<height))) {
				d = a::distance(x+(width*2),y+(height*2),i+(width*2),j+(height*2));
				if(d<=radius*0.9f) {
					n = (i<0? width+i : (i>=width? i-width : i))+(j<0? height+j : (j>=height? j-height : j))*width;
					map[n] = min;
				}
			}
	plateu(x,y,radius*0.2f,map,value,add);
}

void Planet::plateu(int x,int y,float radius,float *map,float value,bool add) {
	int i,j,n,s = a::round(radius);
	float d,v;
	for(i=x-s; i<x+s*2; ++i)
		for(j=y-s; j<y+s*2; ++j)
			if(((wrap&PLANET_WRAP_HORIZ) || (i>=0 && i<width)) && ((wrap&PLANET_WRAP_VERT) || (j>=0 && j<height))) {
				d = a::distance(x+(width*2),y+(height*2),i+(width*2),j+(height*2));
				if(d<=radius) {
					n = (i<0? width+i : (i>=width? i-width : i))+(j<0? height+j : (j>=height? j-height : j))*width;
					v = (float)(1.0f-(d/radius))*value;
					if(add) map[n] += v;
					else if(map[n]<v) map[n] = v;
				}
			}
}

void Planet::segment(int x,int y,int w,int h,float *map,float value,bool add) {
	int i,j,n,x1,y1;
	for(i=0; i<w; ++i)
		for(j=0; j<h; ++j) {
			x1 = (x+i)%width,y1 = (y+j)%height;
			if(((wrap&PLANET_WRAP_HORIZ) || (x1>=0 && x1<width)) && ((wrap&PLANET_WRAP_VERT) || (y1>=0 && y1<height))) {
				n = x1+y1*width;
				if(add) map[n] += value;
				else if(map[n]<value) map[n] = value;
			}
		}
}

float Planet::sparkle() {
	float r = chaos.real64();
	return life_exists*0.01f*r*r*r;
}

const char *Planet::getEraDescription(int e) {
	return era_descriptions[e>PLANET_ERA_NEW && e<=PLANET_ERA_COMPLETED? e : era];
}

RGBA_Color Planet::getColor(int i,int filter) {
	PlanetPoint &p = surface[i];
	if(filter==1) return ir_color(p.temperature);
	else if(filter==2) return environment_color(p.environment);
	else {
		if(p.temperature>MAGMA_LEVEL) return magma_color(adjust_value(p.temperature-2.0f));
		else if(p.temperature<TUNDRA_LEVEL) return glacier_color(adjust_value(p.temperature-0.6f));
		else if(p.sea>COAST_LEVEL) {
			if(filter==3 && p.population>CITY_LEVEL) return city_color(adjust_value(p.population-CITY_LEVEL));
			else if(filter==4 && p.vegetation>PLANCTON_LEVEL) return plancton_color(adjust_value((p.vegetation-.5f)*2.0f));
			else if(filter==5) return mountain_color(adjust_value((p.altitude)));
			else return ocean_color(adjust_value(p.sea));
		} else {
			if(p.population>CITY_LEVEL) return city_color(adjust_value(p.population-CITY_LEVEL));
			else if(p.vegetation>FOREST_LEVEL) return vegetation_color(adjust_value((p.vegetation-0.5f)*2.0f));
			else if(p.vegetation>FOREST_LEVEL) return vegetation_color(adjust_value((p.vegetation-0.5f)*2.0f));
			else if(p.altitude>MOUNTAIN_LEVEL) return mountain_color(adjust_value((p.altitude-sea_level)*sea_level_multiply));
			else return desert_color(adjust_value((p.altitude-sea_level)*sea_level_multiply));
		}
	}
	return (RGBA_Color){0,0,0,0};
}

#define RIGHT1(x) (x>=width-1? x+1-width : x+1)
#define BOTTOM1(y) (y>=height-1? y+1-height : y+1)
#define LEFT1(x) (x<1? x+width-1 : x-1)
#define TOP1(y) (y<1? y+height-1 : y-1)
#define RIGHT2(x) (x>=width-2? x+2-width : x+2)
#define BOTTOM2(y) (y>=height-2? y+2-height : y+2)
#define LEFT2(x) (x<=1? width+x-2 : x-2)
#define TOP2(y) (y<=1? height+y-2 : y-2)

void Planet::getArea(int x,int y,int radius,PlanetPoint **a) {
	bool hw = (wrap&PLANET_WRAP_HORIZ),vw = (wrap&PLANET_WRAP_VERT);
	a[0] = &surface[x+y*width];
	if(radius>=1) {
		a[2] = vw || y<height-1? &surface[x+BOTTOM1(y)*width] : 0;
		a[4] = vw || y>0? &surface[x+TOP1(y)*width] : 0;
		if(hw || x<width-1) {
			a[1] = &surface[RIGHT1(x)+y*width];
			if(radius>=2) {
				a[5] = vw || y>0? &surface[RIGHT1(x)+TOP1(y)*width] : 0;
				a[6] = vw || y<height-1? &surface[RIGHT1(x)+BOTTOM1(y)*width] : 0;
			}
		} else a[1] = a[5] = a[6] = 0;
		if(hw || x>0) {
			a[3] = &surface[LEFT1(x)+y*width];
			if(radius>=2) {
				a[7] = vw || y<height-1? &surface[LEFT1(x)+BOTTOM1(y)*width] : 0;
				a[8] = vw || y>0? &surface[LEFT1(x)+TOP1(y)*width] : 0;
			}
		} else a[3] = a[7] = a[8] = 0;

		if(radius>=3) {
			if(hw || x<width-2) {
				a[9] = &surface[RIGHT2(x)+y*width];
				if(radius>=4) {
					a[13] = vw || y>0? &surface[RIGHT2(x)+TOP1(y)*width] : 0;
					a[14] = vw || y<height-1? &surface[RIGHT2(x)+BOTTOM1(y)*width] : 0;
				}
			} else a[9] = a[13] = a[14] = 0;

			if(vw || y<height-2) {
				a[10] = &surface[x+BOTTOM2(y)*width];
				if(radius>=4) {
					a[15] = hw || x<width-1? &surface[RIGHT1(x)+BOTTOM2(y)*width] : 0;
					a[16] = hw || x>0? &surface[LEFT1(x)+BOTTOM2(y)*width] : 0;
				}
			} else a[10] = a[15] = a[16] = 0;

			if(hw || x>1) {
				a[11] = &surface[LEFT2(x)+y*width];
				if(radius>=4) {
					a[17] = vw || y<height-1? &surface[LEFT2(x)+BOTTOM1(y)*width] : 0;
					a[18] = vw || y>0? &surface[LEFT2(x)+TOP1(y)*width] : 0;
				}
			} else a[11] = a[17] = a[18] = 0;

			if(vw || y>1) {
				a[12] = &surface[x+TOP2(y)*width];
				if(radius>=4) {
					a[19] = hw || x>0? &surface[LEFT1(x)+TOP2(y)*width] : 0;
					a[20] = hw || x<width-1? &surface[RIGHT1(x)+TOP2(y)*width] : 0;
				}
			} else a[12] = a[19] = a[20] = 0;
		}
	}
}


}; /* namespace a */

