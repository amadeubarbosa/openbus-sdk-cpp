PROJNAME=client
APPNAME=${PROJNAME}

include config

USE_NODEPEND= Yes

SRC= src/client.cpp stubs/hello.cc stubs/encoding.cc

IDLS= idl/hello/hello.idl idl/sharedauth/encoding.idl

STUBS= stubs/hello.cc stubs/hello.h stubs/encoding.h stubs/encoding.cc

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --no-paths --poa --any --typecode ../idl/hello/hello.idl
	cd stubs ; ${MICO_BIN}/idl --no-paths --poa --any --typecode ../idl/sharedauth/encoding.idl

genstubs: $(STUBS)

