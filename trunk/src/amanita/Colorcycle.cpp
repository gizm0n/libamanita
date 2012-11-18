
#include <stdlib.h>
#include <string.h>
#include <amanita/Colorcycle.h>
#include <amanita/String.h>


namespace a {


#define DEFAULT_VARIATION 5


static const char *scheme_name[] = { "Daylight","Palette","Neon","Gold","Silver","Ebony","Blood","Lapiz Lazuli","Amethyst","Fire","Foliage" };

static const uint32_t schemes16[] = {
	// Daylight 0-10
	0x0000,0x0013,0x9cdf,0xffff,0xffe0,
	0xf800,0xffe0,0xffff,0x9cdf,0xffff,0x0013,
	// Palette 11-54
	0x001f,0x04df,0x07ff,0x07f3,0x07e0,
	0x9fe0,0xffe0,0xfcc0,0xf800,0xf813,0xf81f,
	0x000c,0x018c,0x032c,0x0326,0x0320,
	0x3320,0x6320,0x6180,0x6000,0x6006,0x600c,
	0x0006,0x0106,0x0186,0x0184,0x0180,
	0x2180,0x3180,0x3100,0x3000,0x3004,0x3006,
	0x000c,0x018c,0x032c,0x0326,0x0320,
	0x3320,0x6320,0x6180,0x6000,0x6006,0x600c,
	// Neon 55-65
	0x001f,0x04df,0x07ff,0x07f3,0x07e0,
	0x9fe0,0xffe0,0xfcc0,0xf800,0xf813,0xf81f,
	// Gold 66-69
	0xffe0,0xfff3,0xffe0,0xfcc0,
	// Silver 70-71
	0x3186,0xffff,
	// Ebony 72-73
	0x0000,0x5186,
	// Blood 74-75
	0xc800,0x6000,
	// Lapiz Lazuli 76-77
	0x000c,0x0006,
	// Amethyst 78-79
	0xc819,0x600c,
	// Fire 80-81
	0xf800,0xffe0,
	// Foliage 82-87
	0x0180,0x07e0,0x0186,0x0666,0x6186,0x67e6,
};

static const uint32_t schemes24[] = {
	// Daylight 0-10
	0x000000,0x000099,0x9999ff,0xffffff,0xffff00,
	0xff0000,0xffff00,0xffffff,0x9999ff,0xffffff,0x000099,
	// Palette 11-54
	0x0000ff,0x0099ff,0x00ffff,0x00ff99,0x00ff00,
	0x99ff00,0xffff00,0xff9900,0xff0000,0xff0099,0xff00ff,
	0x000066,0x003366,0x006666,0x006633,0x006600,
	0x336600,0x666600,0x663300,0x660000,0x660033,0x660066,
	0x000033,0x002233,0x003333,0x003322,0x003300,
	0x223300,0x333300,0x332200,0x330000,0x330022,0x330033,
	0x000066,0x003366,0x006666,0x006633,0x006600,
	0x336600,0x666600,0x663300,0x660000,0x660033,0x660066,
	// Neon 55-65
	0x0000ff,0x0099ff,0x00ffff,0x00ff99,0x00ff00,
	0x99ff00,0xffff00,0xff9900,0xff0000,0xff0099,0xff00ff,
	// Gold 66-69
	0xffff00,0xffff9d,0xffff00,0xff9900,
	// Silver 70-71
	0x333333,0xffffff,
	// Ebony 72-73
	0x000000,0x553333,
	// Blood 74-75
	0xcc0000,0x660000,
	// Lapiz Lazuli 76-77
	0x000066,0x000033,
	// Amethyst 78-79
	0xcc00cc,0x660066,
	// Fire 80-81
	0xff0000,0xffff00,
	// Foliage 82-87
	0x003300,0x00ff00,0x003333,0x00cc33,0x663333,0x66ff33,
};

static const int scheme_offset[] = {
	0,11,55,66,70,72,74,76,78,80,82,
};

static const int scheme_length[] = {
	11,44,11,4,2,2,2,2,2,2,6
};



/*uint32_t blend16a(uint32_t from,uint32_t to,uint32_t alpha) {
	if(alpha==0) return from;
	if(alpha>=0x3ff) return to;
	register uint32_t r = (from>>11);
	register uint32_t g = ((from&0x07ff)>>5);
	register uint32_t b = (from&0x001f);
	return ((r+(alpha*((to>>11)-r)>>10))<<11) |
			((g+(alpha*(((to&2047)>>5)-g)>>10))<<5) |
			((b+(alpha*((to&31)-b)>>10)));
}*/

uint32_t blend16a(uint32_t from,uint32_t to,uint32_t alpha) {
	if(alpha==0) return from;
	if(alpha>=0xffff) return to;
	register uint32_t r = (from>>11);
	register uint32_t g = ((from&0x07ff)>>5);
	register uint32_t b = (from&0x001f);
	return ((r+(alpha*((to>>11)-r)>>16))<<11) |
			((g+(alpha*(((to&2047)>>5)-g)>>16))<<5) |
			((b+(alpha*((to&31)-b)>>16)));
}


/*uint32_t blend24a(uint32_t from,uint32_t to,uint32_t alpha) {
	if(alpha==0) return from;
	if(alpha>=0x3ff) return to;
	register uint32_t r = (((((to>>10)&0x003fc0))*alpha)+(((from>>10)&0x003fc0)*(0x3ff-alpha)))&0x00ff0000;
	register uint32_t g = (((to&0x00ff00)*alpha)+((from&0x00ff00)*(0x3ff-alpha)))&0x03fc0000;
	register uint32_t b = (((to&0x0000ff)*alpha)+((from&0x0000ff)*(0x3ff-alpha)))&0x0003fc00;
	return r|((g|b)>>10);
}*/

uint32_t blend24a(uint32_t from,uint32_t to,uint32_t alpha) {
	if(alpha==0) return from;
	if(alpha>=0xffff) return to;
	register uint32_t r = (((((to>>16)&0x0000ff))*alpha)+(((from>>16)&0x0000ff)*(0xffff-alpha)))&0x00ff0000;
	register uint32_t g = (((to&0x00ff00)*alpha)+((from&0x00ff00)*(0xffff-alpha)))&0xff000000;
	register uint32_t b = (((to&0x0000ff)*alpha)+((from&0x0000ff)*(0xffff-alpha)))&0x00ff0000;
	return r|((g|b)>>16);
}



Colorcycle::Colorcycle() {
	colors = 0;
	clear();
}

Colorcycle::Colorcycle(Colorcycle &c) {
	colors = 0;
	setScheme(c);
}

Colorcycle::Colorcycle(int c,int o,int v,int s,int b) {
	colors = 0;
	setScheme(c,o,v,s,b);
}

Colorcycle::Colorcycle(const uint32_t *c,int l,int o,int v,int s,int b) {
	colors = 0;
	setScheme(c,l,o,v,s,b);
}

Colorcycle::~Colorcycle() {
	clear();
}

void Colorcycle::clear() {
	if(scheme==-1 && colors) free(colors);
	scheme = -1;
	colors = 0;
	len = off = var = speed = bits = 0;
	blend = 0;
}

void Colorcycle::setScheme(Colorcycle &c) {
	clear();
	scheme = c.scheme;
	if(scheme==-1) {
		colors = (uint32_t *)malloc(len*sizeof(uint32_t));
		memcpy(colors,c.colors,len*sizeof(uint32_t));
	} else colors = c.colors;
	len = c.len;
	off = c.off;
	var = c.var;
	speed = c.speed;
	bits = c.bits;
	blend = c.blend;
}

void Colorcycle::setScheme(int c,int o,int v,int s,int b) {
	clear();
	if(b<8) b = b*8;
	if(c>=0 && c<CC_SCHEMES && (b==16 || b==24 || b==32)) {
		int so = scheme_offset[c];
		scheme = c;
		colors = (uint32_t *)(b==16? &schemes16[so] : &schemes24[so]);
		len = scheme_length[c];
		off = (o%len)<<16;
		var = v==0? 0xff : v;
		speed = (s==0 || s>0xff? v : s)<<6;
		bits = b==16? 16 : 24;
		blend = bits==16? blend16a : blend24a;
	}
}

void Colorcycle::setScheme(const uint32_t *c,int l,int o,int v,int s,int b) {
	clear();
	if(b<8) b = b*8;
	if(c && l>0 && (b==16 || b==24 || b==32)) {
		colors = (uint32_t *)malloc(l*sizeof(uint32_t));
		memcpy(colors,c,l*sizeof(uint32_t));
		len = l;
		off = (o%len)<<16;
		var = v==0? 0xff : v;
		speed = s==0? v : s;
		bits = b==16? 16 : 24;
		blend = bits==16? blend16a : blend24a;
	}
}

uint32_t Colorcycle::getColor(int n) {
	int o,l = (len<<16),c1,c2;
	o = (off+n*var)%l;
	c1 = o>>16,c2 = (c1+1)%len,o = (o&0xffff);
	return blend(colors[c1],colors[c2],o);
}

void Colorcycle::write(uint32_t *c,int l,bool a) {
	int n,c1 = off>>16,c2 = (c1+1)%len,i = off&0xffff;
	for(n=0; n<l; ++n) {
		c[n] = blend(colors[c1],colors[c2],i);
		i += var;
		if(i>0xffff) i &= 0xffff,c1 = c2,c2 = (c1+1)%len;
	}
	if(a) advance();
}

void Colorcycle::write(uint32_t *c,int l,double s,bool a) {
	double v = (double)var*s,i = (double)(off&0xffff);
	int n,c1 = off>>16,c2 = (c1+1)%len;
	for(n=0; n<l; ++n) {
		c[n] = blend(colors[c1],colors[c2],(int)i);
		i += v;
		if(i>0xffff) i -= 0xffff,c1 = c2,c2 = (c1+1)%len;
	}
	if(a) advance(s);
}

void Colorcycle::advance() {
	off += speed;
	if(off>=(len<<16)) off -= len<<16;
}

void Colorcycle::advance(double s) {
	off += (int)((double)speed*s);
	if(off>=(len<<16)) off -= len<<16;
}

const char *Colorcycle::getSchemeName() {
	return scheme>=0 && scheme<CC_SCHEMES? scheme_name[scheme] : "";
}

void Colorcycle::write(FILE *fp) {
	int i;
	fprintf(fp,"Colorcycle: %d %d %d %d %d %d\n",scheme,len,off,var,speed,bits);
	if(scheme==-1)
		for(i=0; i<len; ++i) {
			if(bits==16) fprintf(fp,"#%04X\n",colors[i]&0xffff);
			else fprintf(fp,"#%06X\n",colors[i]&0xffffff);
		}
}

void Colorcycle::read(FILE *fp) {
	char str[1025];
	if(fgets(str,1024,fp) && *str) {
		if(!strncmp(str,"Colorcycle:",11)) {
			if(scheme==-1 && colors) free(colors);
			sscanf(&str[11]," %d %d %d %d %d %d",&scheme,&len,&off,&var,&speed,&bits);
			if(scheme!=-1) {
				int so = scheme_offset[scheme];
				colors = (uint32_t *)(bits==16? &schemes16[so] : &schemes24[so]);
			} else {
				int i;
				colors = (uint32_t *)malloc(len*sizeof(uint32_t));
				for(i=0; i<len; ++i) {
					fgets(str,1024,fp);
					sscanf(str,"#%X",&colors[i]);
				}
			}

fprintf(stderr,"Colorcycle: %d %d %d %d %d %d\n",scheme,len,off,var,speed,bits);
fflush(stderr);
if(scheme==-1)
for(int i=0; i<len; ++i) {
if(bits==16) fprintf(stderr,"#%04X\n",colors[i]&0xffff);
else fprintf(stderr,"#%06X\n",colors[i]&0xffffff);
}

		}
	}
}


}; /* namespace a */


