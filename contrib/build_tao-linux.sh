#!/bin/bash

ROOT=${ROOT:=$HOME}
JOBS=${JOBS:=4}
ACE_ROOT=$ROOT/build/ACE_wrappers
TAO_ROOT=$ACE_ROOT/TAO

LOG=".build_`date +%Hh%Mh%Ss`_`date +%d%m%Y`_.out"

function log
{
  echo "[`date +%d%m%Y` `date +%Hh%Mh%Ss`] Running '$1'" | tee -a $LOG
}

function execute 
{
  log "$1"
  eval $1 2>&1 | tee -a $LOG
  return ${PIPESTATUS[0]};
}

export ACE_ROOT=$ACE_ROOT
export TAO_ROOT=$TAO_ROOT
export LD_LIBRARY_PATH=$ACE_ROOT/lib
echo '#include "ace/config-linux.h"' > $ACE_ROOT/ace/config.h
echo 'include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU' > $ACE_ROOT/include/makeinclude/platform_macros.GNU
cd $ACE_ROOT/ace
execute "make -j$JOBS"
execute "make -j$JOBS static_libs_only=1"
cd $ACE_ROOT/apps/gperf
execute "make -j$JOBS"
execute "make -j$JOBS static_libs_only=1"
cd $TAO_ROOT
execute "make -j$JOBS TAO AnyTypeCode PortableServer PI PI_Server CodecFactory"
execute "make -j$JOBS static_libs_only=1 TAO AnyTypeCode PortableServer PI PI_Server CodecFactory"
