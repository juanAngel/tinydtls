
#include "crypt.h"
#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static int dtls_ma_seed = 0;
static FILE *dtls_ma_urandom_fp = NULL;
#define DTLS_MA_DEBUG 1

void dtls_ma_generate_keys(struct PrivKey* privk, struct PubKey* pubk){
	dtls_ma_debug("generate private key");
	dtls_ma_generate_private_key(privk);
	dtls_ma_debug("generate public key");
	dtls_ma_get_public_key(privk, pubk);
	dtls_ma_debug("All done");
}

void dtls_ma_generate_private_key(struct PrivKey* privk){
	int i;
	unsigned char* skStep;
	unsigned char* pStep;
	unsigned char* k;

	mpz_init (privk->n);
	mpz_init (privk->w);
	mpz_init (privk->wMin);
	mpz_init (privk->m);	
	mpz_init (privk->p);	
	mpz_init (privk->q);	
        mpz_init (privk->r);	
        mpz_init (privk->t);	
	
	for(i=0; i < N; i++)
		mpz_init (privk->k[i]);

        mpz_init (privk->sk);
        mpz_init (privk->skInvQ);	
        mpz_init (privk->skInvP);	

        mpz_set_ui(privk->n,N); 
        mpz_set_ui(privk->m,M); 
        mpz_set_ui(privk->w,W); 
	mpz_set_ui(privk->wMin,W_MIN); 

        mpz_set_ui(privk->q,1); 
	mpz_mul_2exp(privk->q, privk->q, Q_BITS); 	


	mpz_set_ui(privk->t,1); 
	mpz_mul_2exp(privk->t, privk->t, T_BITS); 	


        mpz_t a, b, c, d;
        mpz_init (a);
        mpz_init (b); 
	mpz_init (c);	
        mpz_init (d);

	//error

	mpz_set_ui(privk->r,1); 
	mpz_mul_2exp(privk->r, privk->r, E_BITS); 	

        //sk
	skStep = dtls_ma_random_hex(SK_BITS/8);
        mpz_set_str(privk->sk, (char *) skStep, 16);
	free(skStep);
        mpz_mul_ui (privk->sk, privk->sk, 2);
        mpz_add_ui (privk->sk, privk->sk, 1); //privk->sk co-primise with q

       
	//  pStep = (privk->q - (privk->t-1lu)*privk->sk)/(W*privk->r) - privk->t; //$sk*(t-1)+w*r*(t+pStep)<q$
	// do validity check
        mpz_sub_ui (a, privk->t, 1);
        mpz_mul (b, a, privk->sk);
	
	if(mpz_cmp(b, privk->q)>=0){
		dtls_ma_debug("\nError: the parameters T_BITS or SK_BITS too big with respect to Q_BITS");
		return;
	}

	

        mpz_sub (a, privk->q, b); //a = privk->q - (privk->t-1lu)*privk->sk

        mpz_mul_ui (b, privk->r, W);


        mpz_fdiv_q (a, a, b); //a will be the maximum of random numbers that can be added to t to get privk->p
	mpz_add_ui(b, privk->t, 0xFF);

	if(mpz_cmp(a,b)<0){
		dtls_ma_debug("\nError: the parameters E_BITS and SK_BITS too big with respect to Q_BITS");
		return;
	}	

	mpz_sub (a, a, privk->t);
	#ifdef DTLS_MA_DEBUG
	printf("max pstep: %s \n", mpz_get_str(NULL, 16, a));
	#endif


	//privk->p = privk->t + pStep*2+1; //since 4294967296 has only the factor 2 
	pStep = dtls_ma_random_hex(SK_BITS/8);	
        mpz_set_str(b, (char *) pStep, 16);
	free(pStep);
	mpz_mod(b, b, a);	

	mpz_set_ui(c, 2);
	mpz_fdiv_r(d, b, c); //is b odd
	mpz_set_ui(c, 0x0);

	if(mpz_cmp(d,c)==0){ // b is even
		mpz_add_ui(b, b, 1);
		if(mpz_cmp(b,a)>=0){
			mpz_sub_ui(b, b, 2);
		}
	}

	mpz_add(privk->p, privk->t, b);

        do{ 
                mpz_set_ui(c,3); //mpz_invert does not change temp if non-invertible    
	        mpz_invert(c, privk->sk,privk->p);
                
                mpz_mul (b, c, privk->sk);
		mpz_mod (b, b, privk->p);

		if(mpz_get_ui(b)==1){//(inv*privk->sk)%privk->p==1
			#ifdef DTLS_MA_DEBUG			
			printf("sk: %s coprime with: %s \n", mpz_get_str(NULL, 16, privk->sk), mpz_get_str(NULL, 16, privk->p));
			#endif
			mpz_mul_ui (privk->skInvP, c, 1);
			#ifdef DTLS_MA_DEBUG			
			printf("skInvP: %s \n", mpz_get_str(NULL, 16,privk->skInvP));
			#endif
                        break; 
	        }
		else{
			pStep = dtls_ma_random_hex(SK_BITS/8);	
        		mpz_set_str(b, (char *) pStep, 16);
			free(pStep);
			mpz_mod(b , b, a);	

			mpz_set_ui(c, 2);
			mpz_fdiv_r(d, b, c); //is b odd
			mpz_set_ui(c, 0x0);

			if(mpz_cmp(d,c)==0){ // b is even
				mpz_add_ui(b, b, 1);
				if(mpz_cmp(b,a)>=0){
					mpz_sub_ui(b, b, 2);
				}
			}

			mpz_add(privk->p, privk->t, b);                      
		}
                
	}while(1);
         
        //



	for(i=0; i < N; i++){
		k = dtls_ma_random_hex(Q_BITS/8);	
		mpz_set_str(privk->k[i], (char *) k, 16);
		free(k);
	}

        mpz_clear (a);
        mpz_clear (b); 
	mpz_clear (c);
	
        
	/*
        short mm = B; //204*80 = 16320 < 2^14 = 16384   
        	
	for(i=0; i< COMMON_LEN; i++){   	
		short sign = rand()%2;                
		short temp = rand()%mm;
		if(temp<2){
                        i--;
			continue;
   
		}
		if(sign==0)	
			printf("%u, ", temp);
		else
			printf("-%u, ", temp);
	}
        */

	return;
}

