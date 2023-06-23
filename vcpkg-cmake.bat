@echo off

:: CMake wrapper for vcpkg

SetLocal

If "%VCPKG_ROOT%" == "" Call :SetupVCPKG
If ErrorLevel 1 GoTo :EOF
For /F "delims=" %%a In ('"%VCPKG_ROOT%\vcpkg" fetch cmake --x-stderr-status') Do Call :ParseOutput "%%~a"
If ErrorLevel 1 GoTo Fail

"%CMAKE%" %*

EndLocal
GoTo :EOF

:SetupVCPKG
If Not Exist "%~dp0.vcpkg\*" Call :CheckedCall git clone https://github.com/microsoft/vcpkg.git --depth 1 "%~dp0.vcpkg"
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
