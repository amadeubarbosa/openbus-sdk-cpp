#[demo_hello_mak_set_mico_bin
MICO_BIN= $(CURDIR)/../../bin/mico-mt-d
MICO_LIB= $(CURDIR)/../../lib/mico-mt-d
#]
#[demo_hello_mak_set_ldflags
# 
LDFLAGS= -L$(MICO_LIB) -L$(CURDIR)/../../lib -Wl,--start-group           \
  -Wl,-Bstatic -lmico2.3.13 -lassistant-mico-mt-s-d -lboost_chrono-mt-d  \
  -lboost_system-mt-d -lboost_thread-mt-d -lboost_program_options-mt-d   \
  -lscs-mico-mt-d -lcrypto -llogger-mt-d -lopenbus-mico-mt-s-d -lssl     \
  -Wl,-Bdynamic -ldl -Wl,--end-group -g -pthread
#]
#[demo_hello_mak_set_cxxflags
CXXFLAGS= -Ibin -Ibin/stubs -I../../include/mico-mt-d -I../../include    \
 -I../../include/openbus/cpp -I../../include/boost                       \
 -I../../include/openssl-1.0.0 -g -O0 -pthread
#]
#[demo_hello_mak_compile_idl
bin/stubs/hello.cc bin/stubs/hello.h: idl/hello.idl
	mkdir -p bin/stubs
	cd bin/stubs; LD_LIBRARY_PATH=$(MICO_LIB):${LD_LIBRARY_PATH} PATH=${PATH}:$(MICO_BIN) $(MICO_BIN)/idl --typecode --poa --no-paths ../../idl/hello.idl
#]
#[demo_hello_mak_compile_server_cpp
bin/server.o: server/server.cpp bin/stubs/hello.h
	g++ -c $< -o $@ $(CXXFLAGS)
#]
#[demo_hello_mak_compile_client_cpp
bin/client.o: client/client.cpp bin/stubs/hello.h
	g++ -c $< -o $@ $(CXXFLAGS)
#]
#[demo_hello_mak_compile_hello_cc
bin/hello.o: bin/stubs/hello.cc bin/stubs/hello.h
	g++ -c $< -o $@ $(CXXFLAGS)
#]
#[demo_hello_mak_binaries
bin/client: bin/client.o bin/hello.o
	g++ $^ -o $@ $(LDFLAGS)

bin/server: bin/server.o bin/hello.o
	g++ $^ -o $@ $(LDFLAGS)
#]
#[demo_hello_mak_all
all: bin/client bin/server
#]
