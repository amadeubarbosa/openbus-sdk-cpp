PROJNAME= AuditDemo
APPNAME= audit_proxy

include ../config.mak

SRC= proxy/proxy.cpp stubs/hello.cc

INCLUDES+= stubs

