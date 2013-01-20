
#include "../_config.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <amanita/Math.h>
#include <amanita/Random.h>
#include <amanita/sim/Planet.h>


static const char *era_descriptions[] = { "",
	"Birth of the planet. It's a globe of magma, fire, ash and stone.",
	"The great oceans are formed, continents erode by the water, harder minerals becomes mountains.",
	"Life appears in the sea, and spreads.",
	"Plants grow up onto dry land, and continues to spread.",
	"Sapient beings cultivate and create cities.",
	"All is done."
};


namespace a {


inline RGBA_Color environment_color(float c) { return c>=0.99f? (RGBA_Color){0xff,0xff,0xff} : (RGBA_Color){0xff,(uint8_t)(221.0f*c),0}; }
inline RGBA_Color magma_color(float c) { return (RGBA_Color){0xff,(uint8_t)(221.0f*c),0}; }
inline RGBA_Color glacier_color(float c) { return (RGBA_Color){(uint8_t)(0xff-(int)(153.0f*c)),(uint8_t)(0xff-(int)(102.0f*c)),0xff}; }
inline RGBA_Color mountain_color(float c) { return (RGBA_Color){(uint8_t)(255.0f*c),(uint8_t)(221.0f*c),(uint8_t)(187.0f*c)}; }
inline RGBA_Color ocean_color(float c) { return (RGBA_Color){0,(uint8_t)(0xcc-(int)(204.0f*c)),(uint8_t)(0xff-(int)(204.0f*c))}; }
inline RGBA_Color desert_color(float c) { return (RGBA_Color){(uint8_t)(0xcc+(int)(51.0f*c)),(uint8_t)(0xcc+(int)(51.0f*c)),(uint8_t)(0x33+(int)(153.0f*c))}; }
inline RGBA_Color plancton_color(float c) { return (RGBA_Color){0,(uint8_t)(0xff-(int)(204.0f*c)),(uint8_t)(0xcc-(int)(204.0f*c))}; }
inline RGBA_Color vegetation_color(float c) { return (RGBA_Color){(uint8_t)(0x33-(int)(51.0f*c)),(uint8_t)(0xcc-(int)(102.0f*c)),(uint8_t)(0x33-(int)(51.0f*c))}; }
inline RGBA_Color city_color(float c) { return (RGBA_Color){(uint8_t)(0x99+(int)(102.0f*c)),0,0}; }
inline RGBA_Color ir_color(float value) { return value<1.0f? glacier_color(value) : (value>1.6f? magma_color(value-1.6f) : vegetation_color(1.0f-(value-1.0f)*(1.0f/.6f))); }


const float OCEAN_LEVEL				= 0.33f;
const float COAST_LEVEL				= 0.0f;
const float MAGMA_LEVEL				= 2.1f;
const float TUNDRA_LEVEL			= 0.9f;
const float DESERT_LEVEL			= 1.295f;
const float FOREST_LEVEL			= 0.55f;
const float MOUNTAIN_LEVEL			= 0.75f;
const float CITY_LEVEL				= 0.5f;



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
debug_output("Planet::create(1)\n");
	size_t i,n;
	int x,x1,y1,w1;
	float sz;

	clear();

	width = w;
	height = h;
	size = w*h;
	wrap = PLANET_WRAP_HORIZ;

	seaLevel = .5f;
	seaLevelMultiply = 1.0f/seaLevel;
	equator = (float)height/2.0f;

	time = 0;
	cycle = 0;
	era = 0;
	eraPresent = 0;
	eraDescription = 0;

