#!/bin/bash

ROOT=${ROOT:=$HOME}
JOBS=${JOBS:=8}
MICO_ROOT_PATH=$ROOT/build/mico

## Do not touch here, please.
INSTALL=$ROOT/install
##

COMMON_FLAGS="--disable-ssl --disable-elf"
SHARED_FLAGS="--disable-static"
STATIC_FLAGS="--disable-shared"
DEBUG_FLAGS="--disable-optimize --enable-debug"

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

function build
{
  flags=$1
  execute "make distclean"
  execute "./configure $flags"
  execute "make -j$JOBS"
  execute "make -j$JOBS install"
  execute "make distclean"
}

cd $MICO_ROOT_PATH
build "$COMMON_FLAGS $SHARED_FLAGS $DEBUG_FLAGS --prefix=$INSTALL/mico-mt-d"
build "$COMMON_FLAGS $STATIC_FLAGS $DEBUG_FLAGS --prefix=$INSTALL/mico-mt-s-d"
build "$COMMON_FLAGS $SHARED_FLAGS --prefix=$INSTALL/mico-mt"
build "$COMMON_FLAGS $STATIC_FLAGS --prefix=$INSTALL/mico-mt-s"
