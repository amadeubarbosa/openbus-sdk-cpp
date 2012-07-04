PROJNAME= AuditDemo
APPNAME= audit_server

include ../config.mak

SRC= server/server.cpp stubs/hello.cc

INCLUDES+= stubs

