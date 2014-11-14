set INSTALL=%HOME%\install
set BUILD=%HOME%\build
set OPENBUS_ROOT_PATH=%BUILD%\openbus
set BOOST_ROOT_PATH=%BUILD%\boost
set BOOST_BUILD_ROOT_PATH=%BUILD%\boost-build
set SCS_ROOT_PATH=%BUILD%\scs
set LOGGER_ROOT_PATH=%BUILD%\logger
set MICO_ROOT_PATH=%BUILD%\mico
set BOOST_BUILD_INSTALL_PATH=%INSTALL%\boost-build
set OPENBUS_IDL_ROOT_PATH=%BUILD%\openbus-idl
set SCS_IDL_ROOT_PATH=%BUILD%\scs-idl
set MICO_DEBUG_INSTALL_PATH=%INSTALL%\mico-mt-d
set MICO_DEBUG_STATIC_INSTALL_PATH=%INSTALL%\mico-mt-s-d
set MICO_RELEASE_INSTALL_PATH=%INSTALL%\mico-mt
set MICO_RELEASE_STATIC_INSTALL_PATH=%INSTALL%\mico-mt-s
set OPENSSL_DEBUG_INSTALL_PATH=%INSTALL%\openssl-mt-d
set OPENSSL_DEBUG_STATIC_INSTALL_PATH=%INSTALL%\openssl-mt-s-d
set OPENSSL_RELEASE_INSTALL_PATH=%INSTALL%\openssl-mt
set OPENSSL_RELEASE_STATIC_INSTALL_PATH=%INSTALL%\openssl-mt-s
set PACK=%HOME%\packs\openbus-cpp-mico-2.0.0.4-win32-vs2013
set JOBS=8
set TOOLSET=msvc-12.0

mkdir %PACK%\bin
mkdir %PACK%\include
mkdir %PACK%\lib
mkdir %PACK%\idl

copy %BOOST_BUILD_INSTALL_PATH%\bin\b2.exe %PACK%\bin
robocopy /MIR %BOOST_BUILD_ROOT_PATH% %PACK%\boost-build
del /S /Q /F %PACK%\boost-build\.svn

copy %MICO_DEBUG_STATIC_INSTALL_PATH%\bin\idl.exe %PACK%\bin
copy %MICO_DEBUG_STATIC_INSTALL_PATH%\bin\mico-cpp.exe %PACK%\bin
copy %MICO_DEBUG_STATIC_INSTALL_PATH%\bin\gendef.exe %PACK%\bin
robocopy /MIR %MICO_DEBUG_INSTALL_PATH%\lib %PACK%\lib\mico-mt-d
robocopy /MIR %MICO_DEBUG_STATIC_INSTALL_PATH%\lib %PACK%\lib\mico-mt-s-d
robocopy /MIR %MICO_RELEASE_INSTALL_PATH%\lib %PACK%\lib\mico-mt
robocopy /MIR %MICO_RELEASE_STATIC_INSTALL_PATH%\lib %PACK%\lib\mico-mt-s
robocopy /MIR %MICO_DEBUG_INSTALL_PATH%\include %PACK%\include\mico
rem robocopy /MIR %MICO_DEBUG_STATIC_INSTALL_PATH%\include %PACK%\include\mico-mt-s-d
rem robocopy /MIR %MICO_RELEASE_INSTALL_PATH%\include %PACK%\include\mico-mt
rem robocopy /MIR %MICO_RELEASE_STATIC_INSTALL_PATH%\include %PACK%\include\mico-mt-s

copy %OPENSSL_DEBUG_STATIC_INSTALL_PATH%\bin\openssl.exe %PACK%\bin
robocopy /MIR %OPENSSL_DEBUG_INSTALL_PATH%\lib %PACK%\lib\openssl-mt-d
robocopy /MIR %OPENSSL_DEBUG_STATIC_INSTALL_PATH%\lib %PACK%\lib\openssl-mt-s-d
robocopy /MIR %OPENSSL_RELEASE_INSTALL_PATH%\lib %PACK%\lib\openssl-mt
robocopy /MIR %OPENSSL_RELEASE_STATIC_INSTALL_PATH%\lib %PACK%\lib\openssl-mt-s
robocopy /MIR %OPENSSL_RELEASE_STATIC_INSTALL_PATH%\include\openssl %PACK%\include\openssl

robocopy /MIR %BOOST_ROOT_PATH%\boost %PACK%\include\boost\

robocopy /MIR %SCS_ROOT_PATH%\include\scs %PACK%\include\scs

robocopy /MIR %LOGGER_ROOT_PATH%\include\log %PACK%\include\log

robocopy /MIR %MICO_ROOT_PATH%\contrib\pthreads %PACK%\include\pthreads

robocopy /MIR %OPENBUS_IDL_ROOT_PATH%\src %PACK%\idl\v2_0
copy %SCS_IDL_ROOT_PATH%\src\scs.idl %PACK%\idl\v2_0

CALL "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

cd %OPENBUS_ROOT_PATH%
robocopy /MIR demo %PACK%\demo
robocopy /MIR include\openbus %PACK%\include\openbus
b2 -j%JOBS% -d+2 toolset=%TOOLSET% pch=off address-model=32 link=shared runtime-link=shared variant=release variant=debug debug-symbols=on stage-deps
copy stage-deps\* %PACK%\lib

b2 -j%JOBS% -d+2 toolset=%TOOLSET% pch=off address-model=32 link=static runtime-link=static variant=release variant=debug debug-symbols=on stage-deps
copy stage-deps\* %PACK%\lib

b2 -d+2 stage-stubs
robocopy /MIR stage-stubs %PACK%\include\openbus\stubs
