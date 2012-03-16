
#include "_config.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <amanita/aString.h>


aObject_Inheritance(aString,aObject)


static const char upper_hex[17] = "0123456789ABCDEF";
static const char lower_hex[17] = "0123456789abcdef";

static const aString::entity HTMLentities[256] = {
	{ "#0",			2 },			// 0
	{ "#1",			2 },			// 1
	{ "#2",			2 },			// 2
	{ "#3",			2 },			// 3
	{ "#4",			2 },			// 4
	{ "#5",			2 },			// 5
	{ "#6",			2 },			// 6
	{ "#7",			2 },			// 7
	{ "#8",			2 },			// 8
	{ "#9",			2 },			// 9
	{ "#10",			3 },			// 10
	{ "#11",			3 },			// 11
	{ "#12",			3 },			// 12
	{ "#13",			3 },			// 13
	{ "#14",			3 },			// 14
	{ "#15",			3 },			// 15
	{ "#16",			3 },			// 16
	{ "#17",			3 },			// 17
	{ "#18",			3 },			// 18
	{ "#19",			3 },			// 19
	{ "#20",			3 },			// 20
	{ "#21",			3 },			// 21
	{ "#22",			3 },			// 22
	{ "#23",			3 },			// 23
	{ "#24",			3 },			// 24
	{ "#25",			3 },			// 25
	{ "#26",			3 },			// 26
	{ "#27",			3 },			// 27
	{ "#28",			3 },			// 28
	{ "#29",			3 },			// 29
	{ "#30",			3 },			// 30
	{ "#31",			3 },			// 31
	{ 0,				0 },			// 32		<SPACE>
	{ 0,				0 },			// 33		!
	{ "quot",		4 },			// 34		"		&quot;
	{ 0,				0 },			// 35		#
	{ 0,				0 },			// 36		$
	{ 0,				0 },			// 37		%
	{ "amp",			3 },			// 38		&		&amp;
	{ "#39",			3 },			// 39		'		&apos;
	{ 0,				0 },			// 40		(
	{ 0,				0 },			// 41		)
	{ 0,				0 },			// 42		*
	{ 0,				0 },			// 43		+
	{ 0,				0 },			// 44		,
	{ 0,				0 },			// 45		-
	{ 0,				0 },			// 46		.
	{ 0,				0 },			// 47		/
	{ 0,				0 },			// 48		0
	{ 0,				0 },			// 49		1
	{ 0,				0 },			// 50		2
	{ 0,				0 },			// 51		3
	{ 0,				0 },			// 52		4
	{ 0,				0 },			// 53		5
	{ 0,				0 },			// 54		6
	{ 0,				0 },			// 55		7
	{ 0,				0 },			// 56		8
	{ 0,				0 },			// 57		9
	{ 0,				0 },			// 58		:
	{ 0,				0 },			// 59		;
	{ "lt",			2 },			// 60		<		&lt;
	{ 0,				0 },			// 61		=
	{ "gt",			2 },			// 62		>		&gt;
	{ 0,				0 },			// 63		?
	{ 0,				0 },			// 64		@
	{ 0,				0 },			// 65		A
	{ 0,				0 },			// 66		B
	{ 0,				0 },			// 67		C
	{ 0,				0 },			// 68		D
	{ 0,				0 },			// 69		E
	{ 0,				0 },			// 70		F
	{ 0,				0 },			// 71		G
	{ 0,				0 },			// 72		H
	{ 0,				0 },			// 73		I
	{ 0,				0 },			// 74		J
	{ 0,				0 },			// 75		K
	{ 0,				0 },			// 76		L
	{ 0,				0 },			// 77		M
	{ 0,				0 },			// 78		N
	{ 0,				0 },			// 79		O
	{ 0,				0 },			// 80		P
	{ 0,				0 },			// 81		Q
	{ 0,				0 },			// 82		R
	{ 0,				0 },			// 83		S
	{ 0,				0 },			// 84		T
	{ 0,				0 },			// 85		U
	{ 0,				0 },			// 86		V
	{ 0,				0 },			// 87		W
	{ 0,				0 },			// 88		X
	{ 0,				0 },			// 89		Y
	{ 0,				0 },			// 90		Z
	{ 0,				0 },			// 91		[
	{ 0,				0 },			// 92		'\'
	{ 0,				0 },			// 93		]
	{ 0,				0 },			// 94		^
	{ 0,				0 },			// 95		_
	{ 0,				0 },			// 96		`
	{ 0,				0 },			// 97		a
	{ 0,				0 },			// 98		b
	{ 0,				0 },			// 99		c
	{ 0,				0 },			// 100	d
	{ 0,				0 },			// 101	e
	{ 0,				0 },			// 102	f
	{ 0,				0 },			// 103	g
	{ 0,				0 },			// 104	h
	{ 0,				0 },			// 105	i
	{ 0,				0 },			// 106	j
	{ 0,				0 },			// 107	k
	{ 0,				0 },			// 108	l
	{ 0,				0 },			// 109	m
	{ 0,				0 },			// 110	n
	{ 0,				0 },			// 111	o
	{ 0,				0 },			// 112	p
	{ 0,				0 },			// 113	q
	{ 0,				0 },			// 114	r
	{ 0,				0 },			// 115	s
	{ 0,				0 },			// 116	t
	{ 0,				0 },			// 117	u
	{ 0,				0 },			// 118	v
	{ 0,				0 },			// 119	w
	{ 0,				0 },			// 120	x
	{ 0,				0 },			// 121	y
	{ 0,				0 },			// 122	z
	{ 0,				0 },			// 123	{
	{ 0,				0 },			// 124	|
	{ 0,				0 },			// 125	}
	{ 0,				0 },			// 126	~
	{ "#127",		4 },			// 127
	{ "#128",		4 },			// 128
	{ "#129",		4 },			// 129
	{ "#130",		4 },			// 130
	{ "#131",		4 },			// 131
	{ "#132",		4 },			// 132
	{ "#133",		4 },			// 133
	{ "#134",		4 },			// 134
	{ "#135",		4 },			// 135
	{ "#136",		4 },			// 136
	{ "#137",		4 },			// 137
	{ "#138",		4 },			// 138
	{ "#139",		4 },			// 139
	{ "#140",		4 },			// 140
	{ "#141",		4 },			// 141
	{ "#142",		4 },			// 142
	{ "#143",		4 },			// 143
	{ "#144",		4 },			// 144
	{ "#145",		4 },			// 145
	{ "#146",		4 },			// 146
	{ "#147",		4 },			// 147
	{ "#148",		4 },			// 148
	{ "#149",		4 },			// 149
	{ "#150",		4 },			// 150
	{ "#151",		4 },			// 151
	{ "#152",		4 },			// 152
	{ "#153",		4 },			// 153
	{ "#154",		4 },			// 154
	{ "#155",		4 },			// 155
	{ "#156",		4 },			// 156
	{ "#157",		4 },			// 157
	{ "#158",		4 },			// 158
	{ "#159",		4 },			// 159
	{ "nbsp",		4 },			// 160	 		&nbsp;
	{ "iexcl",		5 },			// 161	¡ 	inverted exclamation mark 	&iexcl; 	&#161;
	{ "cent",		4 },			// 162	¢ 	cent 	&cent; 	&#162;
	{ "pound",		5 },			// 163	£ 	pound 	&pound; 	&#163;
	{ "curren",		6 },			// 164	¤ 	currency 	&curren; 	&#164;
	{ "yen",			3 },			// 165	¥ 	yen 	&yen; 	&#165;
	{ "brvbar",		6 },			// 166	¦ 	broken vertical bar 	&brvbar; 	&#166;
	{ "sect",		4 },			// 167	§ 	section 	&sect; 	&#167;
	{ "uml",			3 },			// 168	¨ 	spacing diaeresis 	&uml; 	&#168;
	{ "copy",		4 },			// 169	© 	copyright 	&copy; 	&#169;
	{ "ordf",		4 },			// 170	ª 	feminine ordinal indicator 	&ordf; 	&#170;
	{ "laquo",		5 },			// 171	« 	angle quotation mark (left) 	&laquo; 	&#171;
	{ "not",			3 },			// 172	¬ 	negation 	&not; 	&#172;
	{ "shy",			3 },			// 173	­ 	soft hyphen 	&shy; 	&#173;
	{ "reg",			3 },			// 174	® 	registered trademark 	&reg; 	&#174;
	{ "macr",		4 },			// 175	¯ 	spacing macron 	&macr; 	&#175;
	{ "deg",			3 },			// 176	° 	degree 	&deg; 	&#176;
	{ "plusmn",		6 },			// 177	± 	plus-or-minus  	&plusmn; 	&#177;
	{ "sup2",		4 },			// 178	² 	superscript 2 	&sup2; 	&#178;
	{ "sup3",		4 },			// 179	³ 	superscript 3 	&sup3; 	&#179;
	{ "acute",		5 },			// 180	´ 	spacing acute 	&acute; 	&#180;
	{ "micro",		5 },			// 181	µ 	micro 	&micro; 	&#181;
	{ "para",		4 },			// 182	¶ 	paragraph 	&para; 	&#182;
	{ "middot",		6 },			// 183	· 	middle dot 	&middot; 	&#183;
	{ "cedil",		5 },			// 184	¸ 	spacing cedilla 	&cedil; 	&#184;
	{ "sup1",		4 },			// 185	¹ 	superscript 1 	&sup1; 	&#185;
	{ "ordm",		4 },			// 186	º 	masculine ordinal indicator 	&ordm; 	&#186;
	{ "raquo",		5 },			// 187	» 	angle quotation mark (right) 	&raquo; 	&#187;
	{ "frac14",		6 },			// 188	¼ 	fraction 1/4 	&frac14; 	&#188;
	{ "frac12",		6 },			// 189	½ 	fraction 1/2 	&frac12; 	&#189;
	{ "frac34",		6 },			// 190	¾ 	fraction 3/4 	&frac34; 	&#190;
	{ "iquest",		6 },			// 191	¿ 	inverted question mark 	&iquest; 	&#191;
	{ "Agrave",		6 },			// 192	À 	capital a, grave accent 	&Agrave; 	&#192;
	{ "Aacute",		6 },			// 193	Á 	capital a, acute accent 	&Aacute; 	&#193;
	{ "Acirc",		5 },			// 194	Â 	capital a, circumflex accent 	&Acirc; 	&#194;
	{ "Atilde",		6 },			// 195	Ã 	capital a, tilde 	&Atilde; 	&#195;
	{ "Auml",		4 },			// 196	Ä 	capital a, umlaut mark 	&Auml; 	&#196;
	{ "Aring",		5 },			// 197	Å 	capital a, ring 	&Aring; 	&#197;
	{ "AElig",		5 },			// 198	Æ 	capital ae 	&AElig; 	&#198;
	{ "Ccedil",		6 },			// 199	Ç 	capital c, cedilla 	&Ccedil; 	&#199;
	{ "Egrave",		6 },			// 200	È 	capital e, grave accent 	&Egrave; 	&#200;
	{ "Eacute",		6 },			// 201	É 	capital e, acute accent 	&Eacute; 	&#201;
	{ "Ecirc",		5 },			// 202	Ê 	capital e, circumflex accent 	&Ecirc; 	&#202;
	{ "Euml",		4 },			// 203	Ë 	capital e, umlaut mark 	&Euml; 	&#203;
	{ "Igrave",		6 },			// 204	Ì 	capital i, grave accent 	&Igrave; 	&#204;
	{ "Iacute",		6 },			// 205	Í 	capital i, acute accent 	&Iacute; 	&#205;
	{ "Icirc",		5 },			// 206	Î 	capital i, circumflex accent 	&Icirc; 	&#206;
	{ "Iuml",		4 },			// 207	Ï 	capital i, umlaut mark 	&Iuml; 	&#207;
	{ "ETH",			3 },			// 208	Ð 	capital eth, Icelandic 	&ETH; 	&#208;
	{ "Ntilde",		6 },			// 209	Ñ 	capital n, tilde 	&Ntilde; 	&#209;
	{ "Ograve",		6 },			// 210	Ò 	capital o, grave accent 	&Ograve; 	&#210;
	{ "Oacute",		6 },			// 211	Ó 	capital o, acute accent 	&Oacute; 	&#211;
	{ "Ocirc",		5 },			// 212	Ô 	capital o, circumflex accent 	&Ocirc; 	&#212;
	{ "Otilde",		6 },			// 213	Õ 	capital o, tilde 	&Otilde; 	&#213;
	{ "Ouml",		4 },			// 214	Ö 	capital o, umlaut mark 	&Ouml; 	&#214;
	{ "times",		5 },			// 215	× 	multiplication 	&times; 	&#215;
	{ "Oslash",		6 },			// 216	Ø 	capital o, slash 	&Oslash; 	&#216;
	{ "Ugrave",		6 },			// 217	Ù 	capital u, grave accent 	&Ugrave; 	&#217;
	{ "Uacute",		6 },			// 218	Ú 	capital u, acute accent 	&Uacute; 	&#218;
	{ "Ucirc",		5 },			// 219	Û 	capital u, circumflex accent 	&Ucirc; 	&#219;
	{ "Uuml",		4 },			// 220	Ü 	capital u, umlaut mark 	&Uuml; 	&#220;
	{ "Yacute",		6 },			// 221	Ý 	capital y, acute accent 	&Yacute; 	&#221;
	{ "THORN",		5 },			// 222	Þ 	capital THORN, Icelandic 	&THORN; 	&#222;
	{ "szlig",		5 },			// 223	ß 	small sharp s, German 	&szlig; 	&#223;
	{ "agrave",		6 },			// 224	à 	small a, grave accent 	&agrave; 	&#224;
	{ "aacute",		6 },			// 225	á 	small a, acute accent 	&aacute; 	&#225;
	{ "acirc",		5 },			// 226	â 	small a, circumflex accent 	&acirc; 	&#226;
	{ "atilde",		6 },			// 227	ã 	small a, tilde 	&atilde; 	&#227;
	{ "auml",		4 },			// 228	ä 	small a, umlaut mark 	&auml; 	&#228;
	{ "aring",		5 },			// 229	å 	small a, ring 	&aring; 	&#229;
	{ "aelig",		5 },			// 230	æ 	small ae 	&aelig; 	&#230;
	{ "ccedil",		6 },			// 231	ç 	small c, cedilla 	&ccedil; 	&#231;
	{ "egrave",		6 },			// 232	è 	small e, grave accent 	&egrave; 	&#232;
	{ "eacute",		6 },			// 233	é 	small e, acute accent 	&eacute; 	&#233;
	{ "ecirc",		5 },			// 234	ê 	small e, circumflex accent 	&ecirc; 	&#234;
	{ "euml",		4 },			// 235	ë 	small e, umlaut mark 	&euml; 	&#235;
	{ "igrave",		6 },			// 236	ì 	small i, grave accent 	&igrave; 	&#236;
	{ "iacute",		6 },			// 237	í 	small i, acute accent 	&iacute; 	&#237;
	{ "icirc",		5 },			// 238	î 	small i, circumflex accent 	&icirc; 	&#238;
	{ "iuml",		4 },			// 239	ï 	small i, umlaut mark 	&iuml; 	&#239;
	{ "eth",			3 },			// 240	ð 	small eth, Icelandic 	&eth; 	&#240;
	{ "ntilde",		6 },			// 241	ñ 	small n, tilde 	&ntilde; 	&#241;
	{ "ograve",		6 },			// 242	ò 	small o, grave accent 	&ograve; 	&#242;
	{ "oacute",		6 },			// 243	ó 	small o, acute accent 	&oacute; 	&#243;
	{ "ocirc",		5 },			// 244	ô 	small o, circumflex accent 	&ocirc; 	&#244;
	{ "otilde",		6 },			// 245	õ 	small o, tilde 	&otilde; 	&#245;
	{ "ouml",		4 },			// 246	ö 	small o, umlaut mark 	&ouml; 	&#246;
	{ "divide",		6 },			// 247	÷ 	division 	&divide; 	&#247;
	{ "oslash",		6 },			// 248	ø 	small o, slash 	&oslash; 	&#248;
	{ "ugrave",		6 },			// 249	ù 	small u, grave accent 	&ugrave; 	&#249;
	{ "uacute",		6 },			// 250	ú 	small u, acute accent 	&uacute; 	&#250;
	{ "ucirc",		5 },			// 251	û 	small u, circumflex accent 	&ucirc; 	&#251;
	{ "uuml",		4 },			// 252	ü 	small u, umlaut mark 	&uuml; 	&#252;
	{ "yacute",		6 },			// 253	ý 	small y, acute accent 	&yacute; 	&#253;
	{ "thorn",		5 },			// 254	þ 	small thorn, Icelandic 	&thorn; 	&#254;
	{ "yuml",		4 },			// 255	ÿ 	small y, umlaut mark 	&yuml; 	&#255;
};

