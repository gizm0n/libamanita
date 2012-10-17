
#include "../_config.h"
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL_image.h>
#include <png.h>
#include <amanita/gfx/aGfxImage.h>
#include <amanita/gfx/aGfxGraphics.h>


aObject_Inheritance(aGfxImage,aObject)


int aGfxImage::created = 0;
int aGfxImage::deleted = 0;


inline bool _space(char c) { return c==' ' || c=='\n' || c=='\t' || c=='\r' || c=='\f' || c=='\v' || c=='\0'; }

aGfxImage::aGfxImage() : aObject(),name(0),file(0),surface(0),map(0),mapSize(0),mapRow(0) {
	created++;
}

aGfxImage::aGfxImage(int w,int h) : aObject(),name(0),file(0),surface(0),map(0) {
	created++;
	SDL_PixelFormat *f = g.getScreenFormat();
	SDL_Surface *s = SDL_CreateRGBSurface(/*SDL_HWSURFACE*/SDL_SWSURFACE,w,h,
			f->BitsPerPixel,f->Rmask,f->Gmask,f->Bmask,f->Amask);
	surface = s;
	createMap(1);
}

aGfxImage::aGfxImage(const char *fn) : aObject(),name(0),file(0),surface(0),map(0) {
	created++;
	name = strdup(fn);
	file = strdup(fn);
	load(fn);
	createMap(1);
}

aGfxImage::~aGfxImage() {
	deleted++;
	if(name) { free(name);name = 0; }
	if(file) { free(file);file = 0; }
	if(surface) { SDL_FreeSurface(surface);surface = 0; }
	if(map) { free(map);map = 0; }
}

void aGfxImage::createMap(int sz) {
	if(map) free(map);
	if(sz<=0) sz = 1;
	mapRow = 0,mapSize = sz,map = (SDL_Rect *)malloc(sizeof(SDL_Rect)*mapSize);
	setCell(0,0,0,surface->w,surface->h);
}

void aGfxImage::createMap(int rw,int rh) {
//printf("aGfxImage::createMap(sx=%d,sy=%d)\n",sx,sy);
//fflush(stdout);
	mapRow = (surface->w/rw);
//printf("aGfxImage::createMap(mapRow=%d)\n",mapRow);
//fflush(stdout);
	unsigned int rows = (surface->h/rh);
	mapSize = 1+mapRow*rows;
//printf("aGfxImage::createMap(mapSize=%d)\n",mapSize);
//fflush(stdout);
	if(map) free(map);
	map = (SDL_Rect *)malloc(sizeof(SDL_Rect)*mapSize);
	setCell(0,0,0,surface->w,surface->h);
	for(unsigned int x,y=0,i=1; y<rows; y++)
		for(x=0; x<mapRow; x++,i++)
			setCell(i,x*rw,y*rh,rw,rh);
}

void aGfxImage::createMap(SDL_Rect *m,int l) {
	mapSize = l;
	mapRow = 0;
//printf("aGfxImage::createMap(width=%d,height=%d,mapSize=%d,mapRow=%d)\n",d[2],d[3],mapSize,mapRow);
//fflush(stdout);
	if(map) free(map);
	map = (SDL_Rect *)malloc(sizeof(SDL_Rect)*mapSize);
	memcpy(map,m,sizeof(SDL_Rect)*mapSize);
}


void aGfxImage::load(const char *fn) {
	SDL_Surface *s = IMG_Load(fn);
	surface = SDL_DisplayFormat(s);
	SDL_FreeSurface(s);
}