void dtls_ma_print_private_key(struct PrivKey* privk){
 
  printf("n = %lu \n", mpz_get_ui(privk->n));	  	
  printf("m = %lu \n", mpz_get_ui(privk->m));
  printf("q = %s \n", mpz_get_str(NULL, 10, privk->q));
  printf("t = %s \n", mpz_get_str(NULL, 10, privk->t));	
  printf("p = %s \n", mpz_get_str(NULL, 10, privk->p));	
  printf("sk = %s \n", mpz_get_str(NULL, 10, privk->sk)); 
  printf("r = %s \n", mpz_get_str(NULL, 10, privk->r)); 
  printf("w = %s \n", mpz_get_str(NULL, 10, privk->w));  
  //printf("wMin = %s \n", mpz_get_str(NULL, 10, privk->wMin));  

  int i;
  printf(" private key vector \n"); 
  for(i=0; i< mpz_get_ui(privk->n); i++){
	//printf("%10lu ", privk->k[i]);
        printf(" %s, ", mpz_get_str(NULL, 16, privk->k[i]));
        if((i+1)%6==0){
		printf("\n");
	}
        printf("\n");
  } 
  
}

void dtls_ma_save_private_key(const char* fname, struct PrivKey* privk){
	FILE *fp;

   	fp = fopen(fname, "w");
   	fprintf(fp, "n = %s\n", mpz_get_str(NULL,16,privk->n));
   	fprintf(fp, "m = %s\n", mpz_get_str(NULL,16,privk->m));
  	fprintf(fp, "q = %s\n", mpz_get_str(NULL, 16, privk->q));
	fprintf(fp, "t = %s\n", mpz_get_str(NULL, 16, privk->t));	
  	fprintf(fp, "p = %s\n", mpz_get_str(NULL, 16, privk->p));	
  	fprintf(fp, "sk = %s\n", mpz_get_str(NULL, 16, privk->sk)); 
  	fprintf(fp, "r = %s\n", mpz_get_str(NULL, 16, privk->r)); 
  	fprintf(fp, "w = %s\n", mpz_get_str(NULL, 16, privk->w));
	fprintf(fp, "wMin = %s\n", mpz_get_str(NULL, 16, privk->wMin));  
        fprintf(fp, "skInvP = %s\n", mpz_get_str(NULL, 16, privk->skInvP)); 
	int i;
  	for(i=0; i< N; i++){
        	fprintf(fp, "%s\n", mpz_get_str(NULL, 16, privk->k[i]));
  	} 


        //
   	//fputs("This is testing for fputs...\n", fp);
   	fclose(fp);

}



int dtls_ma_load_private_key(const char* fname, struct PrivKey* privk){
	int i;
	mpz_init (privk->n);
	mpz_init (privk->w);
	mpz_init (privk->wMin);
	mpz_init (privk->m);	
	mpz_init (privk->p);	
	mpz_init (privk->q);	
        mpz_init (privk->r);	
        mpz_init (privk->t);	
	
	for(i=0; i < N; i++)
		mpz_init (privk->k[i]);

        mpz_init (privk->sk);
        mpz_init (privk->skInvQ);	
        mpz_init (privk->skInvP);

	FILE *fp;

   	fp = fopen(fname, "r");
        if(fp==NULL)
		return -1;

        char buf[16];
        fscanf(fp, "n = %s\n", buf);                 
	mpz_set_str (privk->n, buf, 16);


        fscanf(fp, "m = %s\n", buf);
        mpz_set_str (privk->m, buf, 16);

        fscanf(fp, "q = %s\n", buf);
        mpz_set_str (privk->q, buf, 16);

        fscanf(fp, "t = %s\n", buf);
        mpz_set_str (privk->t, buf, 16);

        fscanf(fp, "p = %s\n", buf);
        mpz_set_str (privk->p, buf, 16);

        fscanf(fp, "sk = %s\n", buf);
        mpz_set_str (privk->sk, buf, 16);

        fscanf(fp, "r = %s\n", buf);
        mpz_set_str (privk->r, buf, 16);

        fscanf(fp, "w = %s\n", buf);
        mpz_set_str (privk->w, buf, 16);

        fscanf(fp, "wMin = %s\n", buf);
        mpz_set_str (privk->wMin, buf, 16);

        fscanf(fp, "skInvP = %s\n", buf);
        mpz_set_str (privk->skInvP, buf, 16);



  	for(i=0; i< N; i++){
        	fscanf(fp, "%s\n", buf);
        	mpz_set_str (privk->k[i], buf, 16);
  	} 

   	fclose(fp);
	return 0; // for success
}

void dtls_ma_save_public_key(const char* fname, struct PubKey* pubk){
	FILE *fp;

   	fp = fopen(fname, "w");
   	fprintf(fp, "n = %s\n", mpz_get_str(NULL,16,pubk->n));
  	fprintf(fp, "w = %s\n", mpz_get_str(NULL, 16, pubk->w));
	fprintf(fp, "wMin = %s\n", mpz_get_str(NULL, 16, pubk->wMin));  
        fprintf(fp, "q = %s\n", mpz_get_str(NULL, 16, pubk->q));
	fprintf(fp, "t = %s\n", mpz_get_str(NULL, 16, pubk->t));	
  	fprintf(fp, "bias = %s\n", mpz_get_str(NULL, 16, pubk->bias));	

	int i;
  	for(i=0; i< M; i++){
        	fprintf(fp, "%s\n", mpz_get_str(NULL, 16, pubk->Element[i]));
  	} 


        //
   	//fputs("This is testing for fputs...\n", fp);
   	fclose(fp);
}

 struct PubKey* dtls_ma_load_public_key(const char* fname){
	int i;
	struct PubKey* pubk = malloc(sizeof(struct PubKey));

	mpz_init (pubk->q);	
        mpz_init (pubk->t);
	mpz_init (pubk->n);
	mpz_init (pubk->w);
	mpz_init (pubk->wMin);
	mpz_init (pubk->bias);	
	for(i=0; i < M; i++)
		mpz_init (pubk->Element[i]);

	FILE *fp;

   	fp = fopen(fname, "r");
        if(fp==NULL)
		return NULL;

        char buf[16];
        fscanf(fp, "n = %s\n", buf);                 
	mpz_set_str (pubk->n, buf, 16);


        fscanf(fp, "w = %s\n", buf);
        mpz_set_str (pubk->w, buf, 16);

        fscanf(fp, "wMin = %s\n", buf);
        mpz_set_str (pubk->wMin, buf, 16);

        fscanf(fp, "q = %s\n", buf);
        mpz_set_str (pubk->q, buf, 16);

        fscanf(fp, "t = %s\n", buf);
        mpz_set_str (pubk->t, buf, 16);

        fscanf(fp, "bias = %s\n", buf);
        mpz_set_str (pubk->bias, buf, 16);


  	for(i=0; i< M; i++){
        	fscanf(fp, "%s\n", buf);
        	mpz_set_str (pubk->Element[i], buf, 16);
  	} 


   	fclose(fp);
	return pubk;
}

