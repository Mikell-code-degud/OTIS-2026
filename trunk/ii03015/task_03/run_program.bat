@echo off
chcp 65001 >nul
setlocal EnableExtensions
title task_03 - PID simulation (interactive)

rem =====================================================================
rem  Интерактивный запуск программы (при необходимости - со сборкой).
rem  После прогона, если установлен Python, строится график graph.png.
rem =====================================================================

set "ROOT=%~dp0"
set "BUILD=%ROOT%build"
set "CONFIG=Release"
set "EXE=%BUILD%\bin\%CONFIG%\otis_task_03.exe"

if exist "%EXE%" goto run_program

echo  Исполняемый файл не найден - выполняется сборка...
where cmake >nul 2>nul
if errorlevel 1 (
    echo  *** CMake не найден в PATH! ***
    echo  Установите: winget install -e --id Kitware.CMake
    goto error
)

cmake -S "%ROOT%." -B "%BUILD%"
if errorlevel 1 goto error
cmake --build "%BUILD%" --config %CONFIG%
if errorlevel 1 goto error

if not exist "%EXE%" (
    echo  *** otis_task_03.exe не найден после сборки! ***
    goto error
)

:run_program
echo.
echo  Ответьте на вопросы программы: объект, параметры ПИД, задание w, режим.
echo.
pushd "%BUILD%\bin\%CONFIG%"
"%EXE%"

if exist pid_result.csv (
    where python >nul 2>nul
    if not errorlevel 1 python plot.py
)
popd

echo.
pause
endlocal
exit /b 0

:error
echo.
echo *** FAILED! Смотрите вывод выше. ***
pause
endlocal
exit /b 1
