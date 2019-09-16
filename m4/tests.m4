AC_DEFUN([VPGO_INIT_TESTS], [

    can_run_unit_tests="yes"
    if test "x$BOOST_UNIT_TEST_FRAMEWORK_LIB" = "x" ; then
        can_run_unit_tests="no"
        AC_MSG_WARN([could not find boost: unit tests are disabled])
    else
        boost_link="static"
        LIBS_SAVE=$LIBS
        CPPFLAGS_SAVE=$CPPFLAGS
        LIBS="$BOOST_LDFLAGS $BOOST_UNIT_TEST_FRAMEWORK_LIB $LIBS"
        CPPFLAGS="$BOOST_CPPFLAGS $CPPFLAGS -DBOOST_TEST_DYN_LINK -DBOOST_TEST_MODULE=TEST"
        AC_MSG_CHECKING([boost unit test framework linking method])
        dnl Trying to build with dynamic link option, falling back to static otherwise
        AC_LINK_IFELSE([AC_LANG_SOURCE([[@%:@include <boost/test/unit_test.hpp>]])], [boost_link="dynamic"])
        LIBS=$LIBS_SAVE
        CPPFLAGS=$CPPFLAGS_SAVE
        AC_MSG_RESULT([$boost_link])
        if test "x$boost_link" = "xdynamic" ; then
            AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK_CPPFLAGS, [-DBOOST_TEST_DYN_LINK])
        fi
    fi
    AM_CONDITIONAL([UNITTESTS_ENABLED], [test "x$can_run_unit_tests" = "xyes"])
    AC_REQUIRE_AUX_FILE([tap-driver.sh])

])
