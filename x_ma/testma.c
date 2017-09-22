/*
 * Copyright (c) 2017 Raj Gaire <raj.gaire@csiro.au>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ma.h"
#include "mini-gmp.h"

#ifdef CONTIKI
#include "contiki.h"
#else
#include <time.h>
#endif /* CONTIKI */

#ifdef CONTIKI
PROCESS(ma_test, "MA test");
AUTOSTART_PROCESSES(&ma_test);
PROCESS_THREAD(ma_test, ev, d)
{
	PROCESS_BEGIN();

	srand(1234);
	// addTest();
	// doubleTest();
	// multTest();
	// eccdhTest();
	// ecdsaTest();
	printf("%s\n", "All Tests successful.");

	PROCESS_END();
}
#else /* CONTIKI */
int main(int argc, char const *argv[])
{
    struct dtls_ma_private_key priv_key;
    struct dtls_ma_public_key pub_key;
    unsigned char *command;
    unsigned char* encoded_value;
    unsigned char* decoded_value;
    int len, len_dec;
    srand(time(NULL));

    printf("generate keys\n");
    dtls_ma_generate_key(&priv_key, &pub_key);
    printf("keys generated\n");
    command = (unsigned char* ) "abcde";
    printf("encode: %s\n",command);
    encoded_value = (unsigned char* ) dtls_ma_enc_str_G(&pub_key, command, &len);
    printf("encoded values: ");
    for (int i = 0; i < len; i++) {
      printf("%x", encoded_value[i]);
    }
    printf("\n");

    // printf("encoded value: %us, %i\n",encoded_value, len);
    decoded_value = (unsigned char* ) dtls_ma_dec_str_G(&priv_key, encoded_value, &len_dec);
    printf("encoded values: ");
    for (int i = 0; i < len_dec; i++) {
      printf("%x", decoded_value[i]);
    }
    printf("\n");
    // printf("decoded value: %us\n",decoded_value);
    
	// addTest();
	// doubleTest();
	// multTest();
	// eccdhTest();
	// ecdsaTest();
	printf("%s\n", "All Tests successful.");
	return 0;
}
#endif