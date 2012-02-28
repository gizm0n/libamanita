
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <esoteric/calendar.h>
#include <esoteric/astro/astronomy.h>
#include <esoteric/astro/astrology.h>
#include "astronomy_data.h"


#define _DEF_PI			3.1415926535897932384626433832795
#define _DEF_AEARTH		6378137.0
#define _DEF_AU			1.49597870691e8
#define _DEF_CLIGHT		2.99792458e5

const double J2000		= 2451545.0;
const double B1950		= 2433282.423;
const double J1900		= 2415020.0;

																						
const double DTR			= 0.017453292519943295769236907684886;
const double RTD			= 57.295779513082320876798154814105;
const double RTH			= 3.8197186342054880584532103209403;
const double HTR			= 0.26179938779914943653855361527329;
const double RTS			= 2.0626480624709635516e5;
const double STR			= 4.8481368110953599359e-6;

const double PI			= _DEF_PI;
const double PIx2			= _DEF_PI*2.0;
const double PIx1_5		= _DEF_PI*1.5;
const double PIx0_5		= _DEF_PI*0.5;

const double aearth		= _DEF_AEARTH;
const double au			= _DEF_AU;
const double emrat		= 81.300585;
const double Clight		= _DEF_CLIGHT;
const double Clightaud	= 86400.0*_DEF_CLIGHT/_DEF_AU;
const double Rearth		= 0.001*_DEF_AEARTH/_DEF_AU;
const double flat			= 298.257222;


static double ss[NARGS][31];
static double cc[NARGS][31];

static double Args[NARGS];
static double LP_equinox;
static double NF_arcsec;
static double Ea_arcsec;
static double pA_precession;


double mod360(double x) {
	long k = (long)(x/360.0);
	double y = x-k*360.0;
	while(y<0.0) y += 360.0;
	while(y>360.0) y -= 360.0;
	return y;
}

double mod3600(double x) {
	return (x-1296000.*floor(x/1296000.));
}

double dec_to_hour(double d) { return (int)d+(d-(int)d)*0.6; }
double hour_to_dec(double d) { return (int)d+(d-(int)d)*100.0/60.0; }

double dec(double d,double m,double s) { return d+(m/60.0)+(s/3600.0); }
double asn(double a) { return atan(a/sqrt(1-a*a)); }
double acs(double a) { return atan(sqrt(1-a*a)/a); }


static double zatan2(double x,double y) {
	double w;
	short code = 0;
	if(x<0.0) code = 2;
	if(y<0.0) code |= 1;
	if(x==0.0) return code&1? PIx1_5 : (y==0.0? 0.0 : PIx0_5);
	if(y==0.0) return code&2? PI : 0.0;
	switch(code) {
		default:
		case 0: w = 0.0;break;
		case 1: w = PIx2;break;
		case 2:
		case 3: w = PI;break;
	}
	return w+atan(y/x);
}

static void fk4_to_fk5(double p[],double m[],astro_star *el) {
	double a,b,c;
	double *u;
	const double *v;
	double R[6];
	int i,j;
	a = 0.0,b = 0.0;
	for(i=0; i<3; i++) m[i] *= RTS,a += A[i]*p[i],b += Ad[i]*p[i];
	for(i=0; i<3; i++) R[i] = p[i]-A[i]+a*p[i],R[i+3] = m[i]-Ad[i]+b*p[i];
	v = &matrix[0];
	for(i=0; i<6; i++) {
		a = 0.0,u = &R[0];
		for(j=0; j<6; j++) a += *u++ * *v++;
		if(i<3) p[i] = a;
		else m[i-3] = a;
	}
	b = p[0]*p[0]+p[1]*p[1],a = b+p[2]*p[2],c = a,a = sqrt(a);
	el->ra = zatan2(p[0],p[1]);
	el->dec = asin(p[2]/a);
	el->mura = (p[0]*m[1]-p[1]*m[0])/(RTS*b);
	el->mudec = (m[2]*b-p[2]*(p[0]*m[0]+p[1]*m[1]))/(RTS*c*sqrt(b));
	if(el->px>0.0) {
		c = 0.0;
		for(i=0; i<3; i++) c += p[i]*m[i];
		el->v = c/(21.094952663*el->px*RTS*a);
	}
	el->px = el->px/a;
	el->epoch = J2000;
}


/*
static double tdb(double JED) {
	double M,T;
	T = (JED-J2000)/36525.0;
	M = 129596581.038354*T+ 1287104.76154;
	M = M-1296000.0*floor(M/1296000.0);
	M += ((((((((1.62e-20*T-1.0390e-17)*T-3.83508e-15)*T+4.237343e-13)*T+8.8555011e-11)*T-4.77258489e-8)*T-
		1.1297037031e-5)*T+1.4732069041e-4)*T-0.552891801772)*T*T;
	M *= STR;
	return JED+(0.001658*sin(M)+0.000014*sin(M+M))/86400.0;
}
*/

static double modtp(double x) {
	double y = floor(x/PIx2);
	y = x-y*PIx2;
	while(y<0.0) y += PIx2;
	while(y>=PIx2) y -= PIx2;
	return y;
}

static void deltap(double p0[],double p1[],double *dr,double *dd) {
	double dp[3],A,B,P = 0.0,Q = 0.0,x,y,z = 0.0;
	int i;
	for(i=0; i<3; i++) x = p0[i],y = p1[i],P += x*x,Q += y*y,y = y-x,dp[i] = y,z += y*y;
	A = sqrt(P),B = sqrt(Q);
	if((A<1.e-7) || (B<1.e-7) || (z/(P+Q))>5.e-7) {
		P = zatan2(p0[0],p0[1]);
		Q = zatan2(p1[0],p1[1]);
		Q = Q-P;
		while(Q<-PI) Q += PIx2;
		while(Q>PI) Q -= PIx2;
		*dr = Q,P = asin(p0[2]/A),Q = asin(p1[2]/B),*dd = Q-P;
	} else {
		x = p0[0],y = p0[1];
		if(x==0.0) *dr = 1.0e38;
		else Q = y/x,Q =(dp[1]-dp[0]*y/x)/(x*(1.0+Q*Q)),*dr = Q;
		x = p0[2]/A,P = sqrt(1.0-x*x),*dd =(p1[2]/B-x)/P;
	}
}

static double deltat(double Y) {
	double ans,p,B;
	int d[6];
	int i,iy,k;
	if(Y>TABEND) {
		B = Y-TABEND,p = delta_table[TABSIZ-1]-delta_table[TABSIZ-2];
		ans = 0.01*(delta_table[TABSIZ-1]+p*B+((delta_table[TABSIZ-101]-(delta_table[TABSIZ-1]-100.0*p))*1e-4)*B*B);
		return ans;
	}
	if(Y<TABSTART) {
		if(Y>=948.0) B = 0.01*(Y-2000.0),ans =(23.58*B+100.3)*B+101.6;
		else B = 0.01*(Y-2000.0)+3.75,ans = 35.0*B*B+40.;
		return ans;
	}
	p = floor(Y),iy = (int)(p-TABSTART),ans = delta_table[iy],k = iy+1;
	if(k<TABSIZ) {
		p = Y-p,ans += p*(delta_table[k]-delta_table[iy]);
		if(iy-1>=0 && iy+2<TABSIZ) {
			k = iy-2;
			for(i=0; i<5; i++,k++) {
				if((k<0) ||(k+1 >= TABSIZ)) d[i] = 0;
				else d[i] = delta_table[k+1]-delta_table[k];
			}
			for(i=0; i<4; i++) d[i] = d[i+1]-d[i];
			B = 0.25*p*(p-1.0),ans += B*(d[1]+d[2]);
			if(iy+2<TABSIZ) {
				for(i=0; i<3; i++) d[i] = d[i+1]-d[i];
				B = 2.0*B/3.0,ans +=(p-0.5)*B*d[1];
				if(iy-2>=0 && iy+3<=TABSIZ) {
					for(i=0; i<2; i++) d[i] = d[i+1]-d[i];
					B = 0.125*B*(p+1.0)*(p-2.0),ans += B*(d[0]+d[1]);
				}
			}
		}
	}
	ans *= 0.01;
	if(Y<1955.0) B =(Y-1955.0),ans += -0.000091*(-25.8+26.0)*B*B;
	return ans;
}