void dtls_ma_get_public_key(struct PrivKey* privk, struct PubKey* pubk){

	// struct PubKey* pubk = malloc(sizeof(struct PubKey));   
    
	mpz_init (pubk->q);	
        mpz_init (pubk->t);
	mpz_init (pubk->n);
	mpz_init (pubk->w);
	mpz_init (pubk->wMin);
	mpz_init (pubk->bias);		
	unsigned int i,j;
	for(i=0; i < M; i++)
		mpz_init (pubk->Element[i]);

	mpz_add_ui (pubk->n, privk->n, 0);
	
	
	unsigned char* byte = dtls_ma_random_bytes(sizeof(unsigned int));
	unsigned int bias = 0;
	memcpy(&bias, byte, sizeof(unsigned int));
        free(byte); 

	if(bias==0)
		bias = 1;

        mpz_set_ui (pubk->bias, bias);	

	mpz_add_ui (pubk->w, privk->w, 0);
	mpz_add_ui (pubk->wMin, privk->wMin, 0);
	mpz_add_ui (pubk->q, privk->q, 0);
	mpz_add_ui (pubk->t, privk->t, 0);

        mpz_t skInvQ, leftSum, a, /*b, */ e; // unused variable to be removed
        mpz_init (skInvQ);
        mpz_init (leftSum);
        mpz_init (a);
        mpz_init (e);
        //mpz_set_ui(c,3); //mpz_invert does not change temp if non-invertible    
	mpz_invert(skInvQ, privk->sk,pubk->q);
	mpz_mul(skInvQ, skInvQ, privk->p);
	//unsigned long R_MAX = mpz_get_ui(privk->r);

       for(i=0; i<M; i++){
                mpz_set_ui (leftSum, 0);

		for(j=0; j<N;j++){
			mpz_mul_si(a, privk->k[j], common[(i+(j+1)*bias)%COMMON_LEN]);
			mpz_add(leftSum, leftSum, a);
		}


		unsigned char* error = dtls_ma_random_hex(Q_BITS/8);	
		mpz_set_str(e, (char *) error, 16);
		free(error);
		mpz_mod (e, e, privk->r);
			
		if(E_SIGN=='-')
			mpz_mul_si (e, e, -1);

                //printf("\n error %s\n", mpz_get_str(NULL, 16, e));
		mpz_mul(a, skInvQ, e);
		mpz_mod(a, a, privk->q);
		mpz_sub(leftSum, leftSum, a);
		mpz_mod(pubk->Element[i], leftSum, privk->q);

	} 

        mpz_clear (skInvQ);
        mpz_clear (a);
        mpz_clear (e);
	mpz_clear (leftSum);
	// return pubk;
	return;

}


void dtls_ma_print_SIS_sample(char* msg, struct PubKey* pubk, struct Cipher* D){
	//printPubK(msg, pubk);
        //printCipherS(D); 
        int i, j;
        printf("\n\n Copy the expression to https://develop.wolframcloud.com/objects/63785264-af0d-49e5-83e4-36465169a55d\n\nFindInstance[{\n"); 
   	for(j=0; j < N; j++){
 
  		for(i=0; i< M; i++){
		   if(i==M-1)	
		   printf("(%d)*l%d==%ld, \n", (common[(i+(j+1)*mpz_get_ui(pubk->bias))%COMMON_LEN]), i, mpz_get_si(D->element[j]));			
                   else      
		   printf("(%d)*l%d+", (common[(i+(j+1)*mpz_get_ui(pubk->bias))%COMMON_LEN]), i);			
        	}  
   	}

   	for(i=0; i< M; i++){
                if(i==M-1) 
			printf("l%d<=%d, \n", i,W);			
		else
			printf("l%d+", i);			
        } 

	for(i=0; i< M; i++){
                if(i==M-1) 
			printf("l%d>=-%d, \n", i,W);			
		else
			printf("l%d+", i);			
        } 

   	for(i=0; i< M; i++){
                if(i==M-1)
			{printf("l%d<=%d,", i, W);}			
		else
			{printf("l%d<=%d, ", i, W);}			
        } 

   	for(i=0; i< M; i++){
                if(i==M-1)
			{printf("l%d>=-%d", i, W);}			
		else
			{printf("l%d>=-%d, ", i, W);}			
        } 

  	printf("}, \n{"); 
  	  

   	for(i=0; i< M; i++){
		if(i==M-1)
		printf("l%d", i);
                else
		printf("l%d, ", i);			
        } 
	printf("}, Integers, 2000]\n\n");

}


void dtls_ma_print_public_key(char* msg, struct PubKey* pubk){

  printf("\n %s \n", msg);
  printf("n = %lu \n", mpz_get_ui(pubk->n));	  	
  printf("bias = %lu \n", mpz_get_ui(pubk->bias));
  printf("q = %s \n", mpz_get_str(NULL, 16, pubk->q));
  printf("t = %s \n", mpz_get_str(NULL, 16, pubk->t));	
  printf("w = %s \n", mpz_get_str(NULL, 16, pubk->w)); 
  //printf("wMin = %s \n", mpz_get_str(NULL, 16, pubk->wMin)); 


  int i, j;
  for(i=0; i< M; i++){
        for(j=0; j  < N+1; j++){
                if(j==N)
			printf("%s \n", mpz_get_str(NULL, 16,pubk->Element[i]));
                else
			printf("%7d ", (common[(i+(j+1)*mpz_get_ui(pubk->bias))%COMMON_LEN])); //+i)+j

			

        }  
	//printf("%lu \n", pubk->Element[i]);
        
  }
}