const char *aString::blank = "";

#ifdef USE_UNIX
const char *aString::endline = "\n";
#endif
#ifdef USE_WIN32
const char *aString::endline = "\r\n";
#endif
const char *aString::whitespace = " \t\n\r";


aString::aString(size_t c) : aObject(),str(0),len(0),cap(0) {
	if(c>0) setCapacity(c);
}

aString::aString(const char *s,size_t l) : aObject() {
	if(s && *s) {
		if(!l) len = strlen(s);
		cap = len;
		str = (char *)malloc(cap+1);
		if(cap) memcpy(str,s,len+1);
		else *str = '\0';
	} else str = 0,len = 0,cap = 0;
}

aString::aString(const aString *s) : aObject() {
	if(!s || !s->str) str = 0,len = 0,cap = 0;
	else {
		len = s->len,cap = s->cap,str = (char *)malloc(cap+1);
		if(*s->str) memcpy(str,s->str,len+1);
		else *str = '\0';
	}
}

aString::aString(const aString &s) : aObject() {
	if(!s.str) str = 0,len = 0,cap = 0;
	else {
		len = s.len,cap = s.cap,str = (char *)malloc(cap+1);
		if(*s.str) memcpy(str,s.str,len+1);
		else *str = '\0';
	}
}

aString::~aString() {
	::free(str);
	str = 0,len = 0,cap = 0;
}

