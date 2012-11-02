
#include <stdlib.h>
#include <string.h>
#include <amanita/Colorcycle.h>
#include <amanita/String.h>


namespace a {


#define DEFAULT_VARIATION 5


uint32_t blend16a(uint32_t from,uint32_t to,uint32_t alpha) {
	if(alpha==0) return from;
	if(alpha==0x3ff) return to;
	register uint32_t r = (from>>11);
	register uint32_t g = ((from&0x07ff)>>5);
	register uint32_t b = (from&0x001f);
	return ((r+(alpha*((to>>11)-r)>>10))<<11) |
			((g+(alpha*(((to&2047)>>5)-g)>>10))<<5) |
			((b+(alpha*((to&31)-b)>>10)));
}


uint32_t blend32a(uint32_t from,uint32_t to,uint32_t alpha) {
	if(alpha==0) return from;
	if(alpha==0x3ff) return to;
	/* alpha blending the source and background colors */
	register uint32_t r = (((((to>>10)&0x00ff00))*alpha)+(((from>>10)&0x00ff00)*(0x3ff-alpha)))&0x00ff0000;
	register uint32_t g = (((to&0x00ff00)*alpha)+((from&0x00ff00)*(0x3ff-alpha)))&0x03fc0000;
	register uint32_t b = (((to&0x0000ff)*alpha)+((from&0x0000ff)*(0x3ff-alpha)))&0x0003fc00;
	return (to&0xff000000)|r|((g|b)>>10);
}



Colorcycle::Colorcycle() {
	colors = 0,len = 0,off = 0.0,var = 0.0,speed = 0.0,bits = 0,blend = 0;
}

Colorcycle::Colorcycle(const uint32_t *c,int l,int o,int v,int s,int b) {
	colors = 0,len = 0,off = 0.0,var = 0.0,speed = 0.0,bits = 0,blend = 0;
	setScheme(c,l,o,v,b);
}

Colorcycle::~Colorcycle() {
	clear();
}

void Colorcycle::clear() {
	if(colors) free(colors);
	colors = 0,len = 0,off = 0.0,var = 0.0,speed = 0.0,bits = 0,blend = 0;
}

void Colorcycle::setScheme(const uint32_t *c,int l,const char *s,int b) {
	int n = xtoi(s);
	setScheme(c,l,((n>>16)&0xff),(n>>8)&0xff,n&0xff,b);
}

void Colorcycle::setScheme(const uint32_t *c,int l,int o,int v,int s,int b) {
	if(!l) return;
	if(v==0) v = DEFAULT_VARIATION;
	clear();
	len = l;
	off = (o%len)<<10;
	var = v==0 || v>0xff? DEFAULT_VARIATION : v;
	speed = s==0 || s>0xff? v : s;
	bits = b;
	colors = (uint32_t *)malloc(len*sizeof(uint32_t));
	memcpy(colors,c,len*sizeof(uint32_t));
	blend = bits==16? blend16a : blend32a;
}

/*uint32_t Colorcycle::getColor(int n) {
	if(index[n]>=1024) {
		index[n] -= 1024;
		from[n] = to[n];
		to[n] = colors[mindex[n]=(mindex[n]+1)%clen];
	}
	uint32_t c = blend(from[n],to[n],index[n]);
	index[n] += var;
	return c;
}*/

void Colorcycle::writeColors(uint32_t *c,int l,double s) {
	double v = (double)var*s,i = (double)(off&0x3ff);
	int n,c1 = off>>10,c2 = (c1+1)%len;
	for(n=0; n<l; ++n) {
		c[n] = blend(colors[c1],colors[c2],(int)i);
		i += v;
		if(i>=1024.0) i -= 1024.0,c1 = c2,c2 = (c1+1)%len;
	}
	off += speed;
	if(off>=(len<<10)) off -= len<<10;
}

int Colorcycle::print(char *s) {
	return sprintf(s,"%x",((off>>10)<<16)|(var<<8)|speed);
}

void Colorcycle::print(FILE *fp) {
	fprintf(fp,"%x",((off>>10)<<16)|(var<<8)|speed);
}


}; /* namespace a */


