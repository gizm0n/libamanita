
#include <stdlib.h>
#include <stdio.h>
#include <amanita/Vector.h>
#include <amanita/Wiki.h>


static const char *page = "\
==Header 1==\n\
===Header 2===\n\
\n\
Paragraph...:D\n\
\n\
----\n\
\n\
Paragraph [[link]] text '''bold''' text ''italic'' and '''''bold italic'''''.\n\
\n\
===Header 3===\n\
====Header 4====\n\
Paragraph...\n\
http://www.mantichora.net/index.php\n\
text [b]bbcode[/b] text\n\
 Pre text ;) text text\n\
 pre pre pre pre\n\
 text text text.\n\
text, end of paragraph.\n\
\n\
Testing template {{hebrew|RShITh HGLGLIM}} text {{astro|aries|taurus|gemini|cancer|leo|virgo|libra|scorpio|sagittarius|capricorn|aquarius|pisces|sun|moon|mercury|venus|mars|jupiter|saturn|uranus|neptune|pluto}} text.\n\
\n\
[[File:Manticore.jpg|120px|thumb|Image of '''[[Manticore]]''' and companion]]\n\
\n\
<div style=\"color:red\">Div div div.</div>\n\
\n\
:A colon at the start of a line\n\
::causes the line to be indented,\n\
:::most commonly used on Talk pages.\n\
* ''Unordered lists'' are easy to do:\n\
** Start every line with a star.\n\
*** More stars indicate a deeper level.\n\
**: Previous item continues.\n\
** A new line\n\
* in a list\n\
marks the end of the list.\n\
* Of course you can start again.\n\
# ''Numbered lists'' are:\n\
## Very organized\n\
## Easy to follow\n\
#: Previous item continues\n\
A new line marks the end of the list.\n\
# New numbering starts with 1.\n\
\n\
{| style=\"background: yellow; color: green\"\n\
|-\n\
| abc || def || ghi\n\
|- style=\"background: red; color: white\"\n\
| jkl || mno || pqr\n\
|-\n\
| stu || style=\"background: silver\" | vwx || yz\n\
|}\n\
\n\
{| border=\"1\" cellpadding=\"5\" cellspacing=\"0\" align=\"center\"\n\
|+ '''An example table'''\n\
|-\n\
! style=\"background: #efefef;\" | First header\n\
! colspan=\"2\" style=\"background: #ffdead;\" | Second header\n\
|-\n\
| upper left\n\
| &nbsp;\n\
| rowspan=\"2\" style=\"border-bottom: 3px solid grey;\" valign=\"top\" |\n\
right side\n\
|-\n\
| style=\"border-bottom: 3px solid grey;\" | lower left\n\
| style=\"border-bottom: 3px solid grey;\" | lower middle\n\
|-\n\
| colspan=\"3\" align=\"center\" |\n\
{| border=\"0\"\n\
|+ ''A table in a table''\n\
|-\n\
| align=\"center\" width=\"150\" | [[File:Wiki.png]]\n\
| align=\"center\" width=\"150\" | [[File:Wiki.png]]\n\
|-\n\
| align=\"center\" colspan=\"2\" style=\"border-top: 1px solid red;<!--\n\
  --> border-right: 1px solid red; border-bottom: 2px solid red;<!--\n\
  --> border-left: 1px solid red;\" |\n\
Two Wikipedia logos\n\
|}\n\
|}\n\
<nowiki>\n\
The nowiki tag ignores [[wiki]]\n\
''markup''. It reformats text by\n\
removing newlines and multiple\n\
spaces. It still interprets\n\
special characters: &rarr;\n\
</nowiki>\n\
\n\
<pre>\n\
The <pre> tag ignores [[wiki]]\n\
''markup'' as does the <nowiki>\n\
tag. Additionally, <pre> displays\n\
in a mono-spaced font, but does\n\
not  reformat    text    spaces.\n\
It still interprets special\n\
characters: &rarr;\n\
</pre>\n\
\n\
Single square brackets holding\n\
[text without a http URL] are\n\
preserved, but single square\n\
brackets containing a URL are\n\
treated as being an external\n\
[http://www.google.com weblink].\n\
";

static const char *dirs[] = {
	"./",
	"./",
	"/home/amanita/Desktop/",
	"./",
	"./",
};

static const char *urls[] = {
	"http://www.mantichora.net/page/",
	"http://www.mantichora.net/public/",
	"file:///home/amanita/Desktop/",
	"http://www.mantichora.net/public/",
	"http://www.mantichora.net/public/",
};

static const char *smileys[] = {
	":)",":-)",":(",":-(",";)",";-)",":P",":-P",":.",":O",":/","B)","B-)",":|",":-|",":*(",":D",":-D",
	":*",":$",":-$",":[",":-[","X|","8)","8-)",">:)",":O)",
0};

static const int smiley_ids[] = {
	1,1,2,2,3,3,4,4,5,6,7,8,8,9,9,10,11,11,12,13,13,14,14,15,16,16,17,18,
};

void smiley_callback(Wiki &w,int index) {
	w << "<img src=\"http://www.mantichora.net/images/symbols/smiley" << smiley_ids[index] << ".gif\" class=\"smiley\" />";
}

static const char *templates[] = {
	"hebrew","astro",
0};

