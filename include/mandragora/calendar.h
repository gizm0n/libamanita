#ifndef _MANDRAGORA_CALENDAR_H
#define _MANDRAGORA_CALENDAR_H


enum {	
	SUNDAY			= 0,
	MONDAY			= 1,
	TUESDAY			= 2,
	WEDNESDAY		= 3,
	THURSDAY			= 4,
	FRIDAY			= 5,
	SATURDAY			= 6
};


typedef struct calendar_date {
	int year;
	int month;
	int day;
} calendar_date;

typedef struct calendar {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	double second;
	double jd;
	int greg;
} calendar;

double calendar_get_julian_day(int y,int m,double d,int g);
int calendar_is_leap(int y,int g);

void calendar_set(calendar *c,int y,int m,int d,int h,int n,double s,int g);
void calendar_set_gmt(calendar *c,int y,int m,int d,int h,int n,double s,double tz,double dst,int g);
void calendar_set_julian_day(calendar *c,double jd,int g);

calendar_date calendar_julian_to_gregorian(int y,int m,int d);
calendar_date calendar_gregorian_to_julian(int y,int m,int d);
void calendar_doy_to_mad(int dy,int l,int *dm,int *m);

int calendar_day_of_week(calendar *c);
double calendar_day_of_year(calendar *c);
int calendar_days_in_month(calendar *c);
int calendar_days_in_year(calendar *c);
double calendar_fractional_year(calendar *c);


#endif /* _MANDRAGORA_CALENDAR_H */