static void update(astro_data *ad,int f) {
	double T = 2000.0+(ad->JD-J2000)/365.25;
	switch(f) {
		case 0:ad->TDT = ad->JD,ad->UT = ad->JD;break;
		case 1:ad->TDT = ad->JD,ad->UT = ad->TDT-deltat(T)/86400.0;break;
		case 2:ad->UT = ad->JD,ad->TDT = ad->UT+deltat(T)/86400.0;break;
	}
}


static char astronomy_data_path[257] = "./";

void astronomy_set_data_path(const char *path) {
	int len = strlen(path);
	char *p;
	strcpy(astronomy_data_path,path);
	p = &astronomy_data_path[len-1];
#ifdef USE_WIN32
	if(*p!='\\') ++p,*p++ = '\\',*p = '\0';
#else
	if(*p!='/') ++p,*p++ = '/',*p = '\0';
#endif
}

/*
static int fincat(astro_data *ad,const char *fn,int index,char *str) {
	FILE *fp;
	char *r,s[257];
	size_t n;
	*str = '\0';
	sprintf(s,"%s%s",astronomy_data_path,fn);
	fp = fopen(s,"rb");
	if(!fp) return 0;
	r = fgets(s,256,fp);
	if(index>1) fseek(fp,(index-1)*112,SEEK_CUR);
	n = fread(str,110,1,fp);
	str[110] = '\0';
	fclose(fp);
	return 1;
}
*/
static int get_orbit(astro_data *ad,astro_orbit *el,int index) {
	memset(el,0,sizeof(astro_orbit));
	if(index<=0) return 0;
	else {
//		char str[257];
//		char orbnam[80];
//		int num,year,month;
//		size_t ret;
//		double day;
//		FILE *fp;
//		sprintf(str,"%sorbits.cat",astronomy_data_path);
//		fp = fopen(str,"r");
//		if(!fp) return 0;
//		if(index>1) fseek(fp,(index-1)*128,SEEK_SET);
//		ret = fread(str,127,1,fp);
//		fclose(fp);
//		str[127] = 0;
//printf("orbit(%s)\n",str);
//		sscanf(str,"%d %s %d %d %lf %lf %lf %lf %lf %lf %lf %lf",
//			&num,orbnam,&year,&month,&day,&el->M,&el->a,&el->ecc,&el->w,&el->W,&el->i,&el->mag);

		const astro_orbit_data *aod = &astro_orbits_data[index];
		el->M = aod->M;
		el->a = aod->a;
		el->ecc = aod->ecc;
		el->w = aod->w;
		el->W = aod->W;
		el->i = aod->i;
		el->mag = aod->mag;

		el->epoch = el->equinox = calendar_get_julian_day(aod->year,aod->month,aod->day,1);
printf("orbit(%7d,%4d,%02d,%02.1lf,%3.4lf,%2.6lf,%1.6lf,%3.4lf,%3.4lf,%3.4lf,%3.2lf,%.1lf)\n",
		index,aod->year,aod->month,aod->day,el->M,el->a,el->ecc,el->w,el->W,el->i,el->mag,el->epoch);
		return 1;
	}
}

static int get_star(astro_data *ad,astro_star *el,int index) {
	int sign,i;
	char starnam[80],s[256],*p;
	double rh,rm,rs,dd,dm,ds,x,z;
//	if(!fincat(ad,"fixedstars.cat",index,s)) return 0;
//	sscanf(s,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s",
//		&el->epoch,&rh,&rm,&rs,&dd,&dm,&ds,&el->mura,&el->mudec,&el->v,&el->px,&el->mag,starnam);

	const astro_fixedstar_data *afd = &astro_fixedstars_data[index];

	el->epoch = afd->epoch;
	rh = afd->rh;
	rm = afd->rm;
	rs = afd->rs;
	dd = afd->dd;
	dm = afd->dm;
	ds = afd->ds;
	el->mura = afd->mura;
	el->mudec = afd->mudec;
	el->v = afd->v;
	el->px = afd->px;
	el->mag = afd->mag;

	x = el->epoch;
	el->epoch = (x==2000.0? J2000 : (x==1950.0? B1950 : (x==1900.0? J1900 : J2000+365.25*(x-2000.0))));
	el->ra = PIx2*(3600.0*rh+60.0*rm+rs)/86400.0;
	sign = 1;
	if((dd<0.0) || (dm<0.0) || (ds<0.0)) sign = -1;
	if(afd->dd==-127) dd = 0.0;
	z =(3600.0*fabs(dd)+60.0*fabs(dm)+fabs(ds))/RTS;
	if(sign<0) z = -z;
	el->dec = z,el->mura *= 15.0/RTS,el->mudec /= RTS,z = el->px;
	el->px = z<1.0? (z<=0.0? 0.0 : STR*z) : 1.0/(RTS*z);
	return 1;
}




static void sscc(int k,double arg,int n) {
	double cu,su,cv,sv,s;
	int i;
	ss[k][0] = su = sin(arg);
	cc[k][0] = cu = cos(arg);
	ss[k][1] = sv = 2.0*su*cu;
	cc[k][1] = cv = cu*cu-su*su;
	for(i=2; i<n; i++) {
		s =  su*cv+cu*sv;
		cv = cu*cv-su*sv;
		sv = s;
		ss[k][i] = sv;
		cc[k][i] = cv;
	}
}

static void epsiln(astro_data *ad,double J) {
	if(J!=ad->jdeps) {
		double T;
		T = (J-2451545.0)/36525.0;
		T /= 10.0;
		ad->eps = (((((((((2.45e-10*T+5.79e-9)*T+2.787e-7)*T+7.12e-7)*T-3.905e-5)*T-2.4967e-3)*T-
			5.138e-3)*T+1.9989)*T-0.0175)*T-468.33960)*T+84381.406173;
		ad->eps *= STR;
		ad->coseps = cos(ad->eps);
		ad->sineps = sin(ad->eps);
		ad->jdeps = J;
	}
}

