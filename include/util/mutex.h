#ifndef TECGRAF_MUTEX_H_
#define TECGRAF_MUTEX_H_

#include <CORBA.h>

namespace openbus {
  class Mutex {
  public:
    Mutex(MICOMT::Mutex* m) : _mutex(m) { lock(); }
    
    ~Mutex() { if (_locked) unlock(); }
    
    void lock() {
      _mutex->lock();
      _locked = true;
    }
    
    void unlock() {
      _mutex->unlock();
      _locked = false;
    }
    
    bool isLocked() { return _locked; }
  private:
    bool _locked;
    MICOMT::Mutex* _mutex;
  };
}

#endif
