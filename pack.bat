@ECHO OFF

if "%1" == "" GOTO No_Version
	
	REM We have version
	
	REM Prepare directories
	echo.
	echo ----------------------------------
	echo Preparing directories...
	mkdir %1
	mkdir "%1\l10n"
	mkdir "%1\apis"
	mkdir "%1\hlschemes"
	mkdir "%1\plugins"
	
	REM Built program
	echo.
	echo ----------------------------------
	echo Copying program binaries...
	copy "build\juffed.exe" "%1"
	copy "build\libjuff.dll" "%1"
	
	REM localizations, apis, hlschemes
	echo.
	echo ----------------------------------
	echo Copying APIs, l10n, hlschemes...
	copy "build\*.qm" "%1\l10n"
	copy "apis\*.api" "%1\apis"
	copy "hlschemes\*.xml" "%1\hlschemes"

	REM Misc files
	echo.
	echo ----------------------------------
	echo Copying Misc files...
	copy Changelog %1
	copy COPYING %1
	copy win32\cygwin1.dll %1
	copy win32\enca.exe %1
	
	REM Qt and QScintilla libs
	echo.
	echo ----------------------------------
	echo Copying Qt libraries...
	copy "%QTDIR%\bin\QtCore4.dll" "%1"
	copy "%QTDIR%\bin\QtGui4.dll" "%1"
	copy %QTDIR%\bin\QtXml4.dll %1
	copy %QTDIR%\bin\QtNetwork4.dll %1
	copy %QTDIR%\bin\mingwm10.dll %1
	copy %QTDIR%\bin\qscintilla2.dll %1
	
	GOTO End
	
:No_Version
	echo No version specified
	GOTO End

:End
