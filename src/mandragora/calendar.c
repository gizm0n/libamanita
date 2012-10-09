
#include <math.h>
#include <mandragora/calendar.h>
#include <mandragora/astro/astronomy.h>


static const int month_length[14] = { 31,28,31,30,31,30,31,31,30,31,30,31,0 };


static long INT(double value) {
	return (long)(value>=0.0? value : value-1.0);
}

double calendar_get_julian_day(int y,int m,double d,int g) {
	if(m<3) y = y-1,m = m+12;
	long a = 0,b = 0;
	if(g) a = INT(y/100.0),b = 2-a+INT(a/4.0);
	return INT(365.25*(y+4716))+INT(30.6001*(m+1))+d+b-1524.5;
}

int calendar_is_leap(int y,int g) {
	return g? ((y%100)==0? ((y%400)==0) : ((y%4)==0)) : ((y%4)==0);
}

static void adjust(calendar *c) {
	while(c->second>=60.0) c->second -= 60.0,c->minute++;
	while(c->second<0.0) c->second += 60.0,c->minute--;
	while(c->minute>=60) c->minute -= 60,c->hour += 1;
	while(c->minute<0) c->minute += 60,c->hour -= 1;
	while(c->hour>=24) c->hour -= 24,c->day++;
	while(c->hour<0) c->hour += 24,c->day--;
	while(c->month>12) c->month -= 12,c->year++;
	while(c->month<1) c->month += 12,c->year--;
	while(c->day>month_length[c->month]) c->day -= month_length[c->month],c->month++;
	while(c->day<1) c->day += month_length[c->month],c->month++;
	while(c->month>12) c->month -= 12,c->year++;
	while(c->month<1) c->month += 12,c->year--;
}

void calendar_set(calendar *c,int y,int m,int d,int h,int n,double s,int g) {
	c->year = y;
	c->month = m;
	c->day = d;
	c->hour = h;
	c->minute = n;
	c->second = s;
	c->jd = calendar_get_julian_day(y,m,d+(h/24.0)+(n/1440.0)+(s/86400.0),g);
	c->greg = g;
	adjust(c);
}

void calendar_set_gmt(calendar *c,int y,int m,int d,int h,int n,double s,double tz,double dst,int g) {
	double h1,n1;
	int n2;
	tz = -tz;
	h1 = ((double)h)+(tz<0? -(fabs(tz)+dst) : (tz-dst));
	h = (int)h1;
	if(h!=h1) {
		n1 = (h1-h)*60.0;
		n2 = n1;
		n += n2;
		s += (n1-((double)n2))*60.0;
	}
	c->year = y;
	c->month = m;
	c->day = d;
	c->hour = h;
	c->minute = n;
	c->second = s;
	c->jd = calendar_get_julian_day(y,m,d+(h/24.0)+(n/1440.0)+(s/86400.0),g);
	c->greg = g;
	adjust(c);
}

void calendar_set_julian_day(calendar *c,double jd,int g) {
	double tempZ,F,dblDay;
	long Z,A,B,C,D,E;
	c->jd = jd;
	c->greg = g;
	jd += 0.5;
	F = modf(jd,&tempZ);
	Z = (long)tempZ;
	A = Z;
	if(c->greg) {
		long alpha = INT((Z-1867216.25)/36524.25);
		A = Z+1+alpha-INT(alpha/4.0);
	}
	B = A+1524,C = INT((B-122.1)/365.25),D = INT(365.25*C),E = INT((B-D)/30.6001);
	dblDay = B-D-INT(30.6001*E)+F;
	c->day = (int)dblDay;
	c->month = E<14? E-1 : E-13;
	c->year = c->month>2? C-4716 : C-4715;
	F = modf(dblDay,&tempZ);
	c->hour = INT(F*24);
	c->minute = INT((F-(c->hour)/24.0)*1440.0);
	c->second = (F-(c->hour/24.0)-(c->minute/1440.0))*86400.0;
}


int calendar_day_of_week(calendar *c) {
	return (int)(((long)(c->jd+1.5))%7);
}

int calendar_days_in_month(calendar *c) {
	return c->month==2 && calendar_is_leap(c->year,c->greg)? month_length[1]+1 : month_length[c->month-1];
}

int calendar_days_in_year(calendar *c) {
	return calendar_is_leap(c->year,c->greg)? 366 : 365;
}

double calendar_day_of_year(calendar *c) {
	return c->jd-calendar_get_julian_day(c->year,1,1,c->greg)+1;
}

double calendar_fractional_year(calendar *c) {
	long d = calendar_days_in_year(c);
	return c->year+((c->jd-calendar_get_julian_day(c->year,1,1,c->greg))/d);
}

void calendar_doy_to_mad(int doy,int l,int *day,int *mon) {
	long K = l? 1 : 2,m;
	m = INT(9*(K+doy)/275.0+0.98);
	if(doy<32) m = 1;
	*mon = m;
	*day = doy-INT((275*m)/9.0)+(K*INT((m+9)/12.0))+30;
}

calendar_date calendar_julian_to_gregorian(int year,int month,int day) {
	long J,JD,alpha,beta,b,c,d,e;
	calendar julian;
	calendar_date gregorian;
	calendar_set(&julian,year,month,day,0,0,0,0);
	J = calendar_day_of_year(&julian);
	JD = INT(365.25*(year-1))+1721423+J;
	alpha = INT((JD-1867216.25)/36524.25);
	beta = JD+1+alpha-INT(alpha/4.0);
	if(JD<2299161) beta = JD;
	b = beta+1524;
	c = INT((b-122.1)/365.25);
	d = INT(365.25*c);
	e = INT((b-d)/30.6001);
	gregorian.day = b-d-INT(30.6001*e);
	if(e<14) gregorian.month = e-1;
	else gregorian.month = e-13;
	if(gregorian.month>2) gregorian.year = c-4716;
	else gregorian.year = c-4715;
	return gregorian;
}

calendar_date calendar_gregorian_to_julian(int year,int month,int day) {
	long alpha,beta,b,c,d,e;
	calendar_date julian;
	if(month<3) year--,month += 12;
	alpha = INT(year/100.0);
	beta = 2-alpha+INT(alpha/4.0);
	b = INT(365.25*year)+INT(30.6001*(month+1))+day+1722519+beta;
	c = INT((b-122.1)/365.25);
	d = INT(365.25*c);
	e = INT((b-d)/30.6001);
	julian.day = b-d-INT(30.6001*e);
	julian.month = e<14? e-1 : e-13;
	julian.year = julian.month>2? c-4716 : c-4715;
	return julian;
}