void dtls_ma_enc(struct PubKey* pubk, mpz_t v, struct Cipher* D){
        mpz_mod(v, v, pubk->t);
	
	unsigned int i,j;
	mpz_init(D->tail);
        for(i=0; i<N; i++){
		mpz_init(D->element[i]);
	}	

	unsigned short sel;

	unsigned char* rbytes = dtls_ma_random_bytes(2);
        memcpy((unsigned char*)&sel, rbytes, 2);  
	sel = sel%W;

	if(sel<W_MIN){
		sel = W - sel;
	}	
	free(rbytes);

        unsigned char* alpha = malloc(sel); //for addtion

        unsigned char l[M];
        mpz_t partialV,  a;
        mpz_init(partialV);
	mpz_init(a);
	mpz_set_ui(partialV, 0);

	
	for(i=0; i<M; i++){
		l[i]=0x00;
  	}

        for(i=0; i<sel; i++){
		rbytes = dtls_ma_random_bytes(1);
                unsigned char rr = rbytes[0];
		free(rbytes);
		
		alpha[i] = rr%M;			

		l[alpha[i]] = l[alpha[i]] + 1;		
		mpz_add(partialV, partialV, pubk->Element[alpha[i]]);
	}

        //for(i=0; i<M; i++){
	//	if(l1[i]>0)
	//		printf("l%u=-%u, ", i, l1[i]);
	//	else
	//		printf("l%u=%u, ", i, l[i]);
  	//}
	//printf("\n (%u, %u) \n", num_beta, mod_beta);

	//randomly negate D
			
	
		mpz_sub(D->tail, v, partialV);
	        mpz_mod(D->tail, D->tail, pubk->q);

		for(i=0; i<N; i++){            
			mpz_set_ui(D->element[i], 0);  

	            	for(j=0; j<sel; j++){
				mpz_set_si(a, common[(alpha[j]+(i+1)*mpz_get_ui(pubk->bias))%COMMON_LEN]);
	                        mpz_add(D->element[i], D->element[i], a);
	            	} 
	            	
		}        
	
	
	mpz_clear(partialV);
	mpz_clear(a);

	free(alpha);
	return;
}

void dtls_ma_print_cipher_string(struct Cipher* D){ 

  int i;
  printf("cipherS \n"); 
  for(i=0; i< N; i++){
	printf("%s, ",  mpz_get_str(NULL, 16, D->element[i]));
         
        if((i+1)%3==0){
		printf("\n");
	}
  }

  printf(" %s ", mpz_get_str(NULL, 16, D->tail));

  printf("\n"); 
}


int dtls_ma_cipher_out_str(struct Cipher* D, char* buf, int buf_len){
	int i, len=0, pos=0;
	for(i=0; i< N; i++){
		

		if(len>buf_len){
			#ifdef DTLS_MA_DEBUG			
			printf("buf not long enough: expected at least %d, actual %d", len, buf_len);
			#endif
			return -1;
		}

		char* temp = mpz_get_str(NULL,16, D->element[i]);

		sprintf(buf+pos, "%s,",  temp);
		pos=pos+strlen(temp)+1;
		len = len + strlen(temp)+1;
		free(temp);
        }
  
	char* temp = mpz_get_str(NULL,16, D->tail);
	len = len + strlen(temp)+1;

	if(len>buf_len){
		#ifdef DTLS_MA_DEBUG
		printf("The buf is too small: expected %d, actual %d", len, buf_len);
		#endif		
		return -1;
	}

	sprintf(buf+pos, "%s,",  temp);

	pos=pos+strlen(temp)+1;

	//printf("\n sprintf pos %d \n", pos);

	//for(i=0; i<len; i++){
	//	printf(" %02x ", buf[i]);
	//}	
	//printf("\n");
	free(temp);
	return len;
}

struct Cipher* dtls_ma_cipher_in_str(char* buf, int* last){
	struct Cipher* D = malloc(sizeof(struct Cipher));   
        char buf1[N*Q_BITS/8+64];
	int i, pos=0;
	mpz_init(D->tail);
        for(i=0; i<N; i++){
		mpz_init(D->element[i]);
	}

	for(i=0; i< N; i++){
		sscanf(buf+pos, "%[^,],", buf1);
		pos=pos+strlen(buf1)+1;
		if(pos>=N*Q_BITS/8+64){
			#ifdef DTLS_MA_DEBUG			
			printf("The buf1 in dtls_ma_cipher_in_str is too small: expected at least %d, actual %d\n", pos, N*Q_BITS/8+64);
			#endif
			free(D);
			return NULL;
		}
		buf1[pos]=0x00;
		mpz_set_str(D->element[i], buf1, 16);
        }


	sscanf(buf+pos, "%[^,],",  buf1);
	pos=pos+strlen(buf1)+1;
	buf1[pos]=0x00;

	//printf("\n sscanf pos %d \n", pos);
	
        mpz_set_str(D->tail, buf1, 16);

	*last = pos;

	return D;
}


void dtls_ma_dec(struct PrivKey* privk, struct Cipher* D, mpz_t v){
	//printf("decryption \n"); 


	mpz_init(v);
        mpz_t leftsum, a, b;
        mpz_init(leftsum);
        mpz_init(a);
        mpz_init(b);
        mpz_set_ui(leftsum, 0);

        int j; 
        for(j=0;j < N; j++){
                mpz_mul(a, privk->k[j], D->element[j]);
		//mpz_mod(a, a, privk->q);
                mpz_add(leftsum, leftsum, a);
		mpz_mod(leftsum, leftsum, privk->q);
	}



        mpz_add(leftsum, leftsum, D->tail);


	//leftsum = (privk->sk*leftsum)%privk->q;

	mpz_mul(a, privk->sk, leftsum);
	mpz_mod(leftsum, a, privk->q);
	if(E_SIGN=='+')
	{
		mpz_mul(a, leftsum, privk->skInvP);
		mpz_mod(b, a, privk->p);
		mpz_add_ui(v, b, 0);
	}
	else
	{
		mpz_sub(b, privk->q, leftsum);
		mpz_mul(a, b, privk->skInvP);
		mpz_mod(b, a, privk->p);
		mpz_sub(b, privk->p, b);
		mpz_add_ui(v, b, 0);
	}
	
        mpz_clear(leftsum);
        mpz_clear(a);
	mpz_clear(b);
	//return (unsigned short)leftsum;
}


