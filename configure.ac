AC_PREREQ([2.69])
AC_INIT([vpgo], [0.1], [viktprog@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_HEADERS([config.h])

AC_COPYRIGHT([Copyright (c) 2015 Ivanov Viktor])
AC_REVISION([rev4])

AC_CANONICAL_HOST

host_os_windows=no
case ${host_os} in
	*cygwin* | *mingw* )
		host_os_windows=yes
		;;
esac
AM_CONDITIONAL([WINDOWS], [test "x$host_os_windows" = "xyes"])

dnl Should add -mwin32 to enable _WIN32 macro

case ${host_os} in
	*cygwin* )
	CFLAGS="-mwin32 $CFLAGS"
	CXXFLAGS="-mwin32 $CXXFLAGS"
esac

# Checks for programs.
AC_PROG_RANLIB
AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR

AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_11([], [mandatory])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
