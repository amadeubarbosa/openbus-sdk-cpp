// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_MUTEX_H_
#define TECGRAF_SDK_MUTEX_H_

#include <CORBA.h>

namespace openbus 
{
class Mutex 
{
public:
  void lock() 
  {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _mutex.lock();
    #endif
  }

  void unlock() 
  {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _mutex.unlock();
    #endif
  }

  #ifdef OPENBUS_SDK_MULTITHREAD
  MICOMT::Mutex *mutex() 
  { 
    return &_mutex; 
  }
  #endif
private:
  #ifdef OPENBUS_SDK_MULTITHREAD
  MICOMT::Mutex _mutex;
  #endif
};
}
#endif
