from distutils.core import setup, Extension
setup(name='lpki', version='1.0',  \
      ext_modules=[Extension('lpki', ['pki.c','encode.c','crypt.c','mini-gmp.c'])])
