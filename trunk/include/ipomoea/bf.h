#ifndef _IPOMOEA_BF_H
#define _IPOMOEA_BF_H
#ifdef __cplusplus
extern "C" {
#endif


/** Execute a Brainfuck script
 * @param out File to write output to, if NULL output is written to an allocated string and returned (it have to be deallocated with free()).
 * @param src Script to execute.
 * @param in Input to send to script, if NULL stdin is used instead.
 * @return If out is NULL, an allocated string containing the output, otherwise NULL. */
char *bf_exec(FILE *out,const char *src,const char *in);


#ifdef __cplusplus
}
#endif
#endif /* _IPOMOEA_BF_H */

