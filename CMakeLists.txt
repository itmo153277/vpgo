cmake_minimum_required(VERSION 3.25)

project(vpgo CXX)

option(BUILD_TESTING "Enable testing" OFF)
option(ENABLE_LINT "Enable linting" OFF)
option(ENABLE_FAST_MATH "Enable fast math" OFF)

if(BUILD_TESTING)
  include(CTest)
  find_package(GTest REQUIRED)
  include(GoogleTest)
endif()

if(ENABLE_LINT)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /external:anglebrackets /external:W0")
    list(APPEND CMAKE_VS_GLOBALS
      "RunCodeAnalysis=true"
      "EnableMicrosoftCodeAnalysis=true"
      "EnableClangTidyCodeAnalysis=true"
    )
  else()
    add_compile_options(-Wall -Wpedantic -Wextra -Wconversion -Werror)
    find_program(CLANG_TIDY "clang-tidy" REQUIRED)
    find_program(CPPCHECK "cppcheck" REQUIRED)
    find_program(CPPLINT "cpplint" REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY};--quiet;--warnings-as-errors=*")
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK};-q;--enable=style;--error-exitcode=1")
    set(CMAKE_CXX_CPPLINT "${CPPLINT};--quiet")
  endif()
endif()

if(ENABLE_FAST_MATH)
  if(MSVC)
    add_compile_options(/Ox /fp:fast /arch:AVX2)
  else()
    add_compile_options(-march=native -Ofast)
  endif()
endif()

add_subdirectory(core)