static const char *hebrew_chars = "ABGDHVZCTIKLMNSOPTQRST";
static const char *hebrew[] = {
	"&#1488;","&#1489;","&#1490;","&#1491;","&#1492;","&#1493;","&#1494;","&#1495;","&#1496;","&#1497;","&#1499;",
	"&#1500;","&#1502;","&#1504;","&#1505;","&#1506;","&#1508;","&#1510;","&#1511;","&#1512;","&#1513;","&#1514;",
0};

static const char *hebrew_final_chars = "KMNPT";
static const char *hebrew_final[] = {
	"&#1498;","&#1501;","&#1503;","&#1507;","&#1509;",
0};

static const char *astro[] = {
	"aries","taurus","gemini","cancer","leo","virgo","libra","scorpio","sagittarius","capricorn","aquarius","pisces",
	"sun","moon","mercury","venus","mars","jupiter","saturn","uranus","neptune","pluto", 
0};

static const char *astro_unicode[] = {
	"&#x2648;","&#x2649;","&#x264A;","&#x264B;","&#x264C;","&#x264D;","&#x264E;","&#x264F;","&#x2650;","&#x2651;","&#x2652;","&#x2653;",
	"&#x2609;","&#x263D;","&#x263F;","&#x2640;","&#x2642;","&#x2643;","&#x2644;","&#x2645;","&#x2646;","&#x2647;", 
0};

void transliterate_hebrew(Wiki &w,char *p) {
	char *p1;
	const char *p2;
	int n,n2;
	w << "<span style=\"font:18px Symbola;\">";
	for(p1=p; *p1!='\0'; ++p1) {
		n = -1,n2 = -1;
		if(*p1=='C' && p1[1]=='h') n = 8,++p1;
		else if(*p1=='T' && p1[1]=='z') n = 17,++p1;
		else if(*p1=='S' && p1[1]=='h') n = 20,++p1;
		else if(*p1=='T' && p1[1]=='h') n = 21,++p1;
		else if((p2=strchr(hebrew_chars,*p1))) n = p2-hebrew_chars;
		if(n!=-1) {
			if((n==-1 || n==17) && (p1[1]=='\0' || a::isspace(p1[1]))) {
				if(n==17) n2 = 4;
				else if((p2=strchr(hebrew_final_chars,*p1))) n2 = p2-hebrew_final_chars;
			}
			w << (const char *)(n2>=0? hebrew_final[n2] : hebrew[n]);
		} else w << (char)*p1;
	}	
	w << "</span>";
}

void astro_symbols(Wiki &w,char *p) {
	char *p1,*p2;
	int n;
	w << "<span style=\"font:18px Symbola;\">";
	for(p1=p; (p2=strchr(p1,'|')); p1=p2) {
		*p2++ = '\0';
		a::trim(p1);
		n = Vector::find(astro,p1);
		if(n!=-1) w << astro_unicode[n];
	}
	w << "</span>";
}

void template_callback(WikiParams &p) {
	char str[p.param_length+1];
	int n;
	if(p.param_length) p.text->substr(str,p.param,p.param_length);
	else *str = '\0';
fprintf(stderr,"template_callback(tag: %s, param: \"%s\")\n",p.tag,str);
	n = Vector::find(templates,p.tag);
	switch(n) {
		case 0:transliterate_hebrew(*p.wiki,str);break;
		case 1:astro_symbols(*p.wiki,str);break;
	}
}

void tag_nowiki_callback(WikiParams &p) {
	if(p.length>0) {
		String ref;
		ref.append(p.text,p.offset,p.length);
		ref.encodeHTML(HTML_LTGT);
		p.wiki->addRef(ref);
//fprintf(stderr,"tag_nowiki_callback(ref: \"%s\")\n",ref.toCharArray());
	}
};

void tag_pre_callback(WikiParams &p) {
	if(p.length>0) {
		String ref;
		ref.append(p.text,p.offset,p.length);
		ref.encodeHTML(HTML_LTGT);
		ref.insert(0,"\n<pre>");
		ref.append("</pre>\n");
		p.wiki->addRef(ref);
//fprintf(stderr,"tag_nowiki_callback(ref: \"%s\")\n",ref.toCharArray());
	}
};

void link_callback(Wiki &w,WikiLink &link) {
fprintf(stderr,"link_category_callback(title: \"%s\")\n",link.title);
	link.write(w,urls[0]);
};

void link_file_callback(Wiki &w,WikiLink &link) {
	WikiFile file(link,dirs);
	file.write(w,urls);
};

void link_category_callback(Wiki &w,WikiLink &link) {
	w << "<a href=\"" << link.link << "\" class=\"category\">" << link.title << "</a>";
};


int main(int argc, char *argv[]) {
	WikiTagHandler th[] = {
		{ "nowiki",0,tag_nowiki_callback },
		{ "pre",0,tag_pre_callback },
	{0}};
	WikiLinkHandler lh[] = {
		{ "",link_callback },
		{ "File",link_file_callback },
		{ "Category",link_category_callback },
	{0}};
	Wiki wiki;
	wiki.setBaseURL(urls[0]);
	wiki.setTemplateHandler(template_callback);
	wiki.setTagHandlers(th);
	wiki.setLinkHandlers(lh);
	wiki.setSmileys(smileys,smiley_callback);
	wiki.format(page,WIKI_SECTIONS|WIKI_SMILEYS|WIKI_TAGS|WIKI_LINKS|WIKI_BBCODE);

	fprintf(stdout,"\n\nWiki:\n%s\nEOF\n",wiki.toCharArray());
	fflush(stdout);
	return 0;
}
