#include "crypt.h"
#include "encode.h"
#include <stdio.h> /* For printf() */
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

static FILE *fp=NULL;
static int seed=0;

int encodeG(unsigned char* buf, unsigned long buf_len, mpz_t* res){

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
						printf("\nPotential Error: too big T_BITS\n");
					}
				}
				else{
					unsigned char * byteC =  randBytes(1);
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
			int num_of_ab_check =  check(tagged_buf, tagged_buf_len);
			if(num_of_ab==num_of_ab_check)
				break;
			else{
				printf("\n!!!!!Conflict addressed\n");
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

int check(unsigned char* tagged_buf, int tagged_buf_len){
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

				printf("decode block sign check\n");
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

int decodeG(unsigned char* buf, unsigned long buf_len, mpz_t* res, int debug){

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
			//        printf("decode block sing: \n");
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
			unsigned char len_of_last_AB=-1;
			//unsigned char num_of_AB_check;
			int return_len;
			if(num_of_AB>=1){
				len_of_last_AA = (unsigned char) *(tagged_buf_temp+temp_return_len-1);
				len_of_last_AB = (unsigned char) *(tagged_buf_temp+temp_return_len-1);


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

				printf("Error: buf is not long enough (expected length %d bytes)\n", return_len);

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
			//printf("decode buf sign: %02x\n", temp);			
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


int encode(unsigned char* buf, unsigned long buf_len, mpz_t res){
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

int decode(unsigned char* buf, unsigned long buf_len, mpz_t res){
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


unsigned char* randBytes(int buflen){
	int i=0;

	unsigned char* buf = malloc(buflen+1);	
	
	buf[buflen]=0X00;
	
	if( access("/dev/urandom", F_OK ) != -1 ) {
	    	if(fp==NULL){
	   		fp = fopen("/dev/urandom", "r");
		        printf("open /dev/urandom \n");
	        }
	} 

	
	size_t rdLen = 0;


        if(fp==NULL){
		if(seed==0){
			seed=1;
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
    		unsigned char c = getc(fp);
    		if ( c == EOF)
    		{
		        return NULL; 
		}
    		buf[rdLen] = c;
		rdLen++;
	}
	close((int)fp);	
	
	return buf;
}

unsigned char* randomhex(int len){

	unsigned char* temp = randBytes(len);

	unsigned char* res = malloc(2*len+1);
        if(res==NULL)
		printf("malloc failure!\n"); 

	
	int pos=0;
        int i;

	for(i=0; i < len; i++){
		sprintf(res+pos, "%02X", (unsigned char)temp[i]);
		pos=pos+2;
	}
	res[2*len] = 0x00;
	free(temp);

	return res;
}


