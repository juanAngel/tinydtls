int decode(unsigned char* buf, unsigned long buf_len, mpz_t res);
int encode(unsigned char* buf, unsigned long buf_len, mpz_t res);

int encodeG(unsigned char* buf, unsigned long buf_len, mpz_t* res);
int decodeG(unsigned char* buf, unsigned long buf_len, mpz_t* res, int debug);


unsigned char* randBytes(int buflen);
unsigned char* randomhex(int len);


