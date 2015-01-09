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
# DEBUG_FLAGS="debug-linux-elf"
DEBUG_FLAGS="-g"
RELEASE_FLAGS=""

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
  execute "make dist"
  execute "make clean"
  # execute "cp electric-fence-2.2.3/libefence.a ."
  # execute "cp electric-fence-2.2.3/libefence.so.* libefence.so"
  execute "./Configure $PLAT $flags"
  execute "make -j$JOBS"
  execute "make install_sw"
}

if [[ "$OSTYPE" == "darwin"* ]]; then
  PLAT="darwin64-x86_64-cc"
  COMMON_FLAGS+="no-asm"
elif [[ "$OSTYPE" == "linux"* ]]; then
  PLAT="linux-elf"
else
  echo "Sorry, this platform does not seem supported."
  exit 1;
fi

cd $OPENSSL_ROOT_PATH
# execute "wget http://ftp.de.debian.org/debian/pool/main/e/electric-fence/electric-fence_2.2.4.tar.gz"
# execute "tar zxf electric-fence_2.2.4.tar.gz"
# execute "rm -f electric-fence_2.2.4.tar.gz"
# cd $OPENSSL_ROOT_PATH/electric-fence-2.2.3
# execute "make"
# cd $OPENSSL_ROOT_PATH
build "$COMMON_FLAGS $SHARED_FLAGS $DEBUG_FLAGS --prefix=$INSTALL/openssl-mt-d"
build "$COMMON_FLAGS $STATIC_FLAGS $DEBUG_FLAGS --prefix=$INSTALL/openssl-mt-s-d"
build "$COMMON_FLAGS $SHARED_FLAGS $RELEASE_FLAGS --prefix=$INSTALL/openssl-mt"
build "$COMMON_FLAGS $STATIC_FLAGS $RELEASE_FLAGS --prefix=$INSTALL/openssl-mt-s"