static void precess(astro_data *ad,double R[],double J,int direction) {
	if(J!=J2000) {
		double A,B,T,pA,W,z;
		double x[3];
		const double *p;
		int i;
		T = (J-J2000)/36525.0;
		if(direction==1) epsiln(ad,J);
		else epsiln(ad,J2000);
		x[0] = R[0];
		z = ad->coseps*R[1]+ad->sineps*R[2];
		x[2] = -ad->sineps*R[1]+ad->coseps*R[2];
		x[1] = z;
		T /= 10.0;
		p = pAcof;
		pA = *p++;
		for(i=0; i<9; i++) pA = pA*T+*p++;
		pA *= STR*T;
		p = nodecof;
		W = *p++;
		for(i=0; i<10; i++) W = W*T+*p++;
		if(direction==1) z = W+pA;
		else z = W;
		B = cos(z);
		A = sin(z);
		z = B*x[0]+A*x[1];
		x[1] = -A*x[0]+B*x[1];
		x[0] = z;
		p = inclcof;
		z = *p++;
		for(i=0; i<10; i++) z = z*T+*p++;
		if(direction==1) z = -z;
		B = cos(z);
		A = sin(z);
		z = B*x[1]+A*x[2];
		x[2] = -A*x[1]+B*x[2];
		x[1] = z;
		if(direction==1) z = -W;
		else z = -W-pA;
		B = cos(z);
		A = sin(z);
		z = B*x[0]+A*x[1];
		x[1] = -A*x[0]+B*x[1];
		x[0] = z;
		if(direction==1) epsiln(ad,J2000);
		else epsiln(ad,J);
		z = ad->coseps*x[1]-ad->sineps*x[2];
		x[2] = ad->sineps*x[1]+ad->coseps*x[2];
		x[1] = z;
		for(i=0; i<3; i++) R[i] = x[i];
	}
}

static void mean_elements(double J) {
	double x,T,T2;
	T = (J-2451545.0)/36525.0,T2 = T*T;
	/* Mercury */
	x = mod3600(538101628.6889819*T+908103.213);
	x += (6.39e-6*T-0.0192789)*T2;
	Args[0] = STR*x;
	/* Venus */
	x = mod3600(210664136.4335482*T+655127.236);
	x += (-6.27e-6 *T+0.0059381)*T2;
	Args[1] = STR*x;
	/* Earth  */
	x = mod3600(129597742.283429*T+361679.198);
	x += (-5.23e-6*T-2.04411e-2)*T2;
	Ea_arcsec = x;
	Args[2] = STR*x;
	/* Mars */
	x = mod3600( 68905077.493988*T+ 1279558.751);
	x += (-1.043e-5*T+0.0094264)*T2;
	Args[3] = STR*x;
	/* Jupiter */
	x = mod3600(10925660.377991*T+123665.420);
	x +=((((-3.4e-10*T+5.91e-8)*T+4.667e-6)*T+5.706e-5)*T-3.060378e-1)*T2;
	Args[4] = STR*x;
	/* Saturn */
	x = mod3600(4399609.855372*T+180278.752);
	x +=((((8.3e-10*T-1.452e-7)*T-1.1484e-5)*T-1.6618e-4)*T+7.561614E-1)*T2;
	Args[5] = STR*x;
	/* Uranus */
	x = mod3600(1542481.193933*T+1130597.971)+(0.00002156*T-0.0175083)*T2;
	Args[6] = STR*x;
	/* Neptune */
	x = mod3600(786550.320744*T+1095655.149)+(-0.00000895*T+0.0021103)*T2;
	Args[7] = STR*x;
	/* Copied from cmoon.c,DE404 version. Mean elongation of moon = D */
	x = mod3600(1.6029616009939659e+09*T+1.0722612202445078e+06);
	x += (((((-3.207663637426e-013*T+2.555243317839e-011)*T+2.560078201452e-009)*T-3.702060118571e-005)*T+
		6.9492746836058421e-03)*T-6.7352202374457519e+00)*T2;
	Args[9] = STR*x;
	/* Mean distance of moon from its ascending node = F */
	x = mod3600(1.7395272628437717e+09*T+3.3577951412884740e+05);
	x += (((((4.474984866301e-013*T+4.189032191814e-011)*T-2.790392351314e-009)*T-2.165750777942e-006)*T-
		7.5311878482337989e-04)*T-1.3117809789650071e+01)*T2;
	NF_arcsec = x;
	Args[10] = STR*x;
	/* Mean anomaly of sun = l'(J. Laskar) */
	x = mod3600(1.2959658102304320e+08*T+1.2871027407441526e+06);
	x += ((((((((1.62e-20*T-1.0390e-17)*T-3.83508e-15)*T+4.237343e-13)*T+8.8555011e-11)*T-4.77258489e-8)*T-
		1.1297037031e-5)*T+8.7473717367324703e-05)*T-5.5281306421783094e-01)*T2;
	Args[11] = STR*x;
	/* Mean anomaly of moon = l */
	x = mod3600(1.7179159228846793e+09*T+4.8586817465825332e+05);
	x += (((((-1.755312760154e-012)*T+3.452144225877e-011*T-2.506365935364e-008)*T-2.536291235258e-004)*T+
		5.2099641302735818e-02)*T+3.1501359071894147e+01)*T2;
	Args[12] = STR*x;
	/* Mean longitude of moon,re mean ecliptic and equinox of date = L  */
	x = mod3600(1.7325643720442266e+09*T+7.8593980921052420e+05);
	x +=(((((7.200592540556e-014*T+2.235210987108e-010)*T-1.024222633731e-008)*T-6.073960534117e-005)*T+
		6.9017248528380490e-03)*T-5.6550460027471399e+00)*T2;
	LP_equinox = x;
	Args[13] = STR*x;
	/* Precession of the equinox  */
	x = (((((((((-8.66e-20*T-4.759e-17)*T+2.424e-15)*T+1.3095e-12)*T+1.7451e-10)*T-1.8055e-8)*T-0.0000235316)*T+
		0.000076)*T+1.105414)*T+5028.791959)*T;
	/* Moon's longitude re fixed J2000 equinox.  */
	/* Args[13] -= x; */
	pA_precession = STR*x;

	/* Free librations. longitudinal libration 2.891725 years */
	x = mod3600(4.48175409e7*T+8.060457e5);
	Args[14] = STR*x;
	/* libration P,24.2 years */
	x = mod3600(5.36486787e6*T-391702.8);
	Args[15] = STR*x;
	x = mod3600(1.73573e6*T);
	Args[17] = STR*x;
}


static double g1plan(double J,planet_table *plan) {
	int i,j,k,m,k1,ip,np,nt;
	char *p;
	long *pl;
	double su,cu,sv,cv;
	double T,t,sl;
	T =(J-J2000)/plan->timescale;
	mean_elements(J);
	for(i=0; i<NARGS; i++) if((j=plan->max_harmonic[i])>0) sscc(i,Args[i],j);
	p = plan->arg_tbl;
	pl = (long *)plan->lon_tbl;
	sl = 0.0;
	for(;;) {
		np = *p++;
		if(np<0) break;
		if(np==0) {
			nt = *p++,cu = *pl++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pl++;
			sl += cu;
			continue;
		}
		k1 = 0,cv = 0.0,sv = 0.0;
		for(ip=0; ip<np; ip++) {
			j = *p++;
			m = *p++-1;
			if(j) {
				if(j<0) k = -j;
				else k = j;
				k -= 1;
				su = ss[m][k];
				if(j<0) su = -su;
				cu = cc[m][k];
				if(k1==0) sv = su,cv = cu,k1 = 1;
				else t = su*cv+cu*sv,cv = cu*cv-su*sv,sv = t;
			}
		}
		nt = *p++;
		cu = *pl++,su = *pl++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pl++,su = su*T+*pl++;
		sl += cu*cv+su*sv;
	}
	return(plan->trunclvl*sl);
}

