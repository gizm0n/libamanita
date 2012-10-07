#ifndef _IPOMOEA_BASE64_H
#define _IPOMOEA_BASE64_H


size_t base64_encoded_size(size_t len);
size_t base64_decoded_size(size_t len);

void base64_encode(unsigned char *dst,const unsigned char *src,int len);
void base64_decode(unsigned char *dst,const unsigned char *src);


#endif /* _IPOMOEA_BASE64_H */

