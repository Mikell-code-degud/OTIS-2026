@echo off
chcp 65001 >nul
setlocal EnableExtensions
title task_03 - build, tests, doxygen docs

rem =====================================================================
rem  Лабораторная работа №3. Вариант 6. Низамутдинов М.А., группа ИИ-30.
rem  Полный локальный цикл: сборка -> модульные тесты -> документация.
rem
rem  Использование:
rem      run_all.bat             - сборка Release + тесты + Doxygen
rem      run_all.bat Debug       - то же для конфигурации Debug
rem =====================================================================

set "ROOT=%~dp0"
set "BUILD=%ROOT%build"
set "CONFIG=Release"
if not "%~1"=="" set "CONFIG=%~1"

where cmake >nul 2>nul
if errorlevel 1 (
    echo.
    echo  *** CMake не найден в PATH! ***
    echo  Установите: winget install -e --id Kitware.CMake
    echo.
    pause
    exit /b 1
)

echo =======================================
echo  [STEP 1/3] Compiling and Testing...
echo =======================================
cmake -S "%ROOT%." -B "%BUILD%"
if errorlevel 1 goto error

cmake --build "%BUILD%" --config %CONFIG%
if errorlevel 1 goto error

ctest --test-dir "%BUILD%" -C %CONFIG% --output-on-failure > "%ROOT%test_report.txt" 2>&1
set "TEST_RESULT=%errorlevel%"
type "%ROOT%test_report.txt"
if not "%TEST_RESULT%"=="0" goto tests_failed

echo.
echo =======================================
echo  [STEP 2/3] Generating Doxygen Docs...
echo =======================================
set "DOXYGEN="
where doxygen >nul 2>nul
if not errorlevel 1 set "DOXYGEN=doxygen"
if not defined DOXYGEN if exist "%ProgramFiles%\doxygen\bin\doxygen.exe" set "DOXYGEN=%ProgramFiles%\doxygen\bin\doxygen.exe"
if not defined DOXYGEN (
    echo  doxygen не найден - генерация документации пропущена.
    echo  Установите: winget install -e --id DimitriVanHeesch.Doxygen
    goto docs_done
)
pushd "%ROOT%"
"%DOXYGEN%" "%ROOT%Doxyfile"
set "DOXL=%errorlevel%"
popd
if not "%DOXL%"=="0" goto error
echo  Документация готова: doc\html\index.html
:docs_done

echo.
echo =======================================
echo  [STEP 3/3] Execution Finished Successfully!
echo =======================================
echo  Тесты: отчёт в test_report.txt
echo  Документация: doc\html\index.html
pause
endlocal
exit /b 0

:tests_failed
echo.
echo =======================================
echo  РЕЗУЛЬТАТ: ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ (см. test_report.txt).
echo =======================================
pause
endlocal & exit /b %TEST_RESULT%

:error
echo.
echo *** FAILED! Смотрите вывод выше. ***
pause
endlocal
exit /b 1
