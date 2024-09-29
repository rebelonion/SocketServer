@echo off
setlocal enabledelayedexpansion

set "total_lines=0"

:: Use "." to start from the current directory
for /r %%d in (.) do (
    :: Skip the 'thirdparty' directory
    echo %%d | findstr /i /c:"\\thirdparty\\" > nul
    if errorlevel 1 (
        for %%f in ("%%d\*.h" "%%d\*.cpp") do (
            if exist "%%f" (
                set "file_lines=0"
                for /f %%l in ('type "%%f" ^| find /v /c ""') do set "file_lines=%%l"
                set /a "total_lines+=file_lines"
                echo %%f: !file_lines! lines
            )
        )
    )
)

echo Total lines: !total_lines!
pause