	surface = (PlanetPoint *)malloc(sizeof(PlanetPoint)*size);
	memset(&result,0,sizeof(PlanetCreationInfo));

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

debug_output("Planet::create(2)\n");
	memset(surface,0,sizeof(PlanetPoint)*size);

debug_output("Planet::create(3)\n");
	for(i=1,n=ci.continents,x=rnd.uint32(width),w1=width/n; i<=n; i++,x+=w1) {
		x1 = x+rnd.uint32(w1),y1 = height/n+rnd.uint32(height-height*2/n);
		if(x1>=width) x1 -= width;
		sz = (float)n/20.0f;
		sz = (1.0f-sz)+rnd.real64()*sz*2.0f;
		sz = (.1f+ci.landmass*1.0f+.5f*(n/5.0f))*w1*(1.5f-ci.mountains*.7f)*sz;
debug_output("Planet::create(x=%d,x1=%d,y1=%d,sz=%f)\n",x,x1,y1,sz);
		impact(x1,y1,PLANET_TEMPERATURE,sz,.9f,false);
	}
debug_output("Planet::create(4)\n");
	for(i=1,n=50; i<=n; i++)
		impact(rnd.uint32(width),rnd.uint32(height),PLANET_TEMPERATURE,
			(width/20+rnd.uint32(width/10))*(.5f+ci.mountains),rnd.real64()*ci.mountains*.6f-.2f,true);
debug_output("Planet::create(5)\n");

	for(i=0,sz=ci.islands; i<size; i++) {
		PlanetPoint &p = surface[i];
		p.temperature += rnd.real64()*sz*2.0f-sz;
		p.temperature = 3.0f-p.temperature;
		if(p.temperature<2.0f) p.temperature = 2.0f+rnd.real64()*.2f;
		else if(p.temperature>3.0f) p.temperature = 3.0f-rnd.real64()*.1f;
		p.altitude = 1.0f;
	}
debug_output("Planet::create(6)\n");
}

void Planet::run() {
	if(cycle==0 || cycle==cycles) nextEra();
	++time;
	++cycle;
	switch(era) {
		case 1:cool();break;
		case 2:
			climate(.1f);
			ocean();
			break;
		case 3:
		case 4:
			if(biology(.05f)==0) cycle = 0;
			break;
		case 5:
			populate();
			break;
	}
}

void Planet::nextEra() {
	switch(++era) {
		case 1:
			cycles = 1000;
//			for(int i=0; i<size; i++) surface[i].sea = -1.0f;
			break;
		case 2:
			cycles = 100;
			break;
		case 3:
		{
			cycles = 100;
			float t,c;
			for(size_t i=0; i<size; i++) {
				PlanetPoint &p = surface[i];

				t = belowSeaLevel(p.altitude);
				if(t>0) t = pow(1.0f-(t/seaLevel),2);
				else t = pow(1.0f-(fabs(t)/(1.0f-seaLevel)),2);

				c = p.temperature>=.7f? (p.temperature-.7f)/.67f : .0f;
				if(c>1.0f) c -= (c-1.0f)*3.0f;

				p.environment = t*c;
				if(p.environment<.0f) p.environment = .0f;
			}
			break;
		}
		case 4:
		{
			cycles = 100;
			break;
		}
		case 5:
		{
			cycles = 30+(int)(ci.cities*10.0f);
//			PlanetPoint **a;
//			int i,j,sz = sizeof(PlanetPoint);
//			float l;
//			for(i=0,j; i<size; i++) {
//				PlanetPoint &p = surface[i];
//				a = (PlanetPoint **)getArea(i%width,i/width,4,surface,sz);
//				for(j=0,l=.0f; j<9; j++) if(a[j]) l += a[j]->life*(j==0? 1.0f : (j<5? .3f : (j<10? .1f : .03f)));
//				p.population = l;
//			}
			break;
		}
		case 6:
			completed = true;
			break;
	}
	cycle = 0;

debug_output("%s\n",era_descriptions[era]);
}



void Planet::cool() {
	float change = .01f;
	int e = 0;
	for(size_t i=0; i<size; i++) {
		PlanetPoint &p = surface[i];
		if(p.temperature>2.0f) {
			p.temperature -= change;
			p.altitude -= change;
			e++;
		}
		erosion(i,PLANET_TEMPERATURE,.005f);
	}
	if(e==0) cycle = 0;
}


