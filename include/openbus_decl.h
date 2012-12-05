/**
* \file openbus_decl.h
*/


#if defined(_WIN32) && defined(OPENBUS_SDK_SHARED)
#  ifdef OPENBUS_SDK_SOURCE
#    define OPENBUS_SDK_DECL __declspec(dllexport)
#  else
#    define OPENBUS_SDK_DECL __declspec(dllimport)
#  endif
#endif

#ifndef OPENBUS_SDK_DECL
#define OPENBUS_SDK_DECL
#endif

