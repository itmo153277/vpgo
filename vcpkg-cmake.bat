@echo off

:: CMake wrapper for vcpkg

SetLocal

Set VCPKG_BASELINE=98aa6396292d57e737a6ef999d4225ca488859d5

If "%VCPKG_ROOT%" == "" Call :SetupVCPKG
If ErrorLevel 1 GoTo :EOF
For /F "delims=" %%a In ('"%VCPKG_ROOT%\vcpkg" fetch cmake --x-stderr-status') Do Call :ParseOutput "%%~a"
If ErrorLevel 1 GoTo Fail

"%CMAKE%" %*

EndLocal
GoTo :EOF

:CloneVCPKG
If Not Exist "%~dp0.vcpkg" MkDir "%~dp0.vcpkg"
PushD "%~dp0.vcpkg"
Call :CheckedCall git init -q -b main
If ErrorLevel 1 GoTo :EOF
Call :CheckedCall git remote add origin https://github.com/microsoft/vcpkg.git
If ErrorLevel 1 GoTo :EOF
Call :CheckedCall git fetch -q origin "%VCPKG_BASELINE%" --depth 1
If ErrorLevel 1 GoTo :EOF
Call :CheckedCall git checkout -q FETCH_HEAD
If ErrorLevel 1 GoTo :EOF
PopD
GoTo :EOF

:SetupVCPKG
If Not Exist "%~dp0.vcpkg\*" Call :CLoneVCPKG
If ErrorLevel 1 GoTo :EOF
if Not Exist "%~dp0.vcpkg\vcpkg.exe" Call :CheckedCall Call "%~dp0.vcpkg\bootstrap-vcpkg.bat" -disableMetrics
If ErrorLevel 1 GoTo :EOF
Set VCPKG_ROOT=%~dp0.vcpkg
GoTo :EOF

:ParseOutput
If Not Exist "%~1" GoTo PrintParseError
Set CMAKE=%~f1
Exit /B 0

:PrintParseError
Echo.%~1
Exit /B 1

:CheckedCall
copy nul nul >nul
%* 1>nul
If ErrorLevel 1 GoTo Fail
GoTo :EOF

:Fail
(
  EndLocal
  Exit /B %ErrorLevel%
)
