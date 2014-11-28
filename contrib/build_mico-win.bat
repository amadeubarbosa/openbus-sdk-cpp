
set ROOT=%HOME%
set JOBS=8
set MICO_ROOT_PATH=%ROOT%\build\mico

rem Do not touch here, please.
set INSTALL=%ROOT%\install
rem

set BOOST_BUILD_PATH=%INSTALL%\boost-build

set PATH=%BOOST_BUILD_PATH%\bin;%PATH%

cd %MICO_ROOT_PATH%
b2.exe -j%JOBS% threading=multi pch=off address-model=32 link=shared runtime-link=shared variant=debug debug-symbols=on debug-store=database install
robocopy /MIR %MICO_ROOT_PATH%\install\debug\shared %INSTALL%\mico-mt-d

b2.exe -j%JOBS% threading=multi pch=off address-model=32 link=shared runtime-link=shared variant=release debug-symbols=on debug-store=database install
robocopy /MIR %MICO_ROOT_PATH%\install\release\shared %INSTALL%\mico-mt

b2.exe -j%JOBS% threading=multi pch=off address-model=32 link=static runtime-link=static variant=debug debug-symbols=on debug-store=database install
robocopy /MIR %MICO_ROOT_PATH%\install\debug\static %INSTALL%\mico-mt-s-d

b2.exe -j%JOBS% threading=multi pch=off address-model=32 link=static runtime-link=static variant=release debug-symbols=on debug-store=database install
robocopy /MIR %MICO_ROOT_PATH%\install\release\static %INSTALL%\mico-mt-s
