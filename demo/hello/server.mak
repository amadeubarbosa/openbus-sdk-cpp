PROJNAME= HelloDemo
APPNAME= hello_server

include ../config.mak

SRC= server/server.cpp stubs/hello.cc

INCLUDES+= stubs
