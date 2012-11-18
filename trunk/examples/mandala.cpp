
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <amanita/Math.h>
#include <amanita/Random.h>
#include <amanita/Thread.h>
#include <amanita/Colorcycle.h>
#include <amanita/gui/Application.h>
#include <amanita/gui/Graphics.h>

#ifdef USE_WIN32
#include "_resource.h"
#define amanita32_xpm 0
#endif
#ifdef USE_LINUX
#include "../icons/32x32/amanita.xpm"
#endif

using namespace a;
using namespace a::gui;



Application app(APP_SDL,"Mandala","Amanita Mandala");



#define COLOR_CYCLES 10

const int width = 1024;
const int height = 768;

const int lines = width/2;
const double w = (double)width;
const double h = (double)height;
const double xcenter = w/2.0;
const double ycenter = h/2.0;

Colorcycle cycle[COLOR_CYCLES];
uint32_t colors[COLOR_CYCLES][lines];
int cc[] = { 0,0,0,0,0,0,0,0,0,0 };
int ncycl;								// Number of Colorcycle palettes

int timer = 300;
bool clear;
// Radie of the complete mandala
double radie = a::pythagoras(xcenter+1.0,ycenter+1.0)*3.0;

enum {
	MAND_CLEAR				= 0x0001,
	MAND_CENTERED			= 0x0002,
	MAND_FIXED				= 0x0004,
	MAND_LNGRAD				= 0x0008,
	MAND_LNGROF				= 0x0010,
	MAND_STARS				= 0x0020,
	MAND_GALAXY				= 0x0040,
};

struct mandala {
	double x;
	double y;
	double dir;
	double vel;
	int c;
	int ci;
	int a;
	double xrel;
	double yrel;
	double ang;
	int cindex;
	int nangl;							// Number of angles
	double angl[10];					// The angles
	double ainc;						// Increase of the angles per cycle
	double twist;						// Twisting within mandala
	double tdir;						// Twist rotation direction
	double rotation;					// Rotation degree of the mandala
	int style;
};

mandala m[4];
int nm;

const double deg = a::PI/180.0;	// 1º, One degree
const double eci = a::PI/4.0;		// 45º, Eighth of a circle
const double qci = a::PI/2.0;		// 90º, Quarter of a circle
const double hci = a::PI;			// 180º, Half a circle
const double tqc = qci*3.0;		// 270º, Three quarters of a circle
const double sec = eci*7.0;		// 315º, Seven eighths of a circle
const double cir = a::PI*2;		// 360º, Circle

const double fhmin = eci;			// 45º, First half of circle minimum
const double fhmax = deg*157.5;	// 157º, First half of circle maximum
const double shmin = deg*202.5;	// 202º, Second half of circle minimum
const double shmax = sec;			// 315º, Second half of circle maximum

const double twmin = 0.6;			// Twist minimum, percent
const double twmax = 2.2;			// Twist maximum, percent

const double rot = deg*5.0;		// Rotation per cycle
const double inc = 2.0;				// Increase per angle


static void cycles() {
	int bpp = g.getBytesPerPixel();
	if(bpp==2 || bpp==4) {
		int i,n;
		ncycl = (int)(rnd.real64()*rnd.real64()*3.0)+2;
fprintf(stderr,"Cycles[%d]\n",ncycl);
		for(i=0; i<COLOR_CYCLES; ++i) {
			/*if(i==COLOR_CYCLES-1) n = rnd.int32(LIGHT_COLOR_SCHEMES);
//			else if(star) n = LIGHT_COLOR_SCHEMES+rnd.int32(COLOR_SCHEMES-LIGHT_COLOR_SCHEMES);
			else */n = rnd.int32(CC_SCHEMES);
			cc[i] = n;
			/*if(i==0) cycle[i].setScheme(n,rnd.int32(l[n]),rnd.int32(45)+5,rnd.int32(45)+5,bpp);
			else */cycle[i].setScheme(n,0,rnd.int32(120)+20,rnd.int32(120)+20,bpp);
fprintf(stderr,"Cycle[%d]: %s\n",i,cycle[i].getSchemeName());
		}
	}
}

