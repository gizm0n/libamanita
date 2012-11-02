
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <amanita/Fractal.h>
#include <amanita/Math.h>
#include <amanita/Random.h>
#include <amanita/String.h>
#include <amanita/Thread.h>


#define MYERBERG 1.401155189
#define MU 0
#define LAMBDA 1
#define RECIPMU 2
#define RECIPMUPLUSFOURTH 3
#define RECIPLAMBDA 4
#define RECIPLAMBDAMINUSONE 5
#define RECIPMUMINUSMYER 6


namespace a {


static const double set_x[] = { 0.0,-0.5 };
static const double set_y[] = { 0.0, 0.0 };

/* Different escape radius values for different decomposition styles. */
static const double radius_value[] = {
	2.0,		// Plain
	5.0,		// Stripes
	14.0,		// Binary
	5.0,		// Feathers
	8.0,		// 
	3.0,		// 
	4.0,		// 
	4.0,		// 
	3.0,		// 
	5.0,		// 
	2.0,		// 
};

static const char *escape_name[] = { "Circle","Rectangle","And","Pickover","Bio","Bio 2*r*i","Real","Abs Real","Imaginary","Manhattan","Manr" };
static const char *fraction_name[] = { "Plain","Circle","Square","Strip","Halfplane" };
static const char *decomp_name[] = { "Plain","Stripes","Binary","Feathers","","","","","","", };


#define _ET_CIRCLE_			zrn+zin<=b2
#define _ET_RECTANGLE_		zr<=b && zi<=b
#define _ET_AND_				(zrn<=b || zin<=b)
#define _ET_PICKOVER_		zrn<=b && zin<=b
#define _ET_BIO_				fabs(zr)<=b && fabs(zi)<=b
#define _ET_BIO2RI_			fabs(2.0*zr*zi)<=b
#define _ET_REAL_				zr<=b
#define _ET_ABSREAL_			fabs(zr)<=b
#define _ET_IMAGINARY_		zi<=b
#define _ET_MANHATTAN_		((fabs(zr)+fabs(zi))*(fabs(zr)+fabs(zi)))<=b
#define _ET_MANR_				((zr+zi)*(zr+zi))<=b

#define ITERATE(n,f)\
		case n:while(i<max && f) Z2C();break;

#define PERIODIC(n,f)\
		case n:\
			do {\
				ckr = zr,cki = zi,p += p;\
				if(p>max) p = max;\
				while(i<p && f) {\
					Z2C();\
					if((zr==ckr) && (zi==cki)) {\
						tot += i,i = max;\
						return;\
					}\
				}\
			} while(p!=max);\
			break;


void Formula::iterate(int e) {
	i = 1;
	switch(e) {
		ITERATE(ET_CIRCLE,_ET_CIRCLE_)
		ITERATE(ET_RECTANGLE,_ET_RECTANGLE_)
		ITERATE(ET_AND,_ET_AND_)
		ITERATE(ET_PICKOVER,_ET_PICKOVER_)
		ITERATE(ET_BIO,_ET_BIO_)
		ITERATE(ET_BIO2RI,_ET_BIO2RI_)
		ITERATE(ET_REAL,_ET_REAL_)
		ITERATE(ET_ABSREAL,_ET_ABSREAL_)
		ITERATE(ET_IMAGINARY,_ET_IMAGINARY_)
		ITERATE(ET_MANHATTAN,_ET_MANHATTAN_)
		ITERATE(ET_MANR,_ET_MANR_)
	}
	if(i<min) min = i;
	tot += i;
}

void Formula::periodic(int e) {
	double ckr, cki;
	int p = 8;
	i = 1;
	switch(e) {
		PERIODIC(ET_CIRCLE,_ET_CIRCLE_)
		PERIODIC(ET_RECTANGLE,_ET_RECTANGLE_)
		PERIODIC(ET_AND,_ET_AND_)
		PERIODIC(ET_PICKOVER,_ET_PICKOVER_)
		PERIODIC(ET_BIO,_ET_BIO_)
		PERIODIC(ET_BIO2RI,_ET_BIO2RI_)
		PERIODIC(ET_REAL,_ET_REAL_)
		PERIODIC(ET_ABSREAL,_ET_ABSREAL_)
		PERIODIC(ET_IMAGINARY,_ET_IMAGINARY_)
		PERIODIC(ET_MANHATTAN,_ET_MANHATTAN_)
		PERIODIC(ET_MANR,_ET_MANR_)
	}
	if(i<min) min = i;
	tot += i;
}


uint16_t Formula::color(int f,int d) {
	if(i==max) return ITER_BG;
	uint16_t c,n = 0;
	if(f) switch(f) {
		case FE_CIRCLE:n = (uint16_t)(::sqrt(::sqrt((zrn+zin)/b2))*20.0)-19;break;
		case FE_SQUARE:n = (uint16_t)(::sqrt(::sqrt(fabs(zr)/b))*20.0)-16;break;
		case FE_STRIP:n = (uint16_t)(::sqrt(::sqrt(fabs(zr)/b))*20.0)-19;break;
		case FE_HALFPLANE:n = (uint16_t)(::sqrt(::sqrt(::sqrt(fabs(zr)/b)))*10.0);break;
	}
	if(n>20) n = 20;
	c = ((i|ITER_MAX)-n)&ITER_COLOR;
	if(d) switch(d) {
		case DE_STRIPES:return (i&1)? c|ITER_MAX : c;
		case DE_BINARY:return (zi>0.0)? c|ITER_MAX : c;
		case DE_FEATHERS:return (fabs(zr)<b || fabs(zi)<b)? c|ITER_MAX : c;
		case DE_4:return (zr>0.0)? c|ITER_MAX : c;
		case DE_5:return (fabs(zr)<b)? c|ITER_MAX : c;
		case DE_6:return (fabs(zi)<b)? c|ITER_MAX : c;
		case DE_7:return (zr>0.0 && zi>0.0)? c|ITER_MAX : c;
		case DE_8:return (zr>0.0 || zi>0.0)? c|ITER_MAX : c;
		case DE_9:return (fabs(zr)<b && fabs(zi)<b)? c|ITER_MAX : c;
	}
	return c;
}

uint16_t Formula::julia(double x,double y) {
	zr = zoom->x+1.5*(x-fractal->width/2.0)/(0.5*z*fractal->width);
	zi = zoom->y+(y-fractal->height/2.0)/(0.5*z*fractal->height);
	zrn = zr*zr;
	zin = zi*zi;
	periodic(fractal->escape);
	return color(fractal->fraction,fractal->decomp);
}

uint16_t Formula::mandelbrot(double x,double y) {
	zr = zi = zrn = zin = 0.0;
	cr = zoom->left+x*pw;
	ci = zoom->top+y*ph;

	if(fabs(ci)<ph*0.5) ci = 0.0;
	ch = hypot(cr-0.25,ci);
	if(cr<ch-2.0*ch*ch+.25 || (cr+1.0)*(cr+1.0)+ci*ci<1.0/16.0) return ITER_BG;
	else {
		periodic(fractal->escape);
		return color(fractal->fraction,fractal->decomp);
	}
}


Fractal::Fractal(int w,int h,int p,Fractal *f) : params(p),zoom(),formula() {
	int sz = w*h;
	sw = w;
	sh = h;
	width = (double)w;
	height = (double)h;
	colors1 = (uint16_t *)malloc(sz*sizeof(uint16_t));
	memset(colors1,0,sz*sizeof(uint16_t));
	if((params&(FRACT_BUFFERED|FRACT_DBUFFERED))) {
		colors2 = (uint16_t *)malloc(sz*sizeof(uint16_t));
		for(int i=0; i<sz; i++) colors2[i] = ITER_BG;
	} else colors2 = colors1;
	if(f) {
		zoom = f->zoom;
		formula = f->formula;
		set = f->set;
		escape = f->escape;
		fraction = f->fraction;
		decomp = f->decomp;
		range = f->range;
		radius = f->radius;
	}
	formula.fractal = this;
	formula.zoom = &zoom;
	percent = -1;
	time = 0;
	nthreads = 0;
	threads = 0;
	thfin = 0;
}

Fractal::~Fractal() {
	if(threads) delete[] threads;
	threads = 0;
	if(colors2 && colors2!=colors1) free(colors2);
	if(colors1) free(colors1);
	colors1 = colors2 = 0;
}

struct thread_data {
	Fractal *fractal;
	Formula *formula;
	int n;
	int l;
	int t;
	int r;
	int b;
	formula_func func;
};

void thread_func(void *data) {
	thread_data *t = (thread_data *)data;
	Formula formula;
	formula = *t->formula;
//fprintf(stderr,"thread_func:  n: %d, l: %d, t: %d, r: %d, b: %d\n",t->n,t->l,t->t,t->r,t->b);
	t->fractal->calculate(formula,t->n,t->l,t->t,t->r,t->b,t->func);
	if(t->formula->min<formula.min) t->formula->min = formula.min;
	t->formula->tot += formula.tot;
	free(t);
}

void Fractal::render(int i) {
	formula_func func;
	int x,y,t;
	timeval tv;

	if(i>0) formula.max = i;
	else {
		i = formula.min+range;
		formula.max = i>ITER_MAX? ITER_MAX : i;
	}

	formula.b = radius;
	formula.b2 = formula.b*formula.b;
	formula.min = ITER_MAX;
	formula.tot = 0;
	formula.pw = zoom.width/width;
	formula.ph = zoom.height/height;

	switch(set) {
		case SET_JULIA:
			formula.z = 1.0/zoom.z;
			func = &Formula::julia;
			break;
		case SET_MANDELBROT:
			formula.z = zoom.z;
			func = &Formula::mandelbrot;
			break;
	}

	if(params&FRACT_EDGETRACE)
		memset(colors1,0,sw*sh*sizeof(uint16_t));

	thfin = 0;
	percent = 0;
	gettimeofday(&tv,0);
	time = tv.tv_sec*1000000+tv.tv_usec;

	if(params&FRACT_THREADED) {
		nthreads = 4;
		if(!threads) threads = new Thread[nthreads];
		for(y=0,t=0; y<sh; y+=sh/2)
			for(x=0; x<sw; x+=sw/2,++t) {
				thread_data *data = (thread_data *)malloc(sizeof(thread_data));
				data->fractal = this;
				data->formula = &formula;
				data->n = t;
				data->l = x;
				data->t = y;
				data->r = x+sw/2;
				data->b = y+sh/2;
				data->func = func;
//fprintf(stderr,"Fractal::render:  n: %d, l: %d, t: %d, r: %d, b: %d\n",data->n,data->l,data->t,data->r,data->b);
				threads[t].start(thread_func,data);
			}
	} else {
		nthreads = 4;
		for(y=0,t=0; y<sh; y+=sh/2)
			for(x=0; x<sw; x+=sw/2,++t)
				calculate(formula,t,x,y,x+sw/2,y+sh/2,func);
	}
}

enum {
	RIGHT = 1,
	DOWN,
	LEFT,
	UP,
};

void Fractal::calculate(Formula &form,int n,int l,int t,int r,int b,formula_func f) {
	int x,y,yw;
fprintf(stderr,"Fractal::calculate:  n: %d, l: %d, t: %d, r: %d, b: %d\n",n,l,t,r,b);
//fprintf(stderr,"sw: %d, sh: %d\n",sw,sh);
	if(params&FRACT_EDGETRACE) {
		int x1,y1,d,d1;
		uint16_t c,c0,c1;
//		memset(colors1,0,sw*sh*sizeof(uint16_t));
		for(y=t,yw=t*sw; y<b; ++y,yw+=sw) {
			if(y>t) for(x=l; x<r; ++x)
				if((c=colors1[x+yw])&ITER_EDGE) {
//fprintf(stderr,"x: %d, y: %d, c: %d\n",x,y,c);
					for(c0=(c&0xfff),++x; x<r && colors1[x+yw]==c; ++x);
					if(x<r && (!(c1=colors1[x+yw]) || c1==c0)) {
						for(; x<r && (!(c1=colors1[x+yw]) || c1==c0); ++x)
							colors1[x+yw] = c0|ITER_FILL;
						if(x<r && c1!=c) --x;
					} else --x;
				}
			for(x=l; x<r; ++x) {
				while(x<r && colors1[x+yw]) ++x;
				if(x<r) {
					c0 = (form.*f)(x,y),c = c0|ITER_EDGE;
					colors1[x+yw] = c;
					for(; x>0 && (colors1[x-1+yw]|ITER_EDGE)==c; --x);
					x1 = x,y1 = y,d = 0,d1 = 0;
					do {
						if(x1<l || x1>=r || y1<t || y1>=b) c1 = 0xffff;
						else if(!(c1=colors1[x1+yw])) {
							c1 = (form.*f)(x1,y1);
							colors1[x1+yw] = c1;
						}
//fprintf(stderr,"Fractal::trace(x: %d, y: %d, x1: %d, y1: %d, d: %d, c: %d, c1: %d(%d))\n",x,y,x1,y1,d,c,c1,c1|ITER_EDGE);
						if((c1|ITER_EDGE)==c) {
							if(!(c1&ITER_EDGE)) colors1[x1+yw] = c;
							if(d==0 || d==RIGHT) --y1,yw -= sw,d = UP;
							else if(d==DOWN) ++x1,d = RIGHT;
							else if(d==LEFT) ++y1,yw += sw,d = DOWN;
							else if(d==UP) --x1,d = LEFT;
						}
						else if(d==0) break;
						else if(d==RIGHT) ++y1,yw += sw,d = DOWN;
						else if(d==DOWN) --x1,d = LEFT;
						else if(d==LEFT) --y1,yw -= sw,d = UP;
						else if(d==UP) ++x1,d = RIGHT;
						if(d1) {
							if(d==UP && d1==RIGHT) ++x,d = RIGHT,d1 = 0;
							else if(d+1==d1) d1 = d;
							else d1 = 0;
						} else if(d==UP) d1 = d;
					} while(x1!=x || y1!=y);
//fprintf(stderr,"Fractal::trace(done)\n");
					yw = y*sw;
				}
			}
//			break;
		}
	} else {
		for(y=t,yw=t*sw; y<b; ++y,yw+=sw) {
			for(x=l; x<r; ++x) colors1[x+yw] = (form.*f)(x,y);
			percent = y*100/sh;
		}
	}
	finish(n);
}

void Fractal::finish(int t) {
	timeval tv;
	gettimeofday(&tv,0);
	thfin |= (1<<t);
	if(thfin==(0xffffffff>>(32-nthreads))) {
		long n = tv.tv_sec*1000000+tv.tv_usec;
		percent = 100;
		printf("%s: range[%d] escape[%g] min[%d] max[%d] Cr[%f] Ci[%f]\n",
				set==SET_JULIA? "Julia Set" : "Mandelbrot Set",
				range,radius,formula.min,formula.max,formula.cr,formula.ci);
		time = n-time;
		printf("Time: %ld\nIterations: %ld\nTime per iteration: %g\n",time,formula.tot,(double)time/(double)formula.tot);
	}
}

/*
struct stack {
	short x;
	short y;
};

void Fractal::fill(int x,int y,int c) {
	int x1,y1,s = 0,i = 0,c1;
	bool l,r;
	stack st[sh+sw];
	st[s].x = x,st[s].y = y,++s;
fprintf(stderr,"Fractal::fill(x: %d, y: %d, c: %d)\n",x,y,c^ITER_EDGE);
	while(s) {
		++i;
		--s,x1 = st[s].x,y1 = st[s].y;
//fprintf(stderr,"Fractal::fill(x1: %d, y1: %d, s: %d)\n",x1,y1,s);
		colors1[x1+y1*sw] = c^ITER_EDGE;
		if(x1>0 && (!(c1=colors1[x1-1+y1*sw]) || c1==c)) st[s].x = x1-1,st[s].y = y1,++s;
		if(x1<sw-1 && (!(c1=colors1[x1+1+y1*sw]) || c1==c)) st[s].x = x1+1,st[s].y = y1,++s;
		if(x1>0 && (!(c1=colors1[x1-1+y1*sw]) || c1==c)) st[s].x = x1-1,st[s].y = y1,++s;
		if(x1<sw-1 && (!(c1=colors1[x1+1+y1*sw]) || c1==c)) st[s].x = x1+1,st[s].y = y1,++s;
	}
fprintf(stderr,"Fractal::fill(i: %d)\n",i);
}
*/

void Fractal::flip() {
	uint16_t *col = colors1;
	colors1 = colors2;
	colors2 = col;
}

void Fractal::random(int s) {
	int i;
	set = s>=0 && s<SET_NUM? s : rnd.int32(SET_NUM);
	randomStyle();
	if(set==SET_JULIA) {
		double cr,ci;
//		cr = 0.285,ci = 0.01;
//		cr = -0.1,ci = 0.651;
//		cr = -0.75,ci = 0.11;
//		cr = -0.74543,ci = 0.11301;
//		cr = -0.7,ci = 0.27015;
		i = rnd.int32();
		cr = rnd.real64()*0.8+0.1;
		if(i&1) cr = -cr;
		ci = rnd.real64()*0.8+0.1;
		if(i&2) ci = -ci;
		formula.cr = cr;
		formula.ci = ci;
	}
	range = 200;
	formula.min = 0;
	formula.max = 0;
	if(params&(FRACT_BUFFERED|FRACT_DBUFFERED))
		for(i=sw*sh-1; i>=0; --i) colors2[i] = ITER_BG;

	x_point = width/2.0;
	y_point = height/2.0;
	setZoom(set_x[set],set_y[set]);
}

void Fractal::randomStyle() {
	int i;
	escape = fraction = decomp = -1;
	setStyle(
		set,
		rnd.int32(ET_NUM),
		rnd.int32(FE_NUM),
		rnd.int32(DE_NUM)
	);
}

void Fractal::increaseRange(int i) {
	if(i==0) return;
	range += i;
	percent = -1;
}

void Fractal::increaseRadius(bool i) {
	int n = radius;
	if(i) radius += n<4.0f? 1.0f : n<20.0f? 2.0f : n<50.0f? 5.0f : 10.0f;
	else radius -= n>50.0f? 10.0f : n>20.0f? 5.0f : n>4.0f? 2.0f : 1.0f;
	if(radius<0.5f) radius = 0.5f;
	percent = -1;
}

void Fractal::zoomIn() {
	x_point = width/2.0;
	y_point = height/2.0;
	setZoom(zoom.x,zoom.y,zoom.z,0.5);
}

void Fractal::zoomIn(int x,int y,double z) {
	if(z<=0.0) return;
	x_point = (double)x;
	y_point = (double)y;
	if(zoom.z>=5.0e-14) setZoom(
		zoom.left+(x_point/width)*zoom.width,
		zoom.top+(y_point/height)*zoom.height,
		zoom.z,z
	);
}

void Fractal::zoomOut() {
	x_point = width/2.0;
	y_point = height/2.0;
	setZoom(zoom.x,zoom.y,zoom.z,2.0);
}

void Fractal::setStyle(const char *s) {
	uint32_t st = xtoi(s);
	set = 0;
	escape = 0;
	fraction = 0;
	decomp = 0;
	setStyle((st>>25)&0x1f,(st>>20)&0x1f,(st>>15)&0x1f,(st>>10)&0x1f);
	if((s=strchr(s,'/'))) {
		formula.cr = strtod(++s,(char **)&s);
		formula.ci = strtod(++s,(char **)&s);
	}
	formula.min = st&0x3ff;
	formula.max = 0;
}

void Fractal::setStyle(int s,int e,int f,int d) {
	if(s<0 || s>SET_NUM || s==set) s = -1;
	if(e<0 || e>=ET_NUM || e==escape) e = -1;
	if(f<0 || f>=FE_NUM || f==fraction) f = -1;
	if(d<0/* || d>=DE_NUM*/ || d==decomp) d = -1;
	if(s!=-1 || e!=-1 || f!=-1 || d!=-1) {
		if(s!=-1) set = s;
		if(e!=-1) escape = e;
		if(f!=-1) fraction = f;
		if(d!=-1) {
			decomp = d;
			radius = radius_value[d];
		}
		percent = -1;
		printf("Escape[%s]\nFraction[%s]\nDecomposition[%s]\n",
			escape_name[escape],
			fraction_name[fraction],
			decomp_name[decomp]);
	}
}

void Fractal::setZoom(double x,double y,double z,double r) {
	zoom.x = x;
	zoom.y = y;
	zoom.z = z*r;
	zoom.width = 4*zoom.z;
	zoom.height = 4*(height/width)*zoom.z;
	zoom.left = x-zoom.width*0.5;
	zoom.top = y-zoom.height*0.5;

	r = 1.0/r;
	zrect.x = (int16_t)(x_point-width/(2.0*r));
	zrect.y = (int16_t)(y_point-height/(2.0*r));
	zrect.w = (uint16_t)(width/r);
	zrect.h = (uint16_t)(height/r);

	percent = -1;

	printf("Zoom: x[%f] y[%f] z[%f]\n",x,y,z);
}

int Fractal::print(char *s) {
	int l = sprintf(s,"%x",(set<<25)|(escape<<20)|(fraction<<15)|(decomp<<10)|(formula.min&0x3ff));
	if(set==SET_JULIA) l += sprintf(&s[l],"/%g/%g",formula.cr,formula.ci);
	l += sprintf(&s[l]," %g %g %g",zoom.x,zoom.y,zoom.z);
	return l;
}

void Fractal::print(FILE *fp) {
	fprintf(fp,"%x",(set<<25)|(escape<<20)|(fraction<<15)|(decomp<<10)|(formula.min&0x3ff));
	if(set==SET_JULIA) fprintf(fp,"/%g/%g",formula.cr,formula.ci);
	fprintf(fp," %g %g %g",zoom.x,zoom.y,zoom.z);
}


}; /* namespace a */

