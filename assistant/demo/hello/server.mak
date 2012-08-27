PROJNAME= HelloDemo
APPNAME= hello_server

include ../config.mak

SRC= server/server.cpp stubs/hello.cc

INCLUDES+= ./stubs

STUBS=stubs/hello.h stubs/hello.cc
IDLS=idl/hello.idl

genstubs: $(STUBS)

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs
	$(MICO_BIN)/idl --no-paths --any --typecode ../idl/hello.idl
