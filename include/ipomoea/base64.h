#ifndef _IPOMOEA_BASE64_H
#define _IPOMOEA_BASE64_H

/**
 * @file ipomoea/base64.h  
 * @author Per Löwgren
 * @date Modified: 2013-12-29
 * @date Created: 2012-10-08
 */ 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

size_t base64_encoded_size(size_t len);
size_t base64_decoded_size(size_t len);

void base64_encode(unsigned char *dst,const unsigned char *src,size_t len);
int base64_decode(unsigned char *dst,const unsigned char *src);


#ifdef __cplusplus
}
#endif

#endif /* _IPOMOEA_BASE64_H */

