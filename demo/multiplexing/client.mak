PROJNAME= GreetingsDemo
APPNAME= greetings_client

include ../config.mak

SRC= client/client.cpp stubs/greetings.cc

INCLUDES+= stubs

STUBS=stubs/greetings.h stubs/greetings.cc
IDLS=idl/greetings.idl

genstubs: $(STUBS)

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; $(MICO_BIN)/idl --no-paths --any --typecode ../idl/greetings.idl

