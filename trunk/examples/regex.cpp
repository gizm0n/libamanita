
#include <stdlib.h>
#include <stdio.h>
#include <amanita/Regex.h>

using namespace a;

int main(int argc, char *argv[]) {
	Regex r;

	r.match("abc \"def\" \"ghi\" jkl","\".*\"","gi");
	r.print();
	r.match("abc \"def\" \"ghi\" jkl","\".*?\"","gi");
	r.print();
	r.match("abc \"def\" \"ghi\" jkl","def??","gi");
	r.print();
	r.match("abc\ndef","^.","gi");
	r.print();
	r.match("abc\ndef","^.","gim");
	r.print();
	r.match("abc name@host.com def another.name@host.com ghi",
		"\\b([a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,4})\\b","gi");
	r.print();
	r.replace("abc name@host.com def another.name@host.com ghi",
		"\\b([a-z0-9._%+-]{3}@[a-z0-9.-]+\\.[a-z]{2,4})\\b","<a href=\"mailto:\\1\">\\1</a>","gi");
	r.print();
	r.replace("Abc regular expression def regex ghi regexp. Jkl, mno \"regexes\". Pqr, regular expressssions stu.",
		"(reg)(ular\\s)?ex(p|es)?(res+ions?)?","[\\1ex\\3]","gi");
	r.print();
	r.replace("abc 12.34563e+12 def 7865.123 jkl 1738.3680e-23 pqr .123e+12 stu vwx yz.",
		"([0-9]*)\\.[0-9]+([Ee](\\+|-)?[0-9]+)?","int(\\1 \"\\0\")","gi");
	r.print();
	r.replace("abc def ghi jkl mno pqr stu vwx yzabc def ghi jkl mno pqr stu vwx yz",
		"([a-z]{3}\\s+)*","abc(\\1 \"\\0\")","gi");
	r.print();
	r.replace("abc def ghi jkl mno pqr stu vwx yzabc def ghi jkl mno pqr stu vwx yz",
		"([a-z]{3}\\s+){3,4}","abc(\\1 \"\\0\")","gi");
	r.print();
	r.match("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
		"abc|def|xyz","g");
	r.print();
	r.match("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
		"abc(def|xyz)","g");
	r.print();
	r.replace("-{ \"#31\",---3 },...// 31","\"\\#(\\d+)\"","[\\1]\\0","g");
	r.print();
	r.match("abcxxxxxxxxxxyabc","(xx)+y","g");
	r.print();

	exit(0);
	return 0;
}
