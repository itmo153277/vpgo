AC_PREREQ([2.69])

AC_INIT([vpgo], [0.1], [viktprog@gmail.com])
AC_COPYRIGHT([Copyright (c) 2019 Ivanov Viktor])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([auxdir])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES(yes)], [AC_SUBST([AM_DEFAULT_VERBOSITY], [1])])

AC_CANONICAL_HOST

DX_HTML_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_INIT_DOXYGEN([vpgo], [./Doxyfile], [doc])
AM_CONDITIONAL([MAKE_DOXYGEN_MAN], [test "x$DX_FLAG_man" = "x1"])

AC_CONFIG_HEADERS([config.h])

AC_PROG_RANLIB
AM_PROG_AR

VPGO_INIT_CPP
VPGO_INIT_DEPS
VPGO_INIT_TESTS

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
