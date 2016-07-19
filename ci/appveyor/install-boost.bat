@echo off


Setlocal EnableDelayedExpansion EnableExtensions

echo "install-boost.bat"

:: create a download & install directories:
mkdir %APPVEYOR_BUILD_FOLDER%\downloads 2>NUL
mkdir %COMMON_INSTALL_DIR% 2>NUL
if defined LOCAL_DEBUG dir %DOWNLOADS_DIR%
if defined LOCAL_DEBUG dir %COMMON_INSTALL_DIR%



echo ==================================== BOOST

:: deducing variables
set BOOST_VER_USC=%BOOST_VERSION:.=_%
set BOOST_SHORT_VER=%BOOST_VER_USC:_0=%


set BOOST_INSTALL_DIR=%COMMON_INSTALL_DIR%
set BOOST_INCLUDE_DIR=%BOOST_INSTALL_DIR%\include\boost-%BOOST_SHORT_VER%
set BOOST_LIBRARY_DIR=%BOOST_INSTALL_DIR%\lib
set BOOST_THREAD_LIB=%BOOST_INSTALL_DIR%\lib\libboost_thread-vc%MSVC_VER:.=%-mt-%BOOST_SHORT_VER%.lib
set BOOST_SYSTEM_LIB=%BOOST_INSTALL_DIR%\lib\libboost_system-vc%MSVC_VER:.=%-mt-%BOOST_SHORT_VER%.lib
set BOOST_ADDRESS_MODEL=%arch%
set BOOST_TOOLSET=msvc-%MSVC_VER%
set BOOST_SRC_DIR=%BUILD_ROOT_DIR%\boost_%BOOST_VER_USC%
set MSBUILD_CONFIGURATION=%CONFIGURATION%

if defined LOCAL_DEBUG (
    echo BOOST_VERSION %BOOST_VERSION%
    echo BOOST_VER_USC %BOOST_VER_USC%
    echo BOOST_SHORT_VER %BOOST_SHORT_VER%
    echo BOOST_INSTALL_DIR %BOOST_INSTALL_DIR%
    echo BOOST_INCLUDE_DIR %BOOST_INCLUDE_DIR%
    echo BOOST_LIBRARY_DIR %BOOST_LIBRARY_DIR%
    echo BOOST_THREAD_LIB %BOOST_THREAD_LIB%
    echo BOOST_SYSTEM_LIB %BOOST_SYSTEM_LIB%
    echo BOOST_ADDRESS_MODEL %BOOST_ADDRESS_MODEL%
    echo BOOST_TOOLSET %BOOST_TOOLSET%
    echo CMAKE_GENERATOR %CMAKE_GENERATOR%
)

:: check that everything needed from boost is there
set BOOST_INSTALL_FLAG=10
if not exist %BOOST_INCLUDE_DIR%\ ( set BOOST_INSTALL_FLAG=1 )
if not exist %BOOST_LIBRARY_DIR%\ ( set BOOST_INSTALL_FLAG=2 )
if not exist %BOOST_THREAD_LIB% ( set BOOST_INSTALL_FLAG=3 )
if not exist %BOOST_SYSTEM_LIB% ( set BOOST_INSTALL_FLAG=4 )

if defined LOCAL_DEBUG echo BOOST_INSTALL_FLAG %BOOST_INSTALL_FLAG%


if not "%BOOST_INSTALL_FLAG%"=="10" (

    :: download if needed
    if not exist %DOWNLOADS_DIR%\boost_%BOOST_VER_USC%.zip (
        echo ***** Downloading Boost %BOOST_VERSION% ...
        pushd %DOWNLOADS_DIR%
        curl -L -O -S -s http://downloads.sourceforge.net/project/boost/boost/%BOOST_VERSION%/boost_%BOOST_VER_USC%.zip
        popd
        if not exist %DOWNLOADS_DIR%\boost_%BOOST_VER_USC%.zip (
            echo something went wrong on boost %BOOST_VERSION% download !!!!!!!!!
            if defined LOCAL_DEBUG dir %DOWNLOADS_DIR%
            Exit \B 1
        )
    ) else (
        echo **** Boost_%BOOST_VER_USC%  already downloaded
    )

    echo **** Extracting Boost_%BOOST_VERSION%.zip ...
    pushd %DOWNLOADS_DIR%
    7z x -o%BUILD_ROOT_DIR%\ boost_%BOOST_VER_USC%.zip
    popd
    if not exist %BOOST_SRC_DIR% (
        echo something went wrong on boost extraction!!!!!!!!!
        if defined LOCAL_DEBUG dir %BOOST_SRC_DIR%
        Exit \B 1
    )

    echo **** Excuting bootstrap.bat...
    if not exist "%BOOST_SRC_DIR%\b2.exe" (
        pushd %BOOST_SRC_DIR%
        call "bootstrap.bat"
        popd
        if not exist "%BOOST_SRC_DIR%\b2.exe" (
            echo something went wrong on booststrap.bat execution!!!!!!!!!
            if defined LOCAL_DEBUG dir %BOOST_SRC_DIR%
            Exit \B 1
        )
    )

    echo **** Excuting  %BOOST_SRC_DIR%\b2.exe ...
    pushd %BOOST_SRC_DIR%
    if defined LOCAL_DEBUG @echo on
    b2 toolset=%BOOST_TOOLSET% variant=release link=static threading=multi address-model=%BOOST_ADDRESS_MODEL% ^
        --with-thread --with-system --prefix=%BOOST_INSTALL_DIR% -d0 install
    if defined LOCAL_DEBUG @echo off
    popd
) else (
    echo Boost_%BOOST_VERSION% already installed
)
echo ====================================

:: =========================================================
:: =========================================================
:: =========================================================

endlocal & set PATH=%PATH%&

goto :eof