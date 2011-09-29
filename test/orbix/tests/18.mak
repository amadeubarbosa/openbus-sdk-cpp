PROJNAME= ACSTester
APPNAME= 18

include ../config

SRC= $(APPNAME).cpp \
  ../stubs/RGSTestC.cxx \
  ../stubs/RGSTestS.cxx \
  $(AUXILIAR_SRC)
