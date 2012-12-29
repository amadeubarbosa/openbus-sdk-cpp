// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_AUTO_LOCK_H_
#define TECGRAF_SDK_AUTO_LOCK_H_

#include "Mutex_impl.hpp"

#include <CORBA.h>

namespace openbus 
{
class AutoLock 
{
public:
  #ifdef OPENBUS_SDK_MULTITHREAD
  AutoLock(Mutex *m) : _mutex(m) 
  { 
    lock(); 
  }
  #else
  AutoLock(Mutex *m) : _mutex(m) 
  { 
  }
  #endif

  ~AutoLock() 
  {
    #ifdef OPENBUS_SDK_MULTITHREAD
    if (_locked) 
    { 
      unlock();
    }
    #endif
  }
  
  void lock() 
  {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _mutex->lock();
    _locked = true;
    #endif
  }
    
  void unlock() 
  {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _mutex->unlock();
    _locked = false;
    #endif
  }
    
  bool isLocked() 
  { 
    #ifdef OPENBUS_SDK_MULTITHREAD
    return _locked;
    #else
    return false;
    #endif
  }
private:
  bool _locked;
  Mutex *_mutex;
};
}

#endif