static void g2plan(double J,planet_table *plan,double *pobj) {
	int i,j,k,m,n,k1,ip,np,nt;
	char *p;
	long *pl,*pr;
	double su,cu,sv,cv;
	double T,t,sl,sr;
	mean_elements(J);
	T =(J-J2000)/plan->timescale;
	n = plan->maxargs;
	for(i=0; i<n; i++) if((j=plan->max_harmonic[i])>0) sscc(i,Args[i],j);
	p = plan->arg_tbl;
	pl = (long *)plan->lon_tbl;
	pr = (long *)plan->rad_tbl;
	sl = 0.0,sr = 0.0;
	for(;;) {
		np = *p++;
		if(np<0) break;
		if(np==0) {
			nt = *p++;
			cu = *pl++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pl++;
			sl += cu;
			cu = *pr++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pr++;
			sr += cu;
			continue;
		}
		k1 = 0,cv = 0.0,sv = 0.0;
		for(ip=0; ip<np; ip++) {
			j = *p++;
			m = *p++-1;
			if(j) {
				if(j<0) k = -j;
				else k = j;
				k -= 1,su = ss[m][k];
				if(j<0) su = -su;
				cu = cc[m][k];
				if(k1==0) sv = su,cv = cu,k1 = 1;
				else t = su*cv+cu*sv,cv = cu*cv-su*sv,sv = t;
			}
		}
		nt = *p++;
		cu = *pl++,su = *pl++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pl++,su = su*T+*pl++;
		sl += cu*cv+su*sv;
		cu = *pr++,su = *pr++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pr++,su = su*T+*pr++;
		sr += cu*cv+su*sv;
	}
	t = plan->trunclvl;
	pobj[0] = t*sl;
	pobj[2] = t*sr;
}

static void g3plan(double J,planet_table *plan,double pobj[],int num) {
	int i,j,k,m,n,k1,ip,np,nt;
	char *p;
	long *pl,*pb,*pr;
	double su,cu,sv,cv;
	double T,t,sl,sb,sr;
	mean_elements(J);
	T =(J-J2000)/plan->timescale,n = plan->maxargs;
	for(i=0; i<n; i++) if((j = plan->max_harmonic[i])>0) sscc(i,Args[i],j);
	p = plan->arg_tbl;
	pl = (long *)plan->lon_tbl;
	pb = (long *)plan->lat_tbl;
	pr = (long *)plan->rad_tbl;
	sl = 0.0,sb = 0.0,sr = 0.0;
	for(;;) {
		np = *p++;
		if(np<0) break;
		if(np==0) {
			nt = *p++;
			cu = *pl++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pl++;
			sl += cu;
			cu = *pb++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pb++;
			sb += cu;
			cu = *pr++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pr++;
			sr += cu;
			continue;
		}
		k1 = 0,cv = 0.0,sv = 0.0;
		for(ip=0; ip<np; ip++) {
			j = *p++;
			m = *p++-1;
			if(j) {
				if(j<0) k = -j;
				else k = j;
				k -= 1,su = ss[m][k];
				if(j<0) su = -su;
				cu = cc[m][k];
				if(k1==0) sv = su,cv = cu,k1 = 1;
				else t = su*cv+cu*sv,cv = cu*cv-su*sv,sv = t;
			}
		}
		nt = *p++;
		cu = *pl++,su = *pl++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pl++,su = su*T+*pl++;
		sl += cu*cv+su*sv;
		cu = *pb++,su = *pb++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pb++,su = su*T+*pb++;
		sb += cu*cv+su*sv;
		cu = *pr++,su = *pr++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pr++,su = su*T+*pr++;
		sr += cu*cv+su*sv;
	}
	t = plan->trunclvl;
	pobj[0] = Args[num-1]+STR*t*sl;
	pobj[1] = STR*t*sb;
	pobj[2] = plan->distance *(1.0+STR*t*sr);
}

extern planet_table moonlr,moonlat;

static void gmoon(astro_data *ad,double J,double rect[],double pol[]) {
	double x,cosB,sinB,cosL,sinL;
	g2plan(J,&moonlr,pol);
	x = pol[0];
	x += LP_equinox;
	if(x<-6.45e5) x += 1.296e6;
	if(x>6.45e5) x -= 1.296e6;
	pol[0] = STR*x;
	x = g1plan(J,&moonlat);
	pol[1] = STR*x;
	x =(1.0+STR*pol[2])*moonlr.distance;
	pol[2] = x;
	epsiln(ad,J);
	cosB = cos(pol[1]);
	sinB = sin(pol[1]);
	cosL = cos(pol[0]);
	sinL = sin(pol[0]);
	rect[0] = cosB*cosL*x;
	rect[1] =(ad->coseps*cosB*sinL-ad->sineps*sinB)*x;
	rect[2] =(ad->sineps*cosB*sinL+ad->coseps*sinB)*x;
}

static void embofs(astro_data *ad,double J,double ea[],double *pr) {
	double pm[3],polm[3],a1,a2;
	int i;
	gmoon(ad,J,pm,polm);
	precess(ad,pm,J,1);
	a1 = 1.0 /(emrat+ 1.0),a2 = 0.0;
	for(i=0; i<3; i++) ea[i] = ea[i]-a1*pm[i],a2 = a2+ea[i]*ea[i];
	*pr = sqrt(a2);
}

static void gplan(double J,planet_table *plan,double pobj[]) {
	register double su,cu,sv,cv,T;
	double t,sl,sb,sr;
	int i,j,k,m,n,k1,ip,np,nt;
	char *p;
	double *pl;
	double *pb;
	double *pr;
	T = (J-J2000)/plan->timescale,n = plan->maxargs;
	for(i=0; i<n; i++) {
		if((j=plan->max_harmonic[i])>0) {
			sr =(mod3600(freqs[i]*T)+phases[i])*STR;
			sscc(i,sr,j);
		}
	}
	p = plan->arg_tbl;
	pl =( double *)plan->lon_tbl;
	pb = (double *)plan->lat_tbl;
	pr = (double *)plan->rad_tbl;
	sl = 0.0,sb = 0.0,sr = 0.0;
	for(;;) {
		np = *p++;
		if(np<0) break;
		if(np==0) {
			nt = *p++;
			cu = *pl++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pl++;
			sl +=  mod3600(cu);
			cu = *pb++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pb++;
			sb += cu;
			cu = *pr++;
			for(ip=0; ip<nt; ip++) cu = cu*T+*pr++;
			sr += cu;
			continue;
		}
		k1 = 0,cv = 0.0,sv = 0.0;
		for(ip=0; ip<np; ip++) {
			j = *p++;
			m = *p++-1;
			if(j) {
				k = j;
				if(j<0) k = -k;
				k -= 1;
				su = ss[m][k];
				if(j<0) su = -su;
				cu = cc[m][k];
				if(k1==0) sv = su,cv = cu,k1 = 1;
				else t = su*cv+cu*sv,cv = cu*cv-su*sv,sv = t;
			}
		}
		nt = *p++;
		cu = *pl++,su = *pl++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pl++,su = su*T+*pl++;
		sl += cu*cv+su*sv;
		cu = *pb++,su = *pb++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pb++,su = su*T+*pb++;
		sb += cu*cv+su*sv;
		cu = *pr++,su = *pr++;
		for(ip=0; ip<nt; ip++) cu = cu*T+*pr++,su = su*T+*pr++;
		sr += cu*cv+su*sv;
	}
	pobj[0] = STR*sl;
	pobj[1] = STR*sb;
	pobj[2] = STR*plan->distance*sr+plan->distance;
}