static void restart(int n=0) {
	int i;
	if(timer<0) timer = 0;
	clear = true;
	nm = n? n : (int)(rnd.real64()*rnd.real64()*4.0)+1;
	cycles();
	for(n=0; n<nm; ++n) {
		m[n].style = 0;
		m[n].nangl = (int)(rnd.real64()*rnd.real64()*2.0)+1;
		for(i=0; i<m[n].nangl; i++) m[n].angl[i] = 0;
		m[n].ainc = 0.00000001+0.001*rnd.real64()*rnd.real64()*rnd.real64();
		if((rnd.int32()&3)==0) {
			m[n].twist = 1.0;
			m[n].tdir = 0.0;
		} else {
			m[n].twist = twmin+rnd.real64()*(twmax-twmin);
			m[n].tdir = 0.001*(rnd.real64()*rnd.real64()*(rnd.real64()<0.5? -1.0 : 1.0));
		}
		m[n].cindex = 0;
		m[n].rotation = 0;
		if(n==0 && !rnd.int32(4)) m[n].style |= MAND_CLEAR;
		if(!rnd.int32(4)) {
			m[n].style |= MAND_STARS;
			if(nm>1 && n>0 && !rnd.int32(3)) m[n].style |= MAND_GALAXY;
		}
		i = rnd.int32();
		if(i&1) {
			m[n].style |= MAND_LNGRAD;
			if(i&2) m[n].style |= MAND_LNGROF;
		}
		i >>= 2;
		if(i&1) {
			if(i&2) {
				m[n].dir = atan2(ycenter-m[n].y,xcenter-m[n].x);
				m[n].vel = a::distance(m[n].x,m[n].y,xcenter,ycenter)/50;
				m[n].style |= MAND_CENTERED|MAND_FIXED;
			} else {
				m[n].dir = 0.0;
				m[n].vel = 0.0;
				m[n].style |= MAND_FIXED;
			}
		} else {
			m[n].dir = rnd.real64()*cir;
			m[n].vel = rnd.real64()*12.0;
		}
		m[n].ci = 1;
fprintf(stderr,"Mandala[%d]: x[%g] y[%g] dir[%g] vel[%g]",n,m[n].x,m[n].y,m[n].dir,m[n].vel);
if(m[n].style&MAND_STARS) {
	if(m[n].style&MAND_GALAXY) fprintf(stderr," galaxy");
	else fprintf(stderr," stars");
}
if(m[n].style&MAND_CLEAR) fprintf(stderr," clear");
if(m[n].style&MAND_LNGROF) fprintf(stderr," lngrof");
else if(m[n].style&MAND_LNGRAD) fprintf(stderr," lngrad");
if(m[n].style&MAND_CENTERED) fprintf(stderr," center");
else if(m[n].style&MAND_FIXED) fprintf(stderr," fixed");
for(i=0; i<1+ncycl; ++i) fprintf(stderr," ci[%s]",cycle[(n*ncycl+i)%COLOR_CYCLES].getSchemeName());
fputc('\n',stderr);
	}
}

static void key(uint32_t sym,uint32_t mod,uint32_t unicode,bool down) {
fprintf(stderr,"key: [sym: 0x%x, mod: 0x%x unicode: %c]\n",sym,mod,unicode);
	if(down) {
		switch(sym) {
			case KEY_SPACE:
			case KEY_RETURN:restart();break;
			case KEY_1:restart(1);break;
			case KEY_2:restart(2);break;
			case KEY_3:restart(3);break;
			case KEY_4:restart(4);break;
			case KEY_c:cycles();break;
		}
	}
}

static void mouse(uint16_t x,uint16_t y,uint8_t button,uint16_t clicks,bool down) {}

static void motion(uint16_t x,uint16_t y,uint16_t dx,uint16_t dy,bool drag) {}