unsigned char* dtls_ma_enc_str_G(struct PubKey* pubk, unsigned char* buf, int* len){
	int group_size = G==0?1:(T_BITS/8-1); //1 or (T_BITS/8-1)	
	
	mpz_t* v = malloc(group_size*sizeof(mpz_t));
	int buf_pos=0;
	int res_pos=0;
	int i;
	
	unsigned long buf_len = *len;
	
	int res_len = 64;
	unsigned char* res = (unsigned char *) malloc(res_len);

	struct Cipher* c = malloc(group_size*sizeof(struct Cipher)); 

	unsigned char temp_buf[N*Q_BITS/8+16];

	while(buf_pos<buf_len){
		int l;
		if(group_size>1){
				 
			l = dtls_ma_encodeG(buf+buf_pos, buf_len-buf_pos, v);
			
			//mpz_init (v0);
			//mpz_init (v1);
			//mpz_init (v2);	
			//mpz_init (v3);	

			//mpz_add_ui(v0, *(v+0), 0);
			//mpz_add_ui(v1, *(v+1), 0);
			//mpz_add_ui(v2, *(v+2), 0);
			//mpz_add_ui(v3, *(v+3), 0);

			//printf("\n encode 0 = %s \n", mpz_get_str(NULL, 16, v0));
                        //printf(" encode 1 = %s \n", mpz_get_str(NULL, 16, v1));
			//printf(" encode 2 = %s \n", mpz_get_str(NULL, 16, v2));
                        //printf(" encode 3 = %s \n", mpz_get_str(NULL, 16, v3));

		}
		else{
			l = dtls_ma_encode(buf+buf_pos, buf_len-buf_pos, *v);
		 	//printf("\n encode buf = %s \n", mpz_get_str(NULL, 2, *v));	 
		}
		
		buf_pos = buf_pos + l;	

		for(i=0; i < group_size; i++){
			dtls_ma_enc(pubk, *(v+i), c+i);
		}

		//printCipherS(c);


		for(i=0; i < group_size; i++){
			//printCipherS(c+i);

 			int cipher_len = dtls_ma_cipher_out_str(c+i, (char *) temp_buf, N*Q_BITS/8+16);
			if(cipher_len==-1){
				dtls_ma_debug("Error: dtls_ma_cipher_out_str \n");		
				return NULL;
			}

			if((res_pos + cipher_len)>res_len){
				res_len = (res_pos + cipher_len)+256;
				res = (unsigned char *) realloc(res, res_len);
				if(res==NULL){	
					dtls_ma_debug("realloc fails\n");
					return NULL;
				}
			}
			memcpy(res+res_pos, temp_buf, cipher_len);
			res_pos = res_pos + cipher_len;
		}
						
  	}
				
	*len = res_pos;
	free(c);
	free(v);		
	return res;
}

unsigned char* dtls_ma_dec_str_G(struct PrivKey* privk,unsigned char* buf, int* buf_len){
	int group_size = G==0?1:(T_BITS/8-1); //1 or (T_BITS/8-1)	
	int res_len = 4;
	unsigned char* res = malloc(res_len);

	int buf_pos =0;
	int res_pos =0;

	mpz_t* v = malloc(group_size*sizeof(mpz_t));
	int len=-1;
	int pos=1;
	int i;
	unsigned char temp_buf[N*Q_BITS/8];
	struct Cipher** D= malloc(group_size*sizeof(struct Cipher*));
	
	// int num_modes= 1<<group_size; unused variable

	int count =0;
	
	while(buf_pos < *buf_len){
		struct Cipher* temp = dtls_ma_cipher_in_str((char *) buf + buf_pos, &pos);
							
		D[count] = temp;
		//printCipherS(D[i]);
		buf_pos = buf_pos + pos;

		count++;
		if(count==group_size){
			count =0;	
		}
		else
			continue;
		
			
							
		//printCipherS(D[0]);	
		//decG(privk, D, v, group_size);
		for(i=0; i < group_size; i++){
			 //printCipherS(D[i]);
			 dtls_ma_dec(privk, D[i], *(v+i));
		}

		//if(mpz_cmp(v0, *(v+0))==0&&mpz_cmp(v1, *(v+1))==0&&mpz_cmp(v2, *(v+2))==0&&mpz_cmp(v3, *(v+3))==0){			
			//printf("decoded buf (mode %d) = %s \n", i, mpz_get_str(NULL, 16, *(v+0)));
                    	//printf("decoded buf (mode %d) = %s \n", i, mpz_get_str(NULL, 16, *(v+1)));
			//printf("decoded buf (mode %d) = %s \n", i, mpz_get_str(NULL, 16, *(v+2)));
			//printf("decoded buf (mode %d) = %s \n", i, mpz_get_str(NULL, 16, *(v+3)));
			//printf("\n");
			//debugcon=1;
		//}

		if(group_size>1){
			len=dtls_ma_decodeG(temp_buf, N*Q_BITS/8, v, 0);
		}
		else
			len=dtls_ma_decode(temp_buf, N*Q_BITS/8, *v);
			

		//printf("decoded buf len = %d \n", len);
		if(len<0){
			#ifdef DTLS_MA_DEBUG			
			printf("failed decryption len = %d \n", len);
			#endif
			*buf_len = -1;
			return NULL;
		}

		//free(D[0]);

		if((res_pos + len)>res_len){
			res_len = (res_pos + len)+256;
			res = (unsigned char *) realloc(res, res_len);
		}

		memcpy(res+res_pos, temp_buf, len);

		res_pos = res_pos + len;
	
	}	

	//mpz_clear(v);
	free(v);
	*buf_len = res_pos; 	
	return res;
}


/* from encode.c */

// static FILE *fp=NULL;
// static int seed=0;