static void kepler(astro_data *ad,double J,astro_orbit *e,double rect[],double polar[]) {
	double alat,E,M,W,temp;
	double epoch,inclination,ascnode,argperih;
	double meandistance,dailymotion,eccent,meananomaly;
	double r,coso,sino,cosa;
	if(e->ptable) {
		if(e==&astro_earth) g3plan(J,(planet_table *)e->ptable,polar,3);
		else gplan(J,(planet_table *)e->ptable,polar);
		E = polar[0];
		e->L = E;
		W = polar[1];
		r = polar[2];
		e->r = r;
		e->epoch = J;
		e->equinox = J2000;
	} else {
		epoch = e->epoch;
		inclination = e->i;
		ascnode = e->W*DTR;
		argperih = e->w;
		meandistance = e->a;
		dailymotion = e->dm;
		eccent = e->ecc;
		meananomaly = e->M;
		if(eccent==1.0) {
			temp = meandistance*sqrt(meandistance);
			W =(J-epoch)*0.0364911624/temp;
			E = 0.0,M = 1.0;
			while(fabs(M)>1.0e-11) {
				temp = E*E;
				temp =(2.0*E*temp+W)/(3.0 *(1.0+temp));
				M = temp-E;
				if(temp != 0.0) M /= temp;
				E = temp;
			}
			r = meandistance*(1.0+E*E),M = atan(E),M = 2.0*M,alat = M+DTR*argperih;
		} else if(eccent>1.0) {
			meandistance = meandistance/(eccent-1.0);
			temp = meandistance*sqrt(meandistance);
			W =(J-epoch)*0.01720209895/temp;
			E = W/(eccent-1.0),M = 1.0;
			while(fabs(M)>1.0e-11) M = -E+eccent*sinh(E)-W,E += M/(1.0-eccent*cosh(E));
			r = meandistance *(-1.0+eccent*cosh(E)),temp =(eccent+1.0)/(eccent-1.0);
			M = sqrt(temp)*tanh(0.5*E),M = 2.0*atan(M),alat = M+DTR*argperih;
		} else {
			if(dailymotion==0.0) dailymotion = 0.9856076686/(e->a*sqrt(e->a));
			dailymotion *= J-epoch;
			M = DTR*(meananomaly+dailymotion),M = modtp(M);
			if(e->L) e->L += dailymotion,e->L = mod360(e->L);
			E = M;
			temp = 1.0;
			do {
				temp = E-eccent*sin(E)-M;
				E -= temp/(1.0-eccent*cos(E));
			} while(fabs(temp)>1.0e-11);
			temp = sqrt((1.0+eccent)/(1.0-eccent));
			W = 2.0*atan(temp*tan(0.5*E));
			W = modtp(W);
			meananomaly *= DTR;
			if(e->L) alat =(e->L)*DTR+W-meananomaly-ascnode;
			else alat = W+DTR*argperih;
			r = meandistance*(1.0-eccent*eccent)/(1.0+eccent*cos(W));
		}
		coso = cos(alat),sino = sin(alat),inclination *= DTR,W = sino*cos(inclination),E = zatan2(coso,W)+ascnode;
		W = sino*sin(inclination),W = asin(W);
	}
	rect[2] = r*sin(W),cosa = cos(W),rect[1] = r*cosa*sin(E),rect[0] = r*cosa*cos(E);
	epsiln(ad,e->equinox);
	W = ad->coseps*rect[1]-ad->sineps*rect[2];
	M = ad->sineps*rect[1]+ad->coseps*rect[2];
	rect[1] = W;
	rect[2] = M;
	precess(ad,rect,e->equinox,1);
	if(e==&astro_earth) embofs(ad,J,rect,&r);
	epsiln(ad,J2000);
	W = ad->coseps*rect[1]+ad->sineps*rect[2];
	M = -ad->sineps*rect[1]+ad->coseps*rect[2];
	E = zatan2(rect[0],W),W = asin(M/r);
	polar[0] = E;
	polar[1] = W;
	polar[2] = r;
}


static void velearth(astro_data *ad,double J) {
	if(J!=ad->jvearth) {
		double e[3],p[3],t;
		int i;
		ad->jvearth = J;
		t = 0.005;
		kepler(ad,ad->TDT-t,&astro_earth,e,p);
		for(i=0; i<3; i++) ad->vearth[i] =(ad->rearth[i]-e[i])/t;
	}
}

static void annuab(astro_data *ad,double p[]) {
	double A,B,C;
	double betai,pV;
	double x[3],V[3],dp[3];
	int i;
	velearth(ad,ad->TDT);
	betai = 0.0;
	pV = 0.0;
	for(i=0; i<3; i++) A = ad->vearth[i]/Clightaud,V[i] = A,betai += A*A,pV += p[i]*A;
	betai = sqrt(1.0-betai),C = 1.0+pV,A = betai/C,B =(1.0 + pV/(1.0+betai))/C;
	for(i=0; i<3; i++) C = A*p[i] + B*V[i],x[i] = C,dp[i] = C-p[i];
	for(i=0; i<3; i++) p[i] = x[i];
}

static void relativity(astro_data *ad,double p[],double q[],double e[]) {
	double C = 1.974e-8/(ad->SE*(1.0+ad->qe)),dp[3];
	int i;
	for(i=0; i<3; i++) dp[i] = C*(ad->pq*e[i]/ad->SE-ad->ep*q[i]/ad->SO),p[i] += dp[i];
}

static void lonlat(astro_data *ad,double pp[],double J,double polar[],int ofdate) {
	double s[3],x,y,z,yy,zz,r;
	int i;
	r = 0.0;
	for(i=0; i<3; i++) x = pp[i],s[i] = x,r += x*x;
	r = sqrt(r);
	if(ofdate) precess(ad,s,J,-1);
	epsiln(ad,J);
	yy = s[1],zz = s[2],x  = s[0];
	y = ad->coseps*yy+ad->sineps*zz,z = -ad->sineps*yy+ad->coseps*zz;
	yy = zatan2(x,y),zz = asin(z/r);
	polar[0] = yy,polar[1] = zz,polar[2] = r;
	if(ad->data&ASTRONOMY_GLON) ad->wrpl->glon = yy*RTD;
	if(ad->data&ASTRONOMY_LAT) ad->wrpl->lat = zz*RTD;
}

static void angles(astro_data *ad,double p[],double q[],double e[]) {
	double a1,a2,s;
	int i;
	ad->EO = 0.0,ad->SE = 0.0,ad->SO = 0.0,ad->pq = 0.0,ad->ep = 0.0,ad->qe = 0.0;
	for(i=0; i<3; i++) {
		a1 = e[i],a2 = q[i],s = p[i];
		ad->EO += s*s,ad->SE += a1*a1,ad->SO += a2*a2;
		ad->pq += s*a2,ad->ep += a1*s,ad->qe += a2*a1;
	}
	ad->EO = sqrt(ad->EO);
	ad->SO = sqrt(ad->SO);
	ad->SE = sqrt(ad->SE);
	if(ad->SO>1.0e-12) {
		ad->pq /= ad->EO*ad->SO;
		ad->qe /= ad->SO*ad->SE;
	}
	ad->ep /= ad->SE*ad->EO;
}



static void lightt(astro_data *ad,astro_orbit *elemnt,double q[],double e[]) {
	double p[3],p0[3],ptemp[3],dp[3];
	double P,Q,E,t,x,y;
	int i,k;
	for(i=0; i<3; i++) p0[i] = q[i]-e[i];
	E = 0.0;
	for(i=0; i<3; i++) E += e[i]*e[i];
	E = sqrt(E);
	for(k=0; k<2; k++) {
		P = 0.0,Q = 0.0;
		for(i=0; i<3; i++) y = q[i],x = y-e[i],p[i] = x,Q += y*y,P += x*x;
		P = sqrt(P),Q = sqrt(Q);
		t =(P+1.97e-8*log((E+P+Q)/(E-P+Q)))/173.1446327;
		kepler(ad,ad->TDT-t,elemnt,q,ptemp);
	}
	for(i=0; i<3; i++) x = q[i]-e[i],p[i] = x,dp[i] = x-p0[i];
	velearth(ad,ad->TDT);
	for(i=0; i<3; i++) p[i] += ad->vearth[i]*t;
	deltap(p,p0,&ad->dradt,&ad->ddecdt);
	ad->dradt /= t;
	ad->ddecdt /= t;
}

