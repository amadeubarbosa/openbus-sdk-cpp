PROJNAME=certifier
APPNAME=${PROJNAME}

include config

OPENBUS_INC= ${OPENBUS_HOME}/include
OPENBUS_LDIR= ${OPENBUS_HOME}/lib

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . \
  stubs \
  ${MICO_INC} \
  ${OPENBUS_INC} \
  ${OPENBUS_INC}/scs \
  ${OPENBUS_INC}/openbus/cpp \
  ${OPENBUS_INC}/openbus/cpp/stubs \
  ${OPENBUS_INC}/boost

LDIR+= ${OPENBUS_LDIR} ${MICO_LDIR}

LIBS+= mico${MICOVERSION} dl crypto ssl

SLIB= ${OPENBUS_LDIR}/$(OPENBUS_LIB) \
      ${OPENBUS_LDIR}/$(SCS_LIB)

USE_NODEPEND= Yes

SRC= src/certifier.cpp stubs/service.cc

IDLS= idl/service.idl

STUBS= stubs/service.cc stubs/service.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../idl/service.idl

genstubs: $(STUBS)