int dtls_ma_encodeG(unsigned char* buf, unsigned long buf_len, mpz_t* res){

	unsigned int block_len = T_BITS/8;
	int i;


	unsigned int group_size = block_len-1;
	unsigned char* block_sign = malloc(block_len);

	for(i=0; i < block_len; i++){
		block_sign[i] = 0x00; 
	}

	unsigned char * tagged_buf;
	int ori_block_num = buf_len/group_size;
	if(buf_len%group_size!=0)		
		ori_block_num++;

	int tagged_buf_len;
	int return_len = 0;
	if(ori_block_num>group_size){
		tagged_buf_len = block_len*group_size; //e.g., 4*5 = 20
		return_len = tagged_buf_len - group_size;
	}
	else if (ori_block_num==group_size){
		tagged_buf_len = buf_len+group_size;
		return_len = buf_len;
	}
	else{
		tagged_buf_len = block_len*group_size;
		return_len = buf_len;	
	}		
			
	tagged_buf = (unsigned char *) malloc(tagged_buf_len);

	int g=0;
	while(1){	
		//   (AA _ _  AB _ _ _ _ AB _ _ _ _ AB _ _ _ _)
		unsigned char num_of_ab =0;	
		for(i=0; i < tagged_buf_len; i++){
			if(i%block_len==0){				
				if(g<ori_block_num)
					tagged_buf[i] = 0xAA;
				else{
					tagged_buf[i] = 0xAB;
					num_of_ab++;
				}
				g++;				
				continue;
			}

			unsigned char c;
			if(i-g<buf_len)
				c = *(buf+i-g);
			else{
				if(i==(tagged_buf_len-1)){
					c = buf_len%(block_len-1);
					unsigned char temp = num_of_ab<<4;
					c = c | temp;
					if(block_len-1>=16){
						dtls_ma_debug("\nPotential Error: too big T_BITS\n");
					}
				}
				else{
					unsigned char * byteC =  dtls_ma_random_bytes(1);
					c = *byteC;
					free(byteC);
				}
			}
			unsigned char s = common_sign[c]; ; 
			block_sign[g-1] = block_sign[g-1] ^ s^c;
		
			tagged_buf[i] = c;
		}
	


		for(i=0; i < block_len-1; i++){
			if(block_sign[i]==0xab||block_sign[i]==0xaa){
				block_sign[i] = 0xcc;
			} 
		}

		for(i=0; i < block_len-1; i++){
			block_sign[block_len-1] = block_sign[block_len-1]^block_sign[i]; 
		}	
	
		//printf("encode block sign\n");
		//for(i=0; i < block_len-1; i++){
		//	printf("%02x ", block_sign[i]);
		//}
		//printf("\n");	

		g=0;
		for(i=0; i < tagged_buf_len; i++){
			int index = i%block_len;
			if(index==0){
				tagged_buf[i] = tagged_buf[i]^block_sign[g];
				g++;
			}
			else{	
				if(index==g){
					tagged_buf[i] = tagged_buf[i]^block_sign[block_len-1];
				}
				else
				{
					tagged_buf[i] = tagged_buf[i]^block_sign[index-1];
				}
			}
		
		}

		if(num_of_ab==0)
			break;
		else{
			int num_of_ab_check =  dtls_ma_check(tagged_buf, tagged_buf_len);
			if(num_of_ab==num_of_ab_check)
				break;
			else{
				dtls_ma_debug("\n!!!!!Conflict addressed\n");
			}	

		}
	

	};		


	//printf("encode num_of_ab=%d, num_of_ab (check)= %d \n", num_of_ab, num_of_ab_check);


	for(g=0; g < group_size; g++){
		mpz_init(*(res+g));		
	}


	g=0;
	
	for(i=0; i < tagged_buf_len; i++){
		unsigned char c = *(tagged_buf+i);
		if(i%block_len==0){
			mpz_set_ui(*(res+g), c);
			g++;
			continue;
		}
				
		mpz_mul_2exp(*(res+g-1), *(res+g-1), 8);
		mpz_add_ui(*(res+g-1), *(res+g-1), c);
	}

	free(tagged_buf);
	//free(buf_len_sign);
	free(block_sign);
	return return_len;
}

int dtls_ma_check(unsigned char* tagged_buf, int tagged_buf_len){
	int i, j, k, g;
	unsigned int block_len = T_BITS/8;	 

	unsigned int group_size = block_len-1;

	unsigned char* tagged_buf_temp = malloc(group_size*block_len);

	g=0;
        k=0;

	unsigned char* block_sign = malloc(block_len);
	unsigned char* block_sign_check = malloc(block_len-1);


	unsigned char* tag = malloc(group_size);
	
	int temp_return_len = tagged_buf_len;//group_size*block_len;

		int p=group_size;  //0000, 0001, 0011, 0111
		for(k=0; k < group_size+1; k++){
			for(j=0; j <group_size; j++){
				if(j<p)
					tag[j] = 0xAA;
				else
					tag[j] = 0xAB;
			}	

			g=0;
			block_sign[block_len-1] = 0x00;
			for(i=0; i < temp_return_len; i++){
				if(i%block_len==0){
					block_sign[g] = tagged_buf[i]^tag[g];
					block_sign[block_len-1] = block_sign[block_len-1]^block_sign[g];
					g++;
				}
			}
	
			

			g=0;
			for(i=0; i < temp_return_len; i++){
				int index = i%block_len;
				if(index==0){
					tagged_buf_temp[i] = tagged_buf[i]^block_sign[g];
					g++;
				}
				else{	
					if(index==g){
						tagged_buf_temp[i] = tagged_buf[i]^block_sign[block_len-1];
					}
					else
					{
						tagged_buf_temp[i] = tagged_buf[i]^block_sign[index-1];
					}
				}
		
			}


			g = 0;
			for(i=0; i < temp_return_len; i++){
				if(i%block_len==0){
					block_sign_check[g] = 0x00;
					g++;
					continue;
				}
				unsigned char c = *(tagged_buf_temp+i);
		
				unsigned char s = common_sign[c]; 
				block_sign_check[g-1] = block_sign_check[g-1] ^ s^c;
		
			}
			
			for(i=0; i < block_len-1; i++){
				if(block_sign_check[i]==0xab||block_sign_check[i]==0xaa){
					block_sign_check[i] = 0xcc;
				} 
			}
			/*
				printf("\ndecode block sign\n");
					for(i=0; i < block_len-1; i++){
						printf("%02x ", block_sign[i]);
					}
				printf("\n");

				printf("dtls_ma_decode block sign check\n");
					for(i=0; i < block_len-1; i++){
						printf("%02x ", block_sign_check[i]);
					}
				printf("\n");

                         */

			for(g=0; g < group_size; g++){
				if(block_sign_check[g]!=block_sign[g])
					break;
			}
			
			if(g<group_size){
				p--;  //try another tag
				continue; //try another tag combination
			}
			else
				break;
	}

	free(block_sign);
	free(block_sign_check);
	free(tag);
	free(tagged_buf_temp);	
	//printf("p=%d \n", p);
	return (group_size-p);
}

