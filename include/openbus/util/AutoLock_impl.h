#ifndef TECGRAF_AUTO_LOCK_H_
#define TECGRAF_AUTO_LOCK_H_

#include <CORBA.h>

#include "Mutex_impl.h"

namespace openbus {
  class AutoLock {
  public:
    #ifdef OPENBUS_SDK_MULTITHREAD
    AutoLock(Mutex *m) : _mutex(m) { lock(); }
    #else
    AutoLock(Mutex *m) : _mutex(m) { }
    #endif
    ~AutoLock() { 
      #ifdef OPENBUS_SDK_MULTITHREAD
      if (_locked) unlock();
      #endif
    }

    void lock() {
      #ifdef OPENBUS_SDK_MULTITHREAD
      _mutex->lock();
      _locked = true;
      #endif
    }
    
    void unlock() {
      #ifdef OPENBUS_SDK_MULTITHREAD
      _mutex->unlock();
      _locked = false;
      #endif
    }
    
    bool isLocked() { 
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
