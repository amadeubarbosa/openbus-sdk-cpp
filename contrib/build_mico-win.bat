set INSTALL=%HOME%\install
set BUILD=%HOME%\build

set JOBS=4
set TOOLSET=msvc-12.0

set MICO_ROOT_PATH=%BUILD%\mico
set BOOST_BUILD_PATH=%INSTALL%\boost-build

set PATH=%BOOST_BUILD_PATH%\bin;%PATH%

cd %MICO_ROOT_PATH%
b2.exe -j%JOBS% toolset=%TOOLSET% threading=multi pch=off address-model=32 link=shared runtime-link=shared variant=debug variant=release debug-symbols=on install
b2.exe -j%JOBS% toolset=%TOOLSET% threading=multi pch=off address-model=32 link=static runtime-link=static variant=debug variant=release debug-symbols=on install
robocopy /MIR %MICO_ROOT_PATH%/install/%TOOLSET%/debug/shared %INSTALL%/mico-mt-d
robocopy /MIR %MICO_ROOT_PATH%/install/%TOOLSET%/debug/static %INSTALL%/mico-mt-s-d
robocopy /MIR %MICO_ROOT_PATH%/install/%TOOLSET%/release/shared %INSTALL%/mico-mt
robocopy /MIR %MICO_ROOT_PATH%/install/%TOOLSET%/release/static %INSTALL%/mico-mt-s
