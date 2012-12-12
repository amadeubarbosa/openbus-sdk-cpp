#[demo_hello_mak_set_mico_bin
MICO_BIN= $(CURDIR)/../../bin/mico-mt-d
MICO_LIB= $(CURDIR)/../../lib/mico-mt-d
#]
#[demo_hello_mak_set_ldflags
LDFLAGS= -L$(MICO_LIB) -L$(CURDIR)/../../lib -Wl,-Bstatic,-lmico-2.3.13             \
  -Wl,-Bstatic,-lassistant-mico-mt-s-d,-lboost_chrono-mt-d,-lboost_system-mt-d      \
  -Wl,-Bstatic,-lboost_thread-mt-d,-lboost_program_options-mt-d,-lcrypto            \
  -Wl,-Bstatic,-llogger-mt-s-d,openbus-mico-mt-s-d,-lssl
#]
#[demo_hello_mak_set_cxxflags
CXXFLAGS= -Ibin -I$(CURDIR)/../../include/mico-mt-d -I$(CURDIR)../../include    \
 -I$(CURDIR)../../include/openbus/cpp -I$(CURDIR)../../include/boost      \
 -I$(CURDIR)../../include/openssl-1.0.0 -g -O0
#]
#[demo_hello_mak_compile_idl
bin/hello.cc bin/hello.h: idl/hello.idl
        cd bin; LD_LIBRARY_PATH=$(MICO_LIB):${LD_LIBRARY_PATH} PATH=${PATH}:$(MICO_BIN) $(MICO_BIN)/idl --typecode --poa --no-paths $<
#]
#[demo_hello_mak_compile_server_cpp
bin/server.o: server/server.cpp bin/hello.h
	gcc $< -o $@ $(CXXFLAGS)
#]
#[demo_hello_mak_compile_client_cpp
bin/client.o: client/client.cpp bin/hello.h
	gcc $< -o $@ $(CXXFLAGS)
#]
#[demo_hello_mak_compile_hello_cc
bin/hello.o: bin/hello.cc bin/hello.h
	gcc $< -o $@ $(CXXFLAGS)
#]
#[demo_hello_mak_binaries
bin/client: bin/client.o bin/hello.o
	gcc $^ -o $@ $(LDFLAGS)

bin/server: bin/server.o bin/hello.o
	gcc $^ -o $@ $(LDFLAGS)
#]
#[demo_hello_mak_all
all: bin/client bin/server
#]
