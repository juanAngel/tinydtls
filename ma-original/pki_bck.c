
/**
 * \file
 *         Implmentation of a lightweight puiblic key encryption scheme developed in CSIRO
 * \author
 *         Dongxi Liu <dongxi.liu@csiro.au>
 */



#include "crypt.h"
#include "encode.h"
#include <stdio.h> /* For printf() */
#include <stdlib.h>
#include <string.h>
#include <math.h>



/*---------------------------------------------------------------------------*/
void main(int argc, char **argv){
  srand(time(NULL));

   int i, j; 
  struct PrivKey privk, privkLoad;
  genPrivKey(&privk); 

  struct PubKey* pubk; 
  pubk = getPubKey(&privk); 

  printPrivK(&privk);


 
  for(i=0; i < 5; i++){
	int len= 26;
	int vlen=len;	
	unsigned char vran[] = "abcdefghijklmnopqrstunvwxyz";
  	
  	char* buf= enc_str_G(pubk, vran, &len);
	
	printf("\n-ciphertext length: %d\n", len);
  	
  	unsigned char* buf1 = dec_str_G(&privk, buf, &len);

  	printf("\n-decrypted msg length: %d\n", len);
        for(j=0; j < len; j++)
  	      printf("%c", buf1[j]);

	free(buf);
	free(buf1);
    
  } 


  
  printf("\n+++++++++end \n");

  printf("\n");

}

