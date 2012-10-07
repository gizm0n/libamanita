

int rle_compressed_size(const unsigned char *src,int len) {
	const unsigned char *s = src;
	int i,n,l;
	for(i=0,n=1,l=0; i<len; ++i,++s)
		if(i<len-1 && *s==s[1] && n<257) ++n;
		else l += n==1? 1 : 3,n = 1;
	return l;
}

int rle_extracted_size(const unsigned char *src,int len) {
	const unsigned char *s = src;
	int i = 0,n,l;
	while(i<len)
		if(*s!=s[1] || i==len-1) ++i,++l,++s;
		else i += 3,l += (int)s[2]+2,s += 3;
	return l;
}

int rle_compress(unsigned char *dst,const unsigned char *src,int len) {
	unsigned char *d = dst;
	const unsigned char *s = src;
	int i,n,l;
	for(i=0,n=1,l=0; i<len; ++i,++s)
		if(i<len-1 && *s==s[1] && n<257) ++n;
		else if(n==1) *d++ = *s,++l,n = 1;
		else d[0] = d[1] = *s,d[2] = n-2,d += 3,l += 3,n = 1;
	*d = '\0';
	return l;
}

int rle_extract(unsigned char *dst,const unsigned char *src,int len) {
	unsigned char *d = dst,c;
	const unsigned char *s = src;
	int i = 0,n,l;
	while(i<len)
		if(*s!=s[1] || i==len-1) *d++ = *s,++i,++l,++s;
		else for(c=*s,n=(int)s[2]+2,i+=3,l+=n,s+=3; n>0; --n) *d++ = c;
	*d = '\0';
	return l;
}