static void paint(void *data) {
	int n,i,c;
	int pitch,bpp;
	uint8_t *pixels,*p,*cp;
	double an;
	double r,d,v,x1,y1,x2,y2;
	int xi1,yi1,xi2,yi2;
	while(app.isRunning()) {
		app.lock();

		for(i=0; i<COLOR_CYCLES && i<nm*ncycl; ++i)
			cycle[i].write(colors[i],lines);
		if(clear || (m[0].style&MAND_CLEAR)) {
			g.fillRect(0,0,width,height,colors[1][0]);
			clear = false;
		}

		for(n=0; n<nm; ++n)
			for(i=0; i<m[n].nangl; ++i) {
				if(m[n].angl[i]>=shmax || m[n].angl[i]<fhmin) m[n].angl[i] = fhmin+rnd.real64()*qci;
				else if(m[n].angl[i]>=fhmax && m[n].angl[i]<shmin) m[n].angl[i] = shmin;
				m[n].angl[i] += m[n].ainc;
			}


		g.lock();
		pixels = (uint8_t *)g.getPixels();
		pitch = g.getPitch();
		bpp = g.getBytesPerPixel();

		for(n=0; n<nm; ++n) {
			m[n].c = m[n].cindex;
			m[n].a = m[n].nangl;
			m[n].ang = m[n].rotation;
			m[n].xrel = m[n].x/w;
			m[n].yrel = m[n].y/h;
		}
		for(n=0,r=radie; n<nm; ++n,r-=radie*0.2)
			for(d=0; d<r; d+=inc) {
				--m[n].c;
				if(m[n].c<0) m[n].c = lines-1;
				if(++m[n].ci>ncycl) m[n].ci = 1;
				if(++m[n].a>=m[n].nangl) m[n].a = 0;
				if(m[n].x==xcenter && m[n].y==ycenter) {
					an = m[n].angl[m[n].a];
					x1 = m[n].x+(cos(m[n].ang)*d);
					y1 = m[n].y+(sin(m[n].ang)*d);
					x2 = m[n].x+(cos(m[n].ang+an)*(d+inc));
					y2 = m[n].y+(sin(m[n].ang+an)*(d+inc));
					m[n].ang += an*m[n].twist;
				} else {
					an = m[n].angl[m[n].a]*m[n].twist;
					x1 = (cos(m[n].ang)*d);
					y1 = (sin(m[n].ang)*d);
					x2 = (cos(m[n].ang+an)*(d+inc));
					y2 = (sin(m[n].ang+an)*(d+inc));
					x1 = m[n].x+x1*(x1<0? m[n].xrel : fabs(1.0-m[n].xrel));
					y1 = m[n].y+y1*(y1<0? m[n].yrel : fabs(1.0-m[n].yrel));
					x2 = m[n].x+x2*(x2<0? m[n].xrel : fabs(1.0-m[n].xrel));
					y2 = m[n].y+y2*(y2<0? m[n].yrel : fabs(1.0-m[n].yrel));
					m[n].ang += an;
				}
				xi1 = a::round(x1),yi1 = a::round(y1),xi2 = a::round(x2),yi2 = a::round(y2);
				c = (n*ncycl+m[n].ci)%COLOR_CYCLES;
				if(!(m[n].style&MAND_GALAXY)) {
					if(m[n].style&MAND_LNGRAD)
						g.drawLine(xi1,yi1,xi2,yi2,colors[c],(m[n].style&MAND_LNGROF)? m[n].c : 0,lines);
					else g.drawLine(xi1,yi1,xi2,yi2,colors[c][m[n].c]);
				}
				if((m[n].style&MAND_STARS) && xi1>=0 && yi1>=0 && xi1<width && yi1<height) {
					p = pixels+xi1*bpp+yi1*pitch,cp = (uint8_t *)&colors[(n*ncycl)%COLOR_CYCLES][m[n].c];
					for(i=0; i<bpp; ++i) p[i] = cp[i];
				}
				if(m[n].ang>cir) m[n].ang -= cir;
			}
		g.unlock();


		for(n=0; n<nm; ++n) {
			m[n].rotation += rot/m[n].nangl*(m[n].tdir<=0? 1 : -1);
			if(m[n].rotation>cir) m[n].rotation -= cir;
			++m[n].cindex;
			if(m[n].cindex==lines) m[n].cindex = 0;
			m[n].twist += m[n].tdir;
			if(m[n].twist>twmax) m[n].tdir = -m[n].tdir;
			else if(m[n].twist<twmin) m[n].tdir = fabs(m[n].tdir);
			if((m[n].style&MAND_CENTERED) && m[n].x!=xcenter && m[n].y!=ycenter) {
				v = m[n].vel;
				m[n].x += cos(m[n].dir)*v;
				m[n].y += sin(m[n].dir)*v;
				if(fabs(xcenter-m[n].x)<v && fabs(ycenter-m[n].y)<v) {
					m[n].x = xcenter;
					m[n].y = ycenter;
				}
			} else if(!(m[n].style&MAND_FIXED)) {
				d = a::pythagoras(w*0.5,h*0.5);
				v = m[n].vel*(0.2+(a::distance(xcenter,ycenter,m[n].x,m[n].y))/d);
				m[n].x += cos(m[n].dir)*v;
				m[n].y += sin(m[n].dir)*v;
				if(m[n].x>w*0.75) m[n].x = w*0.75,m[n].dir = a::PI-m[n].dir;
				else if(m[n].x<w*0.25) m[n].x = w*0.25,m[n].dir = a::PI-m[n].dir;
				if(m[n].y>h*0.75) m[n].y = h*0.75,m[n].dir = -m[n].dir;
				else if(m[n].y<h*0.25) m[n].y = h*0.25,m[n].dir = -m[n].dir;
			}
		}

		app.unlock();
		g.flip();
		app.pauseFPS(24);

		if(--timer<-3600) restart();
	}
}

int main(int argc,char *argv[]) {
//fprintf(stderr,"main\n");
	int n;
	const WindowIcon icons[] = {
		{ 16,AMANITA_ICON,0,0 },
		{ 32,0,amanita32_xpm,0 },
		{ 48,AMANITA_ICON,0,0 },
	{0}};
	app.setIcons(icons);
	app.setEvents(key,mouse,motion);
	app.open(argc,argv);
	app.init(width,height);
	for(n=0; n<4; ++n) {
		m[n].x = w/4.0+rnd.real64()*w/2.0;
		m[n].y = h/4.0+rnd.real64()*h/2.0;
	}
	restart();
	app.start(paint);
	app.main();
	app.close();
	return 0;
}
