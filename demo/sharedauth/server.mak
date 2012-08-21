PROJNAME= SharedAuthDemo
APPNAME= sharedauth_server

include ../config.mak

SRC= src/server.cpp stubs/hello.cc

INCLUDES+= stubs
