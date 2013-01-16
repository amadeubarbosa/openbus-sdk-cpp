// -*- coding: iso-8859-1 -*-

#ifndef OPENBUS_ASSISTANT_DECL

#ifdef _WIN32
#  ifdef OPENBUS_ASSISTANT_SHARED
#    ifdef OPENBUS_ASSISTANT_SOURCE
#      define OPENBUS_ASSISTANT_DECL __declspec(dllexport)
#    else
#      define OPENBUS_ASSISTANT_DECL __declspec(dllimport)
#    endif
#  endif
#endif

#ifndef OPENBUS_ASSISTANT_DECL
#define OPENBUS_ASSISTANT_DECL
#endif

#endif
