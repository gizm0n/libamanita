#ifndef _IPOMOEA_RLE_H
#define _IPOMOEA_RLE_H


int rle_compressed_size(const unsigned char *src,int len);
int rle_extracted_size(const unsigned char *src,int len);

int rle_compress(unsigned char *dst,const unsigned char *src,int len);
int rle_extract(unsigned char *dst,const unsigned char *src,int len);


#endif /* _IPOMOEA_RLE_H */

