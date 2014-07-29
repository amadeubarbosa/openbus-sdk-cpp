// -*- coding: iso-8859-1-unix -*-

#ifndef OPENBUS_SDK_DECL

#ifdef _WIN32
#  ifdef OPENBUS_SDK_SHARED
#    ifdef OPENBUS_SDK_SOURCE
#      define OPENBUS_SDK_DECL __declspec(dllexport)
#    else
#      define OPENBUS_SDK_DECL __declspec(dllimport)
#    endif
#  endif
#endif

#ifndef OPENBUS_SDK_DECL
#define OPENBUS_SDK_DECL
#endif

#endif