size_t aString::insert(long n,char c) {
	if(c!='\0') {
		if(len==cap) resize(0);
		if(n==len) {
			str[len] = c;
			str[++len] = '\0';
		} else {
			n = move(n,1);
			str[n] = c;
		}
	}
	return 1;
}

size_t aString::insert(long n,const char *s,size_t l) {
	if(s) {
		if(!l) l = strlen(s);
		resize(l);
		if(n==len) {
			memcpy(&str[len],s,l);
			len += l;
			str[len] = '\0';
		} else {
			n = move(n,l);
			memcpy(&str[n],s,l);
		}
	}
	return l;
}

size_t aString::insert(long n,const char *s,long o,long l) {
	if(s && *s) {
		if(o<0 || l<=0) {
			long l1 = (long)strlen(s);
			if(o<0) o = l1+o;
			if(l<=0) l = l1-o+l;
		}
		resize(l);
		if(n==len) {
			memcpy(&str[len],&s[o],l);
			len += l;
			str[len] = '\0';
		} else {
			n = move(n,l);
			memcpy(&str[n],&s[o],l);
		}
	}
	return l;
}

#if __WORDSIZE < 64
size_t aString::inserti32(long n,int32_t i) {
	if(i>=0 && i<=9) return insert(n,(char)('0'+i));
	char s[13],*p = s+12,c = 0;
	*p-- = '\0';
	if(i<0) c = '-',i = -i;
	for(; i; i/=10) *p-- = '0'+(i%10);
	if(c) *p-- = c;
	return insert(n,p+1);
}

size_t aString::insertu32(long n,uint32_t i) {
	if(i<=9) return insert(n,(char)('0'+i));
	char s[12],*p = s+11;
	*p-- = '\0';
	for(; i; i/=10) *p-- = '0'+(i%10);
	return insert(n,p+1);
}
#endif

