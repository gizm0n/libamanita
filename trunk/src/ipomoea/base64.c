

#define __ -1

static const char base64_code[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int base64_index[256] = {
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,62, __,__,__,63, /* '+','/' */
    52,53,54,55, 56,57,58,59, 60,61,__,__, __, 0,__,__, /* '0'-'9', '=' */
    __, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14, /* 'A'-'O' */
    15,16,17,18, 19,20,21,22, 23,24,25,__, __,__,__,__, /* 'P'-'Z' */
    __,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40, /* 'a'-'o' */
    41,42,43,44, 45,46,47,48, 49,50,51,__, __,__,__,__, /* 'p'-'z' */
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
    __,__,__,__, __,__,__,__, __,__,__,__, __,__,__,__,
};

size_t base64_encoded_size(size_t len) {
	return ((len+2)/3)*4;
}

size_t base64_decoded_size(size_t len) {
	return (len/4)*3;
}

void base64_encode(unsigned char *dst,const unsigned char *src,int len) {
	unsigned char *d = dst;
	const unsigned char *s = src;
	int i;
	for(i=0; i<len; i+=3,s+=3,d+=4) {
		d[0] = (unsigned char)base64_code[(int)(s[0]>>2)];
		d[1] = (unsigned char)base64_code[(int)(((s[0]&0x03)<<4)|((s[1]&0xf0)>>4))];
		d[2] = (unsigned char)(len-i>1? base64_code[(int)(((s[1]&0x0f)<<2)|((s[2]&0xc0)>>6))] : '=');
		d[3] = (unsigned char)(len-i>2? base64_code[(int)(s[2]&0x3f)] : '=');
	}
	*d = '\0';
}

void base64_decode(unsigned char *dst,const unsigned char *src) {
	unsigned char *d = dst;
	const unsigned char *s = src;
	int i,j,k,l;
	for(; *s!='\0'; s+=4,d+=3) {
		i = base64_index[(int)s[0]];
		j = base64_index[(int)s[1]];
		k = base64_index[(int)s[2]];
		l = base64_index[(int)s[3]];
		if(i==__ || j==__ || k==__ || l==__) break;
		d[0] = (unsigned char )(i<<2|j>>4);
		d[1] = (unsigned char )(j<<4|k>>2);
		d[2] = (unsigned char )(((k<<6)&0xc0)|l);
	}
	*d = '\0';
}


