/** An example program on how to use the astronomy and astrology functions in libesoteric.
 * 
 * Compile:
 * gcc astro.c -o astro -I /usr/local/include -I /usr/include -lesoteric -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <esoteric/astro/horoscope.h>


const char *zodiac_names[] = { "Aries","Taurus","Gemini","Cancer","Leo","Virgo","Libra","Scorpio","Sagittarius","Capricorn","Aquarius","Pisces" };
const char *planet_names[] = { "Tellus","Sun","Moon","Mercury","Venus","Mars","Jupiter","Saturn","Uranus","Neptune","Pluto",
											"N. Node","True N. Node","S. Node","True S. Node","Chiron","Ceres","Pallas","Juno","Vesta","Vertex","Eastpoint","Fortune","Ascendant","MC" };
const int planet_ids[] = { ASTRO_SUN,ASTRO_MOON,ASTRO_MERCURY,ASTRO_VENUS,ASTRO_MARS,ASTRO_JUPITER,ASTRO_SATURN,ASTRO_URANUS,ASTRO_NEPTUNE,ASTRO_PLUTO,ASTRO_ASCENDANT,ASTRO_MC,-1 };
const char *aspect_names[] = { "Conjunction","Semisextile","Decile","Novile","Semisquare","Septile","Sextile","Quintile","Square","Trine","Sesquiquadrate","Quincunx","Opposition" };
const char *pattern_names[] = { "Stellium","T-Square","Great Square","Great Cross","Great Trine","Yod","Mystical Rectangle","Kite","Great Quintile","Hexagram" };

const double aspect_orbs[13*10] = {
//   0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9
//  sun |asc | mc |rul-|rul-|mer |jup |ura |moon|etc
//  moon|    |    |asc |sun |ven |sat |nep |node|
//      |    |    |    |moon|mar |    |plu |    |
	10.0, 0.0, 0.0,10.0,10.0,10.0,10.0,10.0, 0.0, 0.0,			// ASTRO_CONJUNCTION
	 3.0, 0.0, 0.0, 3.0, 3.0, 3.0, 3.0, 3.0, 0.0, 0.0,			// ASTRO_SEMISEXTILE
	 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0,			// ASTRO_DECILE
	 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0,			// ASTRO_NOVILE
	 3.0, 0.0, 0.0, 3.0, 3.0, 3.0, 3.0, 3.0, 0.0, 0.0,			// ASTRO_SEMISQUARE
	 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0,			// ASTRO_SEPTILE
	 6.0, 0.0, 0.0, 6.0, 6.0, 6.0, 6.0, 6.0, 0.0, 0.0,			// ASTRO_SEXTILE
	 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0,			// ASTRO_QUINTILE
	10.0, 0.0, 0.0,10.0,10.0,10.0,10.0,10.0, 0.0, 0.0,			// ASTRO_SQUARE
	10.0, 0.0, 0.0,10.0,10.0,10.0,10.0,10.0, 0.0, 0.0,			// ASTRO_TRINE
	 3.0, 0.0, 0.0, 3.0, 3.0, 3.0, 3.0, 3.0, 0.0, 0.0,			// ASTRO_SESQUIQUADRATE
	 3.0, 0.0, 0.0, 3.0, 3.0, 3.0, 3.0, 3.0, 0.0, 0.0,			// ASTRO_QUINCUNX
	10.0, 0.0, 0.0,10.0,10.0,10.0,10.0,10.0, 0.0, 0.0,			// ASTRO_OPPOSITION
};


char *astro_write_hms(char *str,double c,const char *deg) {
	int h,m,s;
	h = (int)c;
	c -= h;
	if(c<0.0) c = -c;
	c *= 60;
	m = (int)c;
	c -= m;
	c *= 60;
	s = round(c);
	if(s>=60) s -= 60,m++;
	if(m>=60) m -= 60,h++;
	sprintf(str,"%d%s%02d\'%02d\"",h,deg,m,s);
	return str;
}

double astro_read_coord(const char *str) {
	int h = 0,m = 0,s = 0,i = 0,n = 0;
	char dir,c;
	double d;
	if(!str || !*str) return 0.0;
	for(c=str[i]; c!='\0'; c=str[++i])
		if(c>='0' && c<='9') n = n*10+(c-'0');
		else if((c>='a' && c<='z') || (c>='A' && c<='Z')) h = n,n = 0,dir = c>='a' && c<='z'? c-32 : c;
	if(n<100) m = n;
	else if(n<1000) m = n/10,s = n-m*10;
	else m = n/100,s = n-m*100;
	d = (double)h+((double)m/60.0)+((double)s/3600.0);
	if(dir=='E' || dir=='S') d = -d;
	return d;
}

char *astro_write_longitude(char *str,double c) {
	return astro_write_hms(str,c<0.0? -c : c,c<=0.0? "E" : "W");
}

char *astro_write_latitude(char *str,double c) {
	return astro_write_hms(str,c<0.0? -c : c,c<=0.0? "S" : "N");
}


int main(int argc,char *argv[]) {
	char deg[33];
	int i,j,x,y,sign,m,n,*p;
	double l,lon,lat;
	long t;
	struct timeval tv;
	struct timezone tz;
	astronomy *a;
	horoscope *h;
	astronomy_planet *p1;
	astro_planet *p2;
	astro_house *h2;
	astro_aspect *a2;
	astro_pattern *ap;

	astro_set_aspect_orbs(aspect_orbs);

	lon = astro_read_coord("11e58");
	lat = astro_read_coord("57n43");
	h = horoscope_new("Per Löwgren",1975,6,17,3,30,0,1.0,0.0,1,lon,lat);
	a = astronomy_new(&h->gmt,h->lon,h->lat);

	printf("\nCalculate planets...\n");
	gettimeofday(&tv,&tz);
	t = tv.tv_usec;
	astronomy_get_planets(a,planet_ids,0);
	gettimeofday(&tv,&tz);
	printf("Time to calculate planets: %ld microseconds\n\n",tv.tv_usec-t);

	printf("Astronomy: %d-%02d-%02d %d:%02d, jd(%f), lon(%3.2f), lat(%3.2f)\n",
			h->gmt.year,h->gmt.month,h->gmt.day,h->gmt.hour,h->gmt.minute,h->gmt.jd,h->lon,h->lat);
	printf("Julian Day: %lf   DeltaT: %lf   Siderian time: %s\n",
			h->gmt.jd+a->deltat/86400.0,a->deltat,astro_write_hms(deg,a->sdrlt*24.0,"."));
	printf("Planet:           H Lon:          G Lon:          Lon:            Lat:              Radius: Speed:  Const: Phase:   Sign:\n");
	for(i=0; i<a->nplanets; i++) {
		p1 = &a->planets[i];
		sign = astro_zodiac(p1->glon);
		printf("%-18s",planet_names[p1->id]);
		printf("%10s      ",astro_write_hms(deg,p1->hlon,"."));
		printf("%10s      ",astro_write_hms(deg,p1->glon,"."));
		l = p1->glon;
		while(l>=30.0) l -= 30.0;
		printf("%10s%c     ",astro_write_hms(deg,l,"."),p1->speed<0? 'r' : ' ');
		printf("%10s      ",astro_write_hms(deg,p1->lat,"."));
		printf("%6.2f   %6.2f   %2d     %6.2f   %s\n",p1->radius,p1->speed,p1->constel,p1->phase,zodiac_names[sign]);
	}

	printf("\nCast horoscope...\n");
	gettimeofday(&tv,&tz);
	t = tv.tv_usec;
	horoscope_cast(h,a,HOROSCOPE_NATAL/*|HOROSCOPE_ASPECT_IN_SIGN*/,ASTRO_PLACIDUS);
	gettimeofday(&tv,&tz);
	printf("Time to cast horoscope: %ld microseconds\n\n",tv.tv_usec-t);

	printf("\nHoroscope: %s   %d-%02d-%02d %d:%02d GMT   ",h->name,
			h->gmt.year,h->gmt.month,h->gmt.day,h->gmt.hour,h->gmt.minute);
	printf("%s  ",astro_write_longitude(deg,h->lon));
	printf("%s\n",astro_write_latitude(deg,h->lat));

	printf("Planet:     Longitude:     Sign:       House:\n");
	for(i=0; i<h->nplanets; i++) {
		p2 = &h->planets[i];
		printf("%-12s",planet_names[p2->id]);
		printf("%10s     ",astro_write_hms(deg,fmod(p2->lon,30),"."));
		printf("%-12s%d\n",zodiac_names[p2->sign],p2->house);
	}

	printf("\nHouse:      Longitude:     Sign:\n");
	for(i=0; i<12; i++) {
		h2 = &h->houses[i];
		printf("%2d          %10s     %s\n",i+1,astro_write_hms(deg,fmod(h2->cusp,30.0),"."),zodiac_names[h2->sign]);
	}

	printf("\nAspects (%d):\n",h->naspects);
	for(x=0; x<h->nplanets; x++)
		for(y=x+1; y<h->nplanets; y++) {
			a2 = &h->aspects[x+y*h->nplanets];
			if(a2->type!=-1) {
				p2 = &h->planets[x];
				printf("%2d %-12s / ",p2->id,planet_names[p2->id]);
				p2 = &h->planets[y];
				printf("%2d %-12s%-20s%s\n",p2->id,planet_names[p2->id],aspect_names[a2->type],astro_write_hms(deg,a2->orb,"."));
			}
		}

	printf("\nAspect Patterns (%d):  0x%0*x\n",h->nasppat,sizeof(h->fasppat)*2,h->fasppat);
	for(i=0; i<h->nasppat; i++) {
		ap = &h->asppat[i];
		printf("%s: ",pattern_names[ap->type]);
		for(x=0,n=0; x<ap->nplanets; x++)
			for(j=0; j<h->nplanets && j<32; j++)
				if(ap->planets[x]&(1<<j)) {
					if(n++>0) printf(", ");
					printf("%s",planet_names[h->planets[x*32+j].id]);
				}
		printf("\n");
	}

	horoscope_delete(h);
	astronomy_delete(a);
	return 0;
}


