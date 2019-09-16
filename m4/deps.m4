AC_DEFUN([VPGO_INIT_DEPS], [
    dnl Boost
    AX_BOOST_BASE([1.65], [], [AC_MSG_ERROR([Cannot build without boost])])
    AX_BOOST_UNIT_TEST_FRAMEWORK
    AX_BOOST_PROGRAM_OPTIONS
])