size_t aString::inserti64(long n,int64_t i) {
	if(i>=0 && i<=9) return insert(n,(char)('0'+i));
	char s[22],*p = s+21,c = 0;
	*p-- = '\0';
	if(i<0) c = '-',i = -i;
	for(; i; i/=10) *p-- = '0'+(i%10);
	if(c) *p-- = c;
	return insert(n,p+1);
}

size_t aString::insertu64(long n,uint64_t i) {
	if(i<=9) return insert(n,(char)('0'+i));
	char s[21],*p = s+20;
	*p-- = '\0';
	for(; i; i/=10) *p-- = '0'+(i%10);
	return insert(n,p+1);
}

size_t aString::insert(long n,double f,int d,char c) {
	int64_t n1 = (int64_t)f;
	uint32_t n2,m = 1;
	char s[34],*p = s+33;
	if(d>10) d = 10;
	while(0<d--) m *= 10;
	n2 = (uint32_t)round(fabs(f-n1)*m);

	*p-- = '\0';
	for(; d && n2; --d,n2/=10) *p-- = '0'+(n2%10);
	*p-- = c;
	if(n1<0) c = '-',n1 = -n1;
	else c = 0;
	for(; n1; n1/=10) *p-- = '0'+(n1%10);
	if(c) *p-- = c;
	return insert(n,p+1);
}

size_t aString::insertHex(long n,uint64_t i,bool upper) {
	const char *s = upper? upper_hex : lower_hex;
	if(i<=0xf) return insert(n,s[i&0xf]);
	char h[17],*p = h+16;
	*p-- = '\0';
	for(; i; i>>=4) *p-- = s[i&0xf];
	return insert(n,p+1);
}

size_t aString::insertBase(long n,int64_t i,int base) {
	if(i==0) return insert(n,'0');
	char s[22],*p = s+21,c = 0,b;
	*p-- = '\0';
	if(i<0) c = '-',i = -i;
	for(; i; i/=base) b = i%base,*p-- = b<=9? '0'+b : 'A'+b-10;
	if(c) *p-- = c;
	return insert(n,p+1);
}

size_t aString::insertBase(long n,uint64_t i,int base) {
	if(i==0) return insert(n,'0');
	char s[22],*p = s+21,b;
	*p-- = '\0';
	for(; i; i/=base) b = i%base,*p-- = b<=9? '0'+b : 'A'+b-10;
	return insert(n,p+1);
}

size_t aString::appendf(const char *f, ...) {
	va_list list;
	va_start(list,f);
	size_t l = vappendf(f,list);
	va_end(list);
	return l;
}

enum {
	F_PLUS			= 0x0001,
	F_MINUS			= 0x0002,
	F_HASH			= 0x0008,
	F_PRECISION		= 0x0080,
	F_h				= 0x0200,
	F_l				= 0x0400,
	F_ll				= 0x0800,
	F_L				= 0x1000,
	F_z				= 0x2000,
};

size_t aString::vappendf(const char *f,va_list list) {
	if(!f || !*f) return 0;
	int c;
	uint32_t flags,*w,w1,w2,w2default,ln;
	char pad;
	size_t l = len;
	while(1) {
//debug_output("aString::vappendf(f=%s)\n",f);
		while((c=*f++) && c!='%') {
			if(len==cap) resize(0);
			str[len++] = c;
		}
		if(c=='\0') break;
		c = *f++,flags = 0,w=&w1,w1=0,w2=0,w2default=2,pad = ' ';
		while(c) {
			if(c=='0') pad = '0';
			else if(c==' ') pad = ' ';
			else if(c=='-') flags |= F_MINUS;
			else if(c=='+') flags |= F_PLUS;
			else if(c=='#') flags |= F_HASH;
			else if(c=='.') flags |= F_PRECISION,w = &w2;
			else if(c>='0' && c<='9') {
				*w = *w*10+(c-'0');
				if(flags&F_PRECISION) w2default = 0;
			} else if(c=='*') *w = va_arg(list,int);
			else if(c=='h') flags |= F_h;
			else if(c=='l') {
				if(flags&F_l) flags |= F_ll;
				else flags |= F_l;
			} else if(c=='L') flags |= F_L;
			else if(c=='z') flags |= F_z;
			else break;
			c = *f++;
		}
		switch(c) {
			case '%':
			case 'c':
			case 'C':
				if(c=='c' || c=='C') c = va_arg(list,int);
				if(w1>0) insert(len,(const char)c,w1);
				else insert(len,c);
				break;
			case 'd':
			case 'D':
			case 'i':
			case 'I':
			{
				if(flags&F_z) {
					size_t n = va_arg(list,size_t);
					if((flags&F_PLUS) && n>=0) insert(len,'+');
					insert(len,n);
				} else if(flags&F_ll) {
					long long n = va_arg(list,long long);
					if((flags&F_PLUS) && n>=0) insert(len,'+');
					insert(len,n);
				} else if(flags&F_l) {
					long n = va_arg(list,long);
					if((flags&F_PLUS) && n>=0) insert(len,'+');
					insert(len,n);
				} else {
					int n = va_arg(list,int);
					if((flags&F_PLUS) && n>=0) insert(len,'+');
					insert(len,n);
				}
				break;
			}
			case 'u':
			case 'U':
			{
				if(flags&F_PLUS) insert(len,'+');
				if(flags&F_z) {
					size_t n = va_arg(list,size_t);
					insert(len,n);
				} else if(flags&F_ll) {
					unsigned long long n = va_arg(list,unsigned long long);
					insert(len,n);
				} else if(flags&F_l) {
					unsigned long n = va_arg(list,unsigned long);
					insert(len,n);
				} else {
					unsigned int n = va_arg(list,unsigned int);
					insert(len,n);
				}
				break;
			}
			case 'x':
			case 'X':
			{
				uint64_t n = 0;
				if(flags&F_PLUS) insert(len,'+');
				if(flags&F_z) n = (uint64_t)va_arg(list,size_t);
				else if(flags&F_ll) n = (uint64_t)va_arg(list,unsigned long long);
				else if(flags&F_l) n = (uint64_t)va_arg(list,unsigned long);
				else n = (uint64_t)va_arg(list,unsigned int);
				insertHex(len,n,16);
				break;
			}
			case 'f':
			case 'F':
			{
				double n = va_arg(list,double);
//debug_output("n=%f,flags=%" PRIx32 "\n",n,flags);
				if((flags&F_PLUS) && n>=0.) insert(len,'+');
				insert(len,n,(int)(w2default? ((flags&F_L)? 6 : w2default) : w2));
				break;
			}
			case 's':
			case 'S':
			{
				char *s = va_arg(list,char *);
//debug_output("s=%s\n",s);
				ln = strlen(s);
				if(w2 && ln>w2) ln = w2;
				if(w1 && !(flags&F_MINUS) && ln<w1) insert(len,pad,w1-ln);
				insert(len,s,ln);
				if(w1 && (flags&F_MINUS) && ln<w1) insert(len,pad,w1-ln);
				break;
			}
		}
	}
	str[len] = '\0';
	return len-l;
}

size_t aString::appendUntil(const char **s,const char *end,const char *trim,bool uesc) {
	if(!s || !*s || !**s) return 0;
	if(!trim) trim = end;
	size_t l = len;
	if(!end || !*end) {
		insert(len,*s);
		l = strlen(*s),*s += l;
		return l;
	}
	const char *s2 = *s;
	int c,c2 = -1,t = trim && *trim? 0 : 1;
	unsigned long l2 = 0;
//debug_output("aString::appendUntil: ");
	while((c=*s2++)!='\0') {
//debug_putc(c);
		if(uesc) {
			if(c=='/') {
				c = *s2++;
				if(c=='/') {
					while((c=*s2++)!='\0' && c!='\r' && c!='\n');
					if(c=='\0') break;
					s2--;
					continue;
				} else if(c=='*') {
					c2 = -1;
					while((c=*s2++)!='\0') { if(c=='/' && c2=='*') break;c2 = c; }
					c2 = -1;
					continue;
				} else s2--,c = '/';
			} else if(c=='\\') {
				if((c=*s2++)=='\0') break;
				else if(c=='\\') c2 = '\\';
				else if(c=='n') c2 = '\n';
				else if(c=='t') c2 = '\t';
				else if(c=='r') c2 = '\r';
				else if(c=='f') c2 = '\f';
				else if(c=='v') c2 = '\v';
				else if(c=='b') c2 = '\b';
				else if(c=='a') c2 = '\a';
				else if(c=='0') c2 = '\0';
				else if(c=='\n' || c=='\r') {
					if((c2=*s2++)=='\0') break;
					else if((c=='\n' && c2!='\r') || (c=='\r' && c2!='\n')) s2--;
					c2 = -1;
					continue;
				}
				else c2 = c;
			}
		}
		if(!t && !strchr(trim,c)) t = 1;
		if(t) {
			if(end && strchr(end,c)) break;
			if(len==cap) resize(0);
			if(c2!=-1) str[len++] = (char)c2,c2 = -1,l2 = len;
			else str[len++] = (char)c;
		}
	}
	if(trim && *trim!='\0') while(len>l2 && strchr(trim,str[len-1])) len--;
	str[len] = '\0';
//debug_output("[%s]\n",str);
	*s = s2;
	return len-l;
}

size_t aString::appendUntil(FILE *fp,const char *end,const char *trim,bool uesc) {
	if(!fp) return 0;
	if(!trim) trim = end;
	if(!end) {
		fpos_t pos;
		if(!fgetpos(fp,&pos)) {
			long fpos = ftell(fp);
			fseek(fp,0,SEEK_END);
			long fend = ftell(fp);
			if(fpos>=0 && fend>=0 && fend-fpos>(long)cap) resize(fend-fpos);
			fsetpos(fp,&pos);
		}
	}
	size_t l = len;
	int c,c2 = -1,t = trim && *trim? 0 : 1;
	unsigned long l2 = 0;
//debug_output("aString::appendUntil: ");
	while((c=fgetc(fp)) && c!=EOF) {
//debug_putc(c);
		if(uesc) {
			if(c=='/') {
				c = fgetc(fp);
				if(c=='/') {
					while((c=fgetc(fp))!=EOF && c!='\r' && c!='\n');
					if(c==EOF) break;
					ungetc(c,fp);
					continue;
				} else if(c=='*') {
					c2 = -1;
					while((c=fgetc(fp))!=EOF) { if(c=='/' && c2=='*') break;c2 = c; }
					c2 = -1;
					continue;
				} else { ungetc(c,fp);c = '/'; }
			} else if(c=='\\') {
				if((c=fgetc(fp))==EOF) break;
				else if(c=='\\') c2 = '\\';
				else if(c=='n') c2 = '\n';
				else if(c=='t') c2 = '\t';
				else if(c=='r') c2 = '\r';
				else if(c=='f') c2 = '\f';
				else if(c=='v') c2 = '\v';
				else if(c=='b') c2 = '\b';
				else if(c=='a') c2 = '\a';
				else if(c=='0') c2 = '\0';
				else if(c=='\n' || c=='\r') {
					if((c2=fgetc(fp))==EOF) break;
					else if((c=='\n' && c2!='\r') || (c=='\r' && c2!='\n')) ungetc(c2,fp);
					c2 = -1;
					continue;
				}
				else c2 = c;
			}
		}
		if(!t && !strchr(trim,c)) t = 1;
		if(t) {
			if(end && strchr(end,c)) break;
			if(len==cap) resize(0);
			if(c2!=-1) str[len++] = (char)c2,c2 = -1,l2 = len;
			else str[len++] = (char)c;
		}
	}
	if(trim && *trim!='\0') while(len>l2 && strchr(trim,str[len-1])) len--;
	str[len] = '\0';
//debug_output("[%s]\n",str);
	return len-l;
}

size_t aString::repeat(char c,size_t n) {
	resize(n);
	for(size_t i=0; i<n; ++i) str[len++] = c;
	str[len] = '\0';
	return n;
}

size_t aString::repeat(const char *s,size_t l,size_t n) {
	if(s && n>0) {
		if(!l) l = strlen(s);
		resize(l*n);
		for(size_t i=0; i<n; ++i) {
			memcpy(&str[len],s,l);
			len += l;
		}
		str[len] = '\0';
	}
	return l*n;
}

size_t aString::include(const char *fn) {
	FILE *fp = fopen(fn,"r");
	size_t l = append(fp,true);
	fclose(fp);
	return l;
}

size_t aString::includef(const char *format, ...) {
	char buf[128];
	va_list args;
   va_start(args,format);
	vsnprintf(buf,127,format,args);
   va_end(args);
	return include(buf);
}

size_t aString::print(FILE *fp) {
	if(fp) return fwrite(str,len,1,fp);
	return 0;
}

size_t aString::println(FILE *fp) {
	size_t n = 0;
	if(fp) {
		n += fwrite(str,len,1,fp);
		n += fwrite(endline,strlen(endline),1,fp);
	}
	return n;
}

long aString::find(char c,long o,long l) {
	if(str && *str && c!='\0') {
		if(o<0) o = (long)len+o;
		if(l<=0) l = (long)len-o+l;
		if(o>=0 && l>0 && o+l<=len) {
			for(size_t i=o,n=o+l; i<n; ++i)
				if(str[i]==c) return i;
		}
	}
	return -1;
}

long aString::find(const char *s,long o,long l,long sl) {
	if(str && *str && s && *s) {
		if(o<0) o = (long)len+o;
		if(l<=0) l = (long)len-o+l;
		if(sl<=0) sl = (long)strlen(s)+sl;
		if(o>=0 && l>0 && sl>0 && o+l<=len) {
			for(size_t i=o,n=o+l; i<n; ++i)
				if(str[i]==*s && strncmp(&str[i],s,sl)==0) return i;
		}
	}
	return -1;
}

long aString::findChar(const char *s,long o,long l) {
	if(str && *str && s && *s) {
		if(o<0) o = (long)len+o;
		if(l<=0) l = (long)len-o+l;
		if(o>=0 && l>0 && o+l<=len) {
			for(size_t i=o,n=o+l; i<n; ++i)
				if(strchr(s,str[i])) return i;
		}
	}
	return -1;
}

bool aString::equals(const char *s,long o,long l) {
	if(str && *str && s && *s) {
		if(o<0) o = (long)len+o;
		if(l<=0) l = (long)strlen(s)+l;
		if(o>=0 && l>0 && o+l<=len)
			return strncmp(&str[o],s,l)==0;
	}
	return false;
}

int aString::compare(const char *s,long o,long l) {
	if(str && *str && s && *s) {
		if(o<0) o = (long)len+o;
		if(l<=0) l = (long)strlen(s)+l;
		if(o>=0 && l>0 && o+l<=len)
			return strncmp(&str[o],s,l);
	}
	return str? -1 : 1;
}

long aString::matchNestedTags(const char *tag1,const char *tag2,long o,long l,const char *c1,const char *c2) {
	size_t i,n,l1 = strlen(tag1),l2 = strlen(tag2);
	int nest;
	if(o<0) o = (long)len+o;
	if(l<=0) l = (long)len-o+l;
	for(i=o,n=o+l,nest=0; i<n; ++i) {
		if(str[i]==*tag1 && (l1==1 || !strncmp(&str[i],tag1,l1)) &&
				(!c1 || strchr(c1,str[i+l1]))) i += l1-1,++nest;
		else if(str[i]==*tag2 && (l2==1 || !strncmp(&str[i],tag2,l2)) &&
				(!c2 || strchr(c2,str[i+l2]))) {
			if(--nest==0) return i;
			i += l2-1;
		}
	}
	return -1;
}

size_t aString::count(char c) {
	if(c=='\0' || !str || !len) return 0;
	char *p = str;
	size_t n = 0;
	for(; *p; p++) if(*p==c) ++n;
	return n;
}

size_t aString::count(const char *s) {
	if(!s || !*s || !str || !len) return 0;
	char *p = str;
	size_t n = 0,sl = strlen(s);
	for(; *p && (p=strstr(p,s)); p+=sl,++n);
	return n;
}

size_t aString::replace(const char *s,const char *r) {
	if(!s || !*s || !r) return 0;
	size_t n,l = len;
	n = count(s);
	if(n>0) {
		char *p = str,*p1 = p,*p2;
		size_t sl = strlen(s);
		cap = cap+(strlen(r)-sl)*n;
		str = (char *)malloc(cap+1);
		*str = '\0';
		len = 0;
		for(; *p1 && (p2=strstr(p1,s)); p1=p2+sl) {
			n = (size_t)(p2-p1);
			if(n) insert(len,p1,n);
			insert(len,r);
		}
		if(*p1) insert(len,p1);
		::free(p);
	}
	return len-l;
}

size_t aString::replace(const char *s, ...) {
	size_t i,n,c = 32,l = len;
	va_list vl;
	const char **arr = (const char **)malloc(c*sizeof(const char *)),*vp = s;
	va_start(vl,s);
//debug_output("aString::replace(arr[%d]: \"%s\")\n",0,s);
	for(n=1,arr[0]=vp; 1; ++n) {
		vp = va_arg(vl,const char *);
		arr[n] = vp;
		if(!vp) break;
//debug_output("aString::replace(arr[%d]: \"%s\")\n",(int)n,vp? vp : "NULL");
		if(n==c) c <<= 1,arr = (const char **)realloc(arr,c*sizeof(const char *));
	}
	va_end(vl);
	replace(arr);
	::free(arr);
	return len-l;
}

size_t aString::replace(const char **arr) {
	size_t i,n,c = 32,l = len;
	for(n=0; arr[n]; ++n);
	if(n>1) {
		char *p = str;
		size_t p1;
		int ln[n],r;
//debug_output("aString::replace(n: %d,cap: %d)\n",(int)n,(int)cap);
		for(i=0; i<n; ++i) ln[i] = strlen(arr[i]);
		str = (char *)malloc(cap+1),len = 0;
		for(p1=0,r=0; p[p1]!='\0'; ++p1) {
			for(i=0; i<n; i+=2)
				if(p[p1]==*arr[i] && !strncmp(&p[p1],arr[i],ln[i])) {
					if(len+ln[i+1]+2==cap) resize(ln[i+1]+2);
					memcpy(&str[len],arr[i+1],ln[i+1]);
					p1 += ln[i]-1,len += ln[i+1],r = 1;
					break;
				}
			if(r) r = 0;
			else {
				if(len+2==cap) resize(0);
				str[len++] = p[p1];
			}
		}
		str[len] = '\0';
		::free(p);
	}
	return len-l;
}

size_t aString::stripComments() {
	char *p1,*p2,*p3;
	size_t n;
	for(p1=str,p2=p1,n=0; *p1!='\0'; ++p1) {
		if(*p1=='/' && p1[1]=='/' || p1[1]=='*') {
			if(p1[1]=='/') {
				for(p1+=2; *p1!='\n' && *p1!='\r'; ++p1);
				*p2++ = *p1,++n;
			} else {
				p3 = strstr(p1+2,"*/");
				if(!p3) break;
				else p1 = p3+1,++n;
			}
		} else *p2++ = *p1;
	}
	*p2 = '\0';
	return n;
}

size_t aString::stripHTML() {
	char *p1,*p2,*p3,*p4;
	size_t n;
	for(p1=str,p2=p1,n=0; *p1!='\0'; ++p1) {
		if(*p1=='<') {
			p3 = strchr(p1+1,'>');
			p4 = strchr(p1+1,'<');
			if(!p3) break;
			else if(p4 && p4<p3) *p2++ = *p1;
			else p1 = p3;
			++n;
		} else *p2++ = *p1;
	}
	*p2 = '\0';
	return n;
}

size_t aString::stripHTMLComments() {
	char *p1,*p2,*p3;
	size_t n;
	for(p1=str,p2=p1,n=0; *p1!='\0'; ++p1) {
		if(*p1=='<' && p1[1]=='!' && p1[2]=='-' && p1[3]=='-') {
			p3 = strstr(p1+4,"-->");
			if(!p3) break;
			else p1 = p3+2;
			++n;
		} else *p2++ = *p1;
	}
	*p2 = '\0';
	return n;
}

