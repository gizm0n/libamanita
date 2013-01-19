
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


namespace a {


struct zoom_setting {
	double x;
	double y;
	double z;
};

static const zoom_setting sets[] = {
	{  0.0,  0.0,  1.5 },		//
	{ -0.5,  0.0,  1.0 },		//
};

static const zoom_setting planes[] = {
	{ -0.5,  0.0,  1.0 },		//
	{  1.2,  0.0,  1.3 },		//
	{  6.0,  0.0,  6.2 },		//
	{  1.0,  0.0,  1.6 },		//
	{  0.0,  0.0,  0.7 },		//
	{ -0.8,  0.0,  1.0 },		//
	{  4.0,  0.0,  4.0 },		//
	{  1.2,  0.0,  0.5 },		//
	{  1.0,  0.0,  1.6 },		//
	{  1.0,  0.0,  1.6 },		//
};

static zoom_setting settings[] = {
	{  0.0,  0.0,  1.5 },
	{ -0.5,  0.0,  1.0 },
};

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

static const char *set_name[] = { "Julia","Mandelbrot" };
static const char *plane_name[] = { "µ","1/µ","1/(µ+.25)","Lambda","1/Lambda","1/(Lambda-1)","1/(µ-1.40115)","1/(µ-2)" };
static const char *escape_name[] = { "Circle","Rectangle","And","Pickover","Bio","Bio 2*r*i","Real","Abs Real","Imaginary","Manhattan","Manr" };
static const char *fraction_name[] = { "Plain","LogLogZn/LogN","Z^2/B","Abs Zr/B","Sqrt Abs Zr/B","Abs Zi/B","Sqrt Abs Zi/B","Abs Zr+Zi/B","","","", };
static const char *decomp_name[] = { "Plain","Stripes","Binary","Feathers","","","","","","", };

enum {
	_MU					= 0x0010,
	_INV					= 0x0100,
	_INVMU				= 0x0110,
	_INVMU25				= 0x0111,
	_LAMBDA				= 0x0020,
	_INVLAMBDA			= 0x0120,
	_INVLAMBDA1			= 0x0121,
	_INVMU14				= 0x0112,
	_INVMU2				= 0x0113,
	_LAMBDANEG			= 0x0022,
	_SWAP					= 0x0200,
	_LAMBDASWAP			= 0x0223,
};

static const int plane_mask[] = {
	_MU,
	_INVMU,
	_INVMU25,
	_LAMBDA,
	_INVLAMBDA,
	_INVLAMBDA1,
	_INVMU14,
	_INVMU2,
	_LAMBDANEG,
	_LAMBDASWAP
};

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


#define Z2C zi=2.0*zr*zi+ci,zr=zrn-zin+cr,zrn=zr*zr,zin=zi*zi

#define ITERATE(n,f)\
		case n:while(i<max && f) Z2C,++i;break;

#define PERIODIC(n,f)\
		case n:\
			do {\
				ckr = zr,cki = zi,p += p;\
				if(p>max) p = max;\
				while(i<p && f) {\
					Z2C,++i;\
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

//double nmin,nmax,navg;

uint16_t Formula::colorIndex(int f,int d) {
	if(i==max) return 0;
	int c,n = 0;
	if(f) switch(f) {
		case FE_LOGN:n = (int)(32.0*log(log(::sqrt(zrn+zin)))/log(2.0));break;
		case FE_ZRINB:n = (int)(640.0*::sqrt(::sqrt((zrn+zin)/b2))-960.0);break;
		case FE_ABSZRB:n = (int)(640.0*::sqrt(::sqrt(fabs(zr)/b))-640.0);break;
		case FE_ABSZRB2:n = (int)(640.0*::sqrt(::sqrt(::sqrt(fabs(zr)/b)))-640.0);break;
		case FE_ABSZIB:n = (int)(640.0*::sqrt(::sqrt(fabs(zi)/b))-640.0);break;
		case FE_ABSZIB2:n = (int)(640.0*::sqrt(::sqrt(::sqrt(fabs(zi)/b)))-640.0);break;
		case FE_ABSZRIB:n = (int)(640.0*::sqrt(::sqrt((fabs(zr)+fabs(zi))/(b*2.0)))-640.0);break;
	}

//if(n<nmin) nmin = n;
//if(n>nmax) nmax = n;
//if(navg==0.0) navg = n;
//else navg = (navg+(double)n)*0.5;

	c = ((i<<5)-n);
	if(c<=0) c = 1;
	else if(c>0x7fff) c = 0x7fff;

	if(d) switch(d) {
		case DE_STRIPES:return (i&1)? c|0x8000 : c;
		case DE_BINARY:return (zi>0.0)? c|0x8000 : c;
		case DE_FEATHERS:return (fabs(zr)<b || fabs(zi)<b)? c|0x8000 : c;
		case DE_1:return (zr>0.0)? c|0x800 : c;
		case DE_2:return (fabs(zr)<b)? c|0x8000 : c;
		case DE_3:return (fabs(zi)<b)? c|0x8000 : c;
		case DE_4:return (zr>0.0 && zi>0.0)? c|0x8000 : c;
		case DE_5:return (zr>0.0 || zi>0.0)? c|0x8000 : c;
		case DE_6:return (fabs(zr)<b && fabs(zi)<b)? c|0x8000 : c;
	}
	return c;
}

double Formula::colorFloat(int f,int d) {
	if(i==max) return 0.0;
	double c,n = 0.0;
	if(f) switch(f) {
		case FE_LOGN:n = (32.0*log(log(::sqrt(zrn+zin)))/log(2.0));break;
		case FE_ZRINB:n = (640.0*::sqrt(::sqrt((zrn+zin)/b2))-960.0);break;
		case FE_ABSZRB:n = (640.0*::sqrt(::sqrt(fabs(zr)/b))-640.0);break;
		case FE_ABSZRB2:n = (640.0*::sqrt(::sqrt(::sqrt(fabs(zr)/b)))-640.0);break;
		case FE_ABSZIB:n = (640.0*::sqrt(::sqrt(fabs(zi)/b))-640.0);break;
		case FE_ABSZIB2:n = (640.0*::sqrt(::sqrt(::sqrt(fabs(zi)/b)))-640.0);break;
		case FE_ABSZRIB:n = (640.0*::sqrt(::sqrt((fabs(zr)+fabs(zi))/(b*2.0)))-640.0);break;
	}

//if(n<nmin) nmin = n;
//if(n>nmax) nmax = n;
//if(navg==0.0) navg = n;
//else navg = (navg+(double)n)*0.5;

	c = ((double)(i<<5))-n;
	if(c<0.000001) c = 0.000001;
	else if(c>32767.0) c = 32767.0;

	if(d) switch(d) {
		case DE_STRIPES:return (i&1)? -c : c;
		case DE_BINARY:return (zi>0.0)? -c : c;
		case DE_FEATHERS:return (fabs(zr)<b || fabs(zi)<b)? -c : c;
		case DE_1:return (zr>0.0)? -c : c;
		case DE_2:return (fabs(zr)<b)? -c : c;
		case DE_3:return (fabs(zi)<b)? -c : c;
		case DE_4:return (zr>0.0 && zi>0.0)? -c : c;
		case DE_5:return (zr>0.0 || zi>0.0)? -c : c;
		case DE_6:return (fabs(zr)<b && fabs(zi)<b)? -c : c;
	}
	return c;
}

void Formula::julia(double x,double y) {
	zr = zoom->x+1.5*(x-fractal->width/2.0)/(0.5*z*fractal->width);
	zi = zoom->y+(y-fractal->height/2.0)/(0.5*z*fractal->height);
	zrn = zr*zr;
	zin = zi*zi;
	periodic(fractal->escape);
}

void Formula::mandelbrot(double x,double y) {
	int p = plane_mask[fractal->plane];
	zr = zi = zrn = zin = 0.0;
	cr = zoom->left+x*pw;
	ci = zoom->top+y*ph;

	if(p&_INV) {
		double d = cr*cr+ci*ci;
		cr = cr/d,ci = -ci/d;
		if(p==_INVMU25) cr += 0.25;
		else if(p==_INVMU14) cr -= MYERBERG;
		else if(p==_INVMU2) cr -= 2.0;
		else if(p==_INVLAMBDA1) cr -= 1.0;
	}
	if(p&_LAMBDA) {
		double lr = cr/2.0;
		ci = ci/2.0;
		if(p==_LAMBDANEG) {
			cr = (lr*lr-ci*ci) - lr;
			ci = (ci*lr+lr*ci) - ci; 
		} else {
			cr = lr - (lr*lr-ci*ci);
			ci = ci - (ci*lr+lr*ci); 
		}
	}
	if(p&_SWAP) {
		double t = cr;
		cr = ci, ci = t;
	}

//	if(fabs(ci)<ph*0.5) ci = 0.0;
	ch = hypot(cr-0.25,ci);
	if(cr<ch-2.0*ch*ch+.25 || (cr+1.0)*(cr+1.0)+ci*ci<0.0625) i = max;
	else periodic(fractal->escape);
}


Fractal::Fractal(int w,int h,int p,Fractal *f) : params(p),zoom(),formula() {
	first = 0;
	last = 0;
	node = 0;
	float1 = 0;
	index1 = index2 = 0;
	map = 0;
	setSize(w,h);
	if(f) {
		zoom = f->zoom;
		formula = f->formula;
		set = f->set;
		plane = f->plane;
		escape = f->escape;
		fraction = f->fraction;
		decomp = f->decomp;
		range = f->range;
		radius = f->radius;
	}
	formula.fractal = this;
	formula.zoom = &zoom;
	percent = -1.0;
	time = 0;
	nthreads = 0;
	threads = 0;
	thfin = 0;
}

Fractal::~Fractal() {
	if(first) delete first;
	first = 0;
	last = 0;
	if(threads) delete[] threads;
	threads = 0;
	if(index2 && index2!=index1) free(index2);
	if(index1) free(index1);
	index1 = index2 = 0;
	if(float1) free(float1);
	float1 = 0;
	if(map) free(map);
	map = 0;
}

void Fractal::setSize(int w,int h) {
	int i,sz = w*h;
	sw = w;
	sh = h;
	width = (double)w;
	height = (double)h;
	if(float1) free(float1);
	if(index2 && index2!=index1) free(index2);
	if(index1) free(index1);
	if(map) free(map);
	if(params&FRACT_FLOAT) {
		float1 = (float *)malloc(sz*sizeof(float));
		for(i=0; i<sz; ++i) float1[i] = 0.0f;
		index1 = index2 = 0;
	} else {
		index1 = (uint16_t *)malloc(sz*sizeof(uint16_t));
		memset(index1,0,sz*sizeof(uint16_t));
		if((params&(FRACT_BUFFERED|FRACT_DBUFFERED))) {
			index2 = (uint16_t *)malloc(sz*sizeof(uint16_t));
			for(int i=0; i<sz; i++) index2[i] = ITER_BG;
		} else index2 = index1;
		float1 = 0;
	}
	map = (uint8_t *)malloc(sz*sizeof(uint8_t));
	percent = -1.0;
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
	int n,sz = sw*sh,x,y,t;
	timeval tv;

fprintf(stderr,"Fractal::render(1)\n");
fflush(stderr);
	if(i>0) formula.max = i;
	else {
		i = formula.min+range;
		formula.max = i>ITER_MAX? ITER_MAX : i;
	}

fprintf(stderr,"Fractal::render(2)\n");
fflush(stderr);
	formula.b = radius;
	formula.b2 = formula.b*formula.b;
	formula.min = ITER_MAX;
	formula.tot = 0;
	formula.pw = zoom.width/width;
	formula.ph = zoom.height/height;

fprintf(stderr,"Fractal::render(3)\n");
fflush(stderr);
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

fprintf(stderr,"Fractal::render(4)\n");
fflush(stderr);
	if(params&FRACT_FLOAT)
		for(n=0; n<sz; ++n) float1[n] = 0.0f;
	else if(params&FRACT_EDGETRACE) {
		memset(index1,0,sw*sh*sizeof(uint16_t));
		memset(map,0,sw*sh*sizeof(uint8_t));
	}

fprintf(stderr,"Fractal::render(5)\n");
fflush(stderr);
	thfin = 0;
	percent = 0.0;
	gettimeofday(&tv,0);
	time = tv.tv_sec*1000000+tv.tv_usec;

fprintf(stderr,"Fractal::render(6)\n");
fflush(stderr);
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
		nthreads = 1;
		calculate(formula,0,0,0,sw,sh,func);
/*		nthreads = 4;
		for(y=0,t=0; y<sh; y+=sh/2)
			for(x=0; x<sw; x+=sw/2,++t)
				calculate(formula,t,x,y,x+sw/2,y+sh/2,func);*/
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
	double yp = 100.0/(double)(nthreads*(b-t));
fprintf(stderr,"Fractal::calculate:  n: %d, l: %d, t: %d, r: %d, b: %d\n",n,l,t,r,b);
fflush(stderr);
//fprintf(stderr,"sw: %d, sh: %d\n",sw,sh);

//nmin = 1024;
//nmax = -1024;
//navg = 0.0;

	if(params&FRACT_FLOAT) {
fprintf(stderr,"Fractal::calculate: float %p\n",float1);
fflush(stderr);
		for(y=t,yw=t*sw; y<b; ++y,yw+=sw) {
			for(x=l; x<r; ++x) {
				(form.*f)(x,y);
				float1[x+yw] = form.colorFloat(fraction,decomp);
			}
			percent += yp;
		}
/*	} else if(params&FRACT_EDGETRACE) {
		int x1,y1,d,d1;
		uint16_t c,c0,c1;
		for(y=t,yw=t*sw; y<b; ++y,yw+=sw) {
			if(y>t) for(x=l; x<r; ++x)
				if((c=index1[x+yw])&ITER_EDGE) {
//fprintf(stderr,"x: %d, y: %d, c: %d\n",x,y,c);
					for(c0=(c&0xfff),++x; x<r && index1[x+yw]==c; ++x);
					if(x<r && (!(c1=index1[x+yw]) || c1==c0)) {
						for(; x<r && (!(c1=index1[x+yw]) || c1==c0); ++x)
							index1[x+yw] = c0|ITER_FILL;
						if(x<r && c1!=c) --x;
					} else --x;
				}
			for(x=l; x<r; ++x) {
				while(x<r && index1[x+yw]) ++x;
				if(x<r) {
					(form.*f)(x,y);
					c0 = form.colorIndex(fraction,decomp);
					c = c0|ITER_EDGE;
					index1[x+yw] = c;
					for(; x>0 && (index1[x-1+yw]|ITER_EDGE)==c; --x);
					x1 = x,y1 = y,d = 0,d1 = 0;
					do {
						if(x1<l || x1>=r || y1<t || y1>=b) c1 = 0xffff;
						else if(!(c1=index1[x1+yw])) {
							(form.*f)(x1,y1);
							c1 = form.colorIndex(fraction,decomp);
							index1[x1+yw] = c1;
						}
//fprintf(stderr,"Fractal::trace(x: %d, y: %d, x1: %d, y1: %d, d: %d, c: %d, c1: %d(%d))\n",x,y,x1,y1,d,c,c1,c1|ITER_EDGE);
						if((c1|ITER_EDGE)==c) {
							if(!(c1&ITER_EDGE)) index1[x1+yw] = c;
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
			percent += yp;
		}*/
	} else if(params&FRACT_EDGETRACE) {
		int i,x1,y1,d,d1;
		uint16_t c,c1;
		for(y=t,yw=t*sw,i=0; y<b; ++y,yw+=sw) {
			if(y>t) for(x=l; x<r; ++x)
				if(map[x+yw]&2) {
//					for(++x; x<r && (map[x+yw]&2); ++x);
					for(; x<r && !index1[x+yw] && !(map[x+yw]&4); ++x) map[x+yw] = 9;
				}
			for(x=l; x<r; ++x,++i) {
				if(i==32) i = 0;
				if(map[x+yw]) continue;
				(form.*f)(x,y);
				c = form.colorIndex(fraction,decomp);
				index1[x+yw] = c;
				map[x+yw] = c==0? 3 : 1;
				if(c==0) {
					x1 = x,y1 = y,d = 0,d1 = 0;
					do {
						if(x1<l || x1>=r || y1<t || y1>=b) c1 = 0xffff;
						else if(map[x1+yw]) c1 = index1[x1+yw];
						else {
							(form.*f)(x1,y1);
							c1 = form.colorIndex(fraction,decomp);
							index1[x1+yw] = c1;
							map[x1+yw] = c1==0? 3 : 5;
						}
//fprintf(stderr,"Fractal::trace(x: %d, y: %d, x1: %d, y1: %d, d: %d, c: %d, c1: %d(%d))\n",x,y,x1,y1,d,c,c1,c1|ITER_EDGE);
						if(c1==c) {
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
			percent += yp;
		}
	} else {
		for(y=t,yw=t*sw; y<b; ++y,yw+=sw) {
			for(x=l; x<r; ++x) {
				(form.*f)(x,y);
				index1[x+yw] = form.colorIndex(fraction,decomp);
			}
			percent += yp;
		}
	}
	finish(n);
}

void Fractal::finish(int t) {
	if(threads) threads[0].lock();
	timeval tv;
	gettimeofday(&tv,0);
	thfin |= (1<<t);
	if(thfin==(0xffffffff>>(32-nthreads))) {
		long n = tv.tv_sec*1000000+tv.tv_usec;
		percent = 100.0;
		printf("%s: range[%d] escape[%g] min[%d] max[%d] Cr[%f] Ci[%f]\n",
				set==SET_JULIA? "Julia Set" : "Mandelbrot Set",
				range,radius,formula.min,formula.max,formula.cr,formula.ci);
		time = n-time;
		printf("Time: %ld\nIterations: %ld\nTime per iteration: %g\n",time,formula.tot,(double)time/(double)formula.tot);

//printf("nmin: %g, nmax: %g, navg: %g\n",nmin,nmax,navg);
	}
	if(threads) threads[0].unlock();
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
		index1[x1+y1*sw] = c^ITER_EDGE;
		if(x1>0 && (!(c1=index1[x1-1+y1*sw]) || c1==c)) st[s].x = x1-1,st[s].y = y1,++s;
		if(x1<sw-1 && (!(c1=index1[x1+1+y1*sw]) || c1==c)) st[s].x = x1+1,st[s].y = y1,++s;
		if(x1>0 && (!(c1=index1[x1-1+y1*sw]) || c1==c)) st[s].x = x1-1,st[s].y = y1,++s;
		if(x1<sw-1 && (!(c1=index1[x1+1+y1*sw]) || c1==c)) st[s].x = x1+1,st[s].y = y1,++s;
	}
fprintf(stderr,"Fractal::fill(i: %d)\n",i);
}
*/

void Fractal::flip() {
	uint16_t *col = index1;
	index1 = index2;
	index2 = col;
}

void Fractal::restart(int s) {
	if(s>=0 && s<SET_NUM) set = s;
	settings[set] = sets[set];
	setZoom(settings[set].x,settings[set].y,settings[set].z);
}

void Fractal::random(int s) {
	int i;
	set = s>=0 && s<SET_NUM? s : SET_MANDELBROT;
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

	memcpy(settings,sets,sizeof(sets));

	if(params&(FRACT_BUFFERED|FRACT_DBUFFERED))
		memset(index2,0,sw*sh*sizeof(uint16_t));

	x_point = width/2.0;
	y_point = height/2.0;
	setZoom(sets[set].x,sets[set].y,sets[set].z);
}

void Fractal::randomStyle() {
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
	percent = -1.0;
}

void Fractal::increaseRadius(bool i) {
	int n = radius;
	if(i) radius += n<4.0f? 1.0f : n<20.0f? 2.0f : n<50.0f? 5.0f : 10.0f;
	else radius -= n>50.0f? 10.0f : n>20.0f? 5.0f : n>4.0f? 2.0f : 1.0f;
	if(radius<0.5f) radius = 0.5f;
	percent = -1.0;
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

void Fractal::setSet(int s) {
	if(s>=0 && s<SET_NUM && s!=set) {
		settings[set].x = zoom.x;
		settings[set].y = zoom.y;
		settings[set].z = zoom.z;
		if(s==SET_JULIA) {
			settings[s] = sets[s];
			formula.cr = zoom.x;
			formula.ci = zoom.y;
		}
		set = s;
		printf("Set[%s]\n",set_name[set]);
		setZoom(settings[set].x,settings[set].y,settings[set].z);
	}
}

void Fractal::setPlane(int p) {
	if(p>=0 && p<PL_NUM && p!=plane) {
		set = SET_MANDELBROT;
		plane = p;
		printf("Plane[%s]\n",plane_name[plane]);
		setZoom(planes[plane].x,planes[plane].y,planes[plane].z);
	}
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
		percent = -1.0;
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
	zoom.width = 4.0*zoom.z;
	zoom.height = 4.0*(height/width)*zoom.z;
	zoom.left = x-zoom.width*0.5;
	zoom.top = y-zoom.height*0.5;

	r = 1.0/r;
	zrect.x = (int16_t)(x_point-width/(2.0*r));
	zrect.y = (int16_t)(y_point-height/(2.0*r));
	zrect.w = (uint16_t)(width/r);
	zrect.h = (uint16_t)(height/r);

	percent = -1.0;

	printf("Zoom: x[%g] y[%g] z[%g]\n",x,y,z);
}

static const double speed_limit = 0.025;
static const double speed_accell = 0.0025;
static const double speed_dec_limit = 0.975*0.9775*0.98*0.9825*0.985*0.9875*0.99*0.9925*0.995*0.9975;
static const double speed_inc_limit = 1.0/speed_dec_limit;

void Fractal::resetZoom() {
	if(first) delete first;
	first = 0;
	last = 0;
	node = 0;
	zoom.index = 0;
	zoom.steps = 0;
}

void Fractal::addZoomNode() {
	if(!last || last->x!=zoom.x || last->y!=zoom.y || last->z!=zoom.z) {
		ZoomNode *n = new ZoomNode(zoom.x,zoom.y,zoom.z);
		if(first) last->next = n,last = n;
		else first = last = n;
	}
	last->pause += 10;
}

void Fractal::nextZoomNode() {
	if(node && node->next) node = node->next;
	else node = first;
	setZoom(node->x,node->y,node->z);
	percent = -1.0;
}

void Fractal::startZoom() {
	if(!first) return;
	node = first;
	zoom.x = node->x;
	zoom.y = node->y;
	zoom.z = node->z;
	zoom.index = 0;
	zoom.pause = 0;
	zoom.state = 0;
	zoom.steps = 0;
	zoom.sp = zoom.zsp = 0.0;
	while(nextZoom());
	zoom.steps = zoom.index;
	node = first;
	zoom.index = 0;
	zoom.pause = 0;
	zoom.state = 0;
	zoom.sp = zoom.zsp = 0.0;
	setZoom(node->x,node->y,node->z);
}

bool Fractal::nextZoom() {
	++zoom.index;
	if(!node) return false;
	if(zoom.x==node->x && zoom.y==node->y && zoom.z==node->z) {
		if(zoom.pause==0) zoom.pause = node->pause;
		else if(zoom.pause>1) --zoom.pause;
		else {
			zoom.pause = 0;
			node = node->next;
			if(!node) return false;
			zoom.sp = zoom.zsp = 0.0;
		}
	} else {
		if((zoom.x!=node->x || zoom.y!=node->y) && zoom.z>=node->z) {
			double d = atan2(node->y-zoom.y,node->x-zoom.x);
			double p = zoom.sp*((4.0*zoom.z)/width);
			double x1 = zoom.x+cos(d)*p*10.0,y1 = zoom.y+sin(d)*p*10.0;
			if(((zoom.x<node->x && x1>=node->x) || (zoom.x>node->x && x1<=node->x) ||
				(zoom.y<node->y && y1>=node->y) || (zoom.y>node->y && y1<=node->y)) &&
					zoom.sp>1.0) zoom.sp -= 1.0;
			else if(zoom.sp<10.0) zoom.sp += 1.0;
			x1 = zoom.x+cos(d)*p,y1 = zoom.y+sin(d)*p;
			if(zoom.x<node->x) {
				zoom.x = x1;
				if(zoom.x>=node->x) zoom.x = node->x;
			} else {
				zoom.x = x1;
				if(zoom.x<=node->x) zoom.x = node->x;
			}
			if(zoom.y<node->y) {
				zoom.y = y1;
				if(zoom.y>=node->y) zoom.y = node->y;
			} else {
				zoom.y = y1;
				if(zoom.y<=node->y) zoom.y = node->y;
			}
		} else if(zoom.z!=node->z) {
			if(((zoom.z<node->z && zoom.z*speed_inc_limit>=node->z) ||
				(zoom.z>node->z && zoom.z*speed_dec_limit<=node->z)) &&
					zoom.zsp>speed_accell) zoom.zsp -= speed_accell;
			else if(zoom.zsp<speed_limit) zoom.zsp += speed_accell;
			if(zoom.z<node->z) {
				zoom.z *= 1.0+zoom.zsp;
				if(zoom.z>=node->z) zoom.z = node->z;
			} else {
				zoom.z *= 1.0-zoom.zsp;
				if(zoom.z<=node->z) zoom.z = node->z;
			}
		}
	}
printf("Next Zoom: x[%g][%g] y[%g][%g] z[%g][%g] index[%d]\n",zoom.x,node->x,zoom.y,node->y,zoom.z,node->z,zoom.index);
	return true;
}

bool Fractal::renderZoom(int i) {
	setZoom(zoom.x,zoom.y,zoom.z);
	if(zoom.pause==0) render(i);
	return nextZoom();
}


void Fractal::write(FILE *fp) {
	fprintf(fp,"Fractal: %d %d %d %d %d\n",set,plane,escape,fraction,decomp);
	fprintf(fp,"Zoom: %g %g %g\n",zoom.x,zoom.y,zoom.z);
	if(first) {
		ZoomNode *n = first;
		for(; n; n=n->next)
			fprintf(fp,"Node: %g %g %g %d\n",n->x,n->y,n->z,n->pause);
	}
	fprintf(fp,"\n");
}

void Fractal::read(FILE *fp) {
	char str[1025];
	if(first) delete first;
	first = last = node = 0;
	while(fgets(str,1024,fp) && *str && *str!='\n') {
		if(!strncmp(str,"Fractal:",8)) sscanf(&str[8]," %d %d %d %d %d",&set,&plane,&escape,&fraction,&decomp);
		else if(!strncmp(str,"Zoom:",5)) {
			sscanf(&str[5]," %lf %lf %lf",&zoom.x,&zoom.y,&zoom.z);
			setZoom(zoom.x,zoom.y,zoom.z);
		} else if(!strncmp(str,"Node:",5)) {
			ZoomNode *n = new ZoomNode(0.0,0.0,0.0);
			sscanf(&str[5]," %lf %lf %lf %d",&n->x,&n->y,&n->z,&n->pause);
			if(first) last->next = n,last = n;
			else first = last = n;
		}
	}

fprintf(stderr,"Fractal: %d %d %d %d %d\n",set,plane,escape,fraction,decomp);
fflush(stderr);
fprintf(stderr,"Zoom: %g %g %g\n",zoom.x,zoom.y,zoom.z);
if(first) {
ZoomNode *n = first;
for(; n; n=n->next)
fprintf(stderr,"Node: %g %g %g %d\n",n->x,n->y,n->z,n->pause);
}
fprintf(stderr,"\n");

	percent = -1.0;
}


}; /* namespace a */

