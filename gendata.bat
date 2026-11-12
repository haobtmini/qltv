@echo off
cd /d "%~dp0"
g++ -std=c++17 -O2 gendata.cpp -o gendata.exe
if errorlevel 1 (
    echo Build that bai!
    pause
    exit /b 1
)
gendata.exe
pause