size_t aString::substr(aString &s,long o,long l) {
	if(str && *str) {
		if(o<0) o = len+o;
		if(l<=0) l = len-o+l;
		if(l+2>s.cap) s.resize(l+2);
		memcpy(s.str,&str[o],l);
		s.str[l] = '\0',s.len = l;
		return l;
	}
	s.clear();
	return 0;
}

size_t aString::substr(char *s,long o,long l) {
	if(str && *str) {
		if(o<0) o = len+o;
		if(l<=0) l = len-o+l;
		memcpy(s,&str[o],l);
		s[l] = '\0';
		return l;
	}
	*s = '\0';
	return 0;
}

void aString::newline(const char *nl) {
	size_t p1,p2,l = strlen(nl);
	char *p = str;
	str = (char *)malloc(cap+1);
	for(p1=0,p2=0; p1<len; ++p1) {
		if(p2+l+2==cap) resize(0);
		if(p[p1]=='\n' || p[p1]=='\r') {
			if((p[p1]=='\n' && p[p1+1]=='\r') ||
				(p[p1]=='\r' && p[p1+1]=='\n')) ++p1;
			if(l>1) memcpy(&str[p2],nl,l);
			else str[p2] = *nl;
			p2 += l;
		} else str[p2++] = p[p1];
	}
	str[p2] = '\0';
	::free(p);
}

void aString::escape() {
	if(str && len) {
		size_t i,n = 0;
		for(i=0; i<len; i++) if(isEscSpace(str[i])) n++;
		if(n) {
			if(len+n>=cap) resize(n);
			char c;
			for(i=len-1,c=str[i],n=0; i>=0; c=str[--i]) {
				if(c=='\t') str[i-- +n] = '\\',c = '\\',n++,len++;
				else if(c=='\n') str[i-- +n] = '\\',c = '\\',n++,len++;
				str[i+n] = c;
			}
			str[len] = '\0';
		}
	}
}

void aString::unescape() {
	if(str && len) {
		size_t i,n;
		char c;
		for(i=0,n=0,c=str[i]; i<len; i++,c=str[i+n]) {
			if(c=='\\') {
				n++,c = str[i+n],len--;
				if(c=='t') c = '\t';
				else if(c=='n') c = '\n';
			}
			str[i] = c;
		}
		str[len] = '\0';
	}
}

void aString::quote(const char c) {
	if(c && str && len) {
		if(len+2>=cap) resize(0);
		*str = c;
		for(size_t i=0; i<len; i++) str[i+1] = str[i];
		str[++len] = c,str[++len] = '\0';
	}
}

void aString::unquote() {
	if(str && len>1) {
		char c = *str;
		if((c=='"' || c=='\'') && str[len-1]==c) {
			for(size_t i=1; i<len; i++) str[i-1] = str[i];
			str[len-=2] = '\0';
		}
	}
}

void aString::encodeURL() {
	if(str && len) {
		size_t i,n = 0;
		for(i=0; i<len; i++) if(isURLEncoded(str[i]) && str[i]!=' ') n += 2;
		if(n) {
			if(len+n>=cap) resize(n);
			unsigned char c;
			char *p1=&str[len-1],*p2=&str[len+n];
			*p2-- = '\0',len += n;
			while(p1>=str) {
				if(*p1==' ') *p2-- = '+',p1--;
				else if(isURLEncoded(*p1)) {
					c = *p1, c &= 0xf,*p2-- = (c>9? 'A'+c-10 : '0'+c);
					c = *p1,c >>= 4,*p2-- = (c>9? 'A'+c-10 : '0'+c);
					*p2-- = '%',p1--;
				} else *p2-- = *p1--;
			}
		}
	}
}

void aString::decodeURL() {
	if(str && len) {
		size_t n = 0;
		int c1,c2;
		char *p1 = str,*p2 = p1,*p3 = &str[len];
		while(p1<=p3) {
			if(*p1=='+') *p2++ = ' ',p1++;
			else if(*p1=='%') {
				c1 = p1[1],c1 = c1>='a' && c1<='f'? (c1+10-'a') : (c1>='A' && c1<='F'? (c1+10-'A') : (c1>='0' && c1<='9'? c1-'0' : -1));
				c2 = p1[2],c2 = c2>='a' && c2<='f'? (c2+10-'a') : (c2>='A' && c2<='F'? (c2+10-'A') : (c2>='0' && c2<='9'? c2-'0' : -1));
				if(c1==-1) *p2++ = *p1++;
				else if(c2==-1) p1 += 2,*p2++ = c1,n++;
				else p1 += 3,*p2++ = (c1<<4)|c2,n += 2;
			} else *p2++ = *p1++;
		}
		len -= n;
	}
}

#define select_entity(p) \
c = (unsigned char)p;\
if((c=='\'' || c=='"') && !(f&aSTRING_HTML_QUOTES)) e = 0;\
else if((c=='&') && !(f&aSTRING_HTML_AMP)) e = 0;\
else if((c=='<' || c=='>') && !(f&aSTRING_HTML_LTGT)) e = 0;\
else {\
	e = &HTMLentities[c];\
	if(e->len) {\
		if(*e->name=='#' && !(f&aSTRING_HTML_CODES)) e = 0;\
	}\
}

void aString::encodeHTML(int f) {
	if(str && len) {
		size_t i,n = 0;
		unsigned char c;
		const entity *e;
		for(i=0; i<len; ++i) {
			select_entity(str[i])
			if(e && e->len) n += e->len+1;
		}
		if(n) {
			if(len+n>=cap) resize(n);
			char *p1=&str[len-1],*p2=&str[len+n];
			*p2-- = '\0',len += n;
			while(p1>=str) {
				select_entity(*p1)
				if(e && e->len) {
					*p2 = ';',p2 -= e->len;
					memcpy(p2,e->name,e->len);
					--p2,*p2-- = '&',--p1;
				} else *p2-- = *p1--;
			}
		}
	}
}

void aString::decodeHTML() {
	if(str && len) {
		size_t i,n = 0;
		const entity *e;
		char *p1 = str,*p2 = p1,*p3 = &str[len];
		while(p1<=p3) {
			if(*p1=='&') {
				p1++;
				if(*p1=='#') {
					p1++,n++;
					if(*p1=='x' || *p1=='X') i = fromHex(++p1),n++;
					else i = atoi(p1);
					if(i>255) *p2++ = '?';
					else *p2++ = i;
					while(*p1!=';' && isHex(*p1)) p1++,n++;
					p1++,n++;
				} else {
					for(i=0; i<255; i++) {
						e = &HTMLentities[i];
						if(e->len && *p1==*e->name && !strncmp(p1,e->name,e->len)) {
							*p2++ = i,p1 += e->len,n += e->len;
							break;
						}
					}
					if(*p1==';') p1++,n++;
				}
			} else *p2++ = *p1++;
		}
		len -= n;
	}
}

