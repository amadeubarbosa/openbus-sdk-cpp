#!/bin/bash

ROOT="$( dirname "${BASH_SOURCE[0]}" )"

OPENBUS_ROOT_PATH="$ROOT/../"
BUILD_PATH="$ROOT/../../../build"
INSTALL_PATH="$ROOT/../../../install"
STAGE_DEPS="$OPENBUS_ROOT_PATH/install/deps"

sleep_time=3

function run-interop
{
  interop=$1
  
  if [ 'simple' == ${interop} ] || [ 'multiplexing' == ${interop} ]; then    
    services="server"
  elif [ 'sharedauth' == ${interop} ]; then
    services="server sharedauth"
  elif [ 'reloggedjoin' == ${interop} ]; then
    services="server proxy"
  elif [ 'delegation' == ${interop} ]; then
    services="messenger broadcaster forwarder"
  fi
  
  flavors="mt-s-d mt-d mt-s mt"
  for flavor in $flavors
  do
    SHARED_LIBS="$STAGE_DEPS:$INSTALL_PATH/openssl-$flavor/lib:$BUILD_PATH/ACE_wrappers/lib"

    pids=''
    for service in $services
    do
      echo "[ Running the '${interop}_$service-$flavor' service... ]"
      DYLD_LIBRARY_PATH=$SHARED_LIBS LD_LIBRARY_PATH=$SHARED_LIBS \
        $OPENBUS_ROOT_PATH/stage-interop/${interop}/${interop}_$service-$flavor \
        --private-key $OPENBUS_ROOT_PATH/stage-interop/${interop}/interop_${interop}_cpp_${service}.key&
      pids="$pids $!"
      echo "[ Waiting the '${interop}_$service-$flavor' service to be ready... ]"
      sleep $sleep_time
    done
    
    echo "[ Running the '${interop}_client-$flavor' service... ]"
    DYLD_LIBRARY_PATH=$SHARED_LIBS LD_LIBRARY_PATH=$SHARED_LIBS \
      $OPENBUS_ROOT_PATH/stage-interop/${interop}/${interop}_client-$flavor
    kill -s INT $pids
  done
}

if [[ ! ($# == 1) ]] \
   || ([ $1 != 'all' ] \
   && [ $1 != 'delegation' ] \
   && [ $1 != 'multiplexing' ] \
   && [ $1 != 'reloggedjoin' ] \
   && [ $1 != 'simple' ] \
   && [ $1 != 'sharedauth' ]);then
  echo 'Usage: run.sh [all | delegation | multiplexing | reloggedjoin | sharedauth | simple]'
  exit 1;    
fi

if [ 'all' == $1 ];then
  interops="simple multiplexing sharedauth reloggedjoin delegation"
  for interop in $interops
  do
    run-interop $interop
  done    
else
  run-interop "$1"
fi
