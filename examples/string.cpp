
#include <stdlib.h>
#include <stdio.h>
#include <amanita/String.h>

int main(int argc, char *argv[]) {
	String str;
	str << "\
<html>\n\
<head>\n\
<title>Title</title>\n" <<
"<script>\n\
/** Script comment\n\
 */\n\
alert('Hello world!'); // Hello world alert javascript\n\
</script>\n" <<
"</head>\n\
<body>\n\
<h1>Header</h1>\n\
<!-- HTML comment -->\n\
<p>Paragraph, text, < body of the document.</p>\n\
</body>";

	fprintf(stdout,"File:\n%s\nEOF\n",str.toCharArray());
	str.stripComments(LANG_C);
	fprintf(stdout,"\nStrip C Comments:\n%s\nEOF\n",str.toCharArray());
	str.stripComments(LANG_HTML);
	fprintf(stdout,"\nStrip HTML Comments:\n%s\nEOF\n",str.toCharArray());
	str.stripHTML();
	fprintf(stdout,"\nStrip HTML:\n%s\nEOF\n",str.toCharArray());
	str.newline("<br/>\n");
	fprintf(stdout,"\nNewline <br/>:\n%s\nEOF\n",str.toCharArray());
	str.encodeHTML();
	fprintf(stdout,"\nEncode HTML:\n%s\nEOF\n",str.toCharArray());
	fflush(stdout);
	return 0;
}