static void nutlo(astro_data *ad,double J) {
	if(ad->jdnut!=J) {
		double f,g,T,T2,T10;
		double MM,MS,FF,DD,OM;
		double cu,su,cv,sv,sw;
		double C,D;
		int i,j,k,k1,m;
		const short *p;
		ad->jdnut = J;
		T =(J-2451545.0)/36525.0;
		T2 = T*T;
		T10 = T/10.0;
		OM =(mod3600(-6962890.539*T+450160.280) +(0.008*T+7.455)*T2)*STR;
		MS =(mod3600(129596581.224*T+1287099.804) -(0.012*T+0.577)*T2)*STR;
		MM =(mod3600(1717915922.633*T+485866.733) +(0.064*T+31.310)*T2)*STR;
		FF =(mod3600(1739527263.137*T+335778.877) +(0.011*T-13.257)*T2)*STR;
		DD =(mod3600(1602961601.328*T+1072261.307) +(0.019*T-6.891)*T2)*STR;
		sscc(0,MM,3);
		sscc(1,MS,2);
		sscc(2,FF,4);
		sscc(3,DD,4);
		sscc(4,OM,2);
		C = 0.0,D = 0.0,p = &nutation_table[0];
		for(i=0; i<105; i++) {
			k1 = 0,cv = 0.0,sv = 0.0;
			for(m=0; m<5; m++) {
				j = *p++;
				if(j) {
					k = j;
					if(j<0) k = -k;
					su = ss[m][k-1];
					if(j<0) su = -su;
					cu = cc[m][k-1];
					if(k1==0) sv = su,cv = cu,k1 = 1;
					else sw = su*cv+cu*sv,cv = cu*cv-su*sv,sv = sw;
				}
			}
			f  = *p++;
			if((k=*p++)!=0) f += T10*k;
			g = *p++;
			if((k=*p++)!=0) g += T10*k;
			C += f*sv,D += g*cv;
		}
		C += (-1742.*T10-171996.)*ss[4][0];
		D += (89.*T10+ 92025.)*cc[4][0];
		ad->nutl = 0.0001*STR*C;
		ad->nuto = 0.0001*STR*D;
	}
}

static void nutate(astro_data *ad,double J,double p[]) {
	double ce,se,cl,sl,sino,f;
	double dp[3],p1[3];
	int i;
	nutlo(ad,J);
	epsiln(ad,J);
	f = ad->eps+ad->nuto,ce = cos(f),se = sin(f),sino = sin(ad->nuto),cl = cos(ad->nutl),sl = sin(ad->nutl);
	p1[0] = cl*p[0]-sl*ad->coseps*p[1]-sl*ad->sineps*p[2];
	p1[1] = sl*ce*p[0]+(cl*ad->coseps*ce+ad->sineps*se)*p[1]-(sino +(1.0-cl)*ad->sineps*ce)*p[2];
	p1[2] = sl*se*p[0]+(sino +(cl-1.0)*se*ad->coseps)*p[1]+(cl*ad->sineps*se+ad->coseps*ce)*p[2];
	for(i=0; i<3; i++) dp[i] = p1[i]-p[i];
	for(i=0; i<3; i++) p[i] = p1[i];
}


static int whatconstel(astro_data *ad,double pp[],double epoch) {
	int i,k;
	double ra,dec,d;
	double p[3];
	for(i=0; i<3; i++) p[i] = pp[i];
	precess(ad,p,epoch,1);
	precess(ad,p,2405889.25855,-1);
	d = p[0]*p[0]+p[1]*p[1]+p[2]*p[2];
	d = sqrt(d);
	ra = atan2(p[1],p[0])*(RTD*3600./15.);
	if(ra<0.0) ra += 86400.0;
	dec = asin(p[2]/d)*(RTD*3600.);
	for(i=0; i<NBNDRIES; i++) {
		k = i<<2;
		if(ra>=bndries[k] && ra<bndries[k+1] && dec>bndries[k+2]) return bndries[k+3];
	}
	return -1;
}


static void calc_sun(astro_data *ad) {
	double r,x,y,t;
	double ecr[3],rec[3],pol[3];
	int i;
	double modtp();
	for(i=0; i<3; i++) ecr[i] = -ad->rearth[i];
	r = ad->eapolar[2];
	pol[2] = r;
	for(i=0; i<2; i++) {
		t = pol[2]/173.1446327;
		kepler(ad,ad->TDT-t,&astro_earth,ecr,pol);
	}
	r = pol[2];
	for(i=0; i<3; i++) x = -ecr[i],y = -ad->rearth[i],ecr[i] = x,rec[i] = y,pol[i] = y-x;
	deltap(ecr,rec,&ad->dradt,&ad->ddecdt);
	ad->dradt /= t;
	ad->ddecdt /= t;
	precess(ad,ecr,ad->TDT,-1);
	for(i=0; i<3; i++) rec[i] = ecr[i];
	epsiln(ad,ad->TDT);
	nutate(ad,ad->TDT,ecr);
	if(ad->data&ASTRONOMY_CONSTELLATION) ad->wrpl->constel = whatconstel(ad,ecr,ad->TDT);
	y = ad->coseps*rec[1]+ad->sineps*rec[2];
	y = zatan2(rec[0],y)+ad->nutl;
	if(ad->data&ASTRONOMY_GLON) ad->wrpl->glon = RTD*y;
	if(ad->data&ASTRONOMY_LAT) ad->wrpl->lat = 0.0;
	if(ad->data&ASTRONOMY_HLON) ad->wrpl->hlon = mod360((RTD*y)+180);
	//altaz(pol,UT);
}


static void moonll(astro_data *ad,double J,double rect[],double pol[]) {
	double cosB,sinB,cosL,sinL,y,z;
	double qq[3],pp[3];
	int i;
	epsiln(ad,J);
	gmoon(ad,J,rect,pol);
	ad->obpolar[0] = pol[0];
	ad->obpolar[1] = pol[1];
	ad->obpolar[2] = pol[2];
	pol[0] -= 0.0118*DTR*Rearth/pol[2];
	cosB = cos(pol[1]);
	sinB = sin(pol[1]);
	cosL = cos(pol[0]);
	sinL = sin(pol[0]);
	rect[0] = cosB*cosL;
	rect[1] = ad->coseps*cosB*sinL-ad->sineps*sinB;
	rect[2] = ad->sineps*cosB*sinL+ad->coseps*sinB;
	precess(ad,rect,ad->TDT,1);
	for(i=0; i<3; i++) pp[i] = rect[i]*pol[2],qq[i] = ad->rearth[i]+pp[i];
	angles(ad,pp,qq,ad->rearth);
	precess(ad,rect,ad->TDT,-1);
	nutate(ad,ad->TDT,rect);
	ad->ra = zatan2(rect[0],rect[1]);
	ad->dec = asin(rect[2]);
	cosL = cos(ad->eps+ad->nuto);
	sinL  = sin(ad->eps+ad->nuto);
	y = cosL*rect[1]+sinL*rect[2];
	z = -sinL*rect[1]+cosL*rect[2];
	pol[0] = zatan2(rect[0],y);
	pol[1] = asin(z);
	for(i=0; i<3; i++) rect[i] *= ad->EO;
}

