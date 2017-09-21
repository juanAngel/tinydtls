
#include "crypt.h"
#include "encode.h"
#include <stdio.h> /* For printf() */
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

static mpz_t v0;
static mpz_t v1;
static mpz_t v2;
static mpz_t v3;



void genPrivKey(struct PrivKey* privk){
	// = (struct PrivKey*) malloc(sizeof(struct PrivKey)); 
        //srand(1234624);


	int i, j;
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
	unsigned char* skStep = randomhex(SK_BITS/8);
        mpz_set_str(privk->sk, skStep, 16);
	free(skStep);
        mpz_mul_ui (privk->sk, privk->sk, 2);
        mpz_add_ui (privk->sk, privk->sk, 1); //privk->sk co-primise with q

       
	//  pStep = (privk->q - (privk->t-1lu)*privk->sk)/(W*privk->r) - privk->t; //$sk*(t-1)+w*r*(t+pStep)<q$
	// do validity check
        mpz_sub_ui (a, privk->t, 1);
        mpz_mul (b, a, privk->sk);
	
	if(mpz_cmp(b, privk->q)>=0){
		printf("\nError: the parameters T_BITS or SK_BITS too big with respect to Q_BITS\n");
		return;
	}

	

        mpz_sub (a, privk->q, b); //a = privk->q - (privk->t-1lu)*privk->sk

        mpz_mul_ui (b, privk->r, W);


        mpz_fdiv_q (a, a, b); //a will be the maximum of random numbers that can be added to t to get privk->p
	mpz_add_ui(b, privk->t, 0xFF);

	if(mpz_cmp(a,b)<0){
		printf("\nError: the parameters E_BITS and SK_BITS too big with respect to Q_BITS\n");
		return;
	}	

	mpz_sub (a, a, privk->t);
	printf("max pstep: %s \n", mpz_get_str(NULL, 16, a));	


	//privk->p = privk->t + pStep*2+1; //since 4294967296 has only the factor 2 
	unsigned char* pStep = randomhex(SK_BITS/8);	
        mpz_set_str(b, pStep, 16);
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
			printf("sk: %s coprime with: %s\n", mpz_get_str(NULL, 16, privk->sk), mpz_get_str(NULL, 16, privk->p));
                        mpz_mul_ui (privk->skInvP, c, 1);
                        printf("skInvP: %s \n", mpz_get_str(NULL, 16,privk->skInvP));
                        break; 
	        }
		else{
			pStep = randomhex(SK_BITS/8);	
        		mpz_set_str(b, pStep, 16);
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
		unsigned char* k = randomhex(Q_BITS/8);	
		mpz_set_str(privk->k[i], k, 16);
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

void printPrivK(struct PrivKey* privk){
 
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

void savePrivK(const char* fname, struct PrivKey* privk){
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



int loadPrivK(const char* fname, struct PrivKey* privk){
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

}

void savePubKey(const char* fname, struct PubKey* pubk){
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

 struct PubKey* loadPubKey(const char* fname){
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

struct PubKey* getPubKey(struct PrivKey* privk){

	struct PubKey* pubk = malloc(sizeof(struct PubKey));   
    
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
	
	
	unsigned char* byte = randBytes(sizeof(unsigned int));
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

        mpz_t skInvQ, leftSum, a, b, e;
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


		unsigned char* error = randomhex(Q_BITS/8);	
		mpz_set_str(e, error, 16);
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
	return pubk;


}


void printSISSample(char* msg, struct PubKey* pubk, struct Cipher* D){
	//printPubK(msg, pubk);
        //printCipherS(D); 
        int i, j;
        printf("\n\n Copy the expression to https://develop.wolframcloud.com/objects/63785264-af0d-49e5-83e4-36465169a55d\n\nFindInstance[\{\n"); 
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


void printPubK(char* msg, struct PubKey* pubk){

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


void enc(struct PubKey* pubk, mpz_t v, struct Cipher* D){
        mpz_mod(v, v, pubk->t);
	
	unsigned int i,j;
	mpz_init(D->tail);
        for(i=0; i<N; i++){
		mpz_init(D->element[i]);
	}	

	unsigned short sel;

	unsigned char* rbytes = randBytes(2);
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
		rbytes = randBytes(1);
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

void printCipherS(struct Cipher* D){ 

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


int cipher_out_str(struct Cipher* D, char* buf, int buf_len){
	int i, len=0, pos=0;
	for(i=0; i< N; i++){
		

		if(len>buf_len){
			printf("buf not long enough: expected at least %d, actual %d\n", len, buf_len);	
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
		printf("The buf is too small: expected %d, actual %d\n", len, buf_len);	
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

struct Cipher* cipher_in_str(char* buf, int* last){
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
			printf("The buf1 in cipher_in_str is too small: expected at least %d, actual %d\n", pos, N*Q_BITS/8+64);
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


void dec(struct PrivKey* privk, struct Cipher* D, mpz_t v){
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


unsigned char* enc_str_G(struct PubKey* pubk, unsigned char* buf, int* len){
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
				 
			l = encodeG(buf+buf_pos, buf_len-buf_pos, v);
			
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
			l = encode(buf+buf_pos, buf_len-buf_pos, *v);
		 	//printf("\n encode buf = %s \n", mpz_get_str(NULL, 2, *v));	 
		}
		
		buf_pos = buf_pos + l;	

		for(i=0; i < group_size; i++){
			enc(pubk, *(v+i), c+i);
		}

		//printCipherS(c);


		for(i=0; i < group_size; i++){
			//printCipherS(c+i);

 			int cipher_len = cipher_out_str(c+i, temp_buf, N*Q_BITS/8+16);
			if(cipher_len==-1){
				printf("Error: cipher_out_str \n");		
				return NULL;
			}

			if((res_pos + cipher_len)>res_len){
				res_len = (res_pos + cipher_len)+256;
				res = (unsigned char *) realloc(res, res_len);
				if(res==NULL){	
					printf("realloc fails\n");
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

unsigned char* dec_str_G(struct PrivKey* privk,unsigned char* buf, int* buf_len){
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
	
	int num_modes= 1<<group_size;

	int count =0;
	
	while(buf_pos < *buf_len){
		struct Cipher* temp = cipher_in_str(buf+buf_pos, &pos);
							
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
			 dec(privk, D[i], *(v+i));
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
			len=decodeG(temp_buf, N*Q_BITS/8, v, 0);
		}
		else
			len=decode(temp_buf, N*Q_BITS/8, *v);
			

		//printf("decoded buf len = %d \n", len);
		if(len<0){
			printf("failed decryption len = %d \n", len);
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


