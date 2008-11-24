
#include "config.h"
#include <libamanita/String.h>


const String::entity String::HTMLentities[256] = {
	{ "#0",			2ul },			// 0
	{ "#1",			2ul },			// 1
	{ "#2",			2ul },			// 2
	{ "#3",			2ul },			// 3
	{ "#4",			2ul },			// 4
	{ "#5",			2ul },			// 5
	{ "#6",			2ul },			// 6
	{ "#7",			2ul },			// 7
	{ "#8",			2ul },			// 8
	{ "#9",			2ul },			// 9
	{ "#10",			3ul },			// 10
	{ "#11",			3ul },			// 11
	{ "#12",			3ul },			// 12
	{ "#13",			3ul },			// 13
	{ "#14",			3ul },			// 14
	{ "#15",			3ul },			// 15
	{ "#16",			3ul },			// 16
	{ "#17",			3ul },			// 17
	{ "#18",			3ul },			// 18
	{ "#19",			3ul },			// 19
	{ "#20",			3ul },			// 20
	{ "#21",			3ul },			// 21
	{ "#22",			3ul },			// 22
	{ "#23",			3ul },			// 23
	{ "#24",			3ul },			// 24
	{ "#25",			3ul },			// 25
	{ "#26",			3ul },			// 26
	{ "#27",			3ul },			// 27
	{ "#28",			3ul },			// 28
	{ "#29",			3ul },			// 29
	{ "#30",			3ul },			// 30
	{ "#31",			3ul },			// 31
	{ 0,				0ul },			// 32		<SPACE>
	{ 0,				0ul },			// 33		!
	{ "quot",		4ul },			// 34		"		&quot;
	{ 0,				0ul },			// 35		#
	{ 0,				0ul },			// 36		$
	{ 0,				0ul },			// 37		%
	{ "amp",			3ul },			// 38		&		&amp;
	{ "#39",			3ul },			// 39		'		&apos;
	{ 0,				0ul },			// 40		(
	{ 0,				0ul },			// 41		)
	{ 0,				0ul },			// 42		*
	{ 0,				0ul },			// 43		+
	{ 0,				0ul },			// 44		,
	{ 0,				0ul },			// 45		-
	{ 0,				0ul },			// 46		.
	{ 0,				0ul },			// 47		/
	{ 0,				0ul },			// 48		0
	{ 0,				0ul },			// 49		1
	{ 0,				0ul },			// 50		2
	{ 0,				0ul },			// 51		3
	{ 0,				0ul },			// 52		4
	{ 0,				0ul },			// 53		5
	{ 0,				0ul },			// 54		6
	{ 0,				0ul },			// 55		7
	{ 0,				0ul },			// 56		8
	{ 0,				0ul },			// 57		9
	{ 0,				0ul },			// 58		:
	{ 0,				0ul },			// 59		;
	{ "lt",			2ul },			// 60		<		&lt;
	{ 0,				0ul },			// 61		=
	{ "gt",			2ul },			// 62		>		&gt;
	{ 0,				0ul },			// 63		?
	{ 0,				0ul },			// 64		@
	{ 0,				0ul },			// 65		A
	{ 0,				0ul },			// 66		B
	{ 0,				0ul },			// 67		C
	{ 0,				0ul },			// 68		D
	{ 0,				0ul },			// 69		E
	{ 0,				0ul },			// 70		F
	{ 0,				0ul },			// 71		G
	{ 0,				0ul },			// 72		H
	{ 0,				0ul },			// 73		I
	{ 0,				0ul },			// 74		J
	{ 0,				0ul },			// 75		K
	{ 0,				0ul },			// 76		L
	{ 0,				0ul },			// 77		M
	{ 0,				0ul },			// 78		N
	{ 0,				0ul },			// 79		O
	{ 0,				0ul },			// 80		P
	{ 0,				0ul },			// 81		Q
	{ 0,				0ul },			// 82		R
	{ 0,				0ul },			// 83		S
	{ 0,				0ul },			// 84		T
	{ 0,				0ul },			// 85		U
	{ 0,				0ul },			// 86		V
	{ 0,				0ul },			// 87		W
	{ 0,				0ul },			// 88		X
	{ 0,				0ul },			// 89		Y
	{ 0,				0ul },			// 90		Z
	{ 0,				0ul },			// 91		[
	{ 0,				0ul },			// 92		'\'
	{ 0,				0ul },			// 93		]
	{ 0,				0ul },			// 94		^
	{ 0,				0ul },			// 95		_
	{ 0,				0ul },			// 96		`
	{ 0,				0ul },			// 97		a
	{ 0,				0ul },			// 98		b
	{ 0,				0ul },			// 99		c
	{ 0,				0ul },			// 100	d
	{ 0,				0ul },			// 101	e
	{ 0,				0ul },			// 102	f
	{ 0,				0ul },			// 103	g
	{ 0,				0ul },			// 104	h
	{ 0,				0ul },			// 105	i
	{ 0,				0ul },			// 106	j
	{ 0,				0ul },			// 107	k
	{ 0,				0ul },			// 108	l
	{ 0,				0ul },			// 109	m
	{ 0,				0ul },			// 110	n
	{ 0,				0ul },			// 111	o
	{ 0,				0ul },			// 112	p
	{ 0,				0ul },			// 113	q
	{ 0,				0ul },			// 114	r
	{ 0,				0ul },			// 115	s
	{ 0,				0ul },			// 116	t
	{ 0,				0ul },			// 117	u
	{ 0,				0ul },			// 118	v
	{ 0,				0ul },			// 119	w
	{ 0,				0ul },			// 120	x
	{ 0,				0ul },			// 121	y
	{ 0,				0ul },			// 122	z
	{ 0,				0ul },			// 123	{
	{ 0,				0ul },			// 124	|
	{ 0,				0ul },			// 125	}
	{ 0,				0ul },			// 126	~
	{ "#127",		4ul },			// 127
	{ "#128",		4ul },			// 128
	{ "#129",		4ul },			// 129
	{ "#130",		4ul },			// 130
	{ "#131",		4ul },			// 131
	{ "#132",		4ul },			// 132
	{ "#133",		4ul },			// 133
	{ "#134",		4ul },			// 134
	{ "#135",		4ul },			// 135
	{ "#136",		4ul },			// 136
	{ "#137",		4ul },			// 137
	{ "#138",		4ul },			// 138
	{ "#139",		4ul },			// 139
	{ "#140",		4ul },			// 140
	{ "#141",		4ul },			// 141
	{ "#142",		4ul },			// 142
	{ "#143",		4ul },			// 143
	{ "#144",		4ul },			// 144
	{ "#145",		4ul },			// 145
	{ "#146",		4ul },			// 146
	{ "#147",		4ul },			// 147
	{ "#148",		4ul },			// 148
	{ "#149",		4ul },			// 149
	{ "#150",		4ul },			// 150
	{ "#151",		4ul },			// 151
	{ "#152",		4ul },			// 152
	{ "#153",		4ul },			// 153
	{ "#154",		4ul },			// 154
	{ "#155",		4ul },			// 155
	{ "#156",		4ul },			// 156
	{ "#157",		4ul },			// 157
	{ "#158",		4ul },			// 158
	{ "#159",		4ul },			// 159
	{ "nbsp",		4ul },			// 160	 		&nbsp;
	{ "iexcl",		5ul },			// 161	¡ 	inverted exclamation mark 	&iexcl; 	&#161;
	{ "cent",		4ul },			// 162	¢ 	cent 	&cent; 	&#162;
	{ "pound",		5ul },			// 163	£ 	pound 	&pound; 	&#163;
	{ "curren",		6ul },			// 164	¤ 	currency 	&curren; 	&#164;
	{ "yen",			3ul },			// 165	¥ 	yen 	&yen; 	&#165;
	{ "brvbar",		6ul },			// 166	¦ 	broken vertical bar 	&brvbar; 	&#166;
	{ "sect",		4ul },			// 167	§ 	section 	&sect; 	&#167;
	{ "uml",			3ul },			// 168	¨ 	spacing diaeresis 	&uml; 	&#168;
	{ "copy",		4ul },			// 169	© 	copyright 	&copy; 	&#169;
	{ "ordf",		4ul },			// 170	ª 	feminine ordinal indicator 	&ordf; 	&#170;
	{ "laquo",		5ul },			// 171	« 	angle quotation mark (left) 	&laquo; 	&#171;
	{ "not",			3ul },			// 172	¬ 	negation 	&not; 	&#172;
	{ "shy",			3ul },			// 173	­ 	soft hyphen 	&shy; 	&#173;
	{ "reg",			3ul },			// 174	® 	registered trademark 	&reg; 	&#174;
	{ "macr",		4ul },			// 175	¯ 	spacing macron 	&macr; 	&#175;
	{ "deg",			3ul },			// 176	° 	degree 	&deg; 	&#176;
	{ "plusmn",		6ul },			// 177	± 	plus-or-minus  	&plusmn; 	&#177;
	{ "sup2",		4ul },			// 178	² 	superscript 2 	&sup2; 	&#178;
	{ "sup3",		4ul },			// 179	³ 	superscript 3 	&sup3; 	&#179;
	{ "acute",		5ul },			// 180	´ 	spacing acute 	&acute; 	&#180;
	{ "micro",		5ul },			// 181	µ 	micro 	&micro; 	&#181;
	{ "para",		4ul },			// 182	¶ 	paragraph 	&para; 	&#182;
	{ "middot",		6ul },			// 183	· 	middle dot 	&middot; 	&#183;
	{ "cedil",		5ul },			// 184	¸ 	spacing cedilla 	&cedil; 	&#184;
	{ "sup1",		4ul },			// 185	¹ 	superscript 1 	&sup1; 	&#185;
	{ "ordm",		4ul },			// 186	º 	masculine ordinal indicator 	&ordm; 	&#186;
	{ "raquo",		5ul },			// 187	» 	angle quotation mark (right) 	&raquo; 	&#187;
	{ "frac14",		6ul },			// 188	¼ 	fraction 1/4 	&frac14; 	&#188;
	{ "frac12",		6ul },			// 189	½ 	fraction 1/2 	&frac12; 	&#189;
	{ "frac34",		6ul },			// 190	¾ 	fraction 3/4 	&frac34; 	&#190;
	{ "iquest",		6ul },			// 191	¿ 	inverted question mark 	&iquest; 	&#191;
	{ "Agrave",		6ul },			// 192	À 	capital a, grave accent 	&Agrave; 	&#192;
	{ "Aacute",		6ul },			// 193	Á 	capital a, acute accent 	&Aacute; 	&#193;
	{ "Acirc",		5ul },			// 194	Â 	capital a, circumflex accent 	&Acirc; 	&#194;
	{ "Atilde",		6ul },			// 195	Ã 	capital a, tilde 	&Atilde; 	&#195;
	{ "Auml",		4ul },			// 196	Ä 	capital a, umlaut mark 	&Auml; 	&#196;
	{ "Aring",		5ul },			// 197	Å 	capital a, ring 	&Aring; 	&#197;
	{ "AElig",		5ul },			// 198	Æ 	capital ae 	&AElig; 	&#198;
	{ "Ccedil",		6ul },			// 199	Ç 	capital c, cedilla 	&Ccedil; 	&#199;
	{ "Egrave",		6ul },			// 200	È 	capital e, grave accent 	&Egrave; 	&#200;
	{ "Eacute",		6ul },			// 201	É 	capital e, acute accent 	&Eacute; 	&#201;
	{ "Ecirc",		5ul },			// 202	Ê 	capital e, circumflex accent 	&Ecirc; 	&#202;
	{ "Euml",		4ul },			// 203	Ë 	capital e, umlaut mark 	&Euml; 	&#203;
	{ "Igrave",		6ul },			// 204	Ì 	capital i, grave accent 	&Igrave; 	&#204;
	{ "Iacute",		6ul },			// 205	Í 	capital i, acute accent 	&Iacute; 	&#205;
	{ "Icirc",		5ul },			// 206	Î 	capital i, circumflex accent 	&Icirc; 	&#206;
	{ "Iuml",		4ul },			// 207	Ï 	capital i, umlaut mark 	&Iuml; 	&#207;
	{ "ETH",			3ul },			// 208	Ð 	capital eth, Icelandic 	&ETH; 	&#208;
	{ "Ntilde",		6ul },			// 209	Ñ 	capital n, tilde 	&Ntilde; 	&#209;
	{ "Ograve",		6ul },			// 210	Ò 	capital o, grave accent 	&Ograve; 	&#210;
	{ "Oacute",		6ul },			// 211	Ó 	capital o, acute accent 	&Oacute; 	&#211;
	{ "Ocirc",		5ul },			// 212	Ô 	capital o, circumflex accent 	&Ocirc; 	&#212;
	{ "Otilde",		6ul },			// 213	Õ 	capital o, tilde 	&Otilde; 	&#213;
	{ "Ouml",		4ul },			// 214	Ö 	capital o, umlaut mark 	&Ouml; 	&#214;
	{ "times",		5ul },			// 215	× 	multiplication 	&times; 	&#215;
	{ "Oslash",		6ul },			// 216	Ø 	capital o, slash 	&Oslash; 	&#216;
	{ "Ugrave",		6ul },			// 217	Ù 	capital u, grave accent 	&Ugrave; 	&#217;
	{ "Uacute",		6ul },			// 218	Ú 	capital u, acute accent 	&Uacute; 	&#218;
	{ "Ucirc",		5ul },			// 219	Û 	capital u, circumflex accent 	&Ucirc; 	&#219;
	{ "Uuml",		4ul },			// 220	Ü 	capital u, umlaut mark 	&Uuml; 	&#220;
	{ "Yacute",		6ul },			// 221	Ý 	capital y, acute accent 	&Yacute; 	&#221;
	{ "THORN",		5ul },			// 222	Þ 	capital THORN, Icelandic 	&THORN; 	&#222;
	{ "szlig",		5ul },			// 223	ß 	small sharp s, German 	&szlig; 	&#223;
	{ "agrave",		6ul },			// 224	à 	small a, grave accent 	&agrave; 	&#224;
	{ "aacute",		6ul },			// 225	á 	small a, acute accent 	&aacute; 	&#225;
	{ "acirc",		5ul },			// 226	â 	small a, circumflex accent 	&acirc; 	&#226;
	{ "atilde",		6ul },			// 227	ã 	small a, tilde 	&atilde; 	&#227;
	{ "auml",		4ul },			// 228	ä 	small a, umlaut mark 	&auml; 	&#228;
	{ "aring",		5ul },			// 229	å 	small a, ring 	&aring; 	&#229;
	{ "aelig",		5ul },			// 230	æ 	small ae 	&aelig; 	&#230;
	{ "ccedil",		6ul },			// 231	ç 	small c, cedilla 	&ccedil; 	&#231;
	{ "egrave",		6ul },			// 232	è 	small e, grave accent 	&egrave; 	&#232;
	{ "eacute",		6ul },			// 233	é 	small e, acute accent 	&eacute; 	&#233;
	{ "ecirc",		5ul },			// 234	ê 	small e, circumflex accent 	&ecirc; 	&#234;
	{ "euml",		4ul },			// 235	ë 	small e, umlaut mark 	&euml; 	&#235;
	{ "igrave",		6ul },			// 236	ì 	small i, grave accent 	&igrave; 	&#236;
	{ "iacute",		6ul },			// 237	í 	small i, acute accent 	&iacute; 	&#237;
	{ "icirc",		5ul },			// 238	î 	small i, circumflex accent 	&icirc; 	&#238;
	{ "iuml",		4ul },			// 239	ï 	small i, umlaut mark 	&iuml; 	&#239;
	{ "eth",			3ul },			// 240	ð 	small eth, Icelandic 	&eth; 	&#240;
	{ "ntilde",		6ul },			// 241	ñ 	small n, tilde 	&ntilde; 	&#241;
	{ "ograve",		6ul },			// 242	ò 	small o, grave accent 	&ograve; 	&#242;
	{ "oacute",		6ul },			// 243	ó 	small o, acute accent 	&oacute; 	&#243;
	{ "ocirc",		5ul },			// 244	ô 	small o, circumflex accent 	&ocirc; 	&#244;
	{ "otilde",		6ul },			// 245	õ 	small o, tilde 	&otilde; 	&#245;
	{ "ouml",		4ul },			// 246	ö 	small o, umlaut mark 	&ouml; 	&#246;
	{ "divide",		6ul },			// 247	÷ 	division 	&divide; 	&#247;
	{ "oslash",		6ul },			// 248	ø 	small o, slash 	&oslash; 	&#248;
	{ "ugrave",		6ul },			// 249	ù 	small u, grave accent 	&ugrave; 	&#249;
	{ "uacute",		6ul },			// 250	ú 	small u, acute accent 	&uacute; 	&#250;
	{ "ucirc",		5ul },			// 251	û 	small u, circumflex accent 	&ucirc; 	&#251;
	{ "uuml",		4ul },			// 252	ü 	small u, umlaut mark 	&uuml; 	&#252;
	{ "yacute",		6ul },			// 253	ý 	small y, acute accent 	&yacute; 	&#253;
	{ "thorn",		5ul },			// 254	þ 	small thorn, Icelandic 	&thorn; 	&#254;
	{ "yuml",		4ul },			// 255	ÿ 	small y, umlaut mark 	&yuml; 	&#255;
};