void aString::trim(long &o,long &l) {
	if(str && *str) {
		if(o<0) o = (long)len+o;
		if(l<=0) l = (long)len-o;
		if(o>=0 && l>0 && o+l<=len)
		while(isSpace(str[o]) && o<len) ++o,--l;
		while(isSpace(str[o+l]) && l>0) --l;
	}
}

void aString::free() {
	::free(str);
	str = 0,len = 0,cap = 0;
}

void aString::resize(size_t l) {
	if(l<0 || len+l+l<cap) return;
	if(!cap) cap = 1;
	if(l && len+l>=cap) cap = len+l+1;
	if(cap<1024) cap <<= 1;
	else cap += 1024;
	if(!str) str = (char *)malloc(cap+1);
	else str = (char *)realloc(str,cap+1);
	str[cap] = '\0';
//debug_output("aString::resize(len=%d,l=%d,cap=%d)\n",len,l,cap);
}

long aString::move(long n,size_t l) {
	if(n<0) n = len+n;
	if(n>=0 && n<=len && l>0) {
		if(len+l+2>=cap) resize(l);
		if(n<len) {
			memmove(&str[n+l],&str[n],len+1-n);
			len += l;
		}
	}
	return n;
}

void aString::setCapacity(size_t n) {
	if(!n) return;
	cap = n;
	if(!str) str = (char *)malloc(cap+1);
	else str = (char *)realloc(str,cap+1);
	str[cap] = '\0';
//debug_output("aString::resize(len=%d,n=%d,cap=%d)\n",len,n,cap);
}

char aString::charAt(long i) {
	if(i<0) i = (long)len+i;
	return str && i>=0 && i<(long)len? str[i] : '\0';
}

long aString::toInt() {
	char *p = str;
	while(isSpace(*p)) ++p;
	if(*p=='0' && *++p=='x') return fromHex(++p);
	return atol(p);
}

size_t aString::toIntArray(long *n,char c) {
	if(!n || c=='\0' || !str || !len) return 0;
	size_t i;
	char *p1 = str,*p2;
	while(*p1 && !isDigit(*p1)) ++p1;
	if(!*p1) return 0;
	for(i=0; p1 && *p1; ++i,p1=p2) {
		p2 = strchr(p1,c);
		if(p2) *p2++ = '\0';
		if(*p1=='0' && *++p1=='x') n[i] = fromHex(++p1);
		else n[i] = atol(p1);
	}
	return i;
}


size_t aString::nextWord(const char **s,const char *c) {
	size_t l = 0;
	const char *p = *s;
	while(*p && !strchr(c,*p)) p++,l++;
	*s = *p==' '? p+1 : 0;
	return l;
}


uint64_t aString::fromHex(const char *str) {
	if(!str) return 0;
	uint64_t n = 0;
	for(int i=0; isHex(*str) && i<16; i++) n = (n<<4)|fromHex(*str),str++;
	return n;
}

char *aString::toHex(char *h,uint64_t i,bool upper) {
	int u = 0;
	const char *s = upper? upper_hex : lower_hex;
	for(uint64_t n=i; n; n>>=4) u++;
	for(h+=u,*h='\0'; i; i>>=4) *--h = s[i&0xf];//(u=(i&0xf))<10? '0'+u : 'a'+(u-10);
	return h;
}

char *aString::toLower(char *str) { char *s = str;while(*s) *s = toLower(*s),s++;return str; }
char *aString::toUpper(char *str) { char *s = str;while(*s) *s = toUpper(*s),s++;return str; }

int aString::stricmp(const char *str1,const char *str2) {
	char c1 = toLower(*str1++),c2 = toLower(*str2++);
	for(; c1 && c2; c1=toLower(*str1++),c2=toLower(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (!c1 && !c2)? 0 : (!c1? -1 : 1);
}

int aString::strnicmp(const char *str1,const char *str2,size_t n) {
	char c1 = toLower(*str1++),c2 = toLower(*str2++);
	for(; --n && c1 && c2; c1=toLower(*str1++),c2=toLower(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (!n || (!c1 && !c2))? 0 : (!c1? -1 : 1);
}

char *aString::stristr(char *str1,const char *str2) {
	int l = strlen(str2),c1,c2 = toLower(*str2);
	for(; (c1=toLower(*str1)); str1++) if(c1==c2 && strnicmp(str1,str2,l)) return str1;
	return 0;
}

int aString::countTokens(char *str,const char *delim,bool cins) {
	int l = strlen(delim),n = 0;
	if(cins) for(; str && *str; n++) { if((str=stristr(str,delim))) str += l; }
	else for(; str && *str; n++) { if((str=strstr(str,delim))) str += l; }
	return n;
}

char **aString::split(char **list,char *str,const char *delim,bool cins) {
	size_t l = strlen(delim),n = 0;
	if(cins) for(; str && *str; n++) { list[n] = str;if((str=stristr(str,delim))) *str = '\0',str += l; }
	else for(; str && *str; n++) { list[n] = str;if((str=strstr(str,delim))) *str = '\0',str += l; }
	return list;
}

size_t aString::reverse(char *str) {
	if(!str || !*str) return 0;
	size_t l = strlen(str);
	char *p1 = str,*p2 = &str[l-1],c;
	while(p1<p2) c = *p1,*p1++ = *p2,*p2-- = c;
	return l;
}

size_t aString::trim(char *str) {
	if(!str || !*str) return 0;
	size_t l = 0;
	char *p = str;
	while(*p && isSpace(*p)) ++p;
	while(*p) *str++ = *p++,++l;
	if(l) {
		while(isSpace(*--str)) --l;
		*++str = '\0';
	} else *str = '\0';
	return l;
}

void aString::printUTF8(char *d,const char *s,size_t o,size_t l) {
	char c;
	size_t i,n;
	if(o) for(i=0; *s!='\0' && i<o; i++) {
		if(*s&0x80) {
			for(c=(*s<<1),n=1; c&0x80 && n<8; c<<=1,n++);
			while(n--) s++;
		} else s++;
	}
	if(l) for(i=0; *s!='\0' && i<l; i++) {
		if(*s&0x80) {
			for(c=(*s<<1),n=1; c&0x80 && n<8; c<<=1,n++);
			while(n--) *d++ = *s++;
		} else *d++ = *s++;
	}
	*d = '\0';
}

bool aString::isHTMLEntity(unsigned char c) {
	return HTMLentities[c].name!=0;
}