aGfxImage **aGfxImage::readXIM(const char *p,const char *fn,int &n) {
	n = 0;
printf("aGfxImage::readXIM(fn=%s)\n",fn);
fflush(stdout);
	char str[1024];
	sprintf(str,"%s%s",p,fn);
	FILE *fp = fopen(str,"rb");
	if(!fp) {
		perror(str);
		return 0;
	}
// printf("aGfxImage::readXIM()\n");
// fflush(stdout);
	int nlines = 0,f,r = 0,b;
	char *p1 = NULL,*p2 = NULL,c1 = '\0',c2 = '\0';
	fseek(fp,0,SEEK_END);
	f = ftell(fp);
	fseek(fp,0,SEEK_SET);
// printf("aGfxImage::readXIM(sz=%d)\n",f);
// fflush(stdout);
	char *data = (char *)malloc(f+1);
	for(p1=data; (f=fgetc(fp))!=EOF; ) *p1++ = (char)f;//fread(data,f,1,fp);
	*p1 = '\0';//data[f] = '\0';
// printf("aGfxImage::readXIM()\nFILE: %s\n",fn);
// fflush(stdout);
	for(p1=data,p2=p1,f=0,b=0; *p2; p2++) {
// putc(*p2,stderr);
		switch(f) {
			case 0:
				if(*p2=='/') f = 10,r = 0;
				else if(*p2=='"') {
					if(b==1) f = 3;
					else f = 1;
				}
				break;
			case 1: // '"' Inside quotes and outside brackets '"_{}_"'
				if(*p2=='"') f = 0;
				else if(*p2=='\\') f = 20,r = 1;
				else if(*p2=='{') b = 1,nlines++,f = 3;
				break;
			case 2:
				if(c1) *p1++ = c1;
				*p1++ = c2;
			case 3: // Inside brackets '"{_}"'
				if(*p2=='"') f = 0;
				else if(*p2=='\\') f = 20,r = 2;
				else if(*p2=='}') *p1++ = '\0',b = 0,f = 1;
				else *p1++ = *p2;
				break;

			case 10: // '/' Comment, must set r
				if(*p2=='*') f = 11;
				else if(*p2=='/') f = 13;
				else f = r;
				break;
			case 11:if(*p2=='*') f = 12;break; // '/*'
			case 12: // '/* ... *'
				if(*p2=='/') f = r;
				else if(*p2!='*') f = 11;
				break;
			case 13:if(*p2=='\n') f = r;break; // '//'

			case 20: // '\' Escape, must set r
				if(*p2=='n') c1 = 0,c2 = '\n',f = r;
				else if(*p2=='t') c1 = 0,c2 = '\t',f = r;
				else if(*p2=='r') c1 = 0,c2 = '\r',f = r;
				else if(*p2=='f') c1 = 0,c2 = '\f',f = r;
				else if(*p2=='v') c1 = 0,c2 = '\v',f = r;
				else if(*p2=='x') f = 21;
				else c1 = '\\',c2 = *p2,f = r;
				break;
			case 21: // '\x'
				if(*p2>='0' && *p2<='9') c2 = (*p2-'0')<<8,f = 22;
				if(*p2>='a' && *p2<='f') c2 = (*p2-'a'+10)<<8,f = 22;
				if(*p2>='A' && *p2<='A') c2 = (*p2-'A'+10)<<8,f = 22;
				else c1 = '\\',c2 = 'x',f = r;
				break;
			case 22: // '\x[0-9a-fA-F]'
				if(*p2>='0' && *p2<='9') c2 |= (*p2-'0');
				if(*p2>='a' && *p2<='f') c2 |= (*p2-'a'+10);
				if(*p2>='A' && *p2<='A') c2 |= (*p2-'A'+10);
				c1 = 0,f = r;
				break;
		}
	}
	*p1 = '\0';
// printf("\nEOF\nImage::readXIM(nlines=%d)\n",nlines);
// fflush(stdout);
	aGfxImage **images = 0;
	if(nlines>0) {
		const char **lines = (const char **)malloc(sizeof(char *)*nlines);
		lines[0] = data;
		for(f=0,b=1,p1=data; b<nlines; p1++) {
			if(f==1) lines[b++] = p1,f = 0;
			else if(*p1=='\0') f = 1;
		}
		images = parseXIM(p,lines,nlines);
		free(lines);
		n = nlines;
	}
	free(data);
	return images;
}


