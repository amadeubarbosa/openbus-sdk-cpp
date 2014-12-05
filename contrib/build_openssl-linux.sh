#!/bin/bash

ROOT=${ROOT:=$HOME}
JOBS=${JOBS:=1} #It is seems that OpenSSL does not support jobs>1 :(
OPENSSL_ROOT_PATH=$ROOT/build/openssl

## Do not touch here, please.
INSTALL=$ROOT/install
##

COMMON_FLAGS=""
SHARED_FLAGS="shared"
STATIC_FLAGS="no-shared"
DEBUG_FLAGS="debug-linux-elf"
RELEASE_FLAGS="linux-elf"

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
  execute "make clean"
  execute "./Configure $flags"
  execute "make -j$JOBS"
  execute "make install_sw"
  execute "make clean"
}

cd $OPENSSL_ROOT_PATH
# build "$COMMON_FLAGS $SHARED_FLAGS $DEBUG_FLAGS --prefix=$INSTALL/openssl-mt-d"
# build "$COMMON_FLAGS $STATIC_FLAGS $DEBUG_FLAGS --prefix=$INSTALL/openssl-mt-s-d"
build "$COMMON_FLAGS $SHARED_FLAGS $RELEASE_FLAGS --prefix=$INSTALL/openssl-mt"
build "$COMMON_FLAGS $STATIC_FLAGS $RELEASE_FLAGS --prefix=$INSTALL/openssl-mt-s"
