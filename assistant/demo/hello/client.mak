PROJNAME= HelloDemo
APPNAME= hello_client

include ../config.mak

SRC= client/client.cpp stubs/hello.cc

INCLUDES+= ./stubs