aGfxImage **aGfxImage::parseXIM(const char *p,const char *lines[],int &n) {
	struct mapping {
		int sz,rw,rh;
		SDL_Rect *r;
	};
	struct image_template {
		char *name,*path,*data;
		long alpha,colkey,from,to,mapsz;
		mapping *map;
	};
	int nlines = n;
	image_template templates[nlines],*t;
	aGfxImage **images,*img;
	long maprects = 0;
	long num;
	int i,j,m,f,r,b,v,x1,y1,x,y,w,h,rw,rh,nimages;
	long colkey;
	char name[256],path[1024];
	char *p1 = NULL,*p2 = NULL,c = '\0';
	int dd[6],ds = 128,dn,dq;
	mapping *d = (mapping *)malloc(sizeof(mapping)*ds);
	SDL_Rect *dr;
	SDL_Surface *s;

	for(i=0,nimages=0; i<nlines; i++) {
		t = &templates[i],f = strlen(lines[i])+1;
		t->data = (char *)malloc(f+1);
		memcpy(t->data,lines[i],f);
		t->data[f] = '\0'; // Two '\0' at the end, first counts as space in some cases in the loop, second breaks the loop
// printf("\nImage::parseXIM(lines[%d]=\"%s\")\n",i,data);
// fflush(stdout);

		t->name = 0,t->path = 0,t->alpha = 255,t->colkey = 0x00ffff,t->from = 1,t->to = 1;
		t->map = 0,t->mapsz = 0;
		p1 = t->data,dn = 0,f = 0,r = 0,v = 0;
		while(f!=-1) {
// printf("f=%d,p1=%s,p2=%s\n",f,p1,p2);
// fflush(stdout);
			switch(f) {
				case 0: // Start state. Scans for next command.
					while(f==0) {
						if(*p1=='\0') f = -1;
						else if(*p1=='\'' || *p1=='"') f = 3,c = *p1,r = 0,v = 0; // A string of no value.
						else if(*p1=='n' || *p1=='N') {
							if(!strnicmp(p1,"nm=",3)) p1 += 3,f = 10,v = 1;
							else if(!strnicmp(p1,"name=",5)) p1 += 5,f = 10,v = 1;
							else f = 2,r = 0;
						} else if(*p1=='p' || *p1=='P') {
							if(!strnicmp(p1,"p=",2)) p1 += 2,f = 10,v = 2;
							else if(!strnicmp(p1,"path=",5)) p1 += 5,f = 10,v = 2;
							else f = 2,r = 0;
						} else if(*p1=='a' || *p1=='A') {
							if(!strnicmp(p1,"a=",2)) p1 += 2,f = 10,v = 3;
							else if(!strnicmp(p1,"alpha=",6)) p1 += 6,f = 10,v = 3;
							else f = 2,r = 0;
						} else if(*p1=='c' || *p1=='C') {
							if(!strnicmp(p1,"ck=",3)) p1 += 3,f = 10,v = 4;
							else if(!strnicmp(p1,"colkey=",7)) p1 += 7,f = 10,v = 4;
							else f = 2,r = 0;
						} else if(*p1=='f' || *p1=='F') {
							if(!strnicmp(p1,"f=",2)) p1 += 2,f = 10,v = 5;
							else if(!strnicmp(p1,"from=",5)) p1 += 5,f = 10,v = 5;
							else f = 2,r = 0;
						} else if(*p1=='t' || *p1=='T') {
							if(!strnicmp(p1,"t=",2)) p1 += 2,f = 10,v = 6;
							else if(!strnicmp(p1,"to=",3)) p1 += 3,f = 10,v = 6;
							else f = 2,r = 0;
						} else if(*p1=='m' || *p1=='M') {
							if(!strnicmp(p1,"m=",2)) p1 += 2,f = 30,r = 0;
							else if(!strnicmp(p1,"map=",4)) p1 += 4,f = 30,r = 0;
							else f = 2,r = 0;
						} else p1++;
					}
					break;

				case 2: // Scans for next space. Must set r
					while(!_space(*++p1));
					f = r;
					break;

				case 3: // Scans for a char (c). Must set c where c must be !='\\', r and v.
					for(p2=++p1; *p1!='\0'; p1++) {
						if(*p1=='\\') p1++;
						else if(*p1==c) break;
					}
					f = r;
					break;

				case 10: // Must set v.
					if(_space(*p1)) p2 = p1,f = 20;
					else if(*p1=='\'' || *p1=='"') f = 3,c = *p1,r = 20;
					else p2 = p1,f = 2,r = 20;
					break;

				case 20:
					*p1++ = '\0';
// printf("v=%d,p2=%s\n",v,p2);
// fflush(stdout);
					if(p2) {
						if(v==1) t->name = p2;
						else if(v==2) t->path = p2;
						else if(v>=3 && v<=6) {
							if(!strncmp(p2,"0x",2)) num = strtol(&p2[2],0,16);
							else num = strtol(p2,0,10);
							switch(v) {
								case 3:t->alpha = num;break;
								case 4:t->colkey = num;break;
								case 5:t->from = num;break;
								case 6:t->to = num;break;
							}
						}
					}
					if(*p1=='\0') f = -1;
					else f = 0;
					break;

				case 30: // Start of mapping
					if(*p1=='(') p1++,f = 31,r = 0,v = 0;
					else f = 0;
					break;
				case 31: // Mapping loop
					while(*p1>='0' && *p1<='9') r = r*10+(*p1-'0'),p1++;
// printf("r=%d,p1=%s\n",r,p1);
// fflush(stdout);
					dd[v++] = r,r = 0;
					if(*p1==')' || v==6) {
						if(dn==ds) ds *= 2,d = (mapping *)realloc(d,sizeof(mapping)*ds);
						if(v==6) {
							x = dd[0],y = dd[1],w = dd[2],h = dd[3],rw = dd[4],rh = dd[5];
							r = (w/rw)*(h/rh);
// printf("aGfxImage::parseXIM(v=%d,sz=%d,x=%d,y=%d,w=%d,h=%d,rw=%d,rh=%d)\n",v,r,x,y,w,h,rw,rh);
// fflush(stdout);
							d[dn].sz = r,d[dn].r = (SDL_Rect *)malloc(sizeof(SDL_Rect)*r);
							for(y1=y,r=0; y1+rh<=y+h; y1+=rh) for(x1=x; x1+rw<=x+w; x1+=rw)
								d[dn].r[r++] = (SDL_Rect){x1,y1,rw,rh};
							dn++;
						} else if(v>=4) {
							dr = d[dn].r = (SDL_Rect *)malloc(sizeof(SDL_Rect));
							d[dn].sz = 1,dr->x = dd[0],dr->y = dd[1],dr->w = dd[2],dr->h = dd[3];
// printf("aGfxImage::parseXIM(v=%d,sz=%d,x=%d,y=%d,w=%d,h=%d)\n",v,d[dn].sz,dr->x,dr->y,dr->w,dr->h);
// fflush(stdout);
							dn++;
						} else if(v>=2) {
							d[dn].sz = -1,d[dn].rw = dd[0],d[dn].rh = dd[1];
// printf("aGfxImage::parseXIM(v=%d,sz=%d,rw=%d,rh=%d)\n",v,d[dn].sz,d[dn].rw,d[dn].rh);
// fflush(stdout);
							dn++;
						}
						r = 0,v = 0,f = 0;
					}
					if(*p1=='\0') f = -1;
					else p1++;
					break;
				default:f = -1;
			}
		}
		if(dn>0) {
			t->mapsz = dn;
			t->map = (mapping *)malloc(sizeof(mapping)*dn);
			memcpy(t->map,d,sizeof(mapping)*dn);
		}
		if(t->from<t->to) t->to++,nimages += t->to-t->from;
		else t->to = t->from+1,nimages++;
printf("aGfxImage::parseXIM(i=%d,name='%s',path='%s',alpha=%ld,colkey=0x%06lx/%ld,from=%ld,to=%ld)\n",
		i,t->name,t->path,t->alpha,(unsigned long)t->colkey,t->colkey,t->from,t->to-1);
fflush(stdout);
	}
	free(d);

	images = new aGfxImage*[nimages];
	t = &templates[j=0],m = t->from;
	for(i=0; i<nimages; i++,m++) {
		img = images[i] = new aGfxImage();
		if(m==t->to) t = &templates[++j],m = t->from;

		formatString(path,p,t->path,m);
		if((s=IMG_Load(path))==0) {
printf("aGfxImage::parseXIM(IMG_Load(%s))\n",SDL_GetError());
fflush(stdout);
		}
		if(t->colkey!=-1) {
			colkey = SDL_MapRGB(s->format,(t->colkey>>16)&0xff,(t->colkey>>8)&0xff,t->colkey&0xff);
			SDL_SetColorKey(s,SDL_RLEACCEL|SDL_SRCCOLORKEY,colkey);
		}
		if(t->alpha<255) {
			SDL_SetAlpha(s,SDL_SRCALPHA|SDL_RLEACCEL,t->alpha);
			img->surface = SDL_DisplayFormatAlpha(s);
// printf("aGfxImage::parseXIM(alpha=%d,err=%s)\n",alpha,SDL_GetError());
// fflush(stdout);
			if(img->surface==0) {
printf("aGfxImage::parseXIM(SDL_DisplayFormatAlpha(%s))\n",SDL_GetError());
fflush(stdout);
			}
		} else {
			img->surface = SDL_DisplayFormat(s);
			if(img->surface==0) {
printf("aGfxImage::parseXIM(SDL_DisplayFormat(%s))\n",SDL_GetError());
fflush(stdout);
			}
		}
		SDL_FreeSurface(s);
		s = img->surface;
		w = s->w,h = s->h;

		dq = 0,dn = t->mapsz,d = t->map;
		for(f=0,dq=0; f<dn; f++) {
			if(d[f].sz==-1) {
				rw = d[f].rw,rh = d[f].rh,r = (w/rw)*(h/rh);
				d[f].sz = r,d[f].r = (SDL_Rect *)malloc(sizeof(SDL_Rect)*r);
				for(y1=0,r=0; y1+rh<=h; y1+=rh) for(x1=0; x1+rw<=w; x1+=rw)
					d[f].r[r++] = (SDL_Rect){x1,y1,rw,rh};
// printf("aGfxImage::parseXIM(w=%d,h=%d,rw=%d,rh=%d,d[f].sz=%d,r=%d)\n",w,h,rw,rh,d[f].sz,r);
// fflush(stdout);
			}
			dq += d[f].sz;
		}
// printf("aGfxImage::parseXIM(dn=%d,dq=%d)\n",dn,dq);
// fflush(stdout);
		img->mapSize = 1+dq;
		img->mapRow = 0;
		img->map = (SDL_Rect *)malloc(sizeof(SDL_Rect)*img->mapSize);
		img->map[0] = (SDL_Rect){0,0,w,h};
		for(f=0,v=1; f<dn; f++) {
			r=0,b=d[f].sz;
// printf("aGfxImage::parseXIM(5,f=%d,v=%d,b=%d)\n",f,v,b);
// fflush(stdout);
			while(r<b) img->map[v++] = d[f].r[r++];
		}
		formatString(name,0,t->name,m);
		img->name = strdup(name);
		img->file = strdup(path);
// for(f=0; f<img->mapSize; f++) {
// SDL_Rect &rect = img->map[f];
// printf("aGfxImage::parseXIM(map[%d]=(x=%d,y=%d,w=%d,h=%d))\n",f,rect.x,rect.y,rect.w,rect.h);
// fflush(stdout);
// }
		maprects += img->mapSize;
	}
	for(i=0; i<nlines; i++) {
		t = &templates[i];
		for(j=0; j<t->mapsz; j++) free(t->map[j].r);
		free(t->map);
	}
// printf("\nImage::parseXIM(maprects=%d)\n",maprects);
// fflush(stdout);
	n = nimages;
	return images;
}