static void calc_moon(astro_data *ad) {
	int i;
	double ra0,dec0;
	double x,/*y,*/z,lon0;
	double /*pp[3],*/qq[3],pe[3],re[3],moonpp[3],moonpol[3];
	for(i=0; i<3; i++) re[i] = ad->rearth[i];
	moonll(ad,ad->TDT-0.001,moonpp,moonpol);
	ra0 = ad->ra;
	dec0 = ad->dec;
	lon0 = moonpol[0];
	moonll(ad,ad->TDT,moonpp,moonpol);
	ad->dradt = ad->ra-ra0;
	if(ad->dradt>=PI) ad->dradt = ad->dradt-PIx2;
	if(ad->dradt<=-PI) ad->dradt = ad->dradt+PIx2;
	ad->dradt = 1000.0*ad->dradt;
	ad->ddecdt = 1000.0*(ad->dec-dec0);
	lon0 = 1000.0*RTD*(moonpol[0]-lon0);
	z = re[0]*re[0]+re[1]*re[1]+re[2]*re[2];
	z = sqrt(z);
	for(i=0; i<3; i++) re[i] /= z;
	annuab(ad,re);
	precess(ad,re,ad->TDT,-1);
	nutate(ad,ad->TDT,re);
	for(i=0; i<3; i++) re[i] *= z;
	lonlat(ad,re,ad->TDT,pe,0);
	for(i=0; i<3; i++) qq[i] = re[i]+moonpp[i];
	angles(ad,moonpp,qq,re);
	if(ad->data&ASTRONOMY_GLON) ad->wrpl->glon = RTD*moonpol[0];
	if(ad->data&ASTRONOMY_HLON) ad->wrpl->hlon = 0.0;
	if(ad->data&ASTRONOMY_LAT) ad->wrpl->lat = RTD*moonpol[1];
	if(ad->data&ASTRONOMY_CONSTELLATION) ad->wrpl->constel = -1;
	if(ad->data&ASTRONOMY_MOON_PHASE) {
		x = Rearth/moonpol[2];
		x = 0.272453*x+0.0799/RTS;
		x = RTD*acos(-ad->ep);
		x = 0.5*(1.0+ad->pq);
		x = moonpol[0]-pe[0];
		x = modtp(x)*RTD;
		i =(int)(x/90);
		ad->wrpl->phase = x;
		/*x =(x-i*90.0);
		z = moonpol[2]/(12.3685*0.00257357);
		if(x>45.0) {
			y = -(x-90.0)*z;
			i =(i+1)&3;
		} else {
			y = x*z;
		}*/
	}
	/*pp[0] = ad->ra;
	pp[1] = ad->dec;
	pp[2] = moonpol[2];
	altaz(pp,UT);*/
}


static void calc_planet(astro_data *ad) {
	double p[3],temp[3],polar[3];
	int i,data = ad->data;
	kepler(ad,ad->TDT,ad->elobj,ad->robject,ad->obpolar);
	for(i=0; i<3; i++) temp[i] = ad->robject[i];

	ad->data |= ASTRONOMY_GLON;
	lonlat(ad,ad->robject,ad->TDT,polar,1);
	if(ad->data&ASTRONOMY_HLON) ad->wrpl->hlon = ad->wrpl->glon;
	ad->wrpl->glon = 0.0,ad->data = data;

	lightt(ad,ad->elobj,ad->robject,ad->rearth);
	for(i=0; i<3; i++) p[i] = ad->robject[i]-ad->rearth[i];
	angles(ad,p,ad->robject,ad->rearth);
	for(i=0; i<3; i++) p[i] /= ad->EO,temp[i] = p[i];
	relativity(ad,p,ad->robject,ad->rearth);
	annuab(ad,p);
	precess(ad,p,ad->TDT,-1);
	epsiln(ad,ad->TDT);
	nutate(ad,ad->TDT,p);
	if(ad->data&ASTRONOMY_CONSTELLATION) ad->wrpl->constel = whatconstel(ad,p,ad->TDT);
	for(i=0; i<3; i++) p[i] *= ad->EO;
	lonlat(ad,p,ad->TDT,temp,0);
	/*polar[2] = ad->EO;
	altaz(polar,UT);*/
}

static void calc_node(astro_data *ad) {
	double t = (ad->TDT-2451545)/36525,t2 = t*t,t3 = t2*t,t4 = t3*t;
	double n = mod360(125.0445479-1934.1362891*t+0.0020754*t2+t3/467441-t4/60616000);
	if(ad->wrpl->id==ASTRO_TNNODE || ad->wrpl->id==ASTRO_TSNODE) {
		double D = mod360(297.8501921+445267.1114034*t-0.0018819*t2+t3/545868-t4/113065000)*DTR;
		double M = mod360(357.5291092+35999.0502909*t-0.0001536*t2+t3/24490000)*DTR;
		double Mdash = mod360(134.9633964+477198.8675055*t+0.0087414*t2+t3/69699-t4/14712000)*DTR;
		double F = mod360(93.2720950+483202.0175233*t-0.0036539*t2-t3/3526000+t4/863310000)*DTR;
		n = mod360(n-1.4979*sin(2*(D-F))-0.1500*sin(M)-0.1226*sin(2*D)+0.1176*sin(2*F)-0.0801*sin(2*(Mdash-F)));
	}
	if(ad->data&ASTRONOMY_GLON) ad->wrpl->glon = n;
}

static void calc_star(astro_data *ad) {
	astro_star *el = (astro_star *)ad->elobj;
	double p[3],q[3],e[3],m[3],temp[3]/*,polar[3]*/;
	double T,vpi,epoch;
	double cosdec,sindec,cosra,sinra;
	int i;
	while(1) {
		cosdec = cos(el->dec),sindec = sin(el->dec);
		cosra = cos(el->ra),sinra = sin(el->ra);
		q[0] = cosra*cosdec,q[1] = sinra*cosdec,q[2] = sindec;
		vpi = 21.094952663*el->v*el->px;
		m[0] = -el->mura*cosdec*sinra-el->mudec*sindec*cosra+vpi*q[0];
		m[1] = el->mura*cosdec*cosra-el->mudec*sindec*sinra+vpi*q[1];
		m[2] = el->mudec*cosdec+vpi*q[2];
		epoch = el->epoch;
		if(epoch==B1950) fk4_to_fk5(q,m,el);
		else break;
	}
	for(i=0; i<3; i++) e[i] = ad->rearth[i];
	precess(ad,e,epoch,-1);
	T =(ad->TDT-epoch)/36525.0;
	for(i=0; i<3; i++) p[i] = q[i]+T*m[i]-el->px*e[i];
	precess(ad,p,epoch,1);
	for(i=0; i<3; i++) e[i] = ad->rearth[i];
	angles(ad,p,p,e);
	for(i=0; i<3; i++) p[i] /= ad->EO,temp[i] = p[i];
	precess(ad,temp,B1950,-1);
	for(i=0; i<3; i++) temp[i] = p[i];
	precess(ad,temp,ad->TDT,-1);
	relativity(ad,p,p,e);
	annuab(ad,p);
	precess(ad,p,ad->TDT,-1);
	epsiln(ad,ad->TDT);
	nutate(ad,ad->TDT,p);
	ad->dradt = 0.0;
	ad->ddecdt = 0.0;
	/*polar[2] = 1.0e38;
	altaz(polar,UT);*/
}

