PROJNAME= ACSTester
APPNAME= 22

include ../config

SRC= $(APPNAME).cpp \
  ../stubs/RGSTestC.cxx \
  ../stubs/RGSTestS.cxx \
  $(AUXILIAR_SRC)
