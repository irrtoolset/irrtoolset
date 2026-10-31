cmake_minimum_required(VERSION 3.16)

project(irrtoolset VERSION 5.1.4 LANGUAGES C CXX)

set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_BUGREPORT "irrtoolset@lists.isc.org")

option(ENABLE_READLINE "Support fancy command line editing" ON)
option(ENABLE_DEBUG "Turn on debugging code for development" OFF)

include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCXXSourceCompiles)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# The code relies on calling methods on NULL objects, and some GCC 6.x/7.x
# versions break -ftree-vrp, so keep GCC from optimising either of these.
foreach(flag -fno-tree-vrp -fno-delete-null-pointer-checks)
  string(MAKE_C_IDENTIFIER "HAVE_C${flag}" c_var)
  string(MAKE_C_IDENTIFIER "HAVE_CXX${flag}" cxx_var)
  check_c_compiler_flag(${flag} ${c_var})
  check_cxx_compiler_flag(${flag} ${cxx_var})
  if(${c_var})
    add_compile_options($<$<COMPILE_LANGUAGE:C>:${flag}>)
  endif()
  if(${cxx_var})
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${flag}>)
  endif()
endforeach()

if(ENABLE_DEBUG)
  add_compile_options(-fno-inline)
endif()

# config.h
foreach(header malloc.h memory.h regex.h strings.h string.h sys/time.h unistd.h)
  string(MAKE_C_IDENTIFIER "HAVE_${header}" var)
  string(TOUPPER "${var}" var)
  check_include_file(${header} ${var})
endforeach()

set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
check_symbol_exists(timegm time.h HAVE_TIMEGM)
check_symbol_exists(vsnprintf stdio.h HAVE_VSNPRINTF)
check_symbol_exists(memcpy string.h HAVE_MEMCPY)
unset(CMAKE_REQUIRED_DEFINITIONS)

check_cxx_source_compiles("
#include <sys/types.h>
#include <sys/socket.h>
int main() {
  int sock = 0;
  struct sockaddr socknew;
  int addrlen = 5;
  accept(sock, &socknew, (socklen_t *) &addrlen);
}" ACCEPT_USES_SOCKLEN_T)

# flex always provides yylineno
set(HAVE_YYLINENO 1)

if(ENABLE_READLINE)
  find_path(READLINE_INCLUDE_DIR readline/readline.h)
  find_library(READLINE_LIBRARY readline)
  find_library(HISTORY_LIBRARY history)
  if(READLINE_INCLUDE_DIR AND READLINE_LIBRARY)
    set(HAVE_LIBREADLINE 1)
    set(HAVE_LIBHISTORY 1)
    set(READLINE_LIBRARIES ${READLINE_LIBRARY})
    if(HISTORY_LIBRARY)
      list(APPEND READLINE_LIBRARIES ${HISTORY_LIBRARY})
    endif()
    find_library(TERMCAP_LIBRARY NAMES tinfo termcap ncurses)
    if(TERMCAP_LIBRARY)
      list(APPEND READLINE_LIBRARIES ${TERMCAP_LIBRARY})
    endif()
  else()
    message(STATUS "readline not found, building without it")
  endif()
endif()

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

add_subdirectory(src)

install(FILES README.md NEWS AUTHORS COPYING
        DESTINATION ${CMAKE_INSTALL_DOCDIR})
