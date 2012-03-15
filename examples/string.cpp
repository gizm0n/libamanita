
#include <stdlib.h>
#include <stdio.h>
#include <amanita/aRegex.h>

int main(int argc, char *argv[]) {
	aString str;
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
	str.stripComments();
	fprintf(stdout,"Strip Comments:\n%s\nEOF\n",str.toCharArray());
	str.stripHTMLComments();
	fprintf(stdout,"Strip HTML Comments:\n%s\nEOF\n",str.toCharArray());
	str.stripHTML();
	fprintf(stdout,"Strip HTML:\n%s\nEOF\n",str.toCharArray());
	str.newline("<br/>\n");
	fprintf(stdout,"Newline <br/>:\n%s\nEOF\n",str.toCharArray());
	fflush(stdout);
	return 0;
}