void Planet::climate(float change) {
	float lat;
	int x,y,i;
	for(y=0; y<height; y++) {
		lat = 1.0f-pow(fabs(equator-y)/equator,3);
		for(x=0; x<width; x++) {
			PlanetPoint &p = surface[i=x+y*width];
			if(p.sea<.05f) {
				if(p.temperature>(.8f+.5f*(/*((1.0f-p.altitude)/(1.0f-seaLevel))**/lat))) p.temperature -= change;
			} else {
				if(p.temperature>(.8f+.2f*(p.altitude/seaLevel)+.2f*lat)) p.temperature -= change;
				if(p.altitude<.0f) p.temperature += change;
			}
			erosion(i,PLANET_TEMPERATURE,p.sea>.05? .05f : .005f);
			if(p.altitude+p.sea>=seaLevel) p.sea = seaLevel-p.altitude;
			else p.sea += .01f;
		}
	}
}

void Planet::ocean() {
	PlanetPoint **a;
	float as1,as2,s2,med;
	size_t j,sz = sizeof(PlanetPoint);
	for(size_t i=0; i<size; i++) {
		PlanetPoint &p = surface[i];
		erosion(i,PLANET_ALTITUDE,(p.sea>.0f? (1.0f-p.sea)*.001f :
			(p.temperature<.8f? .1f*(1.0f-p.temperature) : .0005f*p.temperature)));
		as1 = p.altitude+p.sea;
		a = (PlanetPoint **)getArea(i%width,i/width,2,surface,sz);
		for(j=1; j<9; j++) if(a[j]) {
			s2 = a[j]->sea,as2 = a[j]->altitude+s2;
			if(as2>as1) {
				med = (as2-as1)/2.0f;
//				if(med>s2) med = s2;
				p.sea += med,a[j]->sea -= med;
			}
		}
	}
}

/*
int Planet::biology(float change) {
	int e = 0;
	for(size_t i=0; i<size; i++) {
		PlanetPoint &p = surface[i];
		if(p.temperature<DESERT_LEVEL || p.vegetation>.1f) {
			if(p.life>=p.environment) p.life = p.environment;
			else p.life += p.environment*change,e++;
			if(p.life>p.vegetation) p.vegetation += p.life*(ci.forest*2.0f)*.2;
			erosion(i,PLANET_VEGETATION,.001f);
		}
	}
	return e;
}
*/


int Planet::biology(float change) {
	int e = 0;
	PlanetPoint **a;
	size_t j,sz = sizeof(PlanetPoint);
	for(size_t i=0; i<size; ++i) {
		PlanetPoint &p = surface[i];
		if(p.temperature<DESERT_LEVEL || p.vegetation>.1f) {
			if(p.life>=0.00001f) {
				if(p.life>=0.28f) {
					a = (PlanetPoint **)getArea(i%width,i/width,2,surface,sz);
					if(era==3) {
						for(j=1; j<=9; ++j) if(j!=5) {
							if(p.sea>0.0f && a[j]->sea==0.0f) {
								a[j]->life += chanceOfEvolution();
								if(a[j]->life>=0.00001f) return -1;
							} else if(a[j]->life==0.0f) a[j]->life = 0.00001f;
						}
					} else for(int j=1; j<=9; j++) if(j!=5 && a[j]->life==0.0f) {
						a[j]->life += 0.00001f;
					}
				}
				if(p.life>=p.environment) p.life = p.environment;
				else p.life += p.environment*change,++e;
				if(p.life>p.vegetation) p.vegetation += p.life*(ci.forest*2.0f)*.2;
				erosion(i,PLANET_VEGETATION,.001f);
			} else if(p.sea>0.0f && p.environment>=0.96f) p.life += chanceOfEvolution();
		}
	}
	return e;
}


