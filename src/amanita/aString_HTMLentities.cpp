
#include "_config.h"
#include <amanita/aString.h>


const aString::entity aString::HTMLentities[256] = {
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