int dtls_ma_decodeG(unsigned char* buf, unsigned long buf_len, mpz_t* res, int debug){

	int i, j, k, g;
	unsigned int block_len = T_BITS/8;	 

	unsigned int group_size = block_len-1;

	unsigned char* buf_temp = malloc(block_len);
	unsigned char* tagged_buf = malloc(group_size*block_len);
	unsigned char* tagged_buf_temp = malloc(group_size*block_len);

	mpz_t b;
	mpz_init(b);

	g=0;
        k=0;
	for(i=0; i<group_size*block_len; i++){
		int index= i%block_len;
		if(index==0){
			if(g>0){
				for(j=block_len-1; j >=0; j--){
					tagged_buf[k] = buf_temp[j];
					k++;
				}
			}
			for(j=0; j < block_len; j++)
				buf_temp[j] = 0x00;
			g++;
		}

		mpz_fdiv_r_2exp (b, *(res+g-1), 8);

		*(buf_temp+index) = (unsigned char) mpz_get_ui(b);
		mpz_fdiv_q_2exp (*(res+g-1), *(res+g-1), 8);

		if(i==group_size*block_len-1){
			for(j=block_len-1; j >=0; j--){
				if(buf_temp[j]==0x00)
					continue;
				else
					break;	
			}
				
			for(; j>=0; j--){
				tagged_buf[k] = buf_temp[j];
				k++;
			}
		}
	}

	mpz_clear(b);

	//printf("\n");
	//for(i=0; i < k; i++){
	//	printf("%02x ", tagged_buf[i]);
	//}
	//printf("\n");

	int temp_return_len = k;	

	unsigned char* block_sign = malloc(block_len);
	unsigned char* block_sign_check = malloc(block_len-1);


	unsigned char* tag = malloc(group_size);
	

		int p=group_size;  //0000, 0001, 0011, 0111
		for(k=0; k < group_size+1; k++){
			for(j=0; j <group_size; j++){
				if(j<p)
					tag[j] = 0xAA;
				else
					tag[j] = 0xAB;
			}
			
			//printf("\n(%d, %d)", k,p);
			//for(j=0; j <group_size; j++){
			//	printf("%02x ", tag[j]);
			//}	
			//printf("\n");		

			g=0;
			block_sign[block_len-1] = 0x00;
			for(i=0; i < temp_return_len; i++){
				if(i%block_len==0){
					block_sign[g] = tagged_buf[i]^tag[g];
					block_sign[block_len-1] = block_sign[block_len-1]^block_sign[g];
					g++;
				}
			}

			{
			//        printf("dtls_ma_decode block sing: \n");
			//	for(i=0; i < block_len-1; i++){
			//		printf("%02x ", block_sign[i]);
			//	}
			//	printf(" (");
                        //        for(i=0; i < block_len-1; i++){
			//		printf("%02x ", tag[i]);
			//	}
			//	printf(")\n");
                         //       printf("--------------------------------\n");

			}	


			g=0;
			for(i=0; i < temp_return_len; i++){
				int index = i%block_len;
				if(index==0){
					tagged_buf_temp[i] = tagged_buf[i]^block_sign[g];
					g++;
				}
				else{	
					if(index==g){
						tagged_buf_temp[i] = tagged_buf[i]^block_sign[block_len-1];
					}
					else
					{
						tagged_buf_temp[i] = tagged_buf[i]^block_sign[index-1];
					}
				}
		
			}


			g = 0;
			for(i=0; i < temp_return_len; i++){
				if(i%block_len==0){
					block_sign_check[g] = 0x00;
					g++;
					continue;
				}
				unsigned char c = *(tagged_buf_temp+i);
		
				unsigned char s = common_sign[c]; 
				block_sign_check[g-1] = block_sign_check[g-1] ^ s^c;
		
			}
			
			for(i=0; i < block_len-1; i++){
				if(block_sign_check[i]==0xab||block_sign_check[i]==0xaa){
					block_sign_check[i] = 0xcc;
				} 
			}

			for(g=0; g < group_size; g++){
				if(block_sign_check[g]!=block_sign[g])
					break;
			}

			if(g<group_size){
				p--;  //try another tag
				continue; //try another tag combination
			}

			//adjust temp_return_len
			int num_of_AA = p;
			int num_of_AB = group_size -p;	
			
			unsigned char len_of_last_AA=-1;
			// unsigned char len_of_last_AB=-1; // unused variable
			//unsigned char num_of_AB_check;
			int return_len;
			if(num_of_AB>=1){
				len_of_last_AA = (unsigned char) *(tagged_buf_temp+temp_return_len-1);
				// len_of_last_AB = (unsigned char) *(tagged_buf_temp+temp_return_len-1);


				len_of_last_AA = len_of_last_AA&0x0F;

				//num_of_AB_check = (len_of_last_AB&0xF0)>>4; 

				//if(num_of_AB!=num_of_AB_check)
				//	continue;

				if(len_of_last_AA==0)
					return_len= num_of_AA*(block_len-1);	
				else
					return_len= (num_of_AA-1)*(block_len-1) + len_of_last_AA;

			}
			else{
				return_len = temp_return_len - group_size;	
			}
			 

			if(return_len>buf_len){
				free(block_sign);
				free(block_sign_check);
				free(tag);
				free(tagged_buf);
				free(tagged_buf_temp);	
				free(buf_temp);
				#ifdef DTLS_MA_DEBUG				
				printf("Error: buf is not long enough (expected length %d bytes)\n", return_len);
				#endif

				return -2;
			}

			if(return_len>0){
				j=0;
				for(i=0; i < temp_return_len; i++){
					if(i%block_len==0){
						continue;
					}
					*(buf+j) = *(tagged_buf_temp+i);
					j++;	
					if(j==return_len)
						break;
				}
			}

			/*
			int l;
			unsigned char temp=0x00;
			for(l=0; l<return_len-1; l++){
				temp=temp^buf[l]^(common_sign[buf[l]]);
			}
			//printf("dtls_ma_decode buf sign: %02x\n", temp);			
			if(buf[l]!=temp){
				printf("------------useful\n");
				continue;
			}
			
			return_len = return_len-1;
			*/
			free(block_sign);
			free(block_sign_check);
			free(tag);
			free(tagged_buf);			
			free(buf_temp);
			free(tagged_buf_temp);	
			//printf("\nreturn length %d bytes, p=%d, last-AA-len=%d, num_of_AB=%d, num_of_AA=%d, num_of_AB_check=%d\n", return_len, p, len_of_last_AA, num_of_AB, num_of_AA, num_of_AB_check);
			return return_len;
			
		
		}
	
	//printf("Error: res is not correctly encoded\n");
	free(block_sign);
	free(block_sign_check);
	free(tag);
	free(tagged_buf);
	free(tagged_buf_temp);	
	free(buf_temp);
	return -1;	
}


