set INSTALL=%HOME%\install
set BUILD=%HOME%\build

set OPENSSL_ROOT_PATH=%BUILD%\openssl

cd %OPENSSL_ROOT_PATH%
perl Configure debug-VC-WIN64A no-asm --prefix=%INSTALL%\openssl-mt-d
CALL "ms\do_win64a.bat"
nmake -f ms\ntdll.mak install
copy out32dll.dbg\libeay32.pdb %INSTALL%\openssl-mt-d\lib\
copy out32dll.dbg\ssleay32.pdb %INSTALL%\openssl-mt-d\lib\
copy out32dll.dbg\4758cca.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\aep.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\atalla.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\capi.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\chil.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\cswift.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\gmp.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\gost.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\nuron.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\padlock.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\sureware.pdb %INSTALL%\openssl-mt-d\lib\engines\
copy out32dll.dbg\ubsec.pdb %INSTALL%\openssl-mt-d\lib\engines\
nmake -f ms\ntdll.mak clean
perl Configure debug-VC-WIN64A no-asm --prefix=%INSTALL%\openssl-mt-s-d
CALL "ms\do_win64a.bat"
nmake -f ms\nt.mak install
copy tmp32.dbg\lib.pdb %INSTALL%\openssl-mt-s-d\lib\
copy tmp32.dbg\app.pdb %INSTALL%\openssl-mt-s-d\bin\
nmake -f ms\nt.mak clean
perl Configure VC-WIN64A no-asm --prefix=%INSTALL%\openssl-mt
CALL "ms\do_win64a.bat"
nmake -f ms\ntdll.mak install
copy out32dll\libeay32.pdb %INSTALL%\openssl-mt\lib\
copy out32dll\ssleay32.pdb %INSTALL%\openssl-mt\lib\
copy out32dll\4758cca.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\aep.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\atalla.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\capi.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\chil.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\cswift.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\gmp.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\gost.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\nuron.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\padlock.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\sureware.pdb %INSTALL%\openssl-mt\lib\engines\
copy out32dll\ubsec.pdb %INSTALL%\openssl-mt\lib\engines\
nmake -f ms\ntdll.mak clean
perl Configure VC-WIN64A no-asm --prefix=%INSTALL%\openssl-mt-s
CALL "ms\do_win64a.bat"
nmake -f ms\nt.mak install
copy tmp32\lib.pdb %INSTALL%\openssl-mt-s\lib\
copy tmp32\app.pdb %INSTALL%\openssl-mt-s\bin\
nmake -f ms\nt.mak clean
