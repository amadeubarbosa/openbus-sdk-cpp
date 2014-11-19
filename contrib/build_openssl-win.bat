set INSTALL=%HOME%\install
set BUILD=%HOME%\build

set OPENSSL_ROOT_PATH=%BUILD%\openssl

cd %OPENSSL_ROOT_PATH%
perl Configure debug-VC-WIN32 no-asm --prefix=%INSTALL%\openssl-mt-d
CALL "ms\do_ms.bat"
nmake -f ms\ntdll.mak install
nmake -f ms\ntdll.mak clean
perl Configure debug-VC-WIN32 no-asm --prefix=%INSTALL%\openssl-mt-s-d
CALL "ms\do_ms.bat"
nmake -f ms\nt.mak install
nmake -f ms\nt.mak clean
perl Configure VC-WIN32 no-asm --prefix=%INSTALL%\openssl-mt
CALL "ms\do_ms.bat"
nmake -f ms\ntdll.mak install
nmake -f ms\ntdll.mak clean
perl Configure VC-WIN32 no-asm --prefix=%INSTALL%\openssl-mt-s
CALL "ms\do_ms.bat"
nmake -f ms\nt.mak install
nmake -f ms\nt.mak clean