void Planet::populate() {
	PlanetPoint **a,*n;
	size_t j,sz = sizeof(PlanetPoint);
	float l,v;
	biology(.005f);
	for(size_t i=0; i<size; i++) {
		PlanetPoint &p = surface[i];
		a = (PlanetPoint **)getArea(i%width,i/width,4,surface,sz);
		for(j=0,l=.0f; j<9; j++) if(a[j]) l += a[j]->life*(j==0? 1.0f : (j<5? .3f : .1f));
		if(p.population>l) p.population = l;
		else {
			p.population += l*.001f;
			for(j=1; j<21; j++) if((n=a[j]) && n->population>.0f && p.population>n->population)
					v = (j<5? .005f : (j<10? .003f : .001f)),p.population += v,n->population -= v;
		}
		for(j=0; j<9; j++) if(a[j]) {
			v = (j==0? .01f : (j<5? .003f : .001f));
			a[j]->life -= p.population*v;
			a[j]->vegetation -= p.population*v*.01f;
		}
	}
}


void Planet::impact(int x,int y,int member,float radius,float value,bool add) {
	int n,s = a::round(radius);
	float d;
	float *p,v;
	for(int i=x-s; i<x+s*2; i++)
		for(int j=y-s*2; j<y+s*4; j++)
			if(((wrap&PLANET_WRAP_HORIZ) || (i>=0 && i<width)) && ((wrap&PLANET_WRAP_VERT) || (j>=0 && j<height))) {
				d = a::distance((long)(x+(width<<1)),(y>>1)+(height<<1),i+(width<<1),(j>>1)+(height<<1));
				if(d<=radius) {
					n = (i<0? width+i : (i>=width? i-width : i))+(j<0? height+j : (j>=height? j-height : j))*width;
					p = (float *)(((char *)&surface[n])+member);
					v = (float)(1.-(d/radius))*value;
					if(add) *p += v;
					else if(*p<v) *p = v;
				}
			}
}

void Planet::erosion(int i,int member,float change) {
	PlanetPoint **a = (PlanetPoint **)getArea(i%width,i/width,2,surface,sizeof(PlanetPoint));
	float n,*n1 = (float *)(((char *)a[0])+member),*n2;
	for(int j=1; j<9; j++) if(a[j]) {
		n2 = (float *)(((char *)a[j])+member);
		if(*n2<=.0f) continue;
		n = (((*n2+1.0f)/(*n1+1.0f))-1.0f)*change;
		*n1 += n,*n2 -= n;
	}
}

int Planet::getEraDescription(int e) {
	return era_descriptions[e>=1 && e<PLANET_ERAS? e : era];
}

RGBA_Color Planet::getColor(int i,int filter) {
	PlanetPoint &p = surface[i];
	if(filter==1) return ir_color(p.temperature);
	else if(filter==2) return environment_color(p.environment);
	else {
		if(p.temperature>MAGMA_LEVEL) return magma_color(adjustValue(p.temperature-2.0f));
		else if(p.temperature<TUNDRA_LEVEL) return glacier_color(adjustValue(p.temperature-0.6f));
		else if(p.sea>COAST_LEVEL) {
			if(filter==3 && p.population>CITY_LEVEL) return city_color(adjustValue(p.population-CITY_LEVEL));
			else if(filter==4 && p.life>.5f) return plancton_color(adjustValue((p.life-.5f)*2.0f));
			else if(filter==5) return mountain_color(adjustValue((p.altitude)));
			else return ocean_color(adjustValue(p.sea));
		} else {
			if(p.population>CITY_LEVEL) return city_color(adjustValue(p.population-CITY_LEVEL));
			else if(p.vegetation>FOREST_LEVEL) return vegetation_color(adjustValue((p.vegetation-.5f)*2.0f));
			else if(p.life>FOREST_LEVEL) return vegetation_color(adjustValue((p.life-.5f)*2.0f));
			else if(p.altitude>MOUNTAIN_LEVEL) return mountain_color(adjustValue((p.altitude-seaLevel)*seaLevelMultiply));
			else return desert_color(adjustValue((p.altitude-seaLevel)*seaLevelMultiply));
		}
	}
	return (RGBA_Color){0,0,0,0};
}

