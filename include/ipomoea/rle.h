#ifndef _IPOMOEA_RLE_H
#define _IPOMOEA_RLE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

size_t rle_compressed_size(const unsigned char *src,size_t len);
size_t rle_extracted_size(const unsigned char *src,size_t len);

size_t rle_compress(unsigned char *dst,const unsigned char *src,size_t len);
size_t rle_extract(unsigned char *dst,const unsigned char *src,size_t len);


#ifdef __cplusplus
}
#endif
#endif /* _IPOMOEA_RLE_H */

