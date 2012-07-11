PROJNAME= GreetingsDemo
APPNAME= greetings_server

include ../config.mak

SRC= server/server.cpp stubs/greetings.cc

INCLUDES+= stubs
