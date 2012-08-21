PROJNAME= SharedAuthClientSharedAuth
APPNAME= sharedauth_client_sharedauth

include ../config.mak

SRC= src/client_sharedauth.cpp stubs/hello.cc stubs/sharedauth.cc

INCLUDES+= stubs