int dtls_ma_encode(unsigned char* buf, unsigned long buf_len, mpz_t res){
	if(buf_len<=0)
		return -1;

	unsigned int block_len = T_BITS/8;
	int i;
	unsigned char block_sign = 0x00;
	unsigned char block_sign_backup = 0x00;	

	unsigned char * tagged_buf;
	int tagged_buf_len = 0;
	if(buf_len>=block_len){
		tagged_buf_len = block_len;
	}
	else{
		tagged_buf_len = buf_len+1;	
	}

	tagged_buf = (unsigned char *) malloc(tagged_buf_len);


	tagged_buf[0] = 0xAA;
	for(i=1; i < tagged_buf_len; i++){
		unsigned char c = *(buf+i-1);
		
		unsigned char s = common_sign[c];  
		block_sign = block_sign ^ s^c;
		block_sign_backup = block_sign_backup^common_sign[(c+1)%256]^c;
		
		tagged_buf[i] = c;
	}

	if(block_sign==0xAA){
		if(block_sign_backup!=0xAA)
			block_sign=block_sign_backup;
		else
			block_sign=0xCC;
	}
	//printf("(%02x %02x %02x; %02x)\n",tagged_buf[0], tagged_buf[1], tagged_buf[2], block_sign);

	for(i=0; i < tagged_buf_len; i++){
		tagged_buf[i] = tagged_buf[i]^block_sign;
	}

	mpz_init(res);
	mpz_set_ui(res, tagged_buf[0]);
	for(i=1; i < tagged_buf_len; i++){
		unsigned char c = *(tagged_buf+i);
				
		mpz_mul_2exp(res, res, 8);
		mpz_add_ui(res, res, c);
	}

	return tagged_buf_len-1;
}

int dtls_ma_decode(unsigned char* buf, unsigned long buf_len, mpz_t res){
	mpz_t b;
	mpz_init(b);

	unsigned char* buf_temp = malloc(T_BITS/8);

	
	unsigned int block_len = T_BITS/8;
	int i, j; 
	for(i=0; i<block_len; i++){
		mpz_fdiv_r_2exp (b, res, 8);

		*(buf_temp+i) = mpz_get_ui(b);
		mpz_fdiv_q_2exp (res, res, 8);
	}	

	mpz_clear(b);
	
	unsigned char block_sign_checked = 0x00;
        unsigned char block_sign = 0x00;
	unsigned char block_sign_backup = 0x00;

	for(i=block_len-1; i>=0; i--){
		if(buf_temp[i]==0x00)
			continue;
		else{ 
			
			block_sign = buf_temp[i]^0xAA;
			break;
		}
	
	}

	for(i=block_len-1; i>=0; i--){
		if(buf_temp[i]==0x00)
			continue;
		else{ 
			unsigned int count = i; 
			for(j=0; j< count; j++){
				*(buf+j) = buf_temp[count-1-j]^block_sign; 
				//produce signature
				unsigned char s = common_sign[*(buf+j)]; ; 
				block_sign_checked = block_sign_checked ^ s^(*(buf+j));
				block_sign_backup = block_sign_backup^common_sign[(*(buf+j)+1)%256]^*(buf+j);
			} 

			if(block_sign_checked==0xAA){
				if(block_sign_backup!=0xAA)
					block_sign_checked=block_sign_backup;	
				else	
					block_sign_checked=0xCC;
			}
			//check signature
			if(block_sign_checked==block_sign)
				return count;
			else    //wrong signature return -1
				return -1;
			

			
			
		}
	
	}	


	return -1;	
}



unsigned char* dtls_ma_random_bytes(int buflen){
	// int i=0; // unused variable
	
	unsigned char* buf = malloc(buflen+1);	
	
	buf[buflen]=0X00;
	
	if( access("/dev/urandom", F_OK ) != -1 ) {
	    	if(dtls_ma_urandom_fp==NULL){
			dtls_ma_urandom_fp = fopen("/dev/urandom", "r");
		        dtls_ma_debug("open /dev/urandom \n");
	        }
	} 

	
	size_t rdLen = 0;


        if(dtls_ma_urandom_fp == NULL){
		if(dtls_ma_seed==0){
			dtls_ma_seed=1;
                        srand(time(NULL));
		}

		while (rdLen < buflen)
		{
	    		unsigned char c = rand()%256;
	    		buf[rdLen] = c;
			rdLen++;
		}
		
		return buf;
	}

	while (rdLen < buflen)
	{
    		unsigned char c = getc(dtls_ma_urandom_fp);
    		if ( c == EOF)
    		{
		        return NULL; 
		}
    		buf[rdLen] = c;
		rdLen++;
	}
	close((int)dtls_ma_urandom_fp);	
	
	return buf;
}

unsigned char* dtls_ma_random_hex(int len){

	unsigned char* temp = dtls_ma_random_bytes(len);

	unsigned char* res = malloc(2*len+1);
        if(res==NULL)
	dtls_ma_debug("malloc failure!\n"); 

	
	int pos=0;
        int i;

	for(i=0; i < len; i++){
		sprintf((char *) res+pos, "%02X", (unsigned char)temp[i]);
		pos=pos+2;
	}
	res[2*len] = 0x00;
	free(temp);

	return res;
}


void dtls_ma_debug(char * msg){
#ifdef DTLS_MA_DEBUG
	printf("%s\n", msg);
#endif
}