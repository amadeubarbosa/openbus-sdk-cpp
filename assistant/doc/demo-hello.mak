#[demo_hello_mak_set_mico_bin
MICO_BIN= $(CURDIR)/../../bin/mico-mt-d
MICO_LIB= $(CURDIR)/../../lib/mico-mt-d
#]
#[demo_hello_mak_compile_idl
bin/hello.cc bin/hello.h: idl/hello.idl
        cd bin; LD_LIBRARY_PATH=$(MICO_LIB):${LD_LIBRARY_PATH} PATH=${PATH}:$(MICO_BIN) $(MICO_BIN)/idl --typecode --poa --no-paths $<
#]
#[demo_hello_mak_compile_server_cpp
bin/server.o: server/server.cpp bin/hello.h
	gcc $< -o $@ $(CXXFLAGS)
#]
