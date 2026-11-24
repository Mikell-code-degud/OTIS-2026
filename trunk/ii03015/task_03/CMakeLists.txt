cmake_minimum_required(VERSION 3.20)

project(OTIS_Task_03_Variant_6 LANGUAGES CXX)

# ---------------------------------------------------------------------------
#  Глобальные настройки компилятора
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# Отдельный подкаталог для каждой конфигурации.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${config}" config_upper)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config_upper} "${CMAKE_BINARY_DIR}/bin/${config}")
endforeach()

option(OTIS_ENABLE_COVERAGE "Включить инструментацию покрытия (gcov/LLVM)" OFF)

# ---------------------------------------------------------------------------
#  Google Test через FetchContent
# ---------------------------------------------------------------------------
include(FetchContent)

set(GTEST_VERSION "1.14.0" CACHE STRING "Версия Google Test")
set(OTIS_GTEST_SOURCE_DIR "" CACHE PATH
    "Готовый каталог с исходниками googletest (задается, если нет сети)")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)

if(OTIS_GTEST_SOURCE_DIR)
    message(STATUS "Google Test: используется локальный каталог ${OTIS_GTEST_SOURCE_DIR}")
    FetchContent_Declare(googletest
        SOURCE_DIR "${OTIS_GTEST_SOURCE_DIR}"
    )
else()
    # Архив, а не git-клон: работает даже если git не установлен.
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
endif()

FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------------------------
#  Библиотека с логикой (объекты управления + анализ устойчивости) - без main()
# ---------------------------------------------------------------------------
add_library(otis_core STATIC
    src/Model1_6.cpp
    src/Model2_8.cpp
    src/Model3_10.cpp
    src/StabilityAnalyzer.cpp
)

target_include_directories(otis_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_features(otis_core PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(otis_core PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(otis_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(OTIS_ENABLE_COVERAGE)
    if(MSVC)
        message(WARNING "Покрытие через gcovr требует GCC/Clang; для MSVC используйте "
                        "OpenCppCoverage или VS Enterprise.")
    else()
        target_compile_options(otis_core PRIVATE --coverage -O0 -g)
        target_link_options(otis_core INTERFACE --coverage)
    endif()
endif()

# ---------------------------------------------------------------------------
#  Основная программа
# ---------------------------------------------------------------------------
add_executable(otis_task_03 src/main.cpp)
target_link_libraries(otis_task_03 PRIVATE otis_core)

add_custom_command(TARGET otis_task_03 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/src/plot.py"
        "$<TARGET_FILE_DIR:otis_task_03>/plot.py"
)

# ---------------------------------------------------------------------------
#  Тесты
# ---------------------------------------------------------------------------
enable_testing()

add_executable(otis_tests
    test/test_main.cpp
    test/test_pid.cpp
)

target_link_libraries(otis_tests PRIVATE otis_core GTest::gtest)

if(MSVC)
    target_compile_options(otis_tests PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(otis_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GoogleTest)
gtest_discover_tests(otis_tests
    DISCOVERY_TIMEOUT 60
    PROPERTIES LABELS "unit"
)

# Отдельная цель: собрать и сразу прогнать тесты.
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_BINARY_DIR}"
            --build-config $<CONFIG> --output-on-failure
    DEPENDS otis_tests
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    COMMENT "Сборка и запуск Google Test через ctest"
)