#define RIGHT1(x) (x>=width-1? x+1-height : x+1)
#define BOTTOM1(y) (y>=height-1? y+1-height : y+1)
#define LEFT1(x) (x<1? x+width-1 : x-1)
#define TOP1(y) (y<1? y+height-1 : y-1)
#define RIGHT2(x) (x>=width-2? x+2-width : x+2)
#define BOTTOM2(y) (y>=height-2? y+2-height : y+2)
#define LEFT2(x) (x<=1? width+x-2 : x-2)
#define TOP2(y) (y<=1? height+y-2 : y-2)

void **Planet::getArea(int x,int y,int radius,void *map,int sz) {
	static void *n[21];
	char *m = (char *)map,hw = (char)(wrap&PLANET_WRAP_HORIZ),vw = (char)(wrap&PLANET_WRAP_VERT);
	n[0] = m+(x+y*width)*sz;
	if(radius>=1) {
		n[2] = vw || y<height-1? m+(x+BOTTOM1(y)*width)*sz : 0;
		n[4] = vw || y>0? m+(x+TOP1(y)*width)*sz : 0;
		if(hw || x<width-1) {
			n[1] = m+(RIGHT1(x)+y*width)*sz;
			if(radius>=2) {
				n[5] = vw || y>0? m+(RIGHT1(x)+TOP1(y)*width)*sz : 0;
				n[6] = vw || y<height-1? m+(RIGHT1(x)+BOTTOM1(y)*width)*sz : 0;
			}
		} else n[1] = n[5] = n[6] = 0;
		if(hw || x>0) {
			n[3] = m+(LEFT1(x)+y*width)*sz;
			if(radius>=2) {
				n[7] = vw || y<height-1? m+(LEFT1(x)+BOTTOM1(y)*width)*sz : 0;
				n[8] = vw || y>0? m+(LEFT1(x)+TOP1(y)*width)*sz : 0;
			}
		} else n[3] = n[7] = n[8] = 0;

		if(radius>=3) {
			if(hw || x<width-2) {
				n[9] = m+(RIGHT2(x)+y*width)*sz;
				if(radius>=4) {
					n[13] = vw || y>0? m+(RIGHT2(x)+TOP1(y)*width)*sz : 0;
					n[14] = vw || y<height-1? m+(RIGHT2(x)+BOTTOM1(y)*width)*sz : 0;
				}
			} else n[9] = n[13] = n[14] = 0;

			if(vw || y<height-2) {
				n[10] = m+(x+BOTTOM2(y)*width)*sz;
				if(radius>=4) {
					n[15] = hw || x<width-1? m+(RIGHT1(x)+BOTTOM2(y)*width)*sz : 0;
					n[16] = hw || x>0? m+(LEFT1(x)+BOTTOM2(y)*width)*sz : 0;
				}
			} else n[10] = n[15] = n[16] = 0;

			if(hw || x>1) {
				n[11] = m+(LEFT2(x)+y*width)*sz;
				if(radius>=4) {
					n[17] = vw || y<height-1? m+(LEFT2(x)+BOTTOM1(y)*width)*sz : 0;
					n[18] = vw || y>0? m+(LEFT2(x)+TOP1(y)*width)*sz : 0;
				}
			} else n[11] = n[17] = n[18] = 0;

			if(vw || y>1) {
				n[12] = m+(x+TOP2(y)*width)*sz;
				if(radius>=4) {
					n[19] = hw || x>0? m+(LEFT1(x)+TOP2(y)*width)*sz : 0;
					n[20] = hw || x<width-1? m+(RIGHT1(x)+TOP2(y)*width)*sz : 0;
				}
			} else n[12] = n[19] = n[20] = 0;
		}
	}
	return n;
}


}; /* namespace a */

