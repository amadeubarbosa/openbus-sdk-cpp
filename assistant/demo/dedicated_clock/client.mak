PROJNAME= DedicatedClockDemo
APPNAME= dedicated_clock_client

include ../config.mak

SRC= client/client.cpp stubs/dedicated_clock.cc

INCLUDES+= stubs

STUBS=stubs/dedicated_clock.h stubs/dedicated_clock.cc
IDLS=idl/dedicated_clock.idl

genstubs: $(STUBS)

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; $(MICO_BIN)/idl --no-paths --any --typecode ../idl/dedicated_clock.idl

