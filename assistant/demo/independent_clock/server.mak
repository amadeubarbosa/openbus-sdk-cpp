PROJNAME= IndependentClockDemo
APPNAME= independent_clock_server

include ../config.mak

SRC= server/server.cpp stubs/independent_clock.cc

INCLUDES+= stubs
