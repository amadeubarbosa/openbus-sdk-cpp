PROJNAME= IndependentClockDemo
APPNAME= independent_clock_client

include ../config.mak

SRC= client/client.cpp stubs/independent_clock.cc

INCLUDES+= stubs

STUBS=stubs/independent_clock.h stubs/independent_clock.cc
IDLS=idl/independent_clock.idl

genstubs: $(STUBS)

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; $(MICO_BIN)/idl --no-paths --any --typecode ../idl/independent_clock.idl