static int calc_object(astro_data *ad,astronomy_planet *pl) {
	int e = 0;
	switch(pl->id&0xff) {
		case ASTRO_SUN:ad->elobj = 0;break;
		case ASTRO_MOON:ad->elobj = 0;break;
		case ASTRO_MERCURY:ad->elobj = &astro_mercury;break;
		case ASTRO_VENUS:ad->elobj = &astro_venus;break;
		case ASTRO_MARS:ad->elobj = &astro_mars;break;
		case ASTRO_JUPITER:ad->elobj = &astro_jupiter;break;
		case ASTRO_SATURN:ad->elobj = &astro_saturn;break;
		case ASTRO_URANUS:ad->elobj = &astro_uranus;break;
		case ASTRO_NEPTUNE:ad->elobj = &astro_neptune;break;
		case ASTRO_PLUTO:ad->elobj = &astro_pluto;break;
		case ASTRO_NNODE:break;
		case ASTRO_TNNODE:break;
		case ASTRO_SNODE:break;
		case ASTRO_TSNODE:break;
		case ASTRO_CHIRON:ad->elobj = &ad->forbit,e = 2060;break;
		case ASTRO_CERES:ad->elobj = &ad->forbit,e = 1;break;
		case ASTRO_PALLAS:ad->elobj = &ad->forbit,e = 2;break;
		case ASTRO_JUNO:ad->elobj = &ad->forbit,e = 3;break;
		case ASTRO_VESTA:ad->elobj = &ad->forbit,e = 4;break;
		case ASTRO_MINOR_PLANET:ad->elobj = &ad->forbit,e = pl->id>>8;break; // Minor Planet
		case ASTRO_FIXED_STAR:ad->elobj = (astro_orbit *)&ad->fstar,e = pl->id>>8;break; // Fixed Star
		default:return 0;
	}
	if((ad->elobj==&ad->forbit && !get_orbit(ad,ad->elobj,e)) ||
		(ad->elobj==(astro_orbit *)&ad->fstar && !get_star(ad,(astro_star *)ad->elobj,e))) return 0;
	ad->wrpl = pl;
	switch(pl->id&0xff) {
		case ASTRO_SUN:calc_sun(ad);break;
		case ASTRO_MOON:calc_moon(ad);break;
		default:calc_planet(ad);break;
		case ASTRO_NNODE:
		case ASTRO_TNNODE:calc_node(ad);break;
		case ASTRO_SNODE:
		case ASTRO_TSNODE:
			calc_node(ad);
			if(ad->data&ASTRONOMY_GLON) ad->wrpl->glon = mod360(ad->wrpl->glon+180);
			if(ad->data&ASTRONOMY_LAT) ad->wrpl->lat = -ad->wrpl->lat;
			break;
		case ASTRO_FIXED_STAR:calc_star(ad);break;
	}
	return 1;
}

static void calc(astro_data *ad,double jd,astronomy_planet *pl,int npl) {
	int i;
	ad->JD = jd;
	ad->TDT = 0.0;
	ad->UT = 0.0;
	ad->elobj = NULL;
	ad->wrpl = NULL;
	for(i=0; i<3; i++) ad->robject[i] = 0,ad->obpolar[i] = 0,ad->rearth[i] = 0,ad->eapolar[i] = 0,ad->vearth[i] = 0.0;
	ad->jdnut = -1.0;
	ad->jdeps = -1.0;
	ad->eps = 0.0;
	ad->coseps = 0.0;
	ad->sineps = 0.0;
	ad->jvearth = -1.0;
	if(ad->data&ASTRONOMY_SPEED) {
		int n,data = ad->data|ASTRONOMY_GLON;
		double sp[npl][3],t = 0.03125;
		astronomy_planet obj = { 0,0.0,0.0,0.0,0.0,0.0,0,0.0 };
		for(n=0,ad->JD=jd-t; n<3; n++,ad->JD+=t) {
			update(ad,1);
			kepler(ad,ad->TDT,&astro_earth,ad->rearth,ad->eapolar);
			if(n==1) {
				ad->data = data;
				for(i=0; i<npl; i++)
					if(calc_object(ad,&pl[i])) sp[i][n] = pl[i].glon;
			} else {
				ad->data = ASTRONOMY_GLON;
				for(i=0; i<npl; i++) {
					obj.id = pl[i].id;
					if(calc_object(ad,&obj)) sp[i][n] = obj.glon;
				}
			}
		}
		for(i=0; i<npl; i++) pl[i].speed = (sp[i][2]-sp[i][0])/(t*2);
	} else {
		update(ad,1);
		kepler(ad,ad->TDT,&astro_earth,ad->rearth,ad->eapolar);
		for(i=0; i<npl; i++) calc_object(ad,&pl[i]);
	}
	ad->JD = jd;
	update(ad,1);
}


static double sidrlt(astro_data *ad,double j,double tlong) {
	double jd0,secs,eqeq,gmst,jd,T0,msday;
	jd = j;
	jd0 = floor(jd);
	secs = j-jd0;
	if(secs<0.5) jd0 -= 0.5,secs += 0.5;
	else jd0 += 0.5,secs -= 0.5;
	secs *= 86400.0;
	T0 =(jd0-J2000)/36525.0;
	nutlo(ad,ad->TDT);
	epsiln(ad,ad->TDT);
	eqeq = 240.0*RTD*ad->nutl*ad->coseps;
	gmst = (((-2.0e-6*T0-3.e-7)*T0+9.27701e-2)*T0+8640184.7942063)*T0+24110.54841;
	msday = (((-(4.*2.0e-6)*T0 -(3.*3.e-7))*T0 +(2.*9.27701e-2))*T0+8640184.7942063)/(86400.*36525.)+1.0;
	gmst = gmst+msday*secs+eqeq+240.0*tlong;
	gmst = gmst-86400.0*floor(gmst/86400.0);
	return gmst;
}

astronomy *astronomy_new(const calendar *gmt,double lon,double lat) {
	astronomy *a;
	if(gmt==NULL) return NULL;
	a = (astronomy *)malloc(sizeof(astronomy));
	memset(a,0,sizeof(astronomy));
	memcpy(&a->gmt,gmt,sizeof(calendar));
	a->lon = lon;
	a->lat = lat;
	return a;
}

void astronomy_delete(astronomy *a) {
	if(a==NULL) return;
	if(a->planets!=NULL) free(a->planets);
	free(a);
}

void astronomy_get_planets(astronomy *a,const int *pl,int data) {
	int i;
	astro_data ad;
	if(a==NULL || pl==NULL) return;
	for(i=0; pl[i]!=-1; i++);
	if(i==0) return;
	if(i!=a->nplanets) {
		if(a->planets!=NULL) free(a->planets);
		a->nplanets = i;
		a->planets = (astronomy_planet *)malloc(sizeof(astronomy_planet)*a->nplanets);
	}
	memset(a->planets,0,sizeof(astronomy_planet)*a->nplanets);
	for(i=0; i<a->nplanets; i++) a->planets[i].id = pl[i];
	a->deltat = deltat(calendar_fractional_year(&a->gmt));
	if(data==0) data = ASTRONOMY_ALL_DATA;
	ad.data = data;
	calc(&ad,a->gmt.jd+a->deltat/86400.0,a->planets,a->nplanets);
	a->sdrlt = sidrlt(&ad,a->gmt.jd,-a->lon)/86400.0;
}


const char *constellation_name(int id) {
	if(id<0 || id>=NCON) return NULL;
	return constel[id];
}

const char *constellation_abbreviation(int id) {
	if(id<0 || id>=NCON) return NULL;
	return constel_abbr[id];
}



