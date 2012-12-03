
#include "_config.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <amanita/Wiki.h>
#include <amanita/net/Socket.h>

/*
static const char *wiki_tags[] = {
0};
*/
static const char *html_tags[] = {
	"abbr","b","big","blockquote","br","caption","center","cite","code","dd","del","div","dl","dt","em","font",
	"h1","h2","h3","h4","h5","h6","hr","i","ins","li","ol","p","pre","rb","rp","rt","ruby","s","small","span",
	"strike","strong","sub","sup","table","td","th","tr","tt","u","ul","var",
0};

static const char *bb_tags[] = {
	"b","i","u","s","url","img","quote","code","size","color","table","tr","th","td","background",
0};

static const uint32_t flagpatterns[] = { 0xfffffff0,0xfffffff0,0xfffffee0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

static const char *protocols[] = {
	"http://",			"{^url:http}",
	"https://",			"{^url:https}",
	"ftp://",			"{^url:ftp}",
	"ftps://",			"{^url:ftps}",
	"news://",			"{^url:news}",
0};
/*
static const char *urls_refs = 0;

static const char *symbols[] = {
	"aries",				"zodiac1",
	"taurus",			"zodiac2",
	"gemini",			"zodiac3",
	"cancer",			"zodiac4",
	"leo",				"zodiac5",
	"virgo",				"zodiac6",
	"libra",				"zodiac7",
	"scorpio",			"zodiac8",
	"sagittarius",		"zodiac9",
	"capricorn",		"zodiac10",
	"aquarius",			"zodiac11",
	"pisces",			"zodiac12",
	"fire",				"elements1",
	"earth",				"elements2",
	"air",				"elements3",
	"water",				"elements4",
	"spirit",			"elements5",
	"conjunction",		"aspects1",
	"opposition",		"aspects13",
	"trine",				"aspects10",
	"square",			"aspects9",
	"sextile",			"aspects7",
	"sun",				"planets1",
	"moon",				"planets2",
	"mercury",			"planets3",
	"venus",				"planets4",
	"mars",				"planets5",
	"jupiter",			"planets6",
	"saturn",			"planets7",
	"uranus",			"planets8",
	"neptune",			"planets9",
	"pluto",				"planets10",
	"ascendant",		"planets15",
	"mc",					"planets16",
	"pentagram",		"symbol1",
	"ankh",				"symbol2",
	"horuseye",			"symbol3",
	"hexagram",			"symbol4",
	"peace",				"symbol5",
	"love",				"symbol6",
	"spiral",			"symbol7",
	"key",				"symbol8",
	"treeoflife",		"symbol9",
	"yinyang",			"symbol10",
	"scarab",			"symbol11",
	"kiss",				"smiley12",
	"blush",				"smiley13",
	"vampire",			"smiley14",
	"dead",				"smiley15",
	"alien",				"smiley16",
	"devil",				"smiley17",
	"z",					"smiley19",
0};
*/
static const char *list_chars = "*#;:";
static const char *list_types[] = { "ul","ol","dl","dl"};
static const char *list_rows[] = { "li","li","dt","dd" };
/*
static const char *wiki_tag_handlers = 0;
*/
static const char *char_style = "'''''";


namespace a {


Object_Inheritance(Wiki,String)


WikiData::WikiData(Wiki &w) : ref(0),tag_handlers(),link_handlers() {
	wiki = &w;
	target = 0;
	base_url = 0;
	primary_smileys = 0;
	secondary_smileys = 0;
	smiley_callback = 0;
}

WikiData::~WikiData() {
	Hashtable::iterator iter;
	WikiTagHandler *th;
	WikiLinkHandler *lh;
	if(target) free(target);
	if(base_url) free(base_url);
	clearSmileys();
	for(iter=tag_handlers.iterate(); (th=(WikiTagHandler *)iter.next());) free(th);
	for(iter=link_handlers.iterate(); (lh=(WikiLinkHandler *)iter.next());) free(lh);
}

void WikiData::setTagHandlers(const WikiTagHandler *th) {
	WikiTagHandler *th1;
	for(int i=0; th[i].tag; ++i) {
		th1 = (WikiTagHandler *)malloc(sizeof(WikiTagHandler));
		memcpy(th1,&th[i],sizeof(WikiTagHandler));
//debug_output("setTagHandlers(i: %d, tag: %s)\n",i,th[i].tag);
		tag_handlers.put(th[i].tag,(void *)th1);
	}
}

WikiTagHandler *WikiData::getTagHandler(const char *tag) {
	return (WikiTagHandler *)tag_handlers.get(tag);
}

void WikiData::setLinkHandlers(const WikiLinkHandler *lh) {
	WikiLinkHandler *lh1;
	for(int i=0; lh[i].nmsp; ++i) {
		lh1 = (WikiLinkHandler *)malloc(sizeof(WikiLinkHandler));
		memcpy(lh1,&lh[i],sizeof(WikiLinkHandler));
		link_handlers.put(lh[i].nmsp,(void *)lh1);
	}
}

WikiLinkHandler *WikiData::getLinkHandler(const char *nmsp) {
	return (WikiLinkHandler *)link_handlers.get(nmsp);
}

void WikiData::clearSmileys() {
	int i;
	if(primary_smileys) {
		for(i=0; primary_smileys[i]; ++i) free(primary_smileys[i]);
		free(primary_smileys);
		primary_smileys = 0;
	}
	if(secondary_smileys) {
		for(i=0; secondary_smileys[i]; ++i) free(secondary_smileys[i]);
		free(secondary_smileys);
		secondary_smileys = 0;
	}
}

void WikiData::setSmileys(const char **s,wiki_smiley_callback cb) {
	int i,n1,n2;
	char s1[33];
	String s2;
	clearSmileys();

//timeval tv1,tv2;
//long delay;
//gettimeofday(&tv1,0);


	for(i=0,n1=0,n2=0; s[i]; ++i)
		if(strchr("#*:;",*s[i])) n1 += 2;
		else n2 += 2;
	primary_smileys = (char **)malloc((n1+1)*sizeof(char *));
	primary_smileys[n1] = 0;
	if(n2) {
		secondary_smileys = (char **)malloc((n2+1)*sizeof(char *));
		secondary_smileys[n2] = 0;
	}
//debug_output("smileys:\n");
	for(i=0,n1=0,n2=0; s[i]; ++i) {
		sprintf(s1,"{^smiley:%d}",i+1);
//debug_output("smiley: \"%s\" => \"%s\"\n",s[i],s1);
		if(strchr("#*:;",*s[i])) {
			primary_smileys[n1] = strdup(s[i]);
			primary_smileys[n1+1] = strdup(s1);
//debug_output("primary_smileys: \"%s\" => \"%s\"\n",primary_smileys[n1],primary_smileys[n1+1]);
			n1 += 2;
		} else {
			secondary_smileys[n2] = (char *)s[i];
			secondary_smileys[n2+1] = strdup(s1);
//debug_output("secondary_smileys: \"%s\" => \"%s\"\n",secondary_smileys[n2],secondary_smileys[n2+1]);
			n2 += 2;
		}
	}
	for(i=0; secondary_smileys[i]; i+=2) {
		s2 = secondary_smileys[i];
		s2.encodeHTML();
		s2.replace((const char **)primary_smileys);
		secondary_smileys[i] = strdup(s2);
	}
	smiley_callback = cb;

//gettimeofday(&tv2,0);
//delay = ((long)tv2.tv_sec*1000000+(long)tv2.tv_usec)-((long)tv1.tv_sec*1000000+(long)tv1.tv_usec);
//debug_output("time: %ld\n",delay);


//if(primary_smileys) for(i=0; primary_smileys[i]; i+=2)
//debug_output("primary_smileys: \"%s\" => \"%s\"\n",primary_smileys[i],primary_smileys[i+1]);
//if(secondary_smileys) for(i=0; secondary_smileys[i]; i+=2)
//debug_output("secondary_smileys: \"%s\" => \"%s\"\n",secondary_smileys[i],secondary_smileys[i+1]);
}



WikiParams::WikiParams(Wiki &w,WikiData &d,String &t) : wiki(&w),data(&d),text(&t) {
	first = 0,start = 0,offset = 0,length = 0,end = 0,*tag = '\0',param = 0,param_length = 0,func = 0,*replace = '\0';
}

WikiLink::WikiLink() {
	wiki = 0,link = 0,nmsp = 0,name = 0,parenthesis = 0,section = 0,title = 0,func = 0;
}

WikiLink::~WikiLink() {
	clear();
}

void WikiLink::clear() {
	if(link) free(link);
	link = 0;
	if(nmsp) free(nmsp);
	nmsp = 0;
	if(name) free(name);
	name = 0;
	if(parenthesis) free(parenthesis);
	parenthesis = 0;
	if(section) free(section);
	section = 0;
	if(title) free(title);
	title = 0;
}

void WikiLink::parse(WikiData &data,char *text) {
	char *p = text,*p1,*p2;
	int n = 0,n1 = 0,n2 = 0,n3 = 0,n4 = 0;
	WikiLinkHandler *lh;
	wiki = data.wiki;
	clear();
	a::trim(p);
	if(*p==':') ++p;
	for(p1=p; *p1!='\0'; ++p1)
		if(*p1=='_') *p1 = ' ';
	if((p1=strchr(p,'|'))) *p1++ = '\0',title = strdup(p1);
	else title = strdup(p);
	a::trim(p);
	if((p2=strchr(p,':'))) {
		*p2++ = '\0',p1 = p,p = p2;
		nmsp = getName(p1);
	} else nmsp = strdup("");
	if((p1=strchr(p,'#'))) *p1++ = '\0',section = getName(p1);
	if((p1=strchr(p,'('))) {
		*p1++ = '\0';
		if((p2=strchr(p1,')'))) *p2++ = '\0';
		parenthesis = a::tolower(getName(p1));
	}
	if(!title) title = strdup(p);
	name = getName(p);

//debug_output("WikiLink::parse(name: %s, nmsp: %s, title: %s)\n",name,nmsp,title);

	n1 = strlen(name),n += n1+1;
	if(*nmsp) n2 += strlen(nmsp),n += n2+1;
	if(parenthesis) n3 += strlen(parenthesis),n += n3+3;
	if(section) n4 += strlen(section),n += n4+1;
	link = (char *)malloc(n),p = link;
	if(n2) {
		strcpy(p,nmsp);
		p += n2,*p++ = ':';
	}
	strcpy(p,name);
	p += n1;
	if(n3) {
		*p++ = '_',*p++ = '(';
		strcpy(p,parenthesis);
		p += n3,*p++ = ')';
	}
	if(n4) {
		*p++ = '#';
		strcpy(p,section);
		p += n4;
	}
	*p = '\0';

	lh = data.getLinkHandler(nmsp);
	if(lh) func = lh->func;
}

char *WikiLink::getName(const char *name) {
	if(name && *name) {
		char *p = strdup(name),*p1 = p,*p2 = p,c,c0 = '\0';
		while(*p2!='\0') {
			c = *p2++;
			if(c==' ' || c=='-') c = '_';
			if(c!='_' || c0!='_') *p1++ = c;
			c0 = c;
		}
		*p = a::toupper(*p);
		return p;
	}
	return strdup("");
}

void WikiLink::write(Wiki &w,const char *url) {
	Wiki w1(w);
	w1.formatInline(title);
	w << "<a href=\"" << url << link << "\" class=\"wiki_link\">" << w1 << "</a>";
}

enum {
	FILE_PNG,
	FILE_GIF,
	FILE_JPG,
	FILE_PDF,
	FILE_DOC,
	FILE_TXT,
	FILE_HTML,
	FILE_ZIP,
};

static const char *file_extensions[] = {
	"png","gif","jpg","pdf","doc","txt","html","zip",
0};

static const int file_types[] = {
	WIKI_IMAGE,WIKI_IMAGE,WIKI_IMAGE,
	WIKI_DOCUMENT,WIKI_DOCUMENT,WIKI_DOCUMENT,WIKI_DOCUMENT,
	WIKI_ARCHIVE,
};


WikiFile::WikiFile(WikiLink &l,const char *dir[]) {
	int n;
	char *p;
	link = &l;
	path = 0;
	ext = 0;
	type = WIKI_FILE;
	name = 0;
	style = 0;
	upright = 0.0;
	width = 0;
	height = 0;
	href = 0;
	alt = 0;
	text = 0;
	image_width = 0;
	image_height = 0;
//debug_output("WikiFile::WikiFile(name: %s)\n",link->name);
	if((p=strrchr(link->name,'.'))) {
		ext = a::tolower(strdup(&p[1]));
		n = search(file_extensions,ext);
		if(n!=-1) {
			type = file_types[n];
			name = strdup(link->name);
			for(p=name; *p; ++p) if(*p=='/' || *p=='\\') *p = '_';

			path = (char *)malloc(strlen(dir[type])+strlen(name)+1);
			*path = '\0';
			strcat(path,dir[type]);
			strcat(path,name);
			
//debug_output("WikiFile::WikiFile(path: %s, type: %d, n: %d)\n",path,type,n);
			if(type==WIKI_IMAGE) { // Read image width and height.
				FILE *fp = fopen(path,"rb");
				if(fp) {
					if(n==FILE_PNG) {
						char s[24];
						fread(s,24,1,fp);
						if(!strncmp(s,"\x89\x50\x4E\x47\x0D\x0A\x1A\x0A",8) && !strncmp(&s[12],"IHDR",4)) {
							image_width = (int)swap_be_32(*((uint32_t *)&s[16]));
							image_height = (int)swap_be_32(*((uint32_t *)&s[20]));
						}
					} else if(n==FILE_GIF) {
						char s[10];
						fread(s,10,1,fp);
						if(!strncmp(s,"GIF",3)) {
							image_width = (int)swap_be_16(*((uint16_t *)&s[6]));
							image_height = (int)swap_be_16(*((uint16_t *)&s[8]));
						}
					} else if(n==FILE_JPG) {
						char s[11];
						fread(s,11,1,fp);
//debug_output("WikiFile::WikiFile(jpg)\n");
						if(!strncmp(s,"\xFF\xD8\xFF\xE0",4) && !strncmp(&s[6],"JFIF\0",5)) {
							unsigned char *s1 = (unsigned char *)s;
							uint16_t bl = swap_be_16(*(uint16_t *)&s[4])-7;
							while(1) {
//debug_output("WikiFile::WikiFile(bl: %d)\n",(int)bl);
								if(feof(fp)) break;
								fseek(fp,bl,SEEK_CUR);
								fread(s,4,1,fp);
//debug_output("WikiFile::WikiFile(s: %04hx)\n",swap_be_16(*(uint16_t *)s));
								if(*s1!=0xff) break;
//debug_output("WikiFile::WikiFile()\n");
								if(s1[1]==0xc0) {
									fread(s,5,1,fp);
									image_height = (int)swap_be_16(*((uint16_t *)&s[1]));
									image_width = (int)swap_be_16(*((uint16_t *)&s[3]));
									break;
								}
								bl = swap_be_16(*(uint16_t *)&s[2])-2;
							}
						}
					}
					fclose(fp);
//debug_output("WikiFile::WikiFile(width: %d, height: %d)\n",image_width,image_height);
				} else perror(path);
			}
		}
	}
	if(type==WIKI_IMAGE) {
		int w = 0,h = 0;
		if(link->title && *link->title) {
			char *str = strdup(link->title),*p1,*p2;
			p = str;
			while(p) {
				if((p1=strchr(p,'|'))) *p1++ = '\0';
				if(*p=='f' && !strcmp(p,"frame")) style |= WIKI_IMAGE_FRAME;
				else if(*p=='t' && !strcmp(p,"thumb")) style |= WIKI_IMAGE_THUMB|WIKI_ALIGN_RIGHT;
				else if(*p=='l' && !strcmp(p,"left")) style |= WIKI_ALIGN_LEFT;
				else if(*p=='c' && !strcmp(p,"center")) style |= WIKI_ALIGN_CENTER;
				else if(*p=='r' && !strcmp(p,"right")) style |= WIKI_ALIGN_RIGHT;
				else if(*p=='u' && !strncmp(p,"upright",7)) upright = (float)(p[7]=='='? atof(&p[8]) : 1.0);
				else if(*p=='l' && !strncmp(p,"link=",5)) href = strdup(&p[5]);
				else if(*p=='a' && !strncmp(p,"alt=",4)) alt = strdup(&p[4]);
				else if(*p>='0' && *p<= '9' && (p2=strstr(p,"px"))) {
					*p2 = '\0',p2 = strchr(p,'x');
					if(p2) *p2++ = '\0',h = atoi(p2);
					w = atoi(p);
				} else {
					if(text) free(text);
					text = strdup(p);
debug_output("WikiFile::WikiFile(text: %s)\n",text);
				}
				p = p1;
			}
			free(str);
		}
		if(w) {
			if(w && h) width = w,height = h;
			else if(image_width && image_height) width = w,height = (w*image_height)/image_width;
			else width = w;
		} else if((style&WIKI_IMAGE_THUMB) || upright>0.0) {
			width = upright>0.0? (int)round(220.0*upright*0.075)*10 : 220;
			if(image_width && image_height) height = (width*image_height)/image_width;
		} else if(image_width && image_height) width = image_width,height = image_height;
	}
}

WikiFile::~WikiFile() {
	if(path) free(path);
	path = 0;
	if(ext) free(ext);
	ext = 0;
	if(name) free(name);
	name = 0;
	if(href) free(href);
	href = 0;
	if(alt) free(alt);
	alt = 0;
	if(text) free(text);
	text = 0;
}

void WikiFile::write(Wiki &w,const char *url[]) {
	switch(type) {
		case WIKI_IMAGE:
		{
			if((style&WIKI_ALIGN_CENTER)) w << "<div class=\"wiki_img_center\">";
			if((style&WIKI_IMAGE_FRAME) || (style&WIKI_IMAGE_THUMB)) {
				w << "<div class=\"wiki_thumb";
				if(!(style&WIKI_ALIGN_CENTER)) {
					if((style&WIKI_ALIGN_LEFT)) w << " wiki_thumb_left";
					else if((style&WIKI_ALIGN_RIGHT)) w << " wiki_thumb_right";
				}
				if(width) w << "\" style=\"width:" << width << "px;";
				w << "\">";
			}
			if(href) w << "<a href=\"" << url[WIKI_IMAGE] << href << "\" class=\"wiki_img_link\">";
			w << "<img src=\"" << url[WIKI_IMAGE] << name << "\" alt=\"" << alt << "\" class=\"wiki_img\"";
			if(width) w << " width=\"" << width << '"';
			if(height) w << " height=\"" << height << '"';
			w << " />";
			if(href) w << "</a>";
			if(((style&WIKI_IMAGE_FRAME) || (style&WIKI_IMAGE_THUMB)) && text) {
debug_output("WikiFile::write(text: %s)\n",text);
				w << "<div>";
				w.formatInline(text);
				w << "</div></div>";
			}
			if((style&WIKI_ALIGN_CENTER)) w << "</div>";
			break;
		}
		case WIKI_DOCUMENT:
			w << "<a href=\"" << url[WIKI_DOCUMENT] << name << "\">" << link->title << "</a>";
			if(href) w << "<a href=\"" << url[WIKI_ARTICLE] << link->link << "\" class=\"wiki_doc\"><div></div></a>";
			break;
		case WIKI_ARCHIVE:
			w << "<a href=\"" << url[WIKI_ARCHIVE] << name << "\">" << link->title << "</a>";
			if(href) w << "<a href=\"" << url[WIKI_ARTICLE] << link->link << "\" class=\"wiki_arc\"><div></div></a>";
			break;
	}
}


static void wiki_char_style(WikiParams &p) {
	const char *start,*end;
	if(p.param==2) start = "<i>",end = "</i>";
	else if(p.param==3) start = "<b>",end = "</b>";
	else if(p.param==5) start = "<i><b>",end = "</b></i>";
	else start = "",end = "";
//char str[p.length+1];
//p.text->substr(str,p.offset,p.length);
//debug_output("wiki_char_style(%s)\n",str);
	*p.wiki << start;
	p.wiki->formatInline(*p.text,false,p.offset,p.length);
	*p.wiki << end;
}

static void wiki_link(WikiParams &p) {
	char s[p.length+1];
	p.text->substr(s,p.offset,p.length);
	p.data->ref << s;
//echo "<p>wiki_link(namespace=$namespace,link=$link,section=$section,title=$title)</p>";
	*p.wiki << "{^link:" << p.data->ref.size() << '}';
//debug_output("wiki_link(%s)\n",s);
}

static void wiki_anchor(WikiParams &p) {
	String anchor;
//debug_output("wiki_anchor(url: %s)\n",url);
//debug_output("wiki_anchor(tag: %s)\n",tag);
	anchor << "<a href=\"" << p.tag << "://";
	anchor.append(p.text,(long)p.offset,(long)p.length);
	anchor << "\">";
	if(p.param_length) anchor.append(p.text,(long)p.param,(long)p.param_length);
	else anchor.append(p.text,(long)p.offset,(long)p.length);
	anchor << "</a>";
//	$target = strpos($url,$sc->domain)!==false? $this->target : ' target="_blank"';
	p.wiki->addRef(anchor);
}

static void bb_tag_html(WikiParams &p) {
	String param;
	if(p.param_length) {
		param.append(p.text,p.param,p.param_length);
		param.encodeHTML();
	}
	*p.wiki << '<' << p.tag << param << '>';
	p.wiki->append(*p.text,(long)p.offset,(long)p.length);
	*p.wiki << "</" << p.tag << '>';
}

static void bb_tag_url(WikiParams &p) {
	Wiki ref(*p.wiki);
	String href;
	if(p.param_length) {
		href.append(p.text,p.param,p.param_length);
		href.encodeHTML();
	}
//	$target = strpos($href,$sc->domain)!==false? $this->target : ' target="_blank"';
	ref << "<a href=\"" << href << "\">";
	ref.formatInline(*p.text,false,p.offset,p.length);
	ref << "</a>";
	return p.wiki->addRef(ref);
}

static void bb_tag_img(WikiParams &p) {
	*p.wiki << "<img src=\"";
	p.wiki->append(*p.text,(long)p.offset,(long)p.length);
	*p.wiki << "\" alt=\"\" />";
}

static void bb_tag_quote(WikiParams &p) {
	// TODO
	p.wiki->formatInline(*p.text,false,p.offset,p.length);
}

static void bb_tag_code(WikiParams &p) {
	*p.wiki << "<pre>";
	p.wiki->formatInline(*p.text,false,p.offset,p.length);
	*p.wiki << "</pre>";
}

static void bb_tag_size(WikiParams &p) {
	if(p.param_length) {
		*p.wiki << "<span style=\"font-size:";
		p.wiki->append(p.text,p.param,p.param_length);
		*p.wiki << "px;\">";
	}
	p.wiki->formatInline(*p.text,false,p.offset,p.length);
	if(p.param_length)
		*p.wiki << "</span>";
}

static void bb_tag_color(WikiParams &p) {
	if(p.param_length) {
		*p.wiki << "<span style=\"color:";
		p.wiki->append(p.text,p.param,p.param_length);
		*p.wiki << "\";>";
	}
	p.wiki->formatInline(*p.text,false,p.offset,p.length);
	if(p.param_length)
		*p.wiki << "</span>";
}

static void bb_tag_background(WikiParams &p) {
	if(p.param_length) {
		*p.wiki << "<span style=\"background-color:";
		p.wiki->append(p.text,p.param,p.param_length);
		*p.wiki << "\";>";
	}
	p.wiki->formatInline(*p.text,false,p.offset,p.length);
	if(p.param_length)
		*p.wiki << "</span>";
}


static const wiki_callback bb_tag_funcs[] = {
	bb_tag_html,
	bb_tag_html,
	bb_tag_html,
	bb_tag_html,
	bb_tag_url,
	bb_tag_img,
	bb_tag_quote,
	bb_tag_code,
	bb_tag_size,
	bb_tag_color,
	bb_tag_html,
	bb_tag_html,
	bb_tag_html,
	bb_tag_html,
	bb_tag_background,
0};



Wiki::Wiki(size_t c) : String(c) {
	flags = 0;
	owner = 0;
	data = new WikiData(*this);
}

Wiki::Wiki(Wiki &w,size_t c) : String(c) {
	flags = w.flags;
	owner = w.owner? w.owner : &w;
	data = w.data;
}

Wiki::~Wiki() {
	if(!owner) {
		delete data;
		data = 0;
	}
}


void Wiki::setTarget(const char *target) {
	if(data->target) ::free(data->target);
	data->target = strdup(target);
}

void Wiki::setBaseURL(const char *url) {
	if(data->base_url) ::free(data->base_url);
	data->base_url = strdup(url);
}

void Wiki::format(const char *text,uint32_t f) {
	clear();
	*this << text;
	format(f);
}

void Wiki::format(uint32_t f) {
	if(!length()) return;
	flags = f;

timeval tv1,tv2;
long delay;
gettimeofday(&tv1,0);

	Wiki w1(*this,cap),w2(*this,cap);
	String s1;

	newline("\n");
	stripComments(LANG_HTML);

//debug_output("w1: \"%s\"\n",w1.toCharArray());

	w2.storeRefs(*this);
	if((flags&WIKI_SMILEYS) && data->primary_smileys)
		w2.replace((const char **)data->primary_smileys);
//debug_output("w1: \"%s\"\n",w1.toCharArray());
	if(flags&WIKI_SECTIONS) {
		w1.formatSections(w2);
		/*for(i=0; i<data->headers.size(); ++i) {
			s = (const char *)data->headers[i];
			s1 = s;
			w2.clear();
			w2.injectRefs(s1);
//debug_output("header[%d]: \"%s\"\n",i,w2.toCharArray());
			data->headers.set(i,w2);
		}*/
	} else w1.formatInline(w2);
	if((flags&WIKI_SMILEYS) && data->secondary_smileys)
		w1.replace((const char **)data->secondary_smileys);
	clear();
	injectRefs(w1);


gettimeofday(&tv2,0);
delay = ((long)tv2.tv_sec*1000000+(long)tv2.tv_usec)-((long)tv1.tv_sec*1000000+(long)tv1.tv_usec);
debug_output("time: %ld\n",delay);
}

enum {
	FORMAT_ALL,
	FORMAT_TABLE_CELL,
};

void Wiki::formatSections(String &text) {
	char ind[65];
	Vector lines;
	lines.split(text,"\n");
	*ind = '\0';
	formatLines(lines,0,0,ind,FORMAT_ALL);
}

size_t Wiki::formatLines(Vector &lines,size_t ln,int lvl,char *ind,int f) {
	size_t i,n = lines.size(),l;
	char *line;
	char c,c1;
	char lists[65];
	Wiki header(*this);
	String section,str;
	for(; ln<n; ++ln) {
		line = (char *)lines[ln],l = strlen(line);
		if(*line=='\0' || !strcmp(line,"----")) {
			if(section) {
				formatInline(section,true);
				section.clear();
			}
			if(!strcmp(line,"----")) *this << "<hr />\n";
		} else {
			c = *line,c1 = line[1];
			if((c=='=' && line[l-1]=='=') ||
					(c=='{' && c1=='|') || 
					((c=='|' || c=='!') && f==FORMAT_TABLE_CELL) ||
					strchr(" *#;:",c)) {
				if(section) {
					formatInline(section,true);
					section.clear();
				}
				if(c=='=') {
					for(i=1; i<6 && line[i]=='=' && line[l-1-i]=='='; ++i);
					str.append(line,i,l-i*2);
					header.formatInline(str);
					data->ref << header;
					header.clear();
					str.clear();
//debug_output("formatLines(size: %d, i: %d)\n",(int)data->headers.size(),(int)i);
					*this << "{^header:" << data->ref.size() << ',' << i << '}';
				} else if(c=='{' && c1=='|') {
					ln = formatTable(lines,ln,lvl,ind);
				} else if((c=='|' || c=='!') && f==FORMAT_TABLE_CELL) {
					break;
				} else if(c==' ') {
					ln = formatPre(lines,ln);
				} else {
					*lists = '\0';
					ln = formatList(lines,ln,lvl,ind,lists);
				}
			} else {
				if(section) section << '\n';
				section << line;
			}
		}
	}
	if(section) formatInline(section,length()? true : false);
	return ln;
}

size_t Wiki::formatPre(Vector &lines,size_t ln) {
	size_t n = lines.size();
	char *line;
	String text;
	for(; ln<n; ++ln) {
		line = (char *)lines[ln];
		if(*line!=' ') break;
		if(text) text << '\n';
		text.append(line,1,0);
	}
	*this << "\n<pre>";
	formatInline(text);
	*this << "</pre>\n";
	if(ln<n) --ln;
	return ln;
}

enum {
	CAPTION,
	TH,
	TD,
};

static const char *table[] = {
	"caption","th","td",
};

size_t Wiki::formatTable(Vector &lines,size_t ln,int lvl,char *ind) {
	size_t n = lines.size();
	char *line = (char *)lines[ln];
	String style,cell;
	char c,c1,*p1,*p2,*p3,close[15];
	int type,rows = 0;

	formatTableStyle(style,&line[2]);
	*this << '\n' << ind << "<table" << style << ">\n";

	for(++ln,*close='\0'; ln<n; ++ln) {
		line = (char *)lines[ln],c = *line,c1 = line[1];
		if(c=='\0') continue;
		if(c=='|' && (c1=='-' || c1=='}')) {
			if(*close) *this << ind << close;
			if(rows) *this << ind << "\t</tr>\n";
			if(c1=='-') {
				formatTableStyle(style,&line[2]);
				*this << ind << "\t<tr" << style << ">\n";
				++rows;
			} else {
				break;
			}
		} else {
			type = c=='!'? TH : (c=='|' && c1=='+'? CAPTION : TD);
			if(!rows && type!=CAPTION) {
				*this << ind << "\t<tr>\n";
				++rows;
			}

			p1 = &line[type==CAPTION? 2 : (type==TH || c=='|'? 1 : 0)];
			String::trim(p1);

			while(p1 && *p1) {
				style.clear();
				if((p2=strchr(p1,'|'))) {
					*p2 = '\0';
					if(p2[1]=='|') p2 += 2;
					else {
						formatTableStyle(style,p1);
						p1 = p2+1;
						if((p3=strchr(p2+1,'|')) && p3[1]=='|') *p3 = '\0',p2 = p3+1;
						else p2 = 0;
					} 
				}
//debug_output("formatTable(p1: %s, style: %s)\n",p1,style.toCharArray());

				*this << ind << "\t<" << table[type] << style << '>';

				if(!p2) {
					line = (char *)lines[ln+1],c = *line,c1 = line[1];
					if(c!='|' && c!='!') {
//						*this << '\n';
						lines.set(ln,p1);
						ind[lvl] = '\t',ind[lvl+1] = '\t',ind[lvl+2] = '\0';
						ln = formatLines(lines,ln,lvl+2,ind,FORMAT_TABLE_CELL);
						ind[lvl] = '\0';
						if(ln<n) --ln;
						*this << ind << "\t</" << table[type] << ">\n";
						break;
					}
				}
				cell = p1;
				cell.trim();
				if(cell) formatInline(cell);
				*this << "</" << table[type] << ">\n";
				p1 = p2;
			}
		}
	}

	*this << ind << "</table>\n";
	return ln;
}

void Wiki::formatTableStyle(String &style,char *line) {
	style.clear();
	if(*line!='\0') {
		style << line;
		style.trim();
		if(style) {
			style.stripHTML();
			style.insert(0,' ');
		}
	}
}


size_t Wiki::formatList(Vector &lines,size_t ln,int lvl,char *ind,char *lists,int l) {
	char *line = (char *)lines[ln];
	char type = line[l];
	size_t i,n;

	for(i=0; list_chars[i]!=type; ++i)
		if(list_chars[i]=='\0') return ln;

	lists[l] = type,lists[l+1] = '\0';

	*this << '\n' << ind << '<' << list_types[i] << ">\n";

	for(n=lines.size(); ln<n; ++ln) {
		line = (char *)lines[ln];
		if(strlen(line)<=(size_t)l || strncmp(lists,line,l+1)!=0) break;
		ln = formatListRow(lines,ln,lvl,ind,lists,l);
	}

	*this << ind << "</" << list_types[i] << ">\n";

	if(ln<n) --ln;
	return ln;
}

size_t Wiki::formatListRow(Vector &lines,size_t ln,int lvl,char *ind,char *lists,int l) {
	char *line = (char *)lines[ln];
	char type = line[l];
	int i,r = 0;
	String s;

	for(i=0; list_chars[i]!=type; ++i)
		if(list_chars[i]=='\0') return ln;

	*this << ind << '<' << list_rows[i] << '>';
	s.clear();
	s.append(line,l+1,0);
	s.trim();
	if(s) formatInline(s);

	ind[lvl] = '\t',ind[lvl+1] = '\0';
	while((line=(char *)lines[ln+1])) {
		if(strncmp(lists,line,l+1)==0 && strchr(list_chars,line[l+1])) {
			ln = formatList(lines,ln+1,lvl+1,ind,lists,l+1);
			r = 1;
		} else break;
	}
	ind[lvl] = '\0';

	if(r) *this << ind;
	*this << "</" << list_rows[i] << ">\n";
	return ln;
}


void Wiki::formatInline(const char *text) {
	String s(text);
	Wiki w(*this);
	w.formatInline(s);
	injectRefs(w);
//	formatInline(s);
}

void Wiki::formatInline(String &text,bool pg,long o,long l) {
	size_t i;
	char c;
	WikiParams p(*this,*data,text);

	if(o<0) o = (long)text.length()+o;
	if(l<=0) l = (long)text.length()+l;
	if(o<0 || l<=0 || o+l>(long)text.length()) return;

	if(pg) *this << "\n<p>";

//debug_output("\nWiki::formatInline:\n");
	for(p.first=o,p.start=o,p.end=o,p.offset=o,p.length=0,l+=o; p.offset<l; ++p.offset) {
		c = text[(long)p.offset];
//debug_putc(c);
		if(c=='\'') { // <-- Character (inline) formatting – applies anywhere.
			for(i=1; (long)(p.offset+i)<l && i<5 && text[p.offset+i]=='\''; ++i);
			if(i>1) {
				if(i>5) i = 5;
				else if(i==4) i = 3;
				p.start = p.offset,p.offset += i;
				if(!(p.length=text.find(char_style,p.offset,0,i))) p.length = l-p.offset;
				else p.length -= p.offset;
				p.end = p.offset+p.length+i,p.func = wiki_char_style,*p.tag = '\0',p.param = i;
			}
		} else if(c=='<') {
			if(flags&WIKI_TAGS) matchWikiTag(p);
//echo '<p>wiki_tag{func='.$func.',tag='.$tag.',param='.$param.',text='.substr($text,$p,$pn).",p=$p,pn=$pn,p1=$p1,p2=$p2,p3=$p3}</p>";
		}
		else if(c=='>') strcpy(p.replace,"&gt;");
		else if(c=='[') {
			c = text[(long)p.offset+1];
			if(c=='[') {
				if((flags&WIKI_LINKS))
					matchWikiLink(p);
			} else if(c=='{') {
				matchURL(p);
			} else if((flags&WIKI_BBCODE)) {
				matchBBTag(p);
//echo '<p>bb_tag{func='.$func.',tag='.$tag.',param='.$param.',text='.substr($text,$p,$pn).",p=$p,pn=$pn,p1=$p1,p2=$p2,p3=$p3}</p>";
			}
		} else if(c=='{') {
			c = text[(long)p.offset+1];
			if(c=='{') {
				matchTemplate(p);
			} else if(c=='^') {
				matchURL(p);
			}
		} else continue;
		if(*p.replace) {
			append(text,(long)p.first,(long)(p.offset-p.first));
			*this << p.replace;
			p.first = p.offset+1,p.start = p.first,*p.replace = '\0';
		} else {
			if(p.end>p.first) {
				if(p.start>p.first) append(text,(long)p.first,(long)p.start-p.first);
				p.first = p.end,p.start = p.first;
			}
			if(p.func) {
//debug_output("\nformatInline(tag: %s)\n",p.tag);
				p.func(p);
				p.func = 0,*p.tag = '\0',p.param = 0,p.param_length = 0;
			}
			if(p.offset<p.end) p.offset = p.end-1;
		}
	}
	if(p.first<l) append(text,(long)p.first,(long)(l-p.first));

	if(pg) *this << "</p>\n";
}


void Wiki::addRef(const char *ref) {
	data->ref << ref;
	*this << "{^ref:" << data->ref.size() << '}';
}


void Wiki::storeRefs(String &text) {
	long p1,p2,p3;
	String r;
	for(p1=0,p2=0,p3=0; (p2=text.find("{^",p2,0,2))!=-1; ) {
		if((p3=text.find('}',p2+2))==-1) break;
		append(text,p1,p2-p1);
		text.substr(r,p2,p3+1-p2);
		p1 = p3+1,p2 = p1;
		r.encodeHTML();
		addRef(r);
	}
	if(p1<(long)text.length()) append(text,p1,0);
	// Replace urls with refs.
	if(flags&WIKI_LINKS) replace(protocols);
//debug_output("storeRefs(%s)\n",toCharArray());
}

void Wiki::injectRefs(String &text) {
	long p1,p2,p3,len = text.length();
	char tag[65],*t1,*t2;
	char *ref,*p;
	long id,value;
	WikiLink link;
//debug_output("injectRefs(\n%s\n)\n",text.toCharArray());
	for(p1=0,p2=0,p3=0; (p2=text.find("{^",p2))!=-1; ) {
		if((p3=text.find('}',p2+2))==-1) break;
		if(p2>p1) append(text,p1,p2-p1);
		text.substr(tag,p2+2,p3-(p2+2));
		t1 = strchr(tag,':');
		*t1++ = '\0';
		value = 0;
		if((t2=strchr(t1,',')))
			*t2++ = '\0',value = atol(t2);
		id = atol(t1);
		p1 = p3+1;
		p2 = p1;
		ref = 0;
		if(!strcmp(tag,"ref")) {
			ref = (char *)data->ref[id-1];
			*this << ref;
		} else if(!strcmp(tag,"header")) {
			ref = (char *)data->ref[id-1];
			p = WikiLink::getName(ref);
//debug_output("injectRefs(header: %s, id: %ld)\n",ref,id-1);

//			$h = $this->strip_html($h);
//			id = LinkHandler::getName(ref);
//echo "<p>h=$h[1],hn=$hn</p>";
			*this << "<a name=\"" << p << "\" id=\"" << p << "\"></a><h" << value << ">" << ref << "</h" << value << ">\n";
			::free(p);
		} else if(!strcmp(tag,"link")) {
			ref = (char *)data->ref[id-1];
debug_output("injectRefs(link: %s, id: %ld)\n",ref,id-1);
			link.parse(*data,ref);
			if(link.func) link.func(*this,link);
			else link.write(*this,data->base_url);
//			$str = $link->content();
//			if(strpos($str,'{^')!==false) $str = $this->injectRefs($str);
		} else if(!strcmp(tag,"smiley")) {
			if(data->smiley_callback)
				data->smiley_callback(*this,id);
		}
	}
	if(p1<len) append(text,p1,0);
}


void Wiki::matchTemplate(WikiParams &p) {
	long n = p.text->matchNestedTags("{{","}}",p.offset);
	if(n>=0) {
		long n2 = p.text->find('|',p.offset,n-p.offset);
		p.start = p.offset;
		p.offset += 2;
		p.length = n-p.offset;
		p.end = p.offset+p.length+2;
		if(n2!=-1) {
			p.param = n2+1;
			p.param_length = n-n2-1;
			n = n2;
		} else {
			p.param = 0;
			p.param_length = 0;
		}
		p.text->substr(p.tag,p.offset,n-p.offset);
		p.func = data->template_callback;
	} else ++p.offset;
}

void Wiki::matchURL(WikiParams &p) {
	char c = p.text->charAt(p.offset);
	if(c=='[') ++p.offset;
	if(p.text->equals("{^url:",p.offset,6)) {
		long n,n2 = p.text->find('}',p.offset+6);
		if(n2>=p.offset+6) {
			p.start = c=='['? p.offset-1 : p.offset;
			p.text->substr(p.tag,p.offset+6,n2-(p.offset+6));
			p.offset = n2+1;
			if(c=='[') {
				n = p.text->find(']',p.offset);
				if(n==-1) n = p.text->length();
				n2 = p.text->findChar(" \n\t",p.offset,n-p.offset);
				if(n2!=-1) {
					long p1 = n2,p2 = n-n2;
					p.text->trim(p1,p2);
					p.param = p1,p.param_length = p2;
				} else n2 = n;
				p.length = n2-p.offset;
				p.end = n+1;
			} else {
				n = p.text->findChar(" \n\t[](){}<>\"'*|",p.offset);
				if(n==-1) n = p.text->length();
				p.length = n-p.offset;
				p.end = n;
			}
//char str[p.length+1];
//p.text->substr(str,p.offset,p.length);
//debug_output("matchURL(str: %s, n: %d, offset: %d, length: %d, end: %d)\n",str,(int)n,(int)p.offset,(int)p.length,(int)p.end);
			p.func = wiki_anchor;
		}
	}
}

void Wiki::matchWikiTag(WikiParams &p) {
	long i,j,k,n/*,len = p.text->length()*/,l,html = -1;
	char c = p.text->charAt(p.offset+1);
	WikiTagHandler *th = 0;
	i = c=='/'? p.offset+2 : p.offset+1;
	j = p.text->findChar(" \n\t>",i);
	p.text->substr(p.tag,i,j-i);
	l = strlen(p.tag);
	th = data->getTagHandler(p.tag);
	if(!th) html = search(html_tags,p.tag);
//debug_output("\nmatchWikiTag(tag: %s, html: %d, lh: %p)\n",p.tag,(int)html,th);
	if(html!=-1 || th) {
		k = p.text->charAt(j)=='>'? j : p.text->find('>',j);
//debug_output("matchWikiTag(i: %ld, j: %ld[%c], k: %ld, c: %c)\n",i,j,p.text->charAt(j),k,c);
		if(k!=-1) {
			if(html!=-1) {
				p.offset = k;
				return;
			} else if(c=='/') { // Error! Ending tag </tag> -removed.
				p.start = p.offset;
				p.end = k+1;
				return;
			} else {
				p.start = p.offset;
				p.length = 0;
				p.end = k+1;
				if((th->style&WIKI_TAG_SIMPLE) || p.text->charAt(k-1)=='/') --k; // Simple tag <tag />.
				else {
					char tag1[l+2],tag2[l+4];
					sprintf(tag1,"<%s",p.tag);
					sprintf(tag2,"</%s>",p.tag);
					if((th->style&WIKI_TAG_NESTED))
						n = p.text->matchNestedTags(tag1,tag2,p.offset,0," \n\t>");
					else n = p.text->find(tag2,p.offset);
					if(n==-1) return; // Error! Unmatched tag -removed.
					p.offset = k+1;
					p.length = n-p.offset;
					p.end = n+l+3;
				}
				if(k>j) {
					long p1 = j,p2 = k-j;
					p.text->trim(p1,p2);
					p.param = p1,p.param_length = p2;
				}
				p.func = th->func;
//debug_output("matchWikiTag(param: %d, param_length: %d)\n",p.param,p.param_length);
				return;
			}
		}
	}
	strcpy(p.replace,"&lt;");
}


void Wiki::matchWikiLink(WikiParams &p) {
	long n = p.text->matchNestedTags("[[","]]",p.offset);
debug_output("matchWikiLink(n: %ld)\n",n);
	if(n>=0) {
		p.start = p.offset;
		p.offset += 2;
		p.length = n-p.offset;
		p.end = p.offset+p.length+2;
		*p.tag = '\0';
		p.param = 0;
		p.param_length = 0;
		p.func = wiki_link;
	} else ++p.offset;
}

void Wiki::matchBBTag(WikiParams &p) {
	if(p.text->charAt(p.offset+1)=='/') return;
	long i,n/*,len = p.text->length()*/,l,bb;
	i = p.offset+1;
	n = p.text->findChar(" \n\t=]",i);
	p.text->substr(p.tag,i,n-i);
	l = strlen(p.tag);
//echo '<p>matchBBTag(tag='.$tag.')</p>';
	bb = search(bb_tags,p.tag);
	if(bb!=-1) {
		char s1[l+2],s2[l+3];
		i = p.text->charAt(n)!=']'? p.text->find(']',n) : n;
		if(i>n) {
			long p1 = n,p2 = i-n;
			if(p.text->charAt(p1)=='=') ++p1,--p2;
			p.text->trim(p1,p2);
			p.param = p1,p.param_length = p2;
		} else p.param = 0,p.param_length = 0;
		sprintf(s1,"[%s",p.tag);
		sprintf(s2,"[/%s]",p.tag);
		n = p.text->matchNestedTags(s1,s2,p.offset,0," \n\t=]");
		if(n!=-1) {
			p.start = p.offset;
			p.offset = i+1;
			p.length = n-p.offset;
			p.end = n+l+3;
			p.func = bb_tag_funcs[bb];
//debug_output("matchBBTag(text: \"%s\", start: %d, offset: %d, length: %d, end: %d, tag: \"%s\")\n",p.text->toCharArray(),(int)p.start,(int)p.offset,(int)p.length,(int)p.end,p.tag);
		}
	}
}

}; /* namespace a */


