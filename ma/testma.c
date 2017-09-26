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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ma.h"
// #include "encode.h"
#include "mini-gmp.h"

#ifdef CONTIKI
#include "contiki.h"
#else
#include <time.h>
#endif /* CONTIKI */

#ifdef CONTIKI
PROCESS(ma_test, "MA test");
AUTOSTART_PROCESSES(&ma_test);
PROCESS_THREAD(ma_test, ev, d) {
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

static char* lpki_enc(char* pubfile, char* command) {
    struct dtls_ma_public_key* pubk;
    int len = strlen(command);

    printf("load pub key \n");
    pubk = dtls_ma_load_public_key(pubfile);

    printf("encode command: %s, length: %d \n", command, len);
    char* buf = (char*)dtls_ma_enc_str_G(pubk, (unsigned char*)command, &len);

    printf("\n-ciphertext length: %d\n", len);
    return buf;
}

static char* lpki_dec(char* privfile, char* buf, int size) {
    struct dtls_ma_private_key privk;
    dtls_ma_load_private_key(privfile, &privk);
    char* buf1 = (char*)dtls_ma_dec_str_G(&privk, (unsigned char*)buf, &size);

    printf("buf size = %d\n", size);
    buf1[size] = '\0';
    printf("decoded value: %s\n", buf1);
    return buf1;
}

static void lpki_keygen() {
    struct dtls_ma_private_key privk;
    struct dtls_ma_public_key pubk;

    dtls_ma_generate_keys(&privk, &pubk);

    printf("save private key\n");
    dtls_ma_save_private_key("./privK.txt", &privk);
    // loadPrivK("./privK.txt", &privkLoad);

    // pubk = getPubKey(&privkLoad);
    printf("save public key\n");
    dtls_ma_save_public_key("./pubK.txt", &pubk);
}

int main(int argc, char const* argv[]) {
    // struct dtls_ma_private_key priv_key;
    // struct dtls_ma_public_key pub_key;
    char* command;
    char* encoded_value;
    char* decoded_value;
    int len_dec;

    // srand(time(NULL));

    printf("generate keys\n");
    lpki_keygen();
    // dtls_ma_generate_key(&priv_key, &pub_key);
    printf("keys generated\n");
    command = (char*)"abcdef";
    encoded_value = (char*)lpki_enc("./pubK.txt", command);

    printf("encoded value: %s\n", encoded_value);
    len_dec = strlen(encoded_value);
    decoded_value = (char*)lpki_dec("./privK.txt", encoded_value, len_dec);
    printf("decoded value: %s\n", decoded_value);

    // addTest();
    // doubleTest();
    // multTest();
    // eccdhTest();
    // ecdsaTest();
    printf("%s\n", "All Tests successful.");
    return 0;
}
#endif