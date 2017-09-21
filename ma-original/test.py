#!/usr/bin/python
import lpki

lpki.keygen()
value = lpki.encrypt("./pubK.txt","abcdef")
print value
print lpki.decrypt("./privK.txt",value)

