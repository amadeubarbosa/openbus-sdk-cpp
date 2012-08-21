PROJNAME= SharedAuthClient
APPNAME= sharedauth_client

include ../config.mak

SRC= src/client.cpp stubs/hello.cc stubs/sharedauth.cc

INCLUDES+= stubs

STUBS=stubs/hello.h stubs/hello.cc stubs/sharedauth.h stubs/sharedauth.cc
IDLS=idl/hello.idl idl/sharedauth.idl

genstubs: $(STUBS)

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; $(MICO_BIN)/idl --no-paths --any --typecode ../idl/hello.idl
	cd stubs ; $(MICO_BIN)/idl --no-paths --any --typecode ../idl/sharedauth.idl

