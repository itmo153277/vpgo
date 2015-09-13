AC_PREREQ([2.69])
AC_INIT([vpgo], [0.1], [viktprog@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([auxdir])
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([vpgo])
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

PKG_CHECK_MODULES([jsoncpp], [jsoncpp >= 0.6.0], [],
	[AC_MSG_ERROR([couldn't find jsoncpp])]);
LIBS="$jsoncpp_LIBS $LIBS"
CPPFLAGS="$jsoncpp_CFLAGS $CPPFLAGS"
CFLAGS="$jsoncpp_CFLAGS $CFLAGS"
CXXFLAGS="$jsoncpp_CFLAGS $CXXFLAGS"

AC_SUBST([CHECK_CFLAGS])
AC_SUBST([CHECK_LIBS])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])

AC_OUTPUT