void aGfxImage::formatString(char *str,const char *p,const char *format,int num) {
	if(!str) return;
	if(p && *p) str = strcpy(str,p)+strlen(p);
	if(format) while(*format)
		if(*format=='#') {
			sprintf(str,"%d",num);
			while(*str) str++;
			format++;
		} else *str++ = *format++;
	*str = '\0';
}


bool aGfxImage::save(const char *fn,SDL_Surface *s) {
	if(strstr(fn,".bmp")) return SDL_SaveBMP(s,fn)==0;
	if(strstr(fn,".png")) {
		bool ret = false;
		FILE *fp = fopen(fn,"wb");
		if(fp) {
			png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
			if(png_ptr) {
				png_infop info_ptr = png_create_info_struct(png_ptr);
				if(info_ptr) {
					//if(!setjmp(png_ptr->jmpbuf)) {
						png_init_io(png_ptr,fp);
						int c = PNG_COLOR_MASK_COLOR; /* grayscale not supported */
						if(s->format->palette) c |= PNG_COLOR_MASK_PALETTE;
						else if(s->format->Amask) c |= PNG_COLOR_MASK_ALPHA;
						png_set_compression_level(png_ptr,Z_BEST_COMPRESSION);
						png_set_IHDR(png_ptr,info_ptr,s->w,s->h,8,c,PNG_INTERLACE_NONE,
							PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
						png_write_info(png_ptr,info_ptr);
						png_set_packing(png_ptr);
						struct { unsigned char r,g,b; } line[s->w];
						int i,j;
						Uint8 *p;
						switch(s->format->BytesPerPixel) {
							case 1:
								for(i=0; i<s->h; i++) {
									for(j=0,p=((Uint8 *)s->pixels)+i*s->pitch; j<s->w; j++,p++)
										SDL_GetRGB((uint32_t)*p,s->format,&line[j].r,&line[j].g,&line[j].b);
									png_write_row(png_ptr,(png_bytep)line);
								}
								break;
							case 2:
								for(i=0; i<s->h; i++) {
									for(j=0,p=((Uint8 *)s->pixels)+i*s->pitch; j<s->w; j++,p+=2)
										SDL_GetRGB((uint32_t)*((uint16_t *)p),s->format,&line[j].r,&line[j].g,&line[j].b);
									png_write_row(png_ptr,(png_bytep)line);
								}
								break;
							case 3:
								if(SDL_BYTEORDER==SDL_BIG_ENDIAN) {
									for(i=0; i<s->h; i++) {
										for(j=0,p=((Uint8 *)s->pixels)+i*s->pitch; j<s->w; j++,p+=3)
											SDL_GetRGB((uint32_t)((p[0]<<16)|(p[1]<<8)|p[2]),s->format,&line[j].r,&line[j].g,&line[j].b);
										png_write_row(png_ptr,(png_bytep)line);
									}
								} else {
									for(i=0; i<s->h; i++) {
										for(j=0,p=((Uint8 *)s->pixels)+i*s->pitch; j<s->w; j++,p+=3)
											SDL_GetRGB((uint32_t)(p[0]|(p[1]<<8)|(p[2]<<16)),s->format,&line[j].r,&line[j].g,&line[j].b);
										png_write_row(png_ptr,(png_bytep)line);
									}
								}
								break;
							case 4:
								for(i=0; i<s->h; i++) {
									for(j=0,p=((Uint8 *)s->pixels)+i*s->pitch; j<s->w; j++,p+=4)
										SDL_GetRGB(*((uint32_t *)p),s->format,&line[j].r,&line[j].g,&line[j].b);
									png_write_row(png_ptr,(png_bytep)line);
								}
								break;
						}
						png_write_end(png_ptr,info_ptr);
						ret = true;
					//}
				} else fprintf(stderr,"png_create_info_struct error!\n");
				png_destroy_write_struct(&png_ptr,&info_ptr);
			} else fprintf(stderr,"png_create_write_struct error!\n");
			fclose(fp);
		} else perror(fn);
		return ret;
	}
	return false;
}


inline void aGfxImage::draw(SDL_Rect &src,SDL_Rect &dst) {
	SDL_BlitSurface(surface,&src,g.getCanvas(),&dst);
}

void aGfxImage::draw(int x,int y,SDL_Rect &src) {
	SDL_Rect dst = { x,y,0,0 };
	draw(src,dst);
}

void aGfxImage::draw(int x,int y,int w,int h,SDL_Rect &src) {
	int i,j;
	SDL_Rect r,dst = { 0,0,0,0 };
	for(i=0; i<h; i+=src.h)
		for(j=0; j<w; j+=src.w) {
			dst.x = x+j,dst.y = y+i;
			if(j+src.w>=w || i+src.h>=h) {
				r = src;
				if(j+src.w>=w) r.w = w-j;
				if(i+src.h>=h) r.h = h-i;
				draw(r,dst);
			} else draw(src,dst);
		}
}


void aGfxImage::setCell(int index,int x,int y,int w,int h) {
	map[index].x = x,map[index].y = y,map[index].w = w,map[index].h = h;
}

