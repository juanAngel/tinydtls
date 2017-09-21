
/**
 * \file
 *         Implmentation of a lightweight puiblic key encryption scheme developed in CSIRO
 * \author
 *         Dongxi Liu <dongxi.liu@csiro.au>
 */



#include "crypt.h"
#include "encode.h"
#include "Python.h"

#include <stdio.h> /* For printf() */
#include <stdlib.h>
#include <math.h>


/*---------------------------------------------------------------------------*/
static PyObject* lpki_enc(PyObject *self, PyObject *args){
  char *command;
  char *pubfile;
  int len;
  struct PubKey* pubk; 

  if (!PyArg_ParseTuple(args, "ss#", &pubfile, &command, &len))
	return NULL;

  pubk = loadPubKey(pubfile);   
  
  printf("load pub key \n");
  char* buf= enc_str_G(pubk, command, &len);

  printf("\n-ciphertext length: %d\n", len);

  return Py_BuildValue("s#", buf,len);
}

static PyObject* lpki_dec(PyObject *self, PyObject *args){
  struct PrivKey privk;
  int size;
  char *buf;
  char *privfile;

	if (!PyArg_ParseTuple(args, "ss#", &privfile,&buf,&size))
		return NULL;

  loadPrivK(privfile, &privk);   
  unsigned char* buf1 = dec_str_G(&privk, buf, &size);


  printf("buf size = %d\n", size);	
	
  return Py_BuildValue("s#",buf1,size);
 
}

static PyObject* lpki_keygen(PyObject *self, PyObject *args){
  struct PrivKey privk, privkLoad;
  genPrivKey(&privk); 
 
  savePrivK("./privK.txt", &privk);
  loadPrivK("./privK.txt", &privkLoad); 

  struct PubKey* pubk; 
  pubk = getPubKey(&privkLoad);      
  savePubKey("./pubK.txt", pubk);

  return Py_BuildValue("i",1);
 
}


static PyMethodDef lpkiMethods[] = {
    {"encrypt",  (PyCFunction)lpki_enc, METH_VARARGS,
     "encrypt a message."},
    {"keygen",  (PyCFunction)lpki_keygen, METH_VARARGS,
     "Generating a key pair."},
    {"decrypt",  (PyCFunction)lpki_dec, METH_VARARGS,
     "decrypt a CT."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC initlpki(void)
{
	(void) Py_InitModule("lpki", lpkiMethods);
}